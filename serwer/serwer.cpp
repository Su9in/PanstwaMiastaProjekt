#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <string>
#include <unordered_map>
#include <random>
#include <csignal>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <atomic>
#include <algorithm> 


#define BUFFER_SIZE UINT16_MAX
#define SERVER_PORT 1313

#define TIME_TO_START_ROUND 4
#define TIME_FOR_OTHER_PLAYERS 10
#define TIME_FOR_VOTE 10

#define ROUND_TIME 80
#define MAX_POINTS 20


struct Player {
    int socket;                     
    sockaddr_in playerAddr;         
    std::string name = "Player";    
    int gameId = -1;                
    std::string playerWhere;        
    std::mutex mtx;
};

struct Game {
    int id;                                                                                 
    std::string name = "Pokoj";
    std::string gameStateText = "EMPTY";
    bool gameState;

    std::string letter;
    int maxPlayers = 3;
    bool playAgain = true;
    int roundTimeSec = ROUND_TIME;
   
    std::mutex mtx;
    std::condition_variable cv;
    bool timerRunning = false;

    std::unordered_map<int, int> players;                                         
    std::vector<int> playersInOrder = {};    

    std::unordered_map<int, std::unordered_map<std::string, std::string>> answers; 
    std::unordered_map<std::string, std::unordered_map<std::string,int>> allAnswersToCategories;

    std::unordered_map<int, int> votes = {{0, 0}, {1, 0}};

    std::unordered_map<int, int> scores;                                            
    std::unordered_map<int, std::unordered_map<std::string, int>> currentScores;   
};

std::unordered_map<int, Player> players;
std::unordered_map<int, Game> games;

int nextGameId = 1;
bool serverRunning = true;

std::string answersFileName = "sieci_panstwa_miasta.csv";
std::unordered_map<std::string, std::unordered_set<std::string>> serverAnswers;
std::vector<std::string> categories;


std::vector<pollfd> pollfds;
std::mutex mtx_check;
std::condition_variable cv_check;


void sendCurrentScores(int gameId) {

    for (const auto& playerSend : games[gameId].players) {

        std::string currentScoresMsg = "CURRENT_SCORES\n";
        send(playerSend.first, currentScoresMsg.c_str(), currentScoresMsg.size(), 0);
    
        std::vector<std::string> currentScores;

        for (const auto& player : games[gameId].currentScores) {

            std::string playerName = players[player.first].name;
            std::string score = "SCORE:" + playerName + "#";

            for (const auto& category : player.second) {          
                score += games[gameId].answers[player.first][category.first] + "," + std::to_string(category.second) + ";"; 
            }

            score += "\n";
            std::cerr << ">> SENDING CURRENT SCORES TO " << player.first << ": " << score << "\n";
            send(playerSend.first, score.c_str(), score.size(), 0);

        }
    }
}


bool askPlayers(int gameFd, int playerFd, std::string category, std::string answer, std::unique_lock<std::mutex> &lock) {
    std::cerr << "\n ------------------- Voting --------------------------\n\n";

    std::cerr << "gameFd:" << gameFd << "\n";
    std::cerr << "playerFd:" << playerFd << "\n";
    std::cerr << "category:" << category << "\n";
    std::cerr << "answer:" << answer << "\n\n";

    Game& game = games[gameFd];

    std::string vote = "VOTE:" + players[playerFd].name + "," + category + "," + answer +"\n";
    std::cerr << "Vote: " << vote << "\n";

    for (const auto& player : games[gameFd].playersInOrder) {
        std::cerr << ">> Sent VOTE to " << players[player].name << "\n";
        send(player, vote.c_str(), vote.size(), 0);
    }

    std::atomic<bool> shortenTimer;
    shortenTimer.store(false);

    game.timerRunning = true;

    std::thread timerThread([&game, &shortenTimer]() {

        auto voteEnd = std::chrono::steady_clock::now() + std::chrono::seconds(TIME_FOR_VOTE);

        while (game.timerRunning) {

            if (shortenTimer.load()) {
                voteEnd = std::chrono::steady_clock::now() + std::chrono::seconds(TIME_FOR_OTHER_PLAYERS);
                shortenTimer.store(false);
            }
            
            auto remainingTime = std::chrono::duration_cast<std::chrono::seconds>(
                voteEnd - std::chrono::steady_clock::now()
            ).count();

            std::string time = "TIME:" + std::to_string(remainingTime) + "\n";

            for (const auto& player : game.players) {
                send(player.first, time.c_str(), time.size(), 0);
                //std::cerr << ">> Sent: TIME" << " to " << players[player.first].name << " (" << player.first<< ")\n\n";
            }

            if (remainingTime <= 0) break;

            auto nextTick = std::chrono::steady_clock::now() + std::chrono::seconds(1);
            std::this_thread::sleep_until(nextTick);
        }

    });


    auto voteEnd = std::chrono::steady_clock::now() + std::chrono::seconds(TIME_FOR_VOTE);
    while (std::chrono::steady_clock::now() < voteEnd) {

        game.cv.wait_until(lock, voteEnd);
 
        int yes = game.votes[1];
        int no = game.votes[0];
        int voters = game.players.size();

        if (yes + no == voters) break;

    }

    game.timerRunning = false;
    if (timerThread.joinable()) {
        timerThread.join();
    }

    int yes = game.votes[1];
    int voters = game.players.size();

    std::cerr << "\t yes count: " << yes << std::endl;
    std::cerr << "\t voters count: " << voters << std::endl;

    if (yes >= voters / 2 + 1) return true;
    else return false;
}



bool checkNick(std::string nick) {
    std::cerr << "# CHECKING NICK #\n";
    for (const auto& player : players) {
        if (player.second.name == nick) return true;
    }
    return false;
}


void showDatabase() {
    std::cerr << "\nSHOWING DATABASE\n";
    for (const auto& cat : serverAnswers) {
        std::cerr << "CATEGORY:" << cat.first << "#" << std::endl;
        for (const auto& ans : cat.second) {
            std::cerr << ans << "*";
        }
        std::cerr << std::endl;
    }
}


void loadCSV(const std::string& fileName, std::unordered_map<std::string, std::unordered_set<std::string>>& serverAnswers) {
    std::ifstream file(fileName);
    std::string line;

    if (!file.is_open()) {
        perror("Can't open answers file.");
        return;
    }

    bool first = true;
    std::cerr << "\n# READING FROM CSV #\n";

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string element;

        int i = 0;

        if (first) {
            while (getline(ss, element, ';')) {
                serverAnswers[element] = {};  
                categories.push_back(element);
                
            } 
            first = false;
        }

        while (getline(ss, element, ';')) {

            if (!element.empty()) {
                std::transform(element.begin(), element.end(), element.begin(), ::tolower);
                serverAnswers[categories[i]].insert(element);
            } 
            ++i;
        }
    }
    file.close();
}


void sendFirstPlayer(int gameId) {
    int playerFd = games[gameId].playersInOrder.front();
    std::string msg = "FIRST\n";
    send(playerFd, msg.c_str(), msg.size(), 0);
    std::cerr << ">> Sent: FIRST" << " to " << players[playerFd].name << " (" << playerFd<< ")\n\n";  
}



char generateLetter() {
    std::default_random_engine gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(65, 90);
    int letter = dist(gen);
    return (char) letter;
}


void broadcastGamesToLobby() {
    std:: string gamesList = "ROOMS:";
    
    for (const auto& pair : games) {
        gamesList += std::to_string(games[pair.first].id);
        gamesList += ";";
        gamesList += games[pair.first].name;
        gamesList += ";";
        gamesList += std::to_string(games[pair.first].maxPlayers);
        gamesList += ";";
        gamesList += std::to_string(games[pair.first].players.size());
        gamesList += ";";
        gamesList += games[pair.first].gameStateText;
        gamesList += ",";
        
    }

    gamesList += "\n";

    std::cerr << "[BROADCAST] [LOBBY] [GAMES]: " << gamesList << std::endl;

    const char* data = gamesList.c_str();
    size_t length = gamesList.size();

    for (const auto& pair : players) {
        if (players[pair.first].playerWhere == "LOBBY") {
            send(pair.first, data, length, 0);
        }
    }

}


void sendScores(int gameId) {
    Game& game = games[gameId];

    std::string scoresMsg = "SCORES:";

    for (const auto& score : game.scores) {
        std::string playerName = players[score.first].name;
        int scoreValue = score.second;

        scoresMsg += playerName + ":" + std::to_string(scoreValue) + ",";
    }

    scoresMsg += "\n";

    for (const auto& player : game.players) {
        send(player.first, scoresMsg.c_str(), scoresMsg.size(), 0);
        std::cerr << ">> Sent: SCORES" << " to " << players[player.first].name << " (" << player.first<< ")\n\n";
    }
}


void validateAnswers(int gameId, std::unique_lock<std::mutex> &lock) {

    Game& game = games[gameId];

    for (const auto& player : game.players) {
        int playerFd = player.first;

        std::cerr << "===================== CHECKING ANSWERS FOR " << players[playerFd].name << " =====================\n"; 

        for (const auto& catAns : game.answers[playerFd]) {

            std::string category = catAns.first;
            std::string answer = catAns.second;
            std::transform(answer.begin(), answer.end(), answer.begin(), ::tolower);

            std::cerr << "=== " << category << ":" << answer << " ===\n"; 

            if (answer == "") {
                std::cerr << "[NO ANSWER]\n";
                game.currentScores[playerFd][category] = 0;
                continue;
            } 

            std::string let = game.letter;
            std::transform(let.begin(), let.end(), let.begin(), ::tolower);
            char letter = let.at(0);

            if (answer.at(0) != letter) {
                std::cerr << "[WRONG LETTER]\n";
                game.currentScores[playerFd][category] = 0;
                continue;
            } 
            
            if (serverAnswers.find(category) != serverAnswers.end()) {
                const auto& categoryAnswers = serverAnswers[category];
                if (categoryAnswers.find(answer) != categoryAnswers.end()) {

                    std::cerr << "[ANSWER IN DATABASE]";

                    if (game.allAnswersToCategories[category][answer] > 1) {
                        std::cerr << "[DUPLICATING OTHER ANSWER]\n";
                        game.scores[playerFd] += MAX_POINTS/2;
                        game.currentScores[playerFd][category] = MAX_POINTS/2;
                    } else {
                        std::cerr << "[ONLY ONE ANSWER LIKE THIS]\n";
                        game.scores[playerFd] += MAX_POINTS;
                        game.currentScores[playerFd][category] = MAX_POINTS;
                    }

                } else {
                    std::cerr << "[NOT IN DATABASE]\n";

                    if (askPlayers(gameId, playerFd, category, answer, lock)) {
                        std::cerr << "[VOTERS - YES]\n";

                        if (game.allAnswersToCategories[category][answer] > 1) {
                            std::cerr << "[DUPLICATING OTHER ANSWER]\n";
                            game.scores[playerFd] += MAX_POINTS/2;
                            game.currentScores[playerFd][category] = MAX_POINTS/2;
                        } else {
                            std::cerr << "[ONLY ONE ANSWER LIKE THIS]\n";
                            game.scores[playerFd] += MAX_POINTS;
                            game.currentScores[playerFd][category] = MAX_POINTS;
                        }
                    } else {
                        std::cerr << "[VOTERS - NO]\n";
                        game.currentScores[playerFd][category] = 0;
                    }

                    game.votes[1] = 0;
                    game.votes[0] = 0;
                }
            } 
        }     
    }

    sendCurrentScores(gameId);
}


void broadcastTimeShortGame(int gameId) {
    std::string msg = "SHORT\n";
    for (const auto& pair : players) {
        if (pair.second.gameId == gameId) {
            send(pair.first, msg.c_str(), msg.size(), 0);
            std::cerr << ">> Sent: SHORT" << " to " << players[pair.first].name << " (" << pair.first<< ")\n\n";
        }
    }
}


void gameRound(Game& game, int gameId, std::unique_lock<std::mutex> &lock) {
    char letter = generateLetter();
    games[gameId].letter = letter;
    std::cerr << "# Chosen letter:" << letter << " #" << std::endl;
    
    std::string startMessage = "START:" + std::string(1, letter) + "\n";
    for (const auto& player : game.players) {
        send(player.first, startMessage.c_str(), startMessage.size(), 0);
        std::cerr << ">> Sent: START" << " to " << players[player.first].name << " (" << player.first<< ")\n\n";
    }

    std::atomic<bool> shortenTimer;
    shortenTimer.store(false);

    game.timerRunning = true;

    std::thread timerThread([&game, &shortenTimer]() {

        auto roundEnd = std::chrono::steady_clock::now() + std::chrono::seconds(game.roundTimeSec);

        while (game.timerRunning) {
            if (shortenTimer.load()) {
                roundEnd = std::chrono::steady_clock::now() + std::chrono::seconds(TIME_FOR_OTHER_PLAYERS);
                shortenTimer.store(false);
            }
            
            auto remainingTime = std::chrono::duration_cast<std::chrono::seconds>(
                roundEnd - std::chrono::steady_clock::now()
            ).count();

            if (game.players.empty()) break;

            std::string time = "TIME:" + std::to_string(remainingTime) + "\n";

            for (const auto& player : game.players) {
                send(player.first, time.c_str(), time.size(), 0);
                std::cerr << ">> Sent: TIME" << " to " << players[player.first].name << " (" << player.first<< ")\n\n";
            }

            if (remainingTime <= 0) break;

            auto nextTick = std::chrono::steady_clock::now() + std::chrono::seconds(1);
            std::this_thread::sleep_until(nextTick);
        }

    });

    auto roundEnd = std::chrono::steady_clock::now() + std::chrono::seconds(game.roundTimeSec);
    while (std::chrono::steady_clock::now() < roundEnd) {

        auto remainingTime = std::chrono::duration_cast<std::chrono::seconds>(
            roundEnd - std::chrono::steady_clock::now()
        ).count();

        game.cv.wait_until(lock, roundEnd);

        if (game.answers.size() == game.players.size()) break;
        if (game.players.empty()) break;
        if (game.answers.size() == 1 && remainingTime > TIME_FOR_OTHER_PLAYERS) {
            broadcastTimeShortGame(gameId);
            shortenTimer.store(true);
        };
    }


    game.timerRunning = false;
    if (timerThread.joinable()) {
        timerThread.join();
    }

    if(games[gameId].players.empty()){
        games.erase(gameId);
        broadcastGamesToLobby();
        return;
    }

    if (!games[gameId].players.empty()) {
        validateAnswers(gameId, lock);
    }
    
}


void startRoundCountdown(Game& game, int gameId, std::unique_lock<std::mutex> &lock) {
    std::string prepareMsg = "PREPARE_START\n";

    for (const auto& player : game.players) {
        send(player.first, prepareMsg.c_str(), prepareMsg.size(), 0);
    }

    game.timerRunning = true;

    std::thread timerThread([&game]() {
        auto roundEnd = std::chrono::steady_clock::now() + std::chrono::seconds(TIME_TO_START_ROUND);
        while (game.timerRunning && std::chrono::steady_clock::now() < roundEnd) {
            auto remainingTime = std::chrono::duration_cast<std::chrono::seconds>(
                roundEnd - std::chrono::steady_clock::now()
            ).count();

            if (game.players.empty()) break;

            std::string time = "TIME:" + std::to_string(remainingTime) + "\n";

            for (const auto& player : game.players) {
                send(player.first, time.c_str(), time.size(), 0);
                std::cerr << ">> Sent: TIME" << " to " << players[player.first].name << " (" << player.first<< ")\n\n";
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });


    auto roundEnd = std::chrono::steady_clock::now() + std::chrono::seconds(7);
    while (std::chrono::steady_clock::now() < roundEnd) {
        game.cv.wait_until(lock, roundEnd);
        if (game.players.empty()) break;
    }

    game.timerRunning = false;
    if (timerThread.joinable()) {
        timerThread.join();
    }

    gameRound(game, gameId, lock);    
}



void gameThread(int gameId) {

    Game& game = games[gameId];
    std::unique_lock<std::mutex> lock(game.mtx);

    while(game.gameState) {
        if (game.playAgain) {
            game.playAgain = false;

            startRoundCountdown(game, gameId, lock);      
        }
    }
}


void startGame(int gameId) {
    std::cerr << "# GAME " << games[gameId].name << " HAS STARTED #" << std::endl;
    
    games[gameId].gameState = true;
    games[gameId].gameStateText = "RUNNING";
    broadcastGamesToLobby();

    std::thread(gameThread, gameId).detach();
}


void broadcastPlayersToLobby() {
    std::string lobbyList = "LOBBY:";

    for (const auto& pair : players) {
        if (pair.second.playerWhere == "LOBBY") {
            lobbyList += players[pair.first].name;
            lobbyList += ",";
        }
    }

    lobbyList += "\n";
    std::cerr << "[BROADCAST] [LOBBY] [PLAYERS]: " << lobbyList << std::endl;

    const char* data = lobbyList.c_str();
    size_t length = lobbyList.size();

    for (const auto& pair : players) {
        if (players[pair.first].playerWhere == "LOBBY") {
            send(pair.first, data, length, 0);
        }
    }
}


void broadcastPlayAgain(int gameId) {
    std::string hello = "AGAIN\n";
    for (const auto& pair : players) {
        if (pair.second.gameId == gameId) {
            send(pair.first, hello.c_str(), hello.size(), 0);
            std::cerr << ">> Sent: AGAIN" << " to " << players[pair.first].name << " (" << pair.first<< ")\n\n";
        }
    }
}


void broadcastPlayersToGame(int gameId) {
    mtx_check.lock();
    sendScores(gameId);
    mtx_check.unlock();
}


void joinGame(int gameId, int clientFd) {
    std::cerr << "# PLAYER " << players[clientFd].name << " HAS JOINED ROOM " << games[gameId].name;

    if (games.find(gameId) == games.end()) {
        return;
    }

    players[clientFd].gameId = gameId;
    games[gameId].players[clientFd] = 1;
    players[clientFd].playerWhere = "GAME";

    games[gameId].scores[clientFd] = 0;
    games[gameId].gameStateText = "WAITING";
    games[gameId].playersInOrder.push_back(clientFd);

    broadcastPlayersToLobby();

    std::string data_joined = "JOINED:";
    data_joined += games[gameId].name;
    data_joined += "\n";

    const char* data = data_joined.c_str();
    size_t length = data_joined.size();

    send(clientFd, data, length, 0);
    std::cerr << ">> Sent: JOINED" << " to " << players[clientFd].name << " (" << clientFd<< ")\n\n";
}


void createGameRoom(std::string gameName, int maxPlayers, int gameId = nextGameId) {
    games[nextGameId].id = gameId;
    games[nextGameId].name = gameName;
    games[nextGameId].gameState = false;
    games[nextGameId].maxPlayers = maxPlayers;

    nextGameId++;
}


void playerLeftGame(int clientFd, bool disconnected = false) {

    std::cerr << "# PLAYER " << players[clientFd].name <<" IS LEAVING A GAME #" << std::endl;

    int gameId = players[clientFd].gameId;
    std::vector<int> &playersInOrder = games[gameId].playersInOrder;

    if (playersInOrder.at(0) == clientFd) {

        playersInOrder.erase(std::remove(playersInOrder.begin(), playersInOrder.end(), clientFd), playersInOrder.end());
        if (!playersInOrder.empty()) {
            sendFirstPlayer(gameId);
        }
        
    } else {
        playersInOrder.erase(std::remove(playersInOrder.begin(), playersInOrder.end(), clientFd), playersInOrder.end());
    }

    games[gameId].currentScores.erase(clientFd);

    players[clientFd].playerWhere = "LOBBY";
    games[gameId].players.erase(clientFd);


    if (games[gameId].players.empty()) {
        games[gameId].cv.notify_all();
        games[gameId].timerRunning = false;
        games[gameId].gameState = false;;
        games.erase(gameId);
    }

    players[clientFd].gameId = -1;

    broadcastGamesToLobby();
    if (!disconnected) {
        broadcastPlayersToLobby();
    }

}


void disconnectClient(int clientSocket) {
    std::cerr << "# CLIENT " << players[clientSocket].name << " HAS DISCONECTED\n";

    int gameId = players[clientSocket].gameId;

    if (players[clientSocket].gameId > -1) {
        playerLeftGame(clientSocket, true);
    }

    if(players[clientSocket].playerWhere == "GAME"){

        if (games[gameId].playersInOrder.at(0) == clientSocket) {
        
            games[gameId].playersInOrder.erase(std::remove(games[gameId].playersInOrder.begin(), games[gameId].playersInOrder.end(), clientSocket), games[gameId].playersInOrder.end());

            if (!games[gameId].playersInOrder.empty()) {
                sendFirstPlayer(gameId);
            }
        } 
    }
    
    if (players[clientSocket].gameId ==-1){
        if (games[gameId].players.empty()) {
            games[gameId].cv.notify_all();
            games[gameId].timerRunning = false;
            games[gameId].gameState = false;
            games.erase(gameId);
        }
    }

    mtx_check.lock();
    players[clientSocket].mtx.lock();
    
    players.erase(clientSocket);

    players[clientSocket].mtx.unlock();
    mtx_check.unlock();

    auto it = std::find_if(pollfds.begin(), pollfds.end(), [clientSocket](const pollfd& pfd) {
        return pfd.fd == clientSocket;
    });

    size_t index = std::distance(pollfds.begin(), it);

    if (gameId != -1) {

        games[gameId].players.erase(clientSocket);

        if (games[gameId].players.size() == 0) {
            games[gameId].gameStateText = "EMPTY";
            broadcastGamesToLobby();
        }

        games[gameId].scores.erase(clientSocket);
        broadcastPlayersToGame(gameId);
    }

    close(clientSocket);
    pollfds.erase(pollfds.begin() + index);

    broadcastPlayersToLobby();
    broadcastGamesToLobby();
}


int startServer() {
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd == -1) {
        perror("Error occured while creating a socket");
        exit(1);
    }
    
    fcntl(serverFd, F_SETFL, O_NONBLOCK);

    const int one = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    int fail = bind(serverFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
    if (fail) {
        perror("Bind failed");
        close(serverFd);
        exit(1);
    }

    fail = listen(serverFd, SOMAXCONN);
    if (fail) {
        perror("Listen failed");
        close(serverFd);
        exit(1);
    }

    createGameRoom("starter room", 3);

    return serverFd;
}


void newConnection(int serverFd, std::vector<pollfd> &pollfds) {

    sockaddr_in clientAddr{};
    socklen_t clientAddrSize = sizeof(clientAddr);

    int clientFd = accept(serverFd, (sockaddr*) &clientAddr, &clientAddrSize);

    if (clientFd == -1) {
        perror("Accept failed");
        return;
    }

    pollfds.push_back({
        clientFd,
        POLLIN,
        -1
    });

    players[clientFd].socket = clientFd;
    players[clientFd].playerAddr = clientAddr;


    fcntl(clientFd, F_SETFL, O_NONBLOCK);    

    std::string hello = "HELLO\n";
    send(clientFd, hello.c_str(), hello.size(), 0);

    std::cerr << ">> Sent: HELLO" << " to " << players[clientFd].name << " (" << clientFd<< ")\n\n";

}


void clientMsgActions(int clientSocket, std::string clientMessage) {

    if (clientMessage == "HELLO") {

        std::cerr << "# PLAYER CONNECTED -> " << inet_ntoa(players[clientSocket].playerAddr.sin_addr) << ":" << ntohs(players[clientSocket].playerAddr.sin_port) << " (" << clientSocket << ")" << " #\n\n";


    } else if (clientMessage.substr(0,5) == "NICK:") {

        std::string nick = clientMessage.substr(5);
        std::string nickResponse = "NICK:";

        if (checkNick(nick)) {
            nickResponse += "TAKEN\n";
            send(clientSocket, nickResponse.c_str(), nickResponse.size(), 0);
            std::cerr << ">> Sent: NICK:TAKEN" << " to " << players[clientSocket].name << " (" << clientSocket<< ")\n\n";
        } else {
            nickResponse += "OK\n";
            send(clientSocket, nickResponse.c_str(), nickResponse.size(), 0);
            std::cerr << ">> Sent: NICK:OK" << " to " << players[clientSocket].name << " (" << clientSocket << ")\n\n";

            players[clientSocket].name = nick;
            players[clientSocket].playerWhere = "LOBBY";

            broadcastPlayersToLobby();
            broadcastGamesToLobby();

            std::cerr << "# PLAYER " << inet_ntoa(players[clientSocket].playerAddr.sin_addr) << ":" << ntohs(players[clientSocket].playerAddr.sin_port) << " (" << clientSocket << ") chose the nick " << players[clientSocket].name << " #"<< std::endl;
        }

        

    } else if (clientMessage.substr(0,12) == "CREATE_ROOM:") {
        clientMessage = clientMessage.substr(12);

        int comma = clientMessage.find(",");
        std::string maxPlayers = clientMessage.substr(comma+1);
        std::string roomName = clientMessage.substr(0, clientMessage.length()-maxPlayers.length()-1);
        
        createGameRoom(roomName, std::stoi(maxPlayers));

        std::string roomMsg = "ROOM_CREATED\n";
        send(clientSocket, roomMsg.c_str(), roomMsg.size(), 0); 
        std::cerr << ">> Sent: ROOM_CREATED" << " to " << players[clientSocket].name << " (" << clientSocket<< ")\n\n";


    } else if (clientMessage.substr(0,9) == "JOIN_ROOM") {
        int gameFd = std::stoi(clientMessage.substr(10, clientMessage.length()));
        joinGame(gameFd, clientSocket);


    } else if (clientMessage == "JOINED_GAME") {
        broadcastGamesToLobby();
        broadcastPlayersToGame(players[clientSocket].gameId);

        int gameId = players[clientSocket].gameId;

        if (games[gameId].playersInOrder.size() == 1) {
            sendFirstPlayer(gameId);
        }


    } else if (clientMessage.substr(0, 8) == "ANSWERS:") {
        int gameId = players[clientSocket].gameId;
        Game& game = games[gameId];

        std::lock_guard<std::mutex> lock(game.mtx);
        std::string answers = clientMessage.substr(8);

        auto& playerAnswers = game.answers[clientSocket];

        size_t ansPos = 0;
        std::string ans;

        std::vector<std::string> separatedAnswers;

        while ((ansPos = answers.find(',')) != std::string::npos) {
            ans = answers.substr(0, ansPos);
            separatedAnswers.push_back(ans);
            answers.erase(0, ansPos+1);
        }

        separatedAnswers.push_back(answers);

        for (const auto& answer : separatedAnswers) {
            size_t separator = answer.find(":");
            std::string category = answer.substr(0, separator);
            std::string answerToCategory = answer.substr(separator+1);

            playerAnswers[category] = answerToCategory;
            std::transform(answerToCategory.begin(), answerToCategory.end(), answerToCategory.begin(), ::tolower);

            if (game.allAnswersToCategories[category].find(answerToCategory) != game.allAnswersToCategories[category].end()) {
                game.allAnswersToCategories[category][answerToCategory] += 1;
            } else {
                game.allAnswersToCategories[category][answerToCategory] = 1;
            }

            games[gameId].cv.notify_all();
            
        }

        game.cv.notify_one();
        

    } else if (clientMessage.substr(0, 10) == "START_GAME") {

        if (games[players[clientSocket].gameId].gameState) {
            int gameId = players[clientSocket].gameId;
            games[gameId].playAgain = true;

            games[gameId].allAnswersToCategories.clear();
            games[gameId].answers.clear();
        }
        else {
            startGame(players[clientSocket].gameId);
        }
        


    } else if (clientMessage.substr(0, 10) == "PLAY_AGAIN") {
        int gameId = players[clientSocket].gameId;
        games[gameId].playAgain = true;

        games[gameId].allAnswersToCategories.clear();
        games[gameId].answers.clear();

        games[gameId].gameStateText = "RUNNING";

        broadcastGamesToLobby();
        broadcastPlayAgain(gameId);


    } else if (clientMessage.substr(0, 2) == "OK") {
        broadcastGamesToLobby();


    } else if (clientMessage.substr(0, 10) == "LEAVE_GAME") {
        playerLeftGame(clientSocket);


    } else if (clientMessage.substr(0, 5) == "VOTE:") {
        int gameId = players[clientSocket].gameId;

        if (clientMessage.at(5) == '0') {
            games[gameId].votes[0] += 1;
        } else {
            games[gameId].votes[1] += 1;
        }
        games[gameId].cv.notify_all();   

    } else if (clientMessage.substr(0, 10) == "GET_SCORES") {
        int gameId = players[clientSocket].gameId;
        sendScores(gameId);
        games[gameId].gameStateText = "WAITING";
        broadcastGamesToLobby();
        games[gameId].cv.notify_all(); 


    } else if (clientMessage == "GET_LOBBY") {
        broadcastGamesToLobby();
        broadcastPlayersToLobby();


    } else if (clientMessage == "PREPARE_AGAIN") {
        std::string prepareMsg = "PREPARE_START\n";
        send(clientSocket, prepareMsg.c_str(), prepareMsg.size(), 0);
    }

}


void handleClient(int clientSocket) {

    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, BUFFER_SIZE);

    int bytesReceived = read(clientSocket, buffer, BUFFER_SIZE);

    if (bytesReceived < 0) {
        std::string error = "Error while receiving message from " + players[clientSocket].name + " (" + std::to_string(clientSocket) + ")";
        perror(error.c_str());
        return;
    }

    else if (bytesReceived == 0) {
        disconnectClient(clientSocket);
    }

    else if (bytesReceived > 0) {

        std::string clientMessage = std::string(buffer);
        std::cerr << "[MSG_WHOLE] " << players[clientSocket].name << ": #" << clientMessage << "#";

        std::istringstream stream(clientMessage);
        std::string msg;
        std::vector<std::string> messages;

        while (std::getline(stream, msg)) {
            std::cerr << "[MSG] " << players[clientSocket].name << ": #" << msg << "# \n\n";
            clientMsgActions(clientSocket, msg);
        }
    }
}


void serverLoop(int serverFd) {

    pollfds.push_back({
        serverFd,
        POLLIN | POLLOUT,
        0
    });

    while (serverRunning) {

        if (poll(pollfds.data(), pollfds.size(), -1) < 0) {
            perror("Poll failed");
            break;
        }

        for (size_t i = 0; i < pollfds.size(); ++i) {

            if (pollfds[i].revents & POLLIN ) {
                if (pollfds[i].fd == serverFd) {
                    newConnection(serverFd, pollfds);            
                } else {
                    handleClient(pollfds[i].fd);
                }
            }
        }

        for (const auto& pair : games) {
            int playersInGame = pair.second.players.size();
            int maxPlayersInGame = pair.second.maxPlayers;

            if (playersInGame == maxPlayersInGame && !pair.second.gameState) {
                std::cerr << "# STARTING THE GAME " << pair.second.name << "... #\n";
                startGame(pair.first);
            }
        }
    }
}

void ctrl_c(int){

    for (auto& [id, game] : games) {
        game.timerRunning = false;
        game.gameState = false;
    }

     for (auto& pfd : pollfds) {
        if (pfd.fd >= 0) {
            close(pfd.fd);
        }
     }
    pollfds.clear();

    serverRunning = false;

}


int main() {
    signal(SIGINT, ctrl_c);
    setvbuf(stdout, NULL, _IONBF, 0);

    loadCSV(answersFileName, serverAnswers);
    //showDatabase();

    int serverFd = startServer();
    sleep(1);

    std::cerr << "### Server has started ###\n\n";

    serverLoop(serverFd);
    
    std::cerr << "### Closing the server ###\n";

    close(serverFd);
    return 0;
}
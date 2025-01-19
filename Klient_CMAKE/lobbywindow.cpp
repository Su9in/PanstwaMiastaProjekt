#include "lobbywindow.h"
#include "roomwindow.h"
#include "createroomwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>

#include <QDebug>
#include <QDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QCloseEvent>
#include <QMessageBox>

LobbyWindow::LobbyWindow(QTcpSocket *socket, bool alreadyIn, QWidget *parent)
    : QWidget(parent), socket(socket), alreadyIn(alreadyIn)
{
    QRegularExpression regex("^[A-Za-zĄĆĘŁŃÓŚŹŻąćęłńóśźż0-9 ]*$");

    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex, this);

    setWindowTitle("Lobby");
    resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(this);

    lobbyLabel = new QLabel("Witaj w lobby!", this);
    layout->addWidget(lobbyLabel, 0, Qt::AlignCenter);

    // middle layouts
    listsLayoutWidget = new QWidget(this);
    QHBoxLayout *listsLayout = new QHBoxLayout(listsLayoutWidget);

    nickLayoutWidget = new QWidget(this);
    QVBoxLayout *nickLayout = new QVBoxLayout(nickLayoutWidget);

    // players and rooms layout box / nick box

    // nick
    nickLineEdit = new QLineEdit(this);
    nickLineEdit->setPlaceholderText("Wpisz swój nick");
    nickLineEdit->setFixedWidth(200);
    nickLineEdit->setValidator(validator);
    sendNickButton = new QPushButton("Wyślij Nick", this);
    sendNickButton->setFixedWidth(200);


    nickLayout->addWidget(nickLineEdit, 0, Qt::AlignCenter);
    nickLayout->addWidget(sendNickButton, 0, Qt::AlignCenter);


    // players
    QVBoxLayout *playersLayout = new QVBoxLayout();

    QLabel *playersListLabel = new QLabel("Gracze w lobby:", this);
    playersLayout->addWidget(playersListLabel, 0, Qt::AlignCenter);

    playerListWidget = new QListWidget(this);
    playerListWidget->setFixedWidth(120);
    playersLayout->addWidget(playerListWidget);


    // rooms
    QVBoxLayout *roomsLayout = new QVBoxLayout();

    QLabel *roomsListLabel = new QLabel("Dostepne pokoje:", this);
    roomsLayout->addWidget(roomsListLabel, 0, Qt::AlignCenter);

    roomListWidget = new QListWidget(this);
    roomsLayout->addWidget(roomListWidget);


    createRoomButton = new QPushButton("Stwórz pokój", this);
    roomsLayout->addWidget(createRoomButton);


    joinRoomButton = new QPushButton("Dołącz do pokoju", this);
    roomsLayout->addWidget(joinRoomButton);



    listsLayout->addLayout(playersLayout);
    listsLayout->addLayout(roomsLayout);

    layout->addWidget(listsLayoutWidget);
    layout->addWidget(nickLayoutWidget);

    listsLayoutWidget->hide();
    nickLayoutWidget->show();

    statusLabel = new QLabel("Status: Czekam na wprowadzenie nicku...", this);
    layout->addWidget(statusLabel, 0, Qt::AlignCenter);


    connect(socket, &QTcpSocket::readyRead, this, &LobbyWindow::onServerMessage);
    connect(socket, &QTcpSocket::disconnected, this, &LobbyWindow::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &LobbyWindow::onError);

    connect(sendNickButton, &QPushButton::clicked, this, &LobbyWindow::onSendNickClicked);
    connect(createRoomButton, &QPushButton::clicked, this, &LobbyWindow::onCreateRoomClicked);
    connect(joinRoomButton, &QPushButton::clicked, this, &LobbyWindow::onJoinRoomClicked);

    if(alreadyIn) {
        onNickSent();
    }

}

void LobbyWindow::closeEvent(QCloseEvent *event) {

    bool exitBool = true;

    if (nickLineEdit->isVisible()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Potwierdzenie", "Czy na pewno chcesz wyjsc?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            qDebug() << "Użytkownik wybrał TAK.";
            std::exit(0);
        } else {
            event->ignore();
            exitBool = false;
        }
    }
    if (exitBool){
        disconnect(socket, &QTcpSocket::readyRead, this, &LobbyWindow::onServerMessage);
        disconnect(socket, &QTcpSocket::disconnected, this, &LobbyWindow::onDisconnected);
        disconnect(socket, &QTcpSocket::errorOccurred, this, &LobbyWindow::onError);


        QPoint currentPos = this->pos();

        if (parentWidget()) {
            parentWidget()->move(currentPos);
            parentWidget()->show();
        }

        event->accept();
    }
}

void LobbyWindow::onDisconnected()
{
    statusLabel->setText("Status: Połączenie z serwerem zostało zerwane.");
    QMessageBox::information(this, "[LOBBY]Brak polaczenia", "Połączenie z serwerem zostało zakończone.");
    qDebug() << "Połączenie z serwerem zerwane.\n";

    this->close();
}



void LobbyWindow::onSendNickClicked()
{
    QString inputNick = nickLineEdit->text();
    if (inputNick.isEmpty()) {
        statusLabel->setText("Status: Nick nie może być pusty.");
        return;
    }

    nick = inputNick;

    inputNick = "NICK:" + inputNick + "\n";

    // Wysyłanie nicku do serwera
    socket->flush();
    socket->write(inputNick.toUtf8());
    statusLabel->setText("Status: Nick wysłany do serwera.");
    qDebug() << "\t<<<<< [LOBBY] Sent NICK";
}

void LobbyWindow::onNickSent()
{
    statusLabel->setText("Status: Nick wyslany");
    lobbyLabel->setText("Witaj w lobby " + nick + "!");

    nickLayoutWidget->hide();
    listsLayoutWidget->show();

    QString getLobby = "GET_LOBBY\n";
    socket->flush();
    socket->write(getLobby.toUtf8());
}

void LobbyWindow::onServerMessage()
{
    QByteArray response = socket->readAll();

    qDebug() << "\t>>>>> [LOBBY]: " << response;

    QStringList messages = QString(response).split('\n', Qt::SkipEmptyParts);


    for (const QString &msg : messages) {
        if (msg.startsWith("LOBBY:")) {
            qDebug() << "\t>>>>> [LOBBY]: " << response;
            //qDebug() << "lobby: " << msg;
            QString playerList = msg;
            playerList.remove(0, 6);
            onUpdatePlayerList(playerList);
        } else if (msg.startsWith("ROOMS:")) {
            qDebug() << "\t>>>>> [LOBBY]: " << response;
            QString roomsList = msg;
            roomsList.remove(0, 6);
            onUpdateRoomList(roomsList);
        } else if (msg.startsWith("JOINED:")) {
            qDebug() << "\t>>>>> [LOBBY]: " << response;
            disconnect(socket, &QTcpSocket::readyRead, this, &LobbyWindow::onServerMessage);
            disconnect(socket, &QTcpSocket::disconnected, this, &LobbyWindow::onDisconnected);
            disconnect(socket, &QTcpSocket::errorOccurred, this, &LobbyWindow::onError);

            QString roomName = msg;

            roomName.remove(0, 7);

            QString joining_room = "JOINED_GAME\n";

            // Wysyłanie nicku do serwera
            socket->flush();
            socket->write(joining_room.toUtf8());
            qDebug() << "\t<<<<< [LOBBY] Sent JOINED_GAME";
            QPoint currentPos = this->pos();
            RoomWindow *room = new RoomWindow(socket, roomName, nick, this);
            room->setWindowFlags(Qt::Window);

            room->setAttribute(Qt::WA_DeleteOnClose);

                // Connect to the destroy signal of the Create Room window
            connect(room, &RoomWindow::destroyed, this, [this]() {
                connect(socket, &QTcpSocket::readyRead, this, &LobbyWindow::onServerMessage);
                connect(socket, &QTcpSocket::disconnected, this, &LobbyWindow::onDisconnected);
                connect(socket, &QTcpSocket::errorOccurred, this, &LobbyWindow::onError);
                qDebug() << "zamknal sie lobby";

                if ((socket->state() == QAbstractSocket::ConnectedState)) {
                    qDebug() << "Socket is open and ready for communication.";
                } else {
                    qDebug() << "Socket is not open or not connected!";
                    this->close();
                }
            });

            room->move(currentPos);
            room->show();
            this->hide();

        } else if (msg.startsWith("NICK:")){
            qDebug() << "\t>>>>> [LOBBY]: " << response;

            if (msg.startsWith("NICK:OK")) {
                onNickSent();
            } else if (msg.startsWith("NICK:TAKEN")) {
                statusLabel->setText("Nick jest ZAJETY");
            }

        } else if (msg.startsWith("CHECK")) {
            QString check = "CHECK\n";
            socket->flush();
            socket->write(check.toUtf8());

        } else {
            qDebug() << "\t>>>>> [LOBBY OTHER]:  " << msg;
            statusLabel->setText("Otrzymano odpowiedź: " + msg);
        }
    }
}






void LobbyWindow::onUpdatePlayerList(const QString &playerList)
{

    playerListWidget->clear();
    QStringList players = playerList.split(",", Qt::SkipEmptyParts);

    qDebug() << "players:" << players;
    playerListWidget->addItems(players);
    playerListWidget->show();

    statusLabel->setText("Lista graczy zaktualizowana.");
}


void LobbyWindow::onUpdateRoomList(const QString &roomList)
{

    roomListWidget->clear();
    QStringList rooms = roomList.split(",", Qt::SkipEmptyParts);

    QStringList roomsSeparated;

    for (const QString &room : rooms) {
        QStringList params = room.split(";", Qt::SkipEmptyParts);
        QString roomId = params[0];
        QString roomName = params[1];
        QString roomMax = params[2];
        QString roomPlayers = params[3];
        QString roomState = params[4];

        QString wholeRoom = roomName + " [#" + roomId + "]" + "\t" + roomPlayers + "/" + roomMax + "\t" + roomState;
        roomsSeparated.append(wholeRoom);
    }


    roomListWidget->addItems(roomsSeparated);

    statusLabel->setText("Lista pokoi zaktualizowana.");
}


void LobbyWindow::onCreateRoomClicked()
{
//    QString createRoomCommand = "CREATE_ROOM\n";
//    socket->write(createRoomCommand.toUtf8());
//    CreateRoomWindow *createRoom = new CreateRoomWindow(socket);
//    createRoom->show();

    disconnect(socket, &QTcpSocket::readyRead, this, &LobbyWindow::onServerMessage);
    disconnect(socket, &QTcpSocket::disconnected, this, &LobbyWindow::onDisconnected);
    disconnect(socket, &QTcpSocket::errorOccurred, this, &LobbyWindow::onError);

    QPoint currentPos = this->pos();
    CreateRoomWindow *createRoomWindow = new CreateRoomWindow(socket, this);

        // Disable the lobby window
    createRoomWindow->setWindowFlags(Qt::Window);
    createRoomWindow->setAttribute(Qt::WA_DeleteOnClose);
    this->setEnabled(false);
    createRoomWindow->setEnabled(true);


    connect(createRoomWindow, &CreateRoomWindow::destroyed, this, [this]() {
        connect(socket, &QTcpSocket::readyRead, this, &LobbyWindow::onServerMessage);
        connect(socket, &QTcpSocket::disconnected, this, &LobbyWindow::onDisconnected);
        connect(socket, &QTcpSocket::errorOccurred, this, &LobbyWindow::onError);

        if ((socket->state() == QAbstractSocket::ConnectedState)) {
            qDebug() << "Socket is open and ready for communication.";
        } else {
            qDebug() << "Socket is not open or not connected!";
            this->close();
        }

        QString okCommand = "OK";

        okCommand += "\n";
        socket->flush();
        socket->write(okCommand.toUtf8());
        qDebug() << "\t<<<<< [LOBBY] Sent OK";

        this->setEnabled(true);
    });

    createRoomWindow->move(currentPos);
    createRoomWindow->show();

    statusLabel->setText("Wysłano żądanie stworzenia pokoju.");
}


void LobbyWindow::onJoinRoomClicked()
{
    QListWidgetItem *selectedRoom = roomListWidget->currentItem();
    if (!selectedRoom) {
        statusLabel->setText("Nie wybrano pokoju.");
        return;
    }


    QString selectedRoomText = selectedRoom->text();

    qDebug() << "\t<<<<< [LOBBY] Sent ROOMSTR " << selectedRoomText;
    QStringList selectedRoomTextList = selectedRoomText.split(QRegularExpression("[\t]"));
    QString roomPlayersText = selectedRoomTextList[1];
    qDebug() << "\t<<<<< [LOBBY] Sent teścig";

    QString selectedRoomName = selectedRoomTextList[0].split("[")[0];
    QStringList roomPlayersTextList = roomPlayersText.split("/");
    int actPlayers = roomPlayersTextList[0].toInt();
    int maxPlayers = roomPlayersTextList[1].toInt();

    QString actRoomState = selectedRoomTextList[2];

    if(actPlayers == maxPlayers){
        QMessageBox::information(this,"Nie można dołączyć","Liczba graczy w pokoju: " + selectedRoomName + "osiągneła limit");
    } else if (actRoomState == "RUNNING"){
        QMessageBox::information(this,"Nie można dołączyć","Gra trwa! Dołącz gdy pokój będzie w stanie WAITING lub EMPTY");
    }
    else {
        QRegularExpression regex(R"#(\[#(\d+)\])#");
        QRegularExpressionMatch match = regex.match(selectedRoomText);
        QString nr = match.captured(1);

        QString joinRoomCommand = "JOIN_ROOM:" + nr + "\n";
        socket->flush();
        socket->write(joinRoomCommand.toUtf8());
        statusLabel->setText("Wysłano żądanie dołączenia do pokoju: " + selectedRoom->text());
        qDebug() << "\t<<<<< [LOBBY] Sent JOIN_ROOM";
    }
}


void LobbyWindow::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QString error = "Błąd: " + socket->errorString();
    statusLabel->setText("Status: " + error);
    QMessageBox::critical(this, "Błąd sieci", error);
}

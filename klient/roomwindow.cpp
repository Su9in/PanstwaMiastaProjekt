#include "roomwindow.h"
#include "summarywindow.h"
#include "lobbywindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLineEdit>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>
#include <QCloseEvent>


RoomWindow::RoomWindow(QTcpSocket *socket, const QString &roomName, const QString &nick, QWidget *parent)
    : QWidget(parent), socket(socket), roomName(roomName), nick(nick)
{
    setWindowTitle("Pokoj: " + roomName + " (" + nick + ") ");
    resize(800, 600);
\
    QRegularExpression regex("^[A-Za-zĄĆĘŁŃÓŚŹŻąćęłńóśźż ]*$");

    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex, this);

    // main horizontal layout
    QHBoxLayout *mainLayout = new QHBoxLayout(this);


    // left-side (players) -------------------------------------------
    playersLayoutWidget = new QWidget(this);
    playersLayoutWidget->setFixedWidth(150);
    QVBoxLayout *playersLayout = new QVBoxLayout(playersLayoutWidget);


    playersLabel = new QLabel("Punktacja graczy", this);
    playersListWidget = new QListWidget(this);

    playersLayout->addWidget(playersLabel);
    playersLayout->addWidget(playersListWidget);

    returnToLobbyButton = new QPushButton("Wróć do lobby", this);
    returnToLobbyButton->setFixedWidth(120);
    playersLayout->addWidget(returnToLobbyButton);



    // right-side (game) ---------------------------------------
    gameLayoutWidget = new QWidget(this);
    QVBoxLayout *gameLayout = new QVBoxLayout(gameLayoutWidget);

    titleLabel = new QLabel("Witaj w pokoju " + roomName + "!", this);
    gameLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    timeLabel = new QLabel("Czas");
    timeLabel->hide();
    gameLayout->addWidget(timeLabel, 0, Qt::AlignCenter);

    infoLabel = new QLabel("Gra rozpocznie sie po osiagnieciu maksymalnej liczby graczy\nlub po wystartowaniu gry przez mistrza gry");
    gameLayout->addWidget(infoLabel, 0, Qt::AlignCenter);

    letterLabel = new QLabel("Wylosowano litere ", this);
    letterLabel->hide();
    gameLayout->addWidget(letterLabel, 0, Qt::AlignCenter);


    // game board

    gameBoard = new QWidget(this);
    gameBoard->setFixedHeight(80);
    QHBoxLayout *gameBoardLayout = new QHBoxLayout(gameBoard);

    QVBoxLayout *countryRow = new QVBoxLayout();
    QLabel *countryLabel = new QLabel("Państwo:", this);
    countryEdit = new QLineEdit(this);
    countryEdit->setValidator(validator);

    countryRow->addWidget(countryLabel);
    countryRow->addWidget(countryEdit);
    gameBoardLayout->addLayout(countryRow);

    QVBoxLayout *cityRow = new QVBoxLayout();
    QLabel *cityLabel = new QLabel("Miasto:", this);
    cityEdit = new QLineEdit(this);
    cityEdit->setValidator(validator);
    cityRow->addWidget(cityLabel);
    cityRow->addWidget(cityEdit);
    gameBoardLayout->addLayout(cityRow);

    QVBoxLayout *objectRow = new QVBoxLayout();
    QLabel *objectLabel = new QLabel("Rzecz:", this);
    objectEdit = new QLineEdit(this);
    objectEdit->setValidator(validator);
    objectRow->addWidget(objectLabel);
    objectRow->addWidget(objectEdit);
    gameBoardLayout->addLayout(objectRow);

    QVBoxLayout *plantRow = new QVBoxLayout();
    QLabel *plantLabel = new QLabel("Roślina:", this);
    plantEdit = new QLineEdit(this);
    plantEdit->setValidator(validator);
    plantRow->addWidget(plantLabel);
    plantRow->addWidget(plantEdit);
    gameBoardLayout->addLayout(plantRow);

    QVBoxLayout *animalRow = new QVBoxLayout();
    QLabel *animalLabel = new QLabel("Zwierzę:", this);
    animalEdit = new QLineEdit(this);
    animalEdit->setValidator(validator);
    animalRow->addWidget(animalLabel);
    animalRow->addWidget(animalEdit);
    gameBoardLayout->addLayout(animalRow);

    gameBoard->hide();
    gameLayout->addWidget(gameBoard, 0, Qt::AlignCenter);

    // buttons


    buttonsLayoutWidget = new QWidget(this);
    QVBoxLayout *buttonsLayout = new QVBoxLayout(buttonsLayoutWidget);

    sendAnswersButton = new QPushButton("Wyślij odpowiedzi", this);
    sendAnswersButton->hide();
    buttonsLayout->addWidget(sendAnswersButton);

    startGameButton = new QPushButton("Rozpocznij gre", this);
    startGameButton->setFixedWidth(100);
    startGameButton->hide();
    buttonsLayout->addWidget(startGameButton);

    playAgainButton = new QPushButton("Graj ponownie", this);
    playAgainButton->setFixedWidth(100);
    playAgainButton->hide();
    buttonsLayout->addWidget(playAgainButton);


    gameLayout->addWidget(buttonsLayoutWidget, 0, Qt::AlignCenter);


    mainLayout->addWidget(playersLayoutWidget);
    mainLayout->addWidget(gameLayoutWidget);

    setLayout(mainLayout);


    isFirst = "false";

    connect(sendAnswersButton, &QPushButton::clicked, this, &RoomWindow::onSendAnswersClicked);
    connect(startGameButton, &QPushButton::clicked, this, &RoomWindow::onStartGameClicked);
    connect(playAgainButton, &QPushButton::clicked, this, &RoomWindow::onPlayAgainClicked);
    connect(returnToLobbyButton, &QPushButton::clicked, this, &RoomWindow::onReturnToLobbyClicked);


    connect(socket, &QTcpSocket::readyRead, this, &RoomWindow::onServerMessage);
    connect(socket, &QTcpSocket::disconnected, this, &RoomWindow::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &RoomWindow::onError);

    timeLabel->show();
}



void RoomWindow::closeEvent(QCloseEvent *event) {



    if ((socket->state() == QAbstractSocket::ConnectedState)) {
        QString leavingRoom = "LEAVE_GAME\n";

        // Wysyłanie nicku do serwera
        socket->flush();
        socket->write(leavingRoom.toUtf8());
        qDebug() << "\t<<<<< [ROOM] Sent LEAVE_GAME";
    }

    disconnect(socket, &QTcpSocket::readyRead, this, &RoomWindow::onServerMessage);
    disconnect(socket, &QTcpSocket::disconnected, this, &RoomWindow::onDisconnected);
    disconnect(socket, &QTcpSocket::errorOccurred, this, &RoomWindow::onError);


    if (parentWidget()) {
            parentWidget()->show();
    }


    event->accept();
}

void RoomWindow::onDisconnected()
{
    //statusLabel->setText("Status: Połączenie z serwerem zostało zerwane.");
    QMessageBox::information(this, "[ROOM]Brak polaczenia", "Połączenie z serwerem zostało zakończone.");
    qDebug() << "Połączenie z serwerem zerwane.\n";



    this->close();

}

void RoomWindow::onStartGameClicked() {
    QString startGame = "START_GAME";
    socket->write((startGame + "\n").toUtf8());
    qDebug() << "\t<<<<< [ROOM] Sent START_GAME";
    timeLabel->show();
    infoLabel->hide();
    titleLabel->show();

    unlockFields();


}

void RoomWindow::onPrepareGame() {
    prepare = "true";
    unlockFields();


    titleLabel->setText("Wystartowano gre!\n Gra rozpocznie sie za:");
    titleLabel->show();

    if (isFirst == "true") {
        letterLabel->hide();
        startGameButton->hide();
        playAgainButton->hide();
    }
}

void RoomWindow::onPlayAgainClicked() {

    QString startGame = "PLAY_AGAIN";
    socket->write((startGame + "\n").toUtf8());
    qDebug() << "\t<<<<< [ROOM] Sent PLAY_AGAIN";
    playAgainButton->hide();
    infoLabel->hide();


    titleLabel->show();
    timeLabel->show();
    gameBoard->hide();
    playAgainButton->hide();


}

void RoomWindow::onSendAnswersClicked() {
    if (isFirst.startsWith("true")) {
        playAgainButton->show();
    }

    gameBoard->hide();
    sendAnswersButton->hide();
    titleLabel->hide();
    //timeLabel->hide();
    letterLabel->hide();

    QStringList answers;
    answers << "PANSTWO:" + countryEdit->text();
    answers << "MIASTO:" + cityEdit->text();
    answers << "RZECZ:" + objectEdit->text();
    answers << "ROSLINA:" + plantEdit->text();
    answers << "ZWIERZE:" + animalEdit->text();


    QString combinedAnswers = answers.join(",");
    socket->write(("ANSWERS:" + combinedAnswers + "\n").toUtf8());
    qDebug() << "\t<<<<< [ROOM] Sent ANSWERS";

    openSummary();

}


void RoomWindow::onFirstPlayer() {
    letterLabel->setText("Jestes mistrzem gry!\nMozesz rozpocza gre w dowolnym momencie!");
    letterLabel->show();
    startGameButton->show();

    isFirst = "true";

}


void RoomWindow::onStartGame(const QString &letter) {


    titleLabel->setText("Wypelnij pola odpowiedziami na podana litere");

    timeLabel->setText("Czas: ");
    timeLabel->show();

    infoLabel->hide();

    letterLabel->setText("Wylosowano litere: " + letter);
    letterLabel->show();

    gameBoard->show();

    startGameButton->hide();
    sendAnswersButton->show();


}

void RoomWindow::openSummary() {
    disconnect(socket, &QTcpSocket::readyRead, this, &RoomWindow::onServerMessage);
    disconnect(socket, &QTcpSocket::disconnected, this, &RoomWindow::onDisconnected);
    disconnect(socket, &QTcpSocket::errorOccurred, this, &RoomWindow::onError);

    QPoint currentPos = this->pos();
    SummaryWindow *summary = new SummaryWindow(socket, this);

    summary->setWindowFlags(Qt::Window);
    summary->setAttribute(Qt::WA_DeleteOnClose);

    connect(summary, &SummaryWindow::destroyed, this, [this]() {
        connect(socket, &QTcpSocket::readyRead, this, &RoomWindow::onServerMessage);
        connect(socket, &QTcpSocket::disconnected, this, &RoomWindow::onDisconnected);
        connect(socket, &QTcpSocket::errorOccurred, this, &RoomWindow::onError);

        if ((socket->state() == QAbstractSocket::ConnectedState)) {
            qDebug() << "Socket is open and ready for communication.";
        } else {
            qDebug() << "Socket is not open or not connected!";
            this->close();
        }


        infoLabel->setText("Czekanie na ponowne rozpoczecie gry...");
        infoLabel->show();

        QString okCommand = "GET_SCORES";

        okCommand += "\n";
        socket->flush();
        socket->write(okCommand.toUtf8());
        qDebug() << "\t<<<<< [ROOM] Sent GET_SCORES";

    });
    summary->move(currentPos);
    summary->show();
    this->hide();

}


void RoomWindow::onServerMessage()
{
    QByteArray response = socket->readAll();

    qDebug() << "\t>>>>> [ROOM]: " << response;

    QStringList messages = QString(response).split('\n', Qt::SkipEmptyParts);



    for (const QString &msg : messages) {
        if (msg.startsWith("ROOM_PLAYERS:")) {
            qDebug() << "\t>>>>> [ROOM]: " << response;
            QString playerList = msg;
            playerList.remove(0,13);


        } else if (msg.startsWith("START:")) {
            qDebug() << "\t>>>>> [ROOM]: " << response;

            QString letter = msg;
            letter.remove(0,6);
            infoLabel->hide();
            titleLabel->show();
            onStartGame(letter);


        } else if (msg.startsWith("TIME:")) {
            QString time = msg;
            timeLabel->setText("Czas: " + time.remove(0, 5));
            timeLabel->show();

            if (msg.startsWith("TIME:0")) {

                if (prepare.startsWith("true")) {
                    prepare = "false";
                } else {
                    onSendAnswersClicked();
                    timeLabel->hide();
                }


            }


        } else if (msg.startsWith("FIRST")) {
            qDebug() << "\t>>>>> [ROOM]: " << response;
            onFirstPlayer();
            timeLabel->hide();



        } else if (msg.startsWith("SCORES:")) {
            qDebug() << "\t>>>>> [ROOM]: " << response;

            QString scores = msg;
            scores.remove(0,7);

            QStringList scoresList = scores.split(",", Qt::SkipEmptyParts);

            QStringList players;

            for (const QString &scorePlayer : scoresList) {
                QStringList player = scorePlayer.split(":", Qt::SkipEmptyParts);
                QString scoreName = player[0];
                QString scorePoints = player[1];

                QString wholeScore = scoreName + "\t" + scorePoints;
                players.append(wholeScore);
            }

            onUpdatePlayerList(players);

            timeLabel->hide();




        } else if (msg.startsWith("AGAIN")) {
            qDebug() << "\t>>>>> [ROOM]: " << response;
            unlockFields();
            gameBoard->hide();
            sendAnswersButton->hide();
            timeLabel->show();
            infoLabel->hide();
            titleLabel->show();




        } else if (msg.startsWith("PREPARE_START")) {
            qDebug() << "\t>>>>> [ROOM]: " << response;
            infoLabel->hide();

            onPrepareGame();

        } else if (msg.startsWith("SHORT")) {
            titleLabel->setText("Ktos juz wypelnil plansze! Pospiesz sie!");
            titleLabel->show();


        }



        else {
            qDebug() << "\t>>>>> [ROOM OTHER]: " << msg;
        }


    }

}


void RoomWindow::onUpdatePlayerList(const QStringList &playerList)
{
    playersListWidget->clear();
    playersListWidget->addItems(playerList);
    playersListWidget->show(); // Upewnij się, że lista jest widoczna


}


void RoomWindow::onReturnToLobbyClicked()
{
    this->close();
}




void RoomWindow::lockFields()
{
    countryEdit->setEnabled(false);
    cityEdit->setEnabled(false);
    objectEdit->setEnabled(false);
    plantEdit->setEnabled(false);
    animalEdit->setEnabled(false);
    sendAnswersButton->setEnabled(false);
}

void RoomWindow::unlockFields()
{
    countryEdit->setEnabled(true);
    cityEdit->setEnabled(true);
    objectEdit->setEnabled(true);
    plantEdit->setEnabled(true);
    animalEdit->setEnabled(true);
    sendAnswersButton->setEnabled(true);

    countryEdit->setText("");
    cityEdit->setText("");
    objectEdit->setText("");
    plantEdit->setText("");
    animalEdit->setText("");
}


void RoomWindow::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QString error = "Błąd: " + socket->errorString();
    QMessageBox::critical(this, "Błąd sieci", error);
}

#include "lobbywindow.h"
#include "createroomwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QCloseEvent>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QDebug>

CreateRoomWindow::CreateRoomWindow(QTcpSocket *socket, QWidget *parent)
    : QWidget(parent), socket(socket)
{
    QRegularExpression regex("^[A-Za-zĄĆĘŁŃÓŚŹŻąćęłńóśźż0-9 ]*$");
    QRegularExpression regex2("^[0-9]*$");

    QRegularExpressionValidator *validator = new QRegularExpressionValidator(regex, this);
    QRegularExpressionValidator *validator2 = new QRegularExpressionValidator(regex2, this);

    setWindowTitle("Create room");
    resize(300, 300);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *createLabel = new QLabel("Wprowadz dane nowego pokoju", this);
    layout->addWidget(createLabel, 0, Qt::AlignCenter);

    QLabel *nameLabel = new QLabel("Nazwa pokoju:", this);
    layout->addWidget(nameLabel, 0, Qt::AlignCenter);
    gameNameLineEdit = new QLineEdit(this);
    gameNameLineEdit->setPlaceholderText("Nazwa pokoju");
    gameNameLineEdit->setFixedWidth(200);
    gameNameLineEdit->setValidator(validator);
    layout->addWidget(gameNameLineEdit, 0, Qt::AlignCenter);

    QLabel *maxLabel = new QLabel("Maksymalna liczba graczy (<= 5)", this);
    layout->addWidget(maxLabel, 0, Qt::AlignCenter);
    maxPLayersLineEdit = new QLineEdit(this);
    maxPLayersLineEdit->setPlaceholderText("Maksymalna liczba graczy");
    maxPLayersLineEdit->setFixedWidth(200);
    maxPLayersLineEdit->setValidator(validator2);
    layout->addWidget(maxPLayersLineEdit, 0, Qt::AlignCenter);


    createRoomButton = new QPushButton("Stworz pokoj", this);
    createRoomButton->setFixedWidth(200);
    layout->addWidget(createRoomButton, 0, Qt::AlignCenter);

    connect(socket, &QTcpSocket::errorOccurred, this, &CreateRoomWindow::onError);
    connect(socket, &QTcpSocket::readyRead, this, &CreateRoomWindow::onServerMessage);
    connect(socket, &QTcpSocket::disconnected, this, &CreateRoomWindow::onDisconnected);
    connect(createRoomButton, &QPushButton::clicked, this, &CreateRoomWindow::onCreateRoomClicked);
}

void CreateRoomWindow::closeEvent(QCloseEvent *event) {
    disconnect(socket, &QTcpSocket::readyRead, this, &CreateRoomWindow::onServerMessage);
    disconnect(socket, &QTcpSocket::disconnected, this, &CreateRoomWindow::onDisconnected);
    disconnect(socket, &QTcpSocket::errorOccurred, this, &CreateRoomWindow::onError);

    event->accept();
}

void CreateRoomWindow::onDisconnected()
{
    QMessageBox::information(this, "[CREATE ROOM] Brak polaczenia", "Połączenie z serwerem zostało zakończone.");
    qDebug() << "Połączenie z serwerem zerwane.\n";

    this->close();
}

void CreateRoomWindow::onServerMessage()
{
    QByteArray response = socket->readAll();

    qDebug() << "create room dostal: " << response;

    QStringList messages = QString(response).split('\n', Qt::SkipEmptyParts);

    for (const QString &msg : messages) {
        if (msg.startsWith("ROOM_CREATED")) {
            disconnect(socket, &QTcpSocket::readyRead, this, &CreateRoomWindow::onServerMessage);
            this->close();

        } else {
            qDebug() << "create room otrzymal inne dane: " << msg;
        }
    }
}

void CreateRoomWindow::onCreateRoomClicked()
{
    QString createRoomCommand = "CREATE_ROOM:";
    QString roomName = gameNameLineEdit->text();
    QString maxPlayers = maxPLayersLineEdit->text();
    if (maxPlayers.toInt()>5 || maxPlayers.toInt()<1) {
        QMessageBox::information(this,"Nie mozna stworzyc pokoju!","Maksymalna liczba graczy w pokoju musi byc w przedziale od 1 do 5!");
        return;
    }

    createRoomCommand += roomName + "," + maxPlayers + "\n";
    socket->flush();
    socket->write(createRoomCommand.toUtf8());
}

void CreateRoomWindow::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QMessageBox::critical(this, "Rozłączono", "Połączenie z serwerem zostało zerwane.");
    this->close();
}

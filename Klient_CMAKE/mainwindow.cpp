#include "mainwindow.h"
#include "lobbywindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), socket(new QTcpSocket(this))
{
    setWindowTitle("Panstwa Miasta!");
    resize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QLabel *welcomeLabel = new QLabel("Witaj w grze Panstwa Miasta!");
    layout->addWidget(welcomeLabel, 0, Qt::AlignCenter);

    QLabel *chooseLabel = new QLabel("Wpisz dane serwera lub polacz sie z domyslnymi.");
    layout->addWidget(chooseLabel, 0, Qt::AlignCenter);


    QSettings settings("../../config.ini", QSettings::IniFormat);
    QString defaultIp = settings.value("server/params/ip").toString();
    int defaultPort = settings.value("server/params/port").toInt();


    QVBoxLayout *inputLayout = new QVBoxLayout();

    QLabel *ipLabel = new QLabel("Adres IP serwera:", this);
    ipLineEdit = new QLineEdit(this);
    ipLineEdit->setText(defaultIp);
    ipLineEdit->setMaxLength(80);

    QLabel *portLabel = new QLabel("Port serwera:", this);
    portLineEdit = new QLineEdit(this);
    portLineEdit->setText(QString::number(defaultPort));
    portLineEdit->setMaxLength(80);

    connectButton = new QPushButton("Połącz", this);

    lobbyButton = new QPushButton("Lobby", this);

    inputLayout->addWidget(ipLabel, 0, Qt::AlignCenter);
    inputLayout->addWidget(ipLineEdit, 0, Qt::AlignCenter);
    inputLayout->addWidget(portLabel, 0, Qt::AlignCenter);
    inputLayout->addWidget(portLineEdit, 0, Qt::AlignCenter);
    inputLayout->addWidget(connectButton, 0, Qt::AlignCenter);

    inputLayout->addWidget(lobbyButton, 0, Qt::AlignCenter);
    lobbyButton->hide();

    layout->addLayout(inputLayout);



    statusLabel = new QLabel("Status: Brak połączenia", this);
    layout->addWidget(statusLabel, 0, Qt::AlignCenter);


    connect(socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);

    connect(connectButton, &QPushButton::clicked, this, &MainWindow::connectToServer);
    connect(lobbyButton, &QPushButton::clicked, this, &MainWindow::onLobbyClicked);

    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onServerMessage);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::onError);
}

MainWindow::~MainWindow()
{
    if (socket->isOpen()) {
        socket->disconnectFromHost();
    }
}

// connect to server with given parameters
void MainWindow::connectToServer()
{
    QString serverAddress = ipLineEdit->text();
    quint16 port = portLineEdit->text().toUInt();

    if (socket->state() != QTcpSocket::UnconnectedState) {
        socket->abort();
    }

    socket->connectToHost(serverAddress, port);
    statusLabel->setText("Status: Łączenie z serwerem...");
}

void MainWindow::openLobby(bool alreadyIn) {
    disconnect(socket, &QTcpSocket::readyRead, this, &MainWindow::onServerMessage);
    disconnect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
    disconnect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::onError);

    QPoint currentPos = this->pos();
    LobbyWindow *lobby = new LobbyWindow(socket, alreadyIn, this);
    lobby->setWindowFlags(Qt::Window);
    lobby->setAttribute(Qt::WA_DeleteOnClose);

    connect(lobby, &LobbyWindow::destroyed, this, [this]() {
        connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onServerMessage);
        connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onDisconnected);
        connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::onError);



        if ((socket->state() == QAbstractSocket::ConnectedState)) {
            qDebug() << "Socket is open and ready for communication.";
            ipLineEdit->setEnabled(false);
            portLineEdit->setEnabled(false);
            connectButton->setEnabled(false);
            lobbyButton->show();
        } else {
            qDebug() << "Socket is not open or not connected!";
            statusLabel->setText("Status: Brak połączenia");
            ipLineEdit->setEnabled(true);
            portLineEdit->setEnabled(true);
            connectButton->setEnabled(true);
            lobbyButton->hide();
        }


        qDebug() << "\t<<<<< wrocil do main";


    });

    lobby->move(currentPos);
    lobby->show();
    this->hide();
}

void MainWindow::onLobbyClicked() {

    openLobby(true);

}


// when connection is successfully established, create lobby window
void MainWindow::onConnected()
{
    statusLabel->setText("Status: Połączono z serwerem!");
    qDebug() << "Połączono z serwerem: " << socket->peerAddress().toString() << "\n";

}


void MainWindow::onDisconnected()
{
    statusLabel->setText("Status: Połączenie z serwerem zostało zerwane.");
    QMessageBox::information(this, "[MAIN]Brak polaczenia", "Połączenie z serwerem zostało zakończone.");
    qDebug() << "Połączenie z serwerem zerwane.\n";

    ipLineEdit->setEnabled(true);
    portLineEdit->setEnabled(true);
    connectButton->setEnabled(true);
    lobbyButton->hide();
}


// message from server recieved
void MainWindow::onServerMessage() {
    QByteArray response = socket->readAll();

    qDebug() << "\t>>>>> [MAIN]: " << response;

    QStringList messages = QString(response).split('\n', Qt::SkipEmptyParts);

    for (const QString &msg : messages) {
        if (msg.startsWith("HELLO")) {
            sendHello();

        } else if (msg.startsWith("CHECK")) {
            QString check = "CHECK\n";
            socket->flush();
            socket->write(check.toUtf8());

        }
        else {
            qDebug() << "\t>>>>> [MAIN OTHER]: " << msg;
        }
    }
}

void::MainWindow::sendHello() {
    disconnect(socket, &QTcpSocket::readyRead, this, &MainWindow::onServerMessage);

    QString hello = "HELLO\n";
    socket->flush();
    socket->write(hello.toUtf8());

    qDebug() << "\t\t<<<<< [MAIN] Sent HELLO";

    openLobby(false);
}

// show error if occurs
void MainWindow::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    QString error = "Błąd: " + socket->errorString();
    statusLabel->setText("Status: " + error);
    QMessageBox::critical(this, "Błąd sieci", error);
    qDebug() << "Błąd socketu:" << socket->errorString();
}

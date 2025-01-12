#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void connectToServer();
    void onConnected();
    void onDisconnected();
    void sendHello();
    void onServerMessage();
    void onLobbyClicked();
    void openLobby(bool alreadyIn);
    void onError(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
    QLineEdit *ipLineEdit;
    QLineEdit *portLineEdit;
    QPushButton *connectButton;
    QPushButton *lobbyButton;
    QLabel *statusLabel;
};

#endif // MAINWINDOW_H

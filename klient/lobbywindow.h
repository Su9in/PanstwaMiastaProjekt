#ifndef LOBBYWINDOW_H
#define LOBBYWINDOW_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include <QListWidget>
#include <QCloseEvent>

class LobbyWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LobbyWindow(QTcpSocket *socket, bool alreadyIn, QWidget *parent = nullptr);

private slots:
    void onSendNickClicked();
    void onNickSent();

    void onServerMessage();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);

    void onUpdatePlayerList(const QString &playerList);
    void onUpdateRoomList(const QString &roomList);

    void onCreateRoomClicked();
    void onJoinRoomClicked();
    void closeEvent(QCloseEvent *event);



private:
    QTcpSocket *socket;
    bool alreadyIn = false;

    QLabel *lobbyLabel;

    QWidget *listsLayoutWidget;
    QWidget *nickLayoutWidget;

    // sending nick
    QLineEdit *nickLineEdit;
    QPushButton *sendNickButton;
    QLabel *statusLabel;

    QString nick;

    // lobby
    QListWidget *playerListWidget;
    QListWidget *roomListWidget;


    QPushButton *createRoomButton;
    QPushButton *joinRoomButton;
};

#endif // LOBBYWINDOW_H

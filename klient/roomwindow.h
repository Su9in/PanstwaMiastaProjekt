#ifndef ROOMWINDOW_H
#define ROOMWINDOW_H

#include <QWidget>
#include <QTcpSocket>
#include <QListWidget>

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QMessageBox>

class RoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RoomWindow(QTcpSocket *socket, const QString &roomName, const QString &nick, QWidget *parent = nullptr);
//    ~RoomWindow();

private slots:

    void onError(QAbstractSocket::SocketError socketError);
    void onServerMessage();
    void onDisconnected();
    void closeEvent(QCloseEvent *event);

    void onUpdatePlayerList(const QStringList &playerList);


    void onStartGameClicked();
    void onPrepareGame();

    void onSendAnswersClicked();

    void onPlayAgainClicked();

    void onFirstPlayer();
    void onStartGame(const QString &letter);

    void lockFields();
    void unlockFields();

    void openSummary();

    void onReturnToLobbyClicked();

private:
    QTcpSocket *socket;

    QWidget *playersLayoutWidget;
    QWidget *gameLayoutWidget;
    QWidget *buttonsLayoutWidget;
    QWidget *gameBoard;

    QString roomName;                   // game room name
    QString nick;                       // nick of player
    QString isFirst;
    QString prepare;

    // players list
    QListWidget *playersListWidget;     // list of players in the room
    QPushButton *returnToLobbyButton;   // return to lobby button



    QLabel *titleLabel;
    QLabel *playersLabel;

    QLabel *timeLabel;
    QLabel *letterLabel;
    QLabel *infoLabel;



    // game fields
    QLineEdit *countryEdit;
    QLineEdit *cityEdit;
    QLineEdit *objectEdit;
    QLineEdit *plantEdit;
    QLineEdit *animalEdit;

    // bottom buttons
    QPushButton *sendAnswersButton;
    QPushButton *startGameButton;
    QPushButton *playAgainButton;

};



#endif // ROOMWINDOW_H

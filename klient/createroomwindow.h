#ifndef CREATEROOMWINDOW_H
#define CREATEROOMWINDOW_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QCloseEvent>
#include <QListWidget>

class CreateRoomWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CreateRoomWindow(QTcpSocket *socket, QWidget *parent = nullptr);

private slots:
    void onError(QAbstractSocket::SocketError socketError);
    void onServerMessage();
    void onDisconnected();
    void onCreateRoomClicked();
    void closeEvent(QCloseEvent *event);

private:
    QTcpSocket *socket;
    QLineEdit *gameNameLineEdit;
    QLineEdit *maxPLayersLineEdit;
    QPushButton *createRoomButton;
};

#endif // CREATEROOMWINDOW_H

#ifndef SUMMARYWINDOW_H
#define SUMMARYWINDOW_H

#include <QWidget>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

#include <QListWidget>
#include <QCloseEvent>



class SummaryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SummaryWindow(QTcpSocket *socket, QWidget *parent = nullptr);

private slots:

    void onError(QAbstractSocket::SocketError socketError);
    void onServerMessage();
    void onDisconnected();

    void onGoodClicked();
    void onBadClicked();
    void onReturnToGameClicked();
    void closeEvent(QCloseEvent *event);



private:
    QTcpSocket *socket;

    QString voted;

    QVBoxLayout *resultsWidgetLayout;


    QLabel *titleLabel;
    QLabel *infoLabel;
    QLabel *timeLabel;

    QLabel *playerLabel;
    QLabel *categoryLabel;
    QLabel *answerLabel;

    QWidget *votingWidget;
    QWidget *resultsWidget;

    QPushButton *goodButton;
    QPushButton *badButton;
    QPushButton *returnToGame;

};



#endif // SUMMARYWINDOW_H

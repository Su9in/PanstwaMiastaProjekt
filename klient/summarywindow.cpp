#include "roomwindow.h"
#include "summarywindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

QWidget* createDynamicWidget(const QString& playerName, const QString& country, const QString& countryPoints,
                             const QString& city, const QString& cityPoints,
                             const QString& object, const QString& objectPoints,
                             const QString& plant, const QString& plantPoints,
                             const QString& animal, const QString& animalPoints) {
    QWidget* widget = new QWidget;


    QVBoxLayout* layout = new QVBoxLayout(widget);


    QLabel* playerNameLabel = new QLabel(playerName);
    layout->addWidget(playerNameLabel);

    QWidget* widgetScores = new QWidget;


    QHBoxLayout* gameBoardLayout = new QHBoxLayout(widgetScores);

    QVBoxLayout *countryRow = new QVBoxLayout();
    QLabel *countryLabel = new QLabel("Państwo:");
    QLineEdit *countryEdit = new QLineEdit();
    countryEdit->setText(country);
    QLabel *countryScoreLabel = new QLabel(countryPoints + " pkt");
    countryRow->addWidget(countryLabel);
    countryRow->addWidget(countryEdit);
    countryRow->addWidget(countryScoreLabel);
    gameBoardLayout->addLayout(countryRow);


    QVBoxLayout *cityRow = new QVBoxLayout();
    QLabel *cityLabel = new QLabel("Miasto:");
    QLineEdit *cityEdit = new QLineEdit();
    QLabel *cityScoreLabel = new QLabel(cityPoints + " pkt");
    cityEdit->setText(city);

    cityRow->addWidget(cityLabel);
    cityRow->addWidget(cityEdit);
    cityRow->addWidget(cityScoreLabel);
    gameBoardLayout->addLayout(cityRow);


    QVBoxLayout *objectRow = new QVBoxLayout();
    QLabel *objectLabel = new QLabel("Rzecz:");
    QLineEdit *objectEdit = new QLineEdit();
    QLabel *objectScoreLabel = new QLabel(objectPoints + " pkt");
    objectEdit->setText(object);

    objectRow->addWidget(objectLabel);
    objectRow->addWidget(objectEdit);
    objectRow->addWidget(objectScoreLabel);
    gameBoardLayout->addLayout(objectRow);


    QVBoxLayout *plantRow = new QVBoxLayout();
    QLabel *plantLabel = new QLabel("Roślina:");
    QLineEdit *plantEdit = new QLineEdit();
    QLabel *plantScoreLabel = new QLabel(plantPoints + " pkt");
    plantEdit->setText(plant);

    plantRow->addWidget(plantLabel);
    plantRow->addWidget(plantEdit);
    plantRow->addWidget(plantScoreLabel);
    gameBoardLayout->addLayout(plantRow);


    QVBoxLayout *animalRow = new QVBoxLayout();
    QLabel *animalLabel = new QLabel("Zwierzę:");
    QLineEdit *animalEdit = new QLineEdit();
    QLabel *animalScoreLabel = new QLabel(animalPoints + " pkt");
    animalEdit->setText(animal);

    animalRow->addWidget(animalLabel);
    animalRow->addWidget(animalEdit);
    animalRow->addWidget(animalScoreLabel);
    gameBoardLayout->addLayout(animalRow);


    layout->addWidget(widgetScores);

    countryEdit->setEnabled(false);
    cityEdit->setEnabled(false);
    objectEdit->setEnabled(false);
    animalEdit->setEnabled(false);
    plantEdit->setEnabled(false);


    return widget;
}

SummaryWindow::SummaryWindow(QTcpSocket *socket, QWidget *parent)
    : QWidget(parent), socket(socket)
{
    setWindowTitle("Podsumowanie");
    resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(this);

    timeLabel = new QLabel("Czas: ", this);
    layout->addWidget(timeLabel, 0, Qt::AlignCenter);

    titleLabel = new QLabel("Wszystkie odpowiedzi udzielone", this);
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);

    infoLabel = new QLabel("Oczekiwanie na sprawdzenie odpowiedzi przez serwer...", this);
    layout->addWidget(infoLabel, 0, Qt::AlignCenter);

    // voting start
    votingWidget = new QWidget(this);
    QVBoxLayout *votingWidgetLayout = new QVBoxLayout(votingWidget);

        // answer
    QWidget *answerWidget = new QWidget(this);
    QVBoxLayout *answerLayout = new QVBoxLayout(answerWidget);

    playerLabel = new QLabel("Gracz: ", this);
    answerLayout->addWidget(playerLabel);
    categoryLabel = new QLabel("Kategoria: ", this);
    answerLayout->addWidget(categoryLabel);
    answerLabel = new QLabel("Odpowiedz: ", this);
    answerLayout->addWidget(answerLabel);

    votingWidgetLayout->addWidget(answerWidget);

        // buttons

    QWidget *answerButtonsWidget = new QWidget(this);
    QHBoxLayout *answerButtonsLayout = new QHBoxLayout(answerButtonsWidget);


    goodButton = new QPushButton("Uznaje odpowiedz", this);
    answerButtonsLayout->addWidget(goodButton);

    badButton = new QPushButton("Nie uznaje odpowiedzi", this);
    answerButtonsLayout->addWidget(badButton);

    votingWidgetLayout->addWidget(answerButtonsWidget);


    // voting end
    layout->addWidget(votingWidget, 0, Qt::AlignCenter);

    resultsWidget = new QWidget(this);
    resultsWidgetLayout = new QVBoxLayout(resultsWidget);


    layout->addWidget(resultsWidget, 0, Qt::AlignCenter);

    returnToGame = new QPushButton("Wroc do gry", this);
    layout->addWidget(returnToGame);

    returnToGame->hide();

    resultsWidget->hide();

    votingWidget->hide();



    connect(socket, &QTcpSocket::disconnected, this, &SummaryWindow::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &SummaryWindow::onError);
    connect(socket, &QTcpSocket::readyRead, this, &SummaryWindow::onServerMessage);


    connect(goodButton, &QPushButton::clicked, this, &SummaryWindow::onGoodClicked);
    connect(badButton, &QPushButton::clicked, this, &SummaryWindow::onBadClicked);
    connect(returnToGame, &QPushButton::clicked, this, &SummaryWindow::onReturnToGameClicked);

    if (socket && socket->isOpen()) {
        qDebug() << "Socket is open and ready for communication.";
    } else {
        qDebug() << "Socket is not open or not connected!";
    }

}


void SummaryWindow::closeEvent(QCloseEvent *event) {
    disconnect(socket, &QTcpSocket::readyRead, this, &SummaryWindow::onServerMessage);
    disconnect(socket, &QTcpSocket::disconnected, this, &SummaryWindow::onDisconnected);
    disconnect(socket, &QTcpSocket::errorOccurred, this, &SummaryWindow::onError);

    if (parentWidget()) {
            parentWidget()->show();
    }


    event->accept();
}

void SummaryWindow::onDisconnected()
{
    QMessageBox::information(this, "[SUMMARY]Brak polaczenia", "Połączenie z serwerem zostało zakończone.");
    qDebug() << "Połączenie z serwerem zerwane.\n";



    this->close();

}



void SummaryWindow::onReturnToGameClicked() {

     this->close();
}

void SummaryWindow::onGoodClicked() {
    qDebug() << "Good clicked";

    QString vote = "VOTE:1\n";

    socket->flush();
    socket->write(vote.toUtf8());

    infoLabel->show();
    votingWidget->hide();

    qDebug() << "Good wyslano";


}

void SummaryWindow::onBadClicked() {
    QString vote = "VOTE:0\n";

    qDebug() << "bad clicked";

    socket->flush();
    socket->write(vote.toUtf8());

    infoLabel->show();
    votingWidget->hide();

    qDebug() << "Bad wyslano";
}





void SummaryWindow::onServerMessage()
{



    QByteArray response = socket->readAll();

    qDebug() << "\nSummary dostal: " << response;

    QStringList messages = QString(response).split('\n', Qt::SkipEmptyParts);


    for (const QString &msg : messages) {
        qDebug() << "\t << msg:" << msg;
        if (msg.startsWith("VOTE:")) {

            titleLabel->setText("Dokonaj wyboru");
            infoLabel->hide();
            votingWidget->show();

            QString voteMsg = msg;

            voteMsg.remove(0,5);

            QStringList vote = voteMsg.split(",", Qt::SkipEmptyParts);
            playerLabel->setText(vote[0]);
            categoryLabel->setText(vote[1]);
            answerLabel->setText(vote[2]);

        } else if (msg.startsWith("TIME:")) {
            QString time = msg;
            timeLabel->setText("Czas: " + time.remove(0, 5));

            // tutaj
            if (msg.startsWith("TIME:0")) {

                onBadClicked();


            }


        } else if (msg.startsWith("CURRENT_SCORES")) {

            qDebug() << "\t << od current_scores:" << msg;

            titleLabel->setText("Wyniki");
            titleLabel->setFixedHeight(50);
            infoLabel->hide();
            votingWidget->hide();
            timeLabel->hide();

            resultsWidget->show();
            returnToGame->show();

        } else if (msg.startsWith("SCORE:")) {

            qDebug() << "\t << od scores:" << msg;

            QString score = msg;

            score.remove(0,6);

            QStringList player = score.split("#", Qt::SkipEmptyParts);

            QString playerName = player[0];

            QStringList categories;
            QStringList scores;

            QStringList scoresList = player[1].split(";", Qt::SkipEmptyParts);

            for (const QString &catScore : scoresList) {
                QStringList player = catScore.split(",");
                categories.append(player[0]);
                scores.append(player[1]);
            }

            qDebug() << "\t << answers:" << categories;
            qDebug() << "\t << scores:" << scores;

            QWidget* widget = createDynamicWidget(playerName, categories[0], scores[0], categories[2], scores[2],
                    categories[1], scores[1],categories[3], scores[3],categories[4], scores[4]);
            resultsWidgetLayout->addWidget(widget);




        } else if (msg.startsWith("PREPARE_START")) {

            QString vote = "PREPARE_AGAIN\n";

            socket->flush();
            socket->write(vote.toUtf8());
            if (socket->waitForBytesWritten(1000)) {
                qDebug() << "Dane zostały wysłane.";
                this->close();
            }

        } else {
            qDebug() << "summarry otrzymal inne dane: " << msg;
        }
    }
}



void SummaryWindow::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
}

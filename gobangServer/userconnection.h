#ifndef USERCONNECTION_H
#define USERCONNECTION_H

#include <QObject>
#include <QTcpSocket>

//用户状态，游戏中，等待中，空闲
enum UserStatus {
    PLAYING,
    WAITING,
    FREE
};

class UserConnection : public QObject
{
    Q_OBJECT
public:
    explicit UserConnection(QObject *parent = nullptr);
    UserConnection(QTcpSocket*);
    ~UserConnection();

    QString name;
    QString status;
    QTcpSocket* socket;
    QString nextChess;
    QString request;


signals:
    void SignalDisconnect();
    void SignalRemoveThis();

private slots:
    void slotDisconnect();


};

#endif // USERCONNECTION_H

#ifndef NETPLAY_H
#define NETPLAY_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QHostAddress>
#include "GameModel.h"



namespace Ui {
class NetPlay;
}

class NetPlay : public QMainWindow
{
    Q_OBJECT

public:
    explicit NetPlay(QWidget *parent = nullptr);
    ~NetPlay();
    QTcpSocket* tcp;


    GameModel::user player1;
    GameModel::user player2;

public slots:
    //接受本地走步信号，向联机玩家传输player1
    void slotNextChess(GameModel::user);



private slots:
    //与服务器新建连接
    void slotNewConnect();
    //连接按钮按下
    void on_connect_clicked();
    //取消连接按钮按下
    void on_disconnect_clicked();
    //断开连接时
    void slotDisconnect();

    //确认双方已经连接成功
    void connectSuccess();
    //将player1发送给服务端
    void sendPlayer1();
    //从服务端接收player2
    void receivePlayer2();




signals:
    void connected();
    void hasNextChess(GameModel::user*);
    void initGame(GameModel::user*, GameModel::user*);
    void hasDisconnected();

private:
    Ui::NetPlay *ui;
    QLabel* m_status;


};

#endif // NETPLAY_H

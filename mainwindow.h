#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GameModel.h"
#include "netplay.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    GameModel *game; // 游戏指针
    GameType game_type; // 存储游戏类型
    int clickPosRow, clickPosCol; // 存储将点击的位置
    void judge(GameModel::user*);//判断游戏胜负平
    GameModel::user *player1;
    GameModel::user *player2;
    QLabel *showNowPlayer;
    QLabel *showName;



public:
    MainWindow(QWidget *parent = 0);
    void initGame(GameModel::user*,GameModel::user*);
    ~MainWindow();


protected:
    // 绘制
    void paintEvent(QPaintEvent *event);
    // 监听鼠标移动情况，方便落子
    void mouseMoveEvent(QMouseEvent *event);
    // 实际落子
    void mouseReleaseEvent(QMouseEvent *event);



public slots:
    void sendBackSlot();//发送返回请求
    void chessOneByNet(GameModel::user*); // 联机下棋


signals:
    void backStartWindow();//返回主菜单信号
    void netPlayer1Chess(GameModel::user);//联机模式player1已经下棋了的信号

private slots:
    void chessOneByPerson(GameModel::user*); // 人下棋走步
    void chessOneByAI(GameModel::user*); // AI下棋走步

};

#endif // MAINWINDOW_H



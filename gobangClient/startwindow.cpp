#include "startwindow.h"

const int ChessBoardMargin = 30; // 棋盘边缘空隙
const int ChessBlockSize = 40; // 格子的大小
//设置开始界面，用代码设计窗体，绑定各按键功能
StartWindow::StartWindow(QWidget *parent) : QMainWindow(parent)
{
    //设置窗口大小
    setFixedSize(ChessBoardMargin * 2 + ChessBlockSize * (ChessBoardSize - 1),ChessBoardMargin * 2 + ChessBlockSize * (ChessBoardSize - 1));
    this->setWindowTitle("五子棋");
    PVPLocalButton.setParent(this);
    PVPNetworkButton.setParent(this);
    PVELocalButton.setParent(this);
    PVPLocalButton.setText("本地双人对战");
    PVELocalButton.setText("人机对战");
    PVPNetworkButton.setText("联网双人对战");
    PVPLocalButton.setFixedSize(120,60);
    PVPNetworkButton.setFixedSize(120,60);
    PVELocalButton.setFixedSize(120,60);
    PVELocalButton.move(250,220);
    PVPLocalButton.move(250,280);
    PVPNetworkButton.move(250,340);
    connect(&PVPLocalButton,&QPushButton::released,this,&StartWindow::pvpLocal);
    connect(&PVELocalButton,&QPushButton::released,this,&StartWindow::pveLocal);
    connect(&PVPNetworkButton,&QPushButton::released,this,&StartWindow::pvpNetwork);

    //当游戏窗口中菜单栏里的返回开始界面被点击后，在startWindow内调用使MainWindow隐藏，startWindow显示
    connect(&w,&MainWindow::backStartWindow,this,&StartWindow::backStartWindow);

    //联机模式下，当本地下棋之后，将player1传给服务端，由服务端转发给对方
    connect(&w, &MainWindow::netPlayer1Chess,&n,&NetPlay::slotNextChess);

    //联机模式下，当对方下棋之后，接收传来的数据，并将其在本地进行操作
    connect(&n, &NetPlay::hasNextChess, &w, &MainWindow::chessOneByNet);
    //初始化游戏，接受NetPlay传来player对象来初始化游戏
    connect(&n, &NetPlay::initGame, &w, &MainWindow::initGame);
    //初始化游戏时，进行开始界面的隐藏和游戏界面的显示
    connect(&n, &NetPlay::initGame, this, [=](){
        n.hide();
        w.show();
    });
    //当联机时，双方连接中断后，进行弹窗警告，并返回主界面
    connect(&n, &NetPlay::hasDisconnected, this, [=](){
        QMessageBox::warning(&w,"结束游戏","连接中断",QMessageBox::Yes);
        StartWindow::backStartWindow();
    });
}
//返回开始界面
void StartWindow::backStartWindow(){
    w.close();
    this->show();
}
//本地双人  初始化两个玩家  后续可通过弹窗输入来添加名字自定义功能
void StartWindow::pvpLocal(){
    GameType type = LOCAL_PERSON;
    GameModel::user player1;
    GameModel::user player2;
    player1.gameType = type;
    player2.gameType = type;
    player1.color = true;
    player2.color = false;
    player1.name = "1P";
    player2.name = "2P";
    w.initGame(&player1,&player2);
    w.show();
    this->hide();
}

//联机 显示netplay界面，进行网络连接
void StartWindow::pvpNetwork(){
    this->hide();
    //显示连接服务器界面
    n.show();
}
//本地人机 初始化两个玩家和游戏类型
void StartWindow::pveLocal(){
    GameType type = BOT;
    GameModel::user player1;
    GameModel::user player2;
    player1.gameType = type;
    player2.gameType = type;
    player1.color = true;
    player2.color = false;
    player1.name = "1P";
    player2.name = "AI";

    w.initGame(&player1, &player2);
    w.show();
    this->hide();
}

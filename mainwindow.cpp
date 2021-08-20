#include "mainwindow.h"
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <math.h>

const int ChessBoardMargin = 30; // 棋盘边缘空隙
const int ChessRadius = 15; // 棋子半径
const int ChessMarkSize = 6; // 落子标记边长
const int ChessBlockSize = 40; // 格子的大小
const int PosDelta = 20; // 鼠标点击的模糊距离上限
int AIDelay = 300; // AI下棋的思考时间

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //设置窗体大小
    setFixedSize(ChessBoardMargin * 2 + ChessBlockSize * (ChessBoardSize - 1),ChessBoardMargin * 2 + ChessBlockSize * (ChessBoardSize - 1));

    setMouseTracking(true);//设置鼠标移动监听必须打开

    QMenu *gameMenu = menuBar()->addMenu(tr("游戏")); //在menuBar中添加一个选项卡
    QAction *returnStart = new QAction("返回主界面", this);//添加一个子选项卡
    connect(returnStart, SIGNAL(triggered()), this, SLOT(sendBackSlot())); //将sendBackSlot()函数与QAction returnStart选项相关联
    gameMenu->addAction(returnStart);

    //显示现在的下棋颜色
    showNowPlayer = new QLabel(this);
    //显示当前下棋选手的名字
    showName = new QLabel(this);
    showName->setGeometry(20,0,100,30);

    showNowPlayer->setGeometry(250,0,100,30);





}

//发送返回主界面的信号
void MainWindow::sendBackSlot(){
    emit backStartWindow();
}

MainWindow::~MainWindow()
{
    if (game)
    {
        delete game;
        game = nullptr;
    }
}

//根据传过来的两个player来进行游戏初始化
void MainWindow::initGame(GameModel::user* play1, GameModel::user* play2){
    player1 = play1;
    player2 = play2;
    game = new GameModel;
    game->gameType = player1->gameType;
    game->player = true;
    game_type = player1->gameType;
    game->gameStatus = PLAYING;
    game->player1 = *player1;
    game->player2 = *player2;
    game->startGame(&game->player1,&game->player2);
    update();
}





//绘图事件，通过update调用
void MainWindow::paintEvent(QPaintEvent*){
    QPainter painter(this);
    QString username;
    std::string name;
    //获取当前游戏棋手的名字
    if(game->player==game->player1.color)
        name = game->player1.name;
    else
        name = game->player2.name;
    username = QString::fromStdString(name);
    showName->setText("玩家："+username);


    QString a = game->player?"黑棋":"白棋";
    showNowPlayer->setText("当前棋手:"+a);


    // 绘制棋盘
    painter.setRenderHint(QPainter::Antialiasing, true); // 抗锯齿
    for (int i = 0; i < ChessBoardSize; i++)
    {
        painter.drawLine(ChessBoardMargin + ChessBlockSize * i,
                         ChessBoardMargin,
                         ChessBoardMargin + ChessBlockSize * i,
                         size().height() - ChessBoardMargin);
        painter.drawLine(ChessBoardMargin,
                         ChessBoardMargin + ChessBlockSize * i,
                         size().width() - ChessBoardMargin,
                         ChessBoardMargin + ChessBlockSize * i);
    }

    QBrush brush;//画刷
    brush.setStyle(Qt::SolidPattern);
    // 绘制落子标记(防止鼠标出框越界)
    if (clickPosRow >= 0 && clickPosRow < ChessBoardSize &&
        clickPosCol >= 0 && clickPosCol < ChessBoardSize &&
        game->gameMapVec[clickPosRow][clickPosCol] == 0)
    {
        if (game->player)
            brush.setColor(Qt::black);
        else
            brush.setColor(Qt::white);
        painter.setBrush(brush);
        painter.drawRect(ChessBoardMargin + ChessBlockSize * clickPosCol - ChessMarkSize / 2,
                         ChessBoardMargin + ChessBlockSize * clickPosRow - ChessMarkSize / 2,
                         ChessMarkSize, ChessMarkSize);
    }

    // 绘制棋子
    for (int i = 0; i < ChessBoardSize; i++)
        for (int j = 0; j < ChessBoardSize; j++)
        {
            if (game->gameMapVec[i][j] == 1)
            {
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(ChessBoardMargin + ChessBlockSize * j - ChessRadius,
                                    ChessBoardMargin + ChessBlockSize * i - ChessRadius,
                                    ChessRadius * 2, ChessRadius * 2);
            }
            else if (game->gameMapVec[i][j] == -1)
            {
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(ChessBoardMargin + ChessBlockSize * j - ChessRadius,
                                    ChessBoardMargin + ChessBlockSize * i - ChessRadius,
                                    ChessRadius * 2, ChessRadius * 2);
            }
        }

}

//鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // 通过鼠标的hover确定落子的标记
    int x = event->x();
    int y = event->y();

    // 棋盘边缘不能落子
    if (x >= ChessBoardMargin &&
        x <= size().width() - ChessBoardMargin &&
        y >= ChessBoardMargin  &&
        y <= size().height()- ChessBoardMargin)
    {
        // 获取最近的左上角的点
        int col = x / ChessBlockSize;
        int row = y / ChessBlockSize;

        int leftTopPosX = ChessBoardMargin + ChessBlockSize * col;
        int leftTopPosY = ChessBoardMargin + ChessBlockSize * row;

        // 根据距离算出合适的点击位置,一共四个点，根据半径距离选最近的
        clickPosRow = -1; // 初始化最终的值
        clickPosCol = -1;
        int len = 0; // 计算完后取整就可以了

        // 确定一个误差在范围内的点，且只可能确定一个出来
        len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (y - leftTopPosY) * (y - leftTopPosY));
        if (len < PosDelta)
        {
            clickPosRow = row;
            clickPosCol = col;
        }
        len = sqrt((x - leftTopPosX - ChessBlockSize) * (x - leftTopPosX - ChessBlockSize) + (y - leftTopPosY) * (y - leftTopPosY));
        if (len < PosDelta)
        {
            clickPosRow = row;
            clickPosCol = col + 1;
        }
        len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (y - leftTopPosY - ChessBlockSize) * (y - leftTopPosY - ChessBlockSize));
        if (len < PosDelta)
        {
            clickPosRow = row + 1;
            clickPosCol = col;
        }
        len = sqrt((x - leftTopPosX - ChessBlockSize) * (x - leftTopPosX - ChessBlockSize) + (y - leftTopPosY - ChessBlockSize) * (y - leftTopPosY - ChessBlockSize));
        if (len < PosDelta)
        {
            clickPosRow = row + 1;
            clickPosCol = col + 1;
        }
    }
    // 存了坐标后也要重绘
    update();
}

//鼠标点击事件
void MainWindow::mouseReleaseEvent(QMouseEvent*)
{
    //由于在mouseMoveEvent中设置clickPosCol和Row初始值为-1，在只移动y轴或x轴的情况下，会出现某值为-1，此时点击无效
    //鼠标点击  务必处于格子内，不能超出,而且该点必须无棋子
    if(clickPosCol>=0&&clickPosRow>=0&&clickPosCol<ChessBoardSize&&clickPosRow<ChessBoardSize
            && game->gameMapVec[clickPosRow][clickPosCol] == 0){
        //本机玩家点击时，将本机玩家数据传输过去
        if(game_type == NET_PERSON&&game->player1.color == game->player){
            game->player1.col = clickPosCol;
            game->player1.row = clickPosRow;
            qDebug()<<game->player1.col<<game->player2.row;
            emit netPlayer1Chess(game->player1);
        }
        //默认player1为本机玩家,联机对战中,player2为联机玩家
        if(game->player1.color==game->player){
            game->player1.col = clickPosCol;
            game->player1.row = clickPosRow;
            if(game->player==game->player1.color){
                chessOneByPerson(&game->player1);
            }
            //人机模式，下棋不能抢人机的步
            if(game_type == BOT && game->player1.color!=game->player){
                //chessOneByAI(&game->player2);
                // 用定时器做一个延迟
                QTimer::singleShot(AIDelay, this, [=](){
                    chessOneByAI(&game->player2);
                });
            }
        }
        //本地双人模式
        else if(game->player2.color==game->player&&game_type==LOCAL_PERSON){
            game->player2.col = clickPosCol;
            game->player2.row = clickPosRow;
            chessOneByPerson(&game->player2);
        }
}


}

void MainWindow::judge(GameModel::user* play){
    // 判断死局
    if (game->isDraw())
    {
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "oops", "draw!");
        if (btnValue == QMessageBox::Ok)
        {
            game->startGame(&game->player1,&game->player2);
            game->gameStatus = PLAYING;
        }

    }
    // 判断输赢
    if (game->isWin(play->row, play->col) && game->gameStatus == PLAYING)
    {
        game->gameStatus = WIN;
        QString str;
        if (game->gameMapVec[play->row][play->col] == 1)
            str = "黑棋";
        else if (game->gameMapVec[play->row][play->col] == -1)
            str = "白棋";
        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "恭喜", str + " 胜!");

        // 重置游戏状态，否则容易死循环
        if (btnValue == QMessageBox::Ok)
        {
            game->startGame(&game->player1,&game->player2);
            game->gameStatus = PLAYING;
        }
    }
}

//AI下棋
void MainWindow::chessOneByAI(GameModel::user* play){
    game->actionByAI(play);
    update();
    MainWindow::judge(play);
}
//联机模式player2下棋
void MainWindow::chessOneByNet(GameModel::user* play){
    game->player2 = *play;

    if(game->player2.color == game->player){

        game->actionByNet(&game->player2);
        update();
        MainWindow::judge(play);
    }
}
//player1下棋，貌似能与chessOneByNet合并优化一下
void MainWindow::chessOneByPerson(GameModel::user* play){
    // 根据当前存储的坐标下子
    // 只有有效点击才下子，并且该处没有子
    game->actionByPerson(play);
    // 重绘
    update();
    MainWindow::judge(play);
}


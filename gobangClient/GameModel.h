#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include<vector>
#include<iostream>
#include<stack>
#include<QTcpSocket>
//游戏类型，本地，联机，人机
enum GameType {
    LOCAL_PERSON,
    NET_PERSON,
    BOT
};
//游戏状态，进行中，胜利，失败，平局
enum GameStatus {
    PLAYING,
    WIN,
    LOSE,
    DRAW
};
//请求类型
enum requestType{
    CONNECT,
    CONNECTED,
    NEXTCHESS,
    DISCONNECTED
};

//棋盘尺寸
const int ChessBoardSize = 15;


class GameModel{

  public:
    //定义传输和用于进行游戏的player结构体
    typedef struct user{
        std::string name;
        bool color;//GameModel中  判断棋手黑子白子
        int row;
        int col;
        requestType request;
        GameType gameType;
    }user;
    GameModel();
    std::vector<std::vector<int> > gameMapVec; //存储当前下子情况，空白为0，白为1，黑为-1
    std::vector<std::vector<int> > scoreMapVec; //根据当前下子情况分析AI下棋分值
    bool player; //标志当前该下棋的棋手颜色
    int surplusBoard;//空棋盘位
    GameType gameType;//游戏模式
    GameStatus gameStatus;//游戏状态
    user player1;
    user player2;




    void startGame(user*, user*);// 开始游戏
    void calculateScore();// 计算评分
    void actionByPerson(user*);// 人执行下棋
    void actionByNet(user*);// 联网执行下棋
    void actionByAI(user*);// 机器下棋
    void updateGameMap(user*);// 落子后更新棋盘
    bool isWin(int row, int col);// 判断是否胜利
    bool isDraw();//判断平手
};

#endif //GAMEMODEL_H

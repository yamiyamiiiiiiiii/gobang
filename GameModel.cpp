#include"GameModel.h"

GameModel::GameModel(){

}

void GameModel::startGame(user* player1, user* player2) {
    surplusBoard = ChessBoardSize * ChessBoardSize;
    gameType = player1->gameType;
    gameMapVec.clear();
    scoreMapVec.clear();
    for (int i = 0; i < ChessBoardSize; i++){
        std::vector<int> lineBoard;
        for (int j = 0; j < ChessBoardSize; j++)
            lineBoard.push_back(0);
        gameMapVec.push_back(lineBoard);
    }
    if(gameType == BOT) {
        for (int i = 0; i < ChessBoardSize; i++){
            std::vector<int> lineBoard;
            for (int j = 0; j < ChessBoardSize; j++)
                lineBoard.push_back(0);
            scoreMapVec.push_back(lineBoard);
        }
    }
    player = true;
}



void GameModel::updateGameMap(user* play) {
    if(play->color)
        gameMapVec[play->row][play->col] = 1;
    else
        gameMapVec[play->row][play->col] = -1;
    player = !player;
}

void GameModel::actionByPerson(user* play) {
    updateGameMap(play);
}

void GameModel::actionByNet(user* play) {
    updateGameMap(play);
}


void GameModel::actionByAI(user* play) {
    // 计算评分
    calculateScore();
    typedef struct point {
        int row = -1;
        int col = -1;
        int score = INT_MIN;
    } P;
    std::stack<point> pointStk;
    P maxPoint;
    maxPoint.score = INT_MIN;
    pointStk.push(maxPoint);
    for(int i = 0; i < ChessBoardSize; i++) {
        for(int j = 0; j < ChessBoardSize; j++) {
            if(gameMapVec[i][j] == 0) { // 首先该位置不能已被占用
                if(scoreMapVec[i][j] >= pointStk.top().score) {
                    maxPoint.row = i;
                    maxPoint.col = j;
                    maxPoint.score = scoreMapVec[i][j];
                    pointStk.push(maxPoint); //遇到有最大的  直接入栈
                }
            }
        }
    }
    play->row = pointStk.top().row;
    play->col = pointStk.top().col;
    //此处默认选最后一个最大的点，有点刻板，没能做到随机
    updateGameMap(play);
}

//计算评分函数 AI对战重点
void GameModel::calculateScore()
{
    // 统计玩家或者电脑连成的子
    int personNum = 0; // 玩家连成子的个数
    int botNum = 0; // AI连成子的个数
    int emptyNum = 0; // 各方向空白位的个数

    // 清空评分数组
    scoreMapVec.clear();
    for (int i = 0; i < ChessBoardSize; i++)
    {
        std::vector<int> lineScores;
        for (int j = 0; j < ChessBoardSize; j++)
            lineScores.push_back(0);
        scoreMapVec.push_back(lineScores);
    }

    // 计分（此处是完全遍历，其实可以用bfs或者dfs加减枝降低复杂度，通过调整权重值，调整AI智能程度以及攻守风格）
    for (int row = 0; row < ChessBoardSize; row++)
        for (int col = 0; col < ChessBoardSize; col++)
        {
            // 空白点就算
            if (gameMapVec[row][col] == 0)
            {
                // 遍历周围八个方向
                for (int y = -1; y <= 1; y++)
                    for (int x = -1; x <= 1; x++)
                    {
                        // 重置
                        personNum = 0;
                        botNum = 0;
                        emptyNum = 0;

                        // 原坐标不算
                        if (!(y == 0 && x == 0))
                        {
                            // 每个方向延伸4个子

                            // 对玩家黑子评分（正反两个方向）
                            for (int i = 1; i <= 4; i++)
                            {
                                if (row + i * y >= 0 && row + i * y < ChessBoardSize &&
                                    col + i * x >= 0 && col + i * x < ChessBoardSize &&
                                    gameMapVec[row + i * y][col + i * x] == 1) // 玩家的子 黑子
                                {
                                    personNum++;
                                }
                                else if (row + i * y >= 0 && row + i * y < ChessBoardSize &&
                                         col + i * x >= 0 && col + i * x < ChessBoardSize &&
                                         gameMapVec[row + i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 4; i++)
                            {
                                if (row - i * y >= 0 && row - i * y < ChessBoardSize &&
                                    col - i * x >= 0 && col - i * x < ChessBoardSize &&
                                    gameMapVec[row - i * y][col - i * x] == 1) // 玩家的子 黑子
                                {
                                    personNum++;
                                }
                                else if (row - i * y >= 0 && row - i * y < ChessBoardSize &&
                                         col - i * x >= 0 && col - i * x < ChessBoardSize &&
                                         gameMapVec[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (personNum == 1)                      // 杀二
                                scoreMapVec[row][col] += 10;
                            else if (personNum == 2)                 // 杀三
                            {
                                if (emptyNum == 1)
                                    scoreMapVec[row][col] += 30;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 40;
                            }
                            else if (personNum == 3)                 // 杀四
                            {
                                // 量变空位不一样，优先级不一样
                                if (emptyNum == 1)
                                    scoreMapVec[row][col] += 55;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 100;
                            }
                            else if (personNum == 4)                 // 杀五
                                scoreMapVec[row][col] += 10000;

                            // 进行一次清空
                            emptyNum = 0;

                            // 对AI黑子评分
                            for (int i = 1; i <= 4; i++)
                            {
                                if (row + i * y >= 0 && row + i * y < ChessBoardSize &&
                                    col + i * x >= 0 && col + i * x < ChessBoardSize &&
                                    gameMapVec[row + i * y][col + i * x] == -1) // AI  白子
                                {
                                    botNum++;
                                }
                                else if (row + i * y >= 0 && row + i * y < ChessBoardSize &&
                                         col + i * x >= 0 && col + i * x < ChessBoardSize &&
                                         gameMapVec[row +i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 4; i++)
                            {
                                if (row - i * y >= 0 && row - i * y < ChessBoardSize &&
                                    col - i * x >= 0 && col - i * x < ChessBoardSize &&
                                    gameMapVec[row - i * y][col - i * x] == -1) // AI的子 白子
                                {
                                    botNum++;
                                }
                                else if (row - i * y > 0 && row - i * y < ChessBoardSize &&
                                         col - i * x > 0 && col - i * x < ChessBoardSize &&
                                         gameMapVec[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (botNum == 0)                      // 普通下子
                                scoreMapVec[row][col] += 5;
                            else if (botNum == 1)                 // 活二
                                scoreMapVec[row][col] += 10;
                            else if (botNum == 2)
                            {
                                if (emptyNum == 1)                // 死三
                                    scoreMapVec[row][col] += 25;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 50;  // 活三
                            }
                            else if (botNum == 3)
                            {
                                if (emptyNum == 1)                // 死四
                                    scoreMapVec[row][col] += 60;
                                else if (emptyNum == 2)
                                    scoreMapVec[row][col] += 110; // 活四
                            }
                            else if (botNum >= 4)
                                scoreMapVec[row][col] += 10100;   // 活五

                        }
                    }

            }
        }
}

//每下一个棋子，对其周围是否凑成5个进行判断
bool GameModel::isWin(int row, int col) {

    //横向判断判断,判断该点是否一排五个点中的最右点，若不是，则向左位移一位再进行判断，位移四次
    for(int i = 0; i < 5; i++) {
        if (row + i < ChessBoardSize &&
                row + i - 4 >= 0 &&
                gameMapVec[row + i][col] == gameMapVec[row + i - 1][col] &&
                gameMapVec[row + i][col] == gameMapVec[row + i - 2][col] &&
                gameMapVec[row + i][col] == gameMapVec[row + i - 3][col] &&
                gameMapVec[row + i][col] == gameMapVec[row + i - 4][col])
            return true;
    }
    //纵向判断，规则同上,判断是否最下点
    for(int i = 0; i < 5; i++) {
        if (col + i < ChessBoardSize &&
                col + i - 4 >= 0 &&
                gameMapVec[row][col + i] == gameMapVec[row][col + i - 1] &&
                gameMapVec[row][col + i] == gameMapVec[row][col + i - 2] &&
                gameMapVec[row][col + i] == gameMapVec[row][col + i - 3] &&
                gameMapVec[row][col + i] == gameMapVec[row][col + i - 4])
            return true;
    }
    //左斜
    for(int i = 0; i < 5; i++) {
        if (col + i < ChessBoardSize &&
                col + i - 4 >= 0 &&
                row + i < ChessBoardSize &&
                row + i - 4 >= 0 &&
                gameMapVec[row + i][col + i] == gameMapVec[row + i - 1][col + i - 1] &&
                gameMapVec[row + i][col + i] == gameMapVec[row + i - 2][col + i - 2] &&
                gameMapVec[row + i][col + i] == gameMapVec[row + i - 3][col + i - 3] &&
                gameMapVec[row + i][col + i] == gameMapVec[row + i - 4][col + i - 4])
            return true;
    }
    //右斜
    for(int i = 0; i < 5; i++) {
        if (col + i < ChessBoardSize &&
                col + i - 4 > 0 &&
                row - i > 0 &&
                row - i + 4 < ChessBoardSize &&
                gameMapVec[row - i][col + i] == gameMapVec[row - i + 1][col + i - 1] &&
                gameMapVec[row - i][col + i] == gameMapVec[row - i + 2][col + i - 2] &&
                gameMapVec[row - i][col + i] == gameMapVec[row - i + 3][col + i - 3] &&
                gameMapVec[row - i][col + i] == gameMapVec[row - i + 4][col + i - 4])
            return true;
    }
    return false;
}

bool GameModel::isDraw() {
    surplusBoard--;
    if(surplusBoard == 0)
        return true;
    return false;
}





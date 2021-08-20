#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QString>
#include "userconnection.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum GameType {
    LOCAL_PERSON,
    NET_PERSON,
    BOT
};

enum requestType{
    CONNECT,
    CONNECTED,
    NEXTCHESS,
    DISCONNECTED
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    typedef struct user{
        std::string name;
        bool color;//GameModel中  判断棋手黑子白子
        int row;
        int col;
        requestType request;
        GameType gameType;
    }user;

private slots:
    void slotNewConnect();

    void on_SetListen_clicked();

private:
    Ui::MainWindow *ui;
    QTcpServer* m_s;
    QVector<UserConnection*> Tcp_List;
    QVector<UserConnection*> freeUser;
    QVector<UserConnection*> busyUser;
    QVector<UserConnection*> waitUser;
};
#endif // MAINWINDOW_H

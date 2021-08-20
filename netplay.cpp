#include "netplay.h"
#include "ui_netplay.h"


NetPlay::NetPlay(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NetPlay)
{
    ui->setupUi(this);
    setWindowTitle("连接服务器");

    //设置文本框默认值
    ui->port->setText("8989");
    ui->ipAddr->setText("127.0.0.1");
    ui->disconnect->setDisabled(true);

    tcp = new QTcpSocket(this);//指定父对象

    //连接成功后，给服务端发送本机player信息
    connect(tcp, &QTcpSocket::connected, this, &NetPlay::slotNewConnect);

    //connect(tcp, &QTcpSocket::disconnected, this, &NetPlay::slotDisconnect);
    //当接收到消息时，即是对方发送了player信息，此时将其赋给player2进行下一步操作
    connect(tcp, &QTcpSocket::readyRead, this, &NetPlay::receivePlayer2);
    //状态栏
    m_status = new QLabel;
    m_status->setText("未连接");
    ui->statusbar->addWidget(new QLabel("连接状态："));
    ui->statusbar->addWidget(m_status);


}

NetPlay::~NetPlay()
{
    delete ui;
}

//发送本机player1信息给服务端
void NetPlay::sendPlayer1(){
    QByteArray data ;
    data.append(reinterpret_cast<char*>(&player1),sizeof(player1));
    tcp->write(data.data(),sizeof(player1));
}
//接收服务端转发回来的信息
void NetPlay::receivePlayer2(){
    QByteArray data = tcp->readAll();
    if(!data.isEmpty()){
        GameModel::user*   temp = reinterpret_cast<GameModel::user*>(data.data());
        player2 = *temp;
    }
    if(player2.request== CONNECT){//当请求为连接时
        player1.request = CONNECTED;
        /**
         * 当接收到对方的connect请求时，说明对方没收到我们的连接信息，
         * 此时需要重发一遍连接信息，由服务器再次转发一次，从而使对方获
         * 取我们的player结构体
         **/
        player1.color = !player2.color;
        sendPlayer1();
        connectSuccess();
        m_status->setText("已连接");
    }
    else if(player2.request==CONNECTED){//当请求为已连接时
        /**
         * //此时可以隐藏NetPlay窗口  并进行黑白色棋子选择
         * 选择颜色上遇到技术性问题，遂决定采用谁后发起请求，谁先手的方式决定颜色
         * 此时说明对方已接收到我们的connect请求，已经保存了我们的player信息
         *
        **/

        connectSuccess();
        m_status->setText("已连接");
    }
    else if(player2.request==NEXTCHESS){//当请求为下棋时
        emit hasNextChess(&player2);
    }
    else if(player2.request==DISCONNECTED){//当请求为断开连接时
        slotDisconnect();
    }

}

void NetPlay::connectSuccess(){//双方已经连接成功，此时进行游戏初始化功能
    emit initGame(&player1,&player2);
}

//player1走一步，发给联机玩家
void NetPlay::slotNextChess(GameModel::user play){
    play.request=NEXTCHESS;
    QByteArray data ;
    data.append(reinterpret_cast<char*>(&play),sizeof(play));
    tcp->write(data.data(),sizeof(play));
}

//player1与服务器建立连接，等待与player2进行配对
void NetPlay::slotNewConnect(){
    std::string user_name = ui->username->text().toStdString();
    player1.gameType=NET_PERSON;
    player1.name=user_name;
    player1.color = true;

    player1.request=CONNECT;
    QByteArray data ;
    data.append(reinterpret_cast<char*>(&player1),sizeof(player1));
    tcp->write(data.data(),sizeof(player1));
    m_status->setText("等待中");
}

//发送请求，请求与服务端连接
void NetPlay::on_connect_clicked()
{
    unsigned short port = ui->port->text().toUShort();
    QString ip = ui->ipAddr->text();
    tcp->abort();
    tcp->connectToHost(QHostAddress(ip),port);
    ui->connect->setDisabled(true);
    ui->disconnect->setEnabled(true);
}

//与服务端断开连接
void NetPlay::on_disconnect_clicked()
{
    tcp->close();
    //tcp->deleteLater();
    m_status->setText("未连接");
    ui->connect->setDisabled(false);
    ui->disconnect->setDisabled(true);
}

//当与联机用户断开连接时
void NetPlay::slotDisconnect(){
    //发出断连的信号，由startWindow接收
    emit hasDisconnected();
    tcp->close();
    //tcp->deleteLater();
    m_status->setText("未连接");
    ui->connect->setDisabled(false);
    ui->disconnect->setDisabled(true);
}




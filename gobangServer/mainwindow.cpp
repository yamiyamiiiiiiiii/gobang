#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->port->setText("8989");
    setWindowTitle("服务器");

    m_s = new QTcpServer(this);
    Tcp_List.clear();

    connect(m_s, &QTcpServer::newConnection, this, &MainWindow::slotNewConnect);

}

void MainWindow::slotNewConnect(){
    if(m_s->hasPendingConnections()){
        QTcpSocket* new_tcp = m_s->nextPendingConnection();
        UserConnection* user = new UserConnection(new_tcp);
        Tcp_List.append(user);
        //接收到信息时，将该信息转发给除自己以外的所有客户端
        connect(user->socket, &QTcpSocket::readyRead, this, [=](){
            QByteArray data = user->socket->readAll();
            //将从本客户端接收的消息转发给其他客户端
            for(int i = 0; i < Tcp_List.size(); i++){
                if(Tcp_List.at(i)->socket != user->socket){
                    Tcp_List.at(i)->socket->write(data);
                }
            }
        });
        //当与某客户端断开连接时，将其断开信息转发给其他所有客户端，并将该客户端从连接池中删除
        connect(user->socket, &QTcpSocket::disconnected, this, [=](){
            MainWindow::user temp;
            temp.request = DISCONNECTED;
            QByteArray data;
            data.append(reinterpret_cast<char*>(&temp),sizeof(temp));
            QVector<UserConnection*>::iterator it;
            for( it=Tcp_List.begin();it!=Tcp_List.end();it++){
                if(*it == user){
                    Tcp_List.erase(it);//由于qVector删除后会自动前移，所以将迭代器向前移动  重新访问该位置
                    it--;
                }
                else{
                    UserConnection* sendTemp = *it;
                    sendTemp->socket->write(data.data(),sizeof(temp));
                }
                if(Tcp_List.empty())//若删除之后qvector为空，退出
                    break;
            }
        });
    }
}



MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_SetListen_clicked()
{
    if(ui->SetListen->text()=="开始监听"){

    unsigned short port = ui->port->text().toUShort();
    m_s->listen(QHostAddress::Any, port);
    ui->SetListen->setText("断开连接");
    }
    else if(ui->SetListen->text()=="断开连接"){
        ui->SetListen->setText("开始监听");
        m_s->close();
    }


}


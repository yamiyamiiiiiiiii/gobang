#include "userconnection.h"



/**
 * 原定为制作多人五子棋联机平台，可获取服务端提供的房间信息和创建房间
 * 然后加入房间进行双人游戏
 *
 * 如今只实现了双人通过服务器进行联机游戏，多人连接会出现各种bug
 * 挖坑，日后填
 *
 */

UserConnection::UserConnection(QObject *parent) : QObject(parent)
{
    //断开连接时发送信号
    connect(socket, &QTcpSocket::disconnected, this, &UserConnection::slotDisconnect);

}

UserConnection::UserConnection(QTcpSocket* tcp){
    this->socket = tcp;
    this->status = FREE;
}

UserConnection::~UserConnection(){
    if(socket->state()==QTcpSocket::ConnectedState)
        socket->close();
}
//
void UserConnection::slotDisconnect(){
    emit SignalDisconnect();
}



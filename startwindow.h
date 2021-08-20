#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "GameModel.h"
#include "mainwindow.h"
#include "netplay.h"
#include <QMessageBox>

class StartWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit StartWindow(QWidget *parent = nullptr);
    void pvpLocal();
    void pvpNetwork();
    void pveLocal();
    void backStartWindow();
signals:


private slots:


private:
    QPushButton PVPLocalButton;
    QPushButton PVELocalButton;
    QPushButton PVPNetworkButton;
    MainWindow w;
    NetPlay n;
    QMessageBox message;
};

#endif // STARTWINDOW_H

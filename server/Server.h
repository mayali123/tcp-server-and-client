#pragma once
//在头文件前面加上下面几行代码
#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif
#include <QtWidgets/QMainWindow>
#include "ui_Server.h"
#include <QTcpServer>
#include <QTcpSocket> 
#include<QDebug>
#include<QTextCodec>
#include<QDateTime>
#include<Qfile>
#include<QMessageBox>
#include<QFileDialog>
#include<QTimer>
class Server : public QMainWindow
{
    Q_OBJECT

public:
    Server(QWidget *parent = Q_NULLPTR);
    
    QTcpServer myserver;
    QTcpSocket* mysocket;
    bool isFirst;
    QFile file;
public slots:
    void new_client();
    void read_data();
    
private:
    Ui::ServerClass ui;
    qint64  filesize;
    qint64 recvsize;
    QString filename;
    QString transform(qint64 filesize);
    QString save_path;
   
};

#pragma once
#ifdef WIN32  
#pragma execution_character_set("utf-8")  
#endif
#include <QtWidgets/QMainWindow>
#include "ui_socket.h"
#include <QTcpSocket> 
#include<QDebug>
#include<QTextCodec>
#include<QDateTime>
#include<Qfile>
#include<QFileDialog>
#include<QTimer>
class socket : public QMainWindow
{
    Q_OBJECT

public:
    socket(QWidget *parent = Q_NULLPTR);
    QString  hostIp;
    QTcpSocket mysocket;
    void send_head();
    void send_file();
    QString transform(qint64 filesize);
public slots:
    void read_data();
private:
    Ui::socketClass ui;
    QString filename;
    qint64 len;
    QFile file;
    qint64 send_size;
    qint64 file_size;
};

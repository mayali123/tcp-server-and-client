#include "Server.h"


Server::Server(QWidget* parent)
{
    //初始化
    isFirst = true;
    ui.setupUi(this);
    this->setFixedSize(680, 300);
    ui.progressBar->setValue(0);
    save_path = "./";
    ui.textBrowser->append("\t\t文件保存路径默认为软件所在路径下,可以设置保存路径\n");


    //通过点击启动或关闭改变监听的状态
    ui.lineEdit->setText("8080");   //设置默认端口号8080
    connect(ui.pushButton, &QPushButton::clicked, [=]() {
        if (myserver.isListening())     //如果服务器在监听
        {
            myserver.close();            //关闭服务器  
            mysocket->disconnectFromHost();//断开连接
            qDebug() << "关闭服务器";
            ui.pushButton->setText("启动");   //将按钮的文字改为启动
        }
        else
        {
            myserver.listen(QHostAddress::Any,ui.lineEdit->text().toUShort());//listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0)
            qDebug() << "开启服务器";
            ui.pushButton->setText("关闭");//将按钮的文字改为关闭
        }
        });
    connect(&myserver, &QTcpServer::newConnection, this, &Server::new_client);

    connect(ui.pushButton_2, &QPushButton::clicked, [=]() {
        save_path = QFileDialog::getExistingDirectory(this,"设置保存的位置");
        qDebug() << save_path;
        });
   
}
void Server::new_client()
{
    mysocket = myserver.nextPendingConnection();

    //显示 客户端的信息
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui.textBrowser->append("time:" + time + "  ip:" + mysocket->localAddress().toString().remove("::ffff:") + "  已连接" + ":\n");
    
    connect(mysocket,&QTcpSocket::readyRead,this,&Server::read_data);
    mysocket->write("connect");
    
}

void  Server::read_data()
{
    
    //获取信号发送者
    QTcpSocket* msocket = dynamic_cast<QTcpSocket*>(sender());
    
    //读取数据
    QByteArray allmeg = msocket->readAll();
   
    recvsize = 0;

   

    if (isFirst == true)
    {
        //分割 获取文件的基本信息
        filename = QString(allmeg).section("##", 0, 0);
        filesize = QString(allmeg).section("##",1,1).toInt();


        //在 textBrowser 显示信息
       
        ui.textBrowser->append(QString("文件名:%1 文件大小:%2\n").arg(filename).arg(transform(filesize)));
        qDebug() << "filename:" << filename << "\tfilesize:" << filesize<<endl;
        //qDebug() << allmeg<<endl;
        


        //在服务器电脑上创建一个 filename 文件
        file.setFileName(QString("%1/%2").arg(save_path).arg(filename));
        bool isOk = file.open(QIODevice::WriteOnly);
        qDebug() <<isOk;

        
        if (QMessageBox::Yes ==QMessageBox::information(this, "传输文件", QString("文件名:%1 文件大小:%2 \n是否接收").arg(filename).arg(transform(filesize)), QMessageBox::Yes | QMessageBox::No))
        {
            //给 客户端 发送一个 开始 传输文件的信号
            mysocket->write("start");
            isFirst = false;
        }
        else 
        { 
            mysocket->write("refuse"); //给 客户端 发送一个 拒绝传输文件的信号
            file.remove();
        }
      
    }
    else
    {
         //qDebug() << "开始写入\n";
         //timer->start(200);
         file.write(allmeg);
         
         ui.progressBar->setValue((1.0*file.size())/ filesize *100);
         
         if(file.size()== filesize)
         {
             //qDebug() << "传输完成" << endl;
             //qDebug() << file.size() <<endl<< filesize;
             ui.textBrowser->append(QString("%1已经传输完毕,保存在 %2\n").arg(filename).arg(save_path));
             mysocket->write("over");

             //timer->stop();

             //重新初始化 
             isFirst = true;
             file.close();
         }
             
        
    }
}


QString  Server::transform(qint64 filesize)
{
    QString unit[] = { "bit","K","M","G","T" };
    int index = 0;
    qint64 i = filesize;
    while (i > 1024)
    {
        i /= 1024;
        index++;
    }
    return QString("%1 %2").arg(i).arg(unit[index]);
}

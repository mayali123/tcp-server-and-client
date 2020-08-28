#include "socket.h"
socket::socket(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setFixedSize(680, 340);
    ui.progressBar->setValue(0);
    ui.pushButton_2->setEnabled(false);
    connect(ui.pushButton_2, &QPushButton::clicked, [=]() {
        filename=QFileDialog::getOpenFileName(this,"选择要发送的文件");

        //打开文件
        file.setFileName(filename);
        bool isOk=file.open(QIODevice::ReadOnly);

        if (!isOk)
            qDebug() << "open fail";
        if(file.size())
            send_head();
        });

    connect(ui.pushButton, &QPushButton::clicked,[=]() {
        if(QAbstractSocket::UnconnectedState ==mysocket.state())
        { 
            mysocket.connectToHost(ui.lineEdit->text(), ui.lineEdit_2->text().toUShort());
            hostIp= ui.lineEdit->text();
            ui.pushButton->setText("断开");
            qDebug() << "链接";
            //mysocket.write("链接");
            ui.pushButton_2->setEnabled(true);
        }
        else
        {
            mysocket.disconnectFromHost();
            ui.pushButton->setText("连接");
        }
       
    });
   
    connect(&mysocket, &QTcpSocket::readyRead, this, &socket::read_data);

    connect(ui.pushButton_3, &QPushButton::clicked, [=]() {
        this->close();
        });
}


void socket::send_head()
{
    QFileInfo fileinfo(filename);
    qDebug() << fileinfo.size()<<endl;
    QString head = QString("%1##%2##").arg(fileinfo.fileName()).arg(fileinfo.size());
    file_size = fileinfo.size();
    qDebug() << file_size;
    mysocket.write(head.toUtf8());
    
   

}

void socket::send_file()
{

    len = 0;
    send_size = 0;
    //发信息
    do
    {

        char send_date[4 * 1024] = { 0 };

        //读文件
        len = file.read(send_date, sizeof(send_date));


        //发送
        len = mysocket.write(send_date, len);

        //
        send_size += len;


        ui.progressBar->setValue((send_size*1.0) / file_size * 100);

       //qDebug() << send_size;
    } while (len > 0);

}

void socket::read_data()
{
    QString data = mysocket.readAll();
    if (data == "start")
    { 
        qDebug() << "开始";
        ui.textBrowser->append(QString("%1 开始传输\n").arg(filename));
        send_file();
    }
    else if (data=="refuse")
    {
        ui.textBrowser->append(QString("客户端拒绝接收 %1\n").arg(filename));
        
    }
    else if (data == "over")
    {
        ui.textBrowser->append(QString("%1 传输完成\n").arg(filename));
       
    }
    else if (data == "connect")
    {
        ui.textBrowser->append(QString("已经与 ip:%1 连接\n").arg(ui.lineEdit->text()));
    }
}

QString  socket::transform(qint64 filesize)
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
//192.168.43.98
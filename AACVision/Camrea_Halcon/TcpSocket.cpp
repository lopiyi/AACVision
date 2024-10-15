#include "TcpSocket.h"
#include <QSettings>
#include <QTextCodec>

TcpSocket::TcpSocket(Common::CameraHalconData& data, QTcpSocket* socket):ip(data.IP), port(data.Port)
{
    this->socket.reset(socket);
}

TcpSocket::~TcpSocket()
{
    //if (socket)
    //{
    //    socket->close();
    //}
}

void TcpSocket::updateData(Common::CameraHalconData& data)
{
    ip = data.IP;
    port = data.Port;
}

QByteArray TcpSocket::Socket_Write_Read_Data(const QByteArray& buffer)
{
    QByteArray bufferserver = "";
    try
    {
        //if (QAbstractSocket::ConnectedState != socket->state())
        //{
        //    emit emitToCamera(u8"�����ж�, ����һ��");
        //    socket->abort();        //���ӷ�����,�˺��������ر��׽��֣�����д�������е��κι������ݡ�
        //    socket->connectToHost(data.IP, data.Port); //���ӷ�����
        //    socket->waitForConnected(3000);
        //}
        if (QAbstractSocket::ConnectedState == socket->state())
        {
            socket->write(buffer);
            socket->waitForBytesWritten(100);
            socket->waitForReadyRead(1000);
            bufferserver = socket->readAll();//��õ���assic��
            bufferserver = bufferserver.toHex().toUpper();
            if (bufferserver == "")
            {
                int againnumber = 1;
                bool againsuccess = false;
                while ((!againsuccess) && (againnumber < 3))
                {
                    bufferserver.clear();

                    if (QAbstractSocket::ConnectedState == socket->state())
                    {
                        socket->write(buffer);
                        socket->waitForBytesWritten(100);
                        socket->waitForReadyRead(1000);
                        bufferserver = socket->readAll();//��õ���assic��
                        bufferserver = bufferserver.toHex().toUpper();
                        emitToCamera(" write_socket_port write again" + QString::number(againnumber));
                        if (bufferserver != "")
                        {
                            againsuccess = true;
                        }
                    }
                    againnumber++;
                }

            }
        }
        else {
            emitToCamera(u8"TCP�����ж�");
        }
        return bufferserver;
    }
    catch (QException& e)
    {
        QString info =  QString("Socket_Write_Read_Data  failed!")+e.what();
        qDebug() << info;
        emitToCamera(info);
        
    }
    return bufferserver;
}

bool TcpSocket::Plc_Translate_ReadMessage(QByteArray readmessage, int data[])
{
    /*************************����****************************/
    //��Ӧ��ʽΪ  D0 00 00 FF FF 03 00 06 00 00 00  +
    if (readmessage.mid(18, 4) == "0000")
    {
        //��ȡ��Ӧ���ݳ���
        QByteArray length = "";
        length.append(readmessage.mid(16, 2));
        length.append(readmessage.mid(14, 2));
        int alength = length.toInt(Q_NULLPTR, 16);
        //for (int i = 0; i < alength; i++)
        //{
        //    QByteArray readdata = "";
        //    readdata.append(readmessage.mid(24 + i * 4, 2));
        //    readdata.append(readmessage.mid(22 + i * 4, 2));
        //    data[i] = readdata.toInt(Q_NULLPTR, 16);
        //    return true;
        //}
        QByteArray readdata = "";
        readdata.append(readmessage.mid(24 + 0 * 4, 2));
        readdata.append(readmessage.mid(22 + 0 * 4, 2));
        bool ok;
        int a = readdata.toInt(&ok, 16);
        data[0] = a;
        return ok;
    }
    else
    {
        return false;
        qDebug() << "melesec plc readmessage fail,maybe TCP/IP fail";
    }
}

QByteArray TcpSocket::Socket_Write_Read_Data1(const QByteArray& buffer)
{
    QByteArray bufferserver = "";
    try
    {
        //if (QAbstractSocket::ConnectedState != socket->state())
        //{
        //    emit emitToCamera(u8"�����ж�, ����һ��");
        //    socket->abort();        //���ӷ�����,�˺��������ر��׽��֣�����д�������е��κι������ݡ�
        //    socket->connectToHost(data.IP, data.Port); //���ӷ�����
        //    socket->waitForConnected(3000);
        //}
        if (QAbstractSocket::ConnectedState == socket->state())
        {
            socket->write(buffer);
            socket->waitForBytesWritten(100);
            socket->waitForReadyRead(1000);
            bufferserver = socket->readAll();//��õ���assic��
            if (bufferserver == "")
            {
                int againnumber = 1;
                bool againsuccess = false;
                while ((!againsuccess) && (againnumber < 6))
                {
                    bufferserver.clear();

                    if (QAbstractSocket::ConnectedState == socket->state())
                    {
                        socket->write(buffer);
                        socket->waitForBytesWritten(100);
                        socket->waitForReadyRead(1000);
                        bufferserver = socket->readAll();//��õ���assic��
                        emitToCamera(" write_socket_port write again" + QString::number(againnumber));
                        if (bufferserver != "")
                        {
                            againsuccess = true;
                        }
                    }
                    againnumber++;
                }

            }
        }
        else {
            emitToCamera(u8"TCP�����ж�");
        }
        return bufferserver;
    }
    catch (QException& e)
    {
        QString info = QString("Socket_Write_Read_Data  failed!") + e.what();
        qDebug() << info;
        emitToCamera(info);

    }
    return bufferserver;
}

void TcpSocket::reconnect()
{
}
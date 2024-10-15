#pragma once
#include <qobject.h>
#include <QTcpSocket>
#include <QException>
#include <memory>
#include <qstring.h>
#include "device_communication.h"
#include "common.hpp"

class TcpSocket:public QObject
{
	Q_OBJECT
public:
	TcpSocket(Common::CameraHalconData& data, QTcpSocket* socket);
	~TcpSocket();
	QByteArray Socket_Write_Read_Data(const QByteArray& buffer);
	QByteArray Socket_Write_Read_Data1(const QByteArray& buffer);
	bool Plc_Translate_ReadMessage(QByteArray readmessage, int data[]);
	void updateData(Common::CameraHalconData& data);
private:
	void reconnect();
signals:
	void emitToCamera(const QString str);
private:
	std::shared_ptr<QTcpSocket> socket=nullptr;
	QString ip;
	int port;
};


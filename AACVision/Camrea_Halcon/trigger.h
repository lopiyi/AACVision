#pragma once

#include <QObject>
#include "common.hpp"
#include "TcpSocket.h"
#include <QTimer>

class Trigger  : public QObject
{
	Q_OBJECT

public:
	Trigger(Common::CameraHalconData& data, TcpSocket* socket, QObject *parent);
	~Trigger();
	bool init();
	void start();
	void stop();
	void updateData(Common::CameraHalconData& data);
private slots:
	void onTimeOut();
signals:
	void emitPicNumberToCamera(const int);
	void emitMessageToCamera(const QString);
private:
	TcpSocket* socket = nullptr;	// tcp
	std::shared_ptr<QTimer> tim = nullptr;		// ∂® ±∆˜£¨…®√ËPLC¥•∑¢≈ƒ’’
	QString info;
	int camID, ScanningCycle, CamplcGrabmark;
};

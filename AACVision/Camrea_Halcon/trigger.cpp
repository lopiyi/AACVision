#include "trigger.h"
#include "simpleQtLogger.h"

Trigger::Trigger(Common::CameraHalconData& data, TcpSocket* socket, QObject *parent)
	: QObject(parent), socket(socket), camID(data.camID), ScanningCycle(data.ScanningCycle), CamplcGrabmark(data.CamplcGrabmark)
{
	tim.reset(new QTimer(this));
	connect(tim.get(), SIGNAL(timeout()), this, SLOT(onTimeOut()));
}

Trigger::~Trigger()
{}

void Trigger::updateData(Common::CameraHalconData& data)
{
	camID = data.camID;
	ScanningCycle = data.ScanningCycle;
	CamplcGrabmark = data.CamplcGrabmark;
}

bool Trigger::init()
{
	info = QString("CCD%1:").arg(camID);
	tim->setInterval(ScanningCycle);
	return true;
}

void Trigger::start()
{
	//LS_INFO(info + QString::fromUtf8(u8"启动扫描"));
	if (!tim->isActive())
	{
		tim->start();
	}
}

void Trigger::stop()
{
	//LS_INFO(info + QString::fromUtf8(u8"停止扫描"));
	if (tim->isActive()) {
		tim->stop();
	}
}

void Trigger::onTimeOut()
{
	try
	{
		tim->stop();
		int data_mark[2] = { 0,10 };//存放吸嘴号数据

		////*****************读取吸嘴号（区分是第几次拍照）*****************
		QByteArray command, receive, commandLog;
		command = SanLing_PLC::MC_Bin3E_read_message("D", CamplcGrabmark, 1, &commandLog);  //从MW20010中获取拍照信号和当前颗数
		//LS_INFO(info + QString::fromUtf8(u8"生成报文"));
		receive = socket->Socket_Write_Read_Data(command);
		//LS_INFO(info + QString::fromUtf8(u8"读取取图标识"));
		bool Plc_Translatesuccess = socket->Plc_Translate_ReadMessage(receive, data_mark);
		//LS_INFO(info + QString::fromUtf8(u8"翻译取图标识"));
		if (!Plc_Translatesuccess)
		{
			emitMessageToCamera(u8"翻译失败");
		}
		if (!Plc_Translatesuccess)//如果翻译失败；则重新读一次数据
		{
			receive = socket->Socket_Write_Read_Data(command);
			socket->Plc_Translate_ReadMessage(receive, data_mark);
		}

		if (data_mark[0])
		{
			//LS_INFO(info + QString::fromUtf8(u8"发送图号%1").arg(data_mark[0]));
			emit emitPicNumberToCamera(data_mark[0]);
		}
		else {
			tim->start();
		}
		//LS_INFO(info + QString::fromUtf8(u8"扫描一次"));
	}
	catch (...)
	{
		emitMessageToCamera(u8"PLC扫描错误");
	}
	
}

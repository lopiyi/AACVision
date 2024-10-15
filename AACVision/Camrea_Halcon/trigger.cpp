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
	//LS_INFO(info + QString::fromUtf8(u8"����ɨ��"));
	if (!tim->isActive())
	{
		tim->start();
	}
}

void Trigger::stop()
{
	//LS_INFO(info + QString::fromUtf8(u8"ֹͣɨ��"));
	if (tim->isActive()) {
		tim->stop();
	}
}

void Trigger::onTimeOut()
{
	try
	{
		tim->stop();
		int data_mark[2] = { 0,10 };//������������

		////*****************��ȡ����ţ������ǵڼ������գ�*****************
		QByteArray command, receive, commandLog;
		command = SanLing_PLC::MC_Bin3E_read_message("D", CamplcGrabmark, 1, &commandLog);  //��MW20010�л�ȡ�����źź͵�ǰ����
		//LS_INFO(info + QString::fromUtf8(u8"���ɱ���"));
		receive = socket->Socket_Write_Read_Data(command);
		//LS_INFO(info + QString::fromUtf8(u8"��ȡȡͼ��ʶ"));
		bool Plc_Translatesuccess = socket->Plc_Translate_ReadMessage(receive, data_mark);
		//LS_INFO(info + QString::fromUtf8(u8"����ȡͼ��ʶ"));
		if (!Plc_Translatesuccess)
		{
			emitMessageToCamera(u8"����ʧ��");
		}
		if (!Plc_Translatesuccess)//�������ʧ�ܣ������¶�һ������
		{
			receive = socket->Socket_Write_Read_Data(command);
			socket->Plc_Translate_ReadMessage(receive, data_mark);
		}

		if (data_mark[0])
		{
			//LS_INFO(info + QString::fromUtf8(u8"����ͼ��%1").arg(data_mark[0]));
			emit emitPicNumberToCamera(data_mark[0]);
		}
		else {
			tim->start();
		}
		//LS_INFO(info + QString::fromUtf8(u8"ɨ��һ��"));
	}
	catch (...)
	{
		emitMessageToCamera(u8"PLCɨ�����");
	}
	
}

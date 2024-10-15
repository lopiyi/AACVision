#pragma once

#include <QObject>
#include <HALCONCpp/HalconCpp.h>
#include "hdevengine/HDevEngineCpp.h"
#include "common.hpp"
#include "TcpSocket.h"
#include "qhalconwindow.h"
#include "DrawItemHalcon.h"
#include <qreadwritelock.h>

using namespace HalconCpp;
using namespace HDevEngineCpp;

class ImageProcess  : public QObject
{
	Q_OBJECT

public:
	ImageProcess(Common::CameraHalconData data, TcpSocket* socket, QHalconWindow* hwin, Common::ShareData* sharedata, QObject *parent);
	~ImageProcess();
	bool init();
	bool run(HObject& image);
	void updateData(Common::CameraHalconData data);
signals:
	void emitMessageToCamera(const QString);
	void emitToUpdataStatistics(const int pic, const bool okng, const bool finnalPic);
private:
	void initData();
	bool HdevEngineinit();//halcon引擎初始化
	void receiveValuePrecess();
	void saveOriginalImage(const QDateTime time, HObject& image);
	void saveResultImage(const QDateTime time);
	void writeValue2PLC();
	bool HImage2QImage(HalconCpp::HImage& from, QImage& to);
	void genResultQImage(HObject& image);
	void showHalconWindow(HObject& image);
	void genEmitData();
	//QString youXiaoShuZi(double v, int n);
public:
	int picID = 0;
private:
	QString info;
	Common::CameraHalconData data;
	Common::PicData picdata;
	TcpSocket* socket = nullptr;	// tcp
	
	std::shared_ptr<HDevProcedure> m_procedure;//halcon有引擎程序
	std::shared_ptr<HDevProcedureCall> m_procedureCall;//引擎响应函数

	//HObject image;//接收到的采集到的图像
	HObject region;
	QImage resultImg;
	QVector<HObject> imageResult;
	//QReadWriteLock m_lock;

	QVector<HTuple> input, output;
	int pic_result[100] = { 2, 0 };	// 写入PLC的数组
	QVector<int> stringValueIndex;
	int showHalconString = 0;
	int showHalconValue = 0;
	int OKNG_flag = 1;
	QVector<int> ok_value_list;

	DrawItemHalcon m_draw;
	QHalconWindow* hwin;
	Common::ShareData* sharedata;
	
	const int iniHeight = 400;
	const int bias = 200;
	bool okng = false;
};

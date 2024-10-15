#pragma once

#include <QObject>
#include "common.hpp"
#include <HALCONCpp/HalconCpp.h>
#include "hdevengine/HDevEngineCpp.h"

using namespace HalconCpp;
using namespace HDevEngineCpp;

class TakePicHalcon  : public QObject
{
	Q_OBJECT

public:
	TakePicHalcon(Common::CameraHalconData& data, QObject *parent);
	~TakePicHalcon();
	bool init();
	bool take(HImage& image);
	bool takeContinueShow(HImage& image);
	void updateData(Common::CameraHalconData& data);
signals:
	void emitMessageToCamera(const QString str);
private:
	QString info;
	HTuple hv_AcqHandle;	// halcon相机句柄
	bool isOpen = false;		// 相机是否打开的标志
	bool continueShow = false;	// 是否连续显示的标志
	bool ok = true;		// 取图成功的标志

	QString DeviceName, TriggerSource;
	int CamExposuretime;
};

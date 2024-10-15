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
	HTuple hv_AcqHandle;	// halcon������
	bool isOpen = false;		// ����Ƿ�򿪵ı�־
	bool continueShow = false;	// �Ƿ�������ʾ�ı�־
	bool ok = true;		// ȡͼ�ɹ��ı�־

	QString DeviceName, TriggerSource;
	int CamExposuretime;
};

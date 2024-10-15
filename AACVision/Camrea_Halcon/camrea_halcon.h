#pragma once

#include "camrea_halcon_global.h"
#include "CameraFactory.h"
#include "CameraHalconInstance.h"

class CAMREA_HALCON_EXPORT Camrea_Halcon:public CameraFactory
{
	Q_OBJECT
		Q_PLUGIN_METADATA(IID "com.kdr2.CameraFactory");
	Q_INTERFACES(CameraFactory);
public:
	Camrea_Halcon();
	virtual QString about() override;
	virtual bool init() override;
	virtual CameraPluginAbstract* getCamera(int k) override;
	virtual QVector<MyAction*> getActions() override;
	virtual void setLog(void* v);
signals:
	void emitMessagesToUI(const QString) override;
private:
	QString info;
	QVector<MyAction*> actions;
	//std::shared_ptr<HDevEngine> m_engine; //halconÒýÇæ
};

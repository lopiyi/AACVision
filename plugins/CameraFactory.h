#pragma once
#include <QtCore/qstring.h>
#include <qobject.h>
#include "camera_plugin.h"
#include "my_action.h"

class CameraFactory :public QObject
{
public:
	CameraFactory() {}
	virtual~CameraFactory() {}
	virtual QString about() = 0;
	virtual bool init() = 0;
	virtual CameraPluginAbstract* getCamera(int k) = 0;
	virtual QVector<MyAction*> getActions() = 0;
	virtual void emitMessagesToUI(const QString) = 0;
	virtual void setLog(void* v) = 0;
public:

};
Q_DECLARE_INTERFACE(CameraFactory, "com.kdr2.CameraFactory");
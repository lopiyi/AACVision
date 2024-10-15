#pragma once

#include <qobject.h>
#include "my_widget.h"
#include "my_action.h"
#include "common.hpp"

class CameraPluginAbstract:public QObject
{
public:
	CameraPluginAbstract(){}
	virtual ~CameraPluginAbstract(){}
	virtual MyWidget* getCameraUI() = 0;
	virtual QWidget* getStatisticsUI() = 0;
	virtual QVector<MyAction*> getActions() = 0;
	virtual bool isOk() = 0;
	virtual void init() = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void setValue(const QString flag, void*v) = 0;
	virtual void signalReflection(const QString) = 0;
	virtual void emitMessagesToUI(const QString str) = 0;
	virtual void emitValueToUI(const QString flag, void*v) = 0;
	virtual void slotFromUI(const QString flag, void*v) = 0;
	virtual void slotReflection(const QString) = 0;
public:
	static Common::ShareData share_data;
	int camID;
};
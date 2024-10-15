#pragma once

#include <qobject.h>
#include "my_widget.h"
#include "my_action.h"

class PluginFunctionAbstract :public QObject
{
public:
	virtual ~PluginFunctionAbstract() {}
	virtual QString about() = 0;
	virtual MyWidget* getUI() = 0;
	virtual QVector<MyAction*> getActions() = 0;
	virtual void init() = 0;
	virtual void setLog(void* v) = 0;
	virtual void emitMessagesToUI(const QString str) = 0;
};
Q_DECLARE_INTERFACE(PluginFunctionAbstract, "com.kdr2.PluginFunctionAbstract");
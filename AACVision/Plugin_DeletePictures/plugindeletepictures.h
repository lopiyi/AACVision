#pragma once

#include "plugin_deletepictures_global.h"
#include "plugins_function.h"
#include <qtimer.h>
#include "setting.h"

class PLUGIN_DELETEPICTURES_EXPORT PluginDeletePictures:public PluginFunctionAbstract
{
	Q_OBJECT
		Q_PLUGIN_METADATA(IID "com.kdr2.PluginFunctionAbstract");
	Q_INTERFACES(PluginFunctionAbstract);
public:
    PluginDeletePictures();
	virtual ~PluginDeletePictures(){}
	virtual QString about() override;
	virtual MyWidget* getUI() override;
	virtual QVector<MyAction*> getActions() override;
	virtual void init() override;
	virtual void setLog(void* v) override;
signals:
	void emitMessagesToUI(const QString str) override;
private slots:
	void getMes(const QString str);
	void onTimeOut();
private:
	
private:
	QTimer *tim = nullptr;
	Setting* set = nullptr;
	QVector<MyAction*> actions;
};

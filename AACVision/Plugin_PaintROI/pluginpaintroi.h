#pragma once

#include "plugin_paintroi_global.h"
#include "plugins_function.h"
#include "paintui.h"

class PLUGIN_PAINTROI_EXPORT PluginPaintROI :public PluginFunctionAbstract
{
	Q_OBJECT
		Q_PLUGIN_METADATA(IID "com.kdr2.PluginFunctionAbstract");
	Q_INTERFACES(PluginFunctionAbstract);
public:
	PluginPaintROI();
	virtual QString about() override;
	virtual MyWidget* getUI() override;
	virtual QVector<MyAction*> getActions() override;
	virtual void init() override;
	virtual void setLog(void* v) override;
signals:
	void emitMessagesToUI(const QString str) override;
public:
    
private:
	QVector<MyAction*> actions;
	PaintUI* paint;
};

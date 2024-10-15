#include "pluginpaintroi.h"

PluginPaintROI::PluginPaintROI():paint(nullptr)
{
}

QString PluginPaintROI::about()
{
    return QString();
}

MyWidget* PluginPaintROI::getUI()
{
    return nullptr;
}

QVector<MyAction*> PluginPaintROI::getActions()
{
	MyAction* act_paint = new MyAction(QString(u8"¹¦ÄÜ/»­ROI"));
	connect(act_paint, &QAction::triggered, [this]() {
		if (!paint)
		{
			paint = new PaintUI;
		}
		paint->showNormal();
		});
	actions.append(act_paint);
    return actions;
}

void PluginPaintROI::init()
{
}

void PluginPaintROI::setLog(void* v)
{
}

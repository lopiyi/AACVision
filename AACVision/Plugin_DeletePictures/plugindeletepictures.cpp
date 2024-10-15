#include "plugindeletepictures.h"
#include "simpleQtLogger.h"
#include <QSettings>
#include <QTextCodec>

PluginDeletePictures::PluginDeletePictures()
{
}

QString PluginDeletePictures::about()
{
	return QString();
}

MyWidget* PluginDeletePictures::getUI()
{
	return nullptr;
}

QVector<MyAction*> PluginDeletePictures::getActions()
{
	MyAction* setting = new MyAction(QString(u8"����/��ʱɾ�ļ�"));
	connect(setting, &QAction::triggered, [this]() {
		if (!set)
		{
			set = new Setting;
			connect(set, &Setting::emitMes, this, &PluginDeletePictures::getMes);
		}
		set->show();
		});
	actions.append(setting);
	return actions;
}

void PluginDeletePictures::init()
{
	set = new Setting;
	connect(set, &Setting::emitMes, this, &PluginDeletePictures::getMes);
	const QString path = QString(u8"./settings/��ʱɾͼƬ.ini");
	QSettings setting(path, QSettings::IniFormat);
	setting.setIniCodec(QTextCodec::codecForName("utf-8"));

	setting.beginGroup("Main");
	int T = setting.value("ScanCycle").toInt();
	T = T > 0 ? T : 6;
	setting.endGroup();
	tim = new QTimer(this);
	tim->setInterval(T * 60 * 60*1000);
	connect(tim, SIGNAL(timeout()), this, SLOT(onTimeOut()));
	tim->start();
	emitMessagesToUI(QString(u8"��ʱɾ�ļ�����������ÿ��%1Сʱɨ��һ��").arg(T));
}

void PluginDeletePictures::setLog(void* v)
{
	simpleqtlogger::SimpleQtLogger::setSimpleQtLogger(static_cast<simpleqtlogger::SimpleQtLogger*>(v));
}

void PluginDeletePictures::getMes(const QString str)
{
	emit emitMessagesToUI(str);
}

void PluginDeletePictures::onTimeOut()
{
	emitMessagesToUI(u8"��ʼɾ�������ļ�");
	set->pb_start_clicked();
}


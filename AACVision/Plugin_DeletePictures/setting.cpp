#include "setting.h"
#include <QSettings>
#include <QTextCodec>
#include <QDir>
#include <QDateTime>
#include <qdebug.h>
#include "simpleQtLogger.h"
#include <QShortcut>
#include <qthread.h>

Setting::Setting(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	//setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
	QShortcut* shortcut1 = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	QObject::connect(shortcut1, &QShortcut::activated, this, &QWidget::close);
	vlay = new QVBoxLayout(ui.scrollArea->widget());
	ui.scrollArea->widget()->setLayout(vlay);
	connect(ui.pb_add, &QPushButton::clicked, this, &Setting::addPath);
	connect(ui.pb_sub, &QPushButton::clicked, this, &Setting::subPath);
	connect(ui.pb_save, &QPushButton::clicked, this, &Setting::pb_save_clicked);
	connect(ui.pb_start, &QPushButton::clicked, this, &Setting::pb_start_clicked);
	loadConfig();
	qRegisterMetaType<QList<quint16> >("QVector<double>");
}

Setting::~Setting()
{
	
}

void Setting::addPath()
{
	pathChanged(true);
}

void Setting::subPath()
{
	pathChanged(false);
}

void Setting::pathChanged(const bool add)
{
	if (add)
	{
		path_num += 1;
		std::shared_ptr<PathUI> w = std::make_shared<PathUI>(path_num);
		vlay->addWidget(w.get());
		pathuis.append(w);
	}
	else {
		if (path_num<=0)
		{
			return;
		}
		path_num -= 1;
		auto last = pathuis.takeLast();
		vlay->removeWidget(last.get());
	}
}

void Setting::pb_save_clicked()
{
	paths.clear();
	days.clear();
	for (auto p : pathuis)
	{
		paths << p->getPath();
		days << p->getDay();
	}
	saveConfig();
}

void Setting::pb_start_clicked()
{
	pb_save_clicked();
	if (!del)
	{
		del = new DeleteFiles;
		connect(this, &Setting::emitDel, del, &DeleteFiles::run);
		QThread* thr = new QThread;
		del->moveToThread(thr);
		thr->start();
	}
	emit emitDel(paths, days);
	last_run_time = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss.zzz");
	const QString path = QString(u8"./settings/定时删图片.ini");
	QSettings setting(path, QSettings::IniFormat);
	setting.setIniCodec(QTextCodec::codecForName("utf-8"));
	setting.beginGroup("Main");
	setting.setValue("last_run_time", last_run_time);
	setting.endGroup();
}

void Setting::saveConfig()
{
	const QString path = QString(u8"./settings/定时删图片.ini");
	QSettings setting(path, QSettings::IniFormat);
	setting.setIniCodec(QTextCodec::codecForName("utf-8"));
	setting.beginGroup("Main");
	setting.setValue("last_run_time", last_run_time);
	setting.setValue("path_num", path_num);
	setting.setValue("ScanCycle", ui.spinBox->value());
	for (int i = 0; i < path_num; i++)
	{
		setting.setValue(QString("path%1").arg(i + 1), paths.at(i));
		setting.setValue(QString("day%1").arg(i + 1), days.at(i));
	}
	setting.endGroup();
}

void Setting::loadConfig()
{
	const QString path = QString(u8"./settings/定时删图片.ini");
	QSettings setting(path, QSettings::IniFormat);
	setting.setIniCodec(QTextCodec::codecForName("utf-8"));

	setting.beginGroup("Main");
	last_run_time = setting.value("last_run_time").toString();
	int num = setting.value("path_num").toInt();
	int T = setting.value("ScanCycle").toInt();
	ui.spinBox->setValue(T > 0 ? T : 6);
	paths.clear();
	days.clear();
	for (int i = 0; i < num; i++)
	{
		pathChanged(true);
		auto p = setting.value(QString("path%1").arg(i + 1)).toString();
		paths << p;
		pathuis[i]->setPath(p);
		auto d = setting.value(QString("day%1").arg(i + 1)).toDouble();
		days << d;
		pathuis[i]->setDay(d);
	}
	setting.endGroup();
}

void Setting::deleteOldImages(const QString& dirPath, int day) {
	//QDir dir(dirPath);
	//if (!dir.exists() || !dir.isReadable()) {
	//	emit emitMes(u8"目录不存在：" + dirPath);
	//	return;
	//}
	////QDateTime start = QDateTime::fromString(last_run_time, "yyyy-MM-dd hh:mm:ss");
	//auto currentTime = QDateTime::currentDateTime();
	//foreach(const QFileInfo& entry, dir.entryInfoList(QDir::Files | QDir::Dirs| QDir::NoDotAndDotDot)) {
	//	//qDebug() << entry.absoluteFilePath();
	//	if (entry.isFile()) {
	//		QFile file(entry.absoluteFilePath());
	//		if (file.open(QIODevice::ReadOnly)) {
	//			QDateTime creationTime = entry.birthTime();
	//			//qDebug() << creationTime;
	//			double s = creationTime.secsTo(currentTime)/86400;
	//			if (s > day) { // 1 month
	//				//emit emitMes(entry.absoluteFilePath());
	//				LS_INFO(QString::fromUtf8(u8"删除文件：")+ entry.absoluteFilePath());
	//				file.remove();
	//			}
	//			file.close();
	//		}
	//		else {
	//			emit emitMes(u8"文件打开失败："+entry.absoluteFilePath());
	//		}
	//	}
	//	else if (entry.isDir()) {
	//		deleteOldImages(entry.path()+"/"+entry.baseName(), day); 
	//	}
	//	else {
	//		emit emitMes(u8"路径有误："+entry.absoluteFilePath());
	//	}
	//}
}
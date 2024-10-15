#include "deletefiles.h"
#include <QSettings>
#include <QTextCodec>
#include <QDir>
#include <QDateTime>
#include "simpleQtLogger.h"
#include <QDebug>

DeleteFiles::DeleteFiles(QObject *parent)
	: QObject(parent)
{
}

DeleteFiles::~DeleteFiles()
{}

void DeleteFiles::run(const QStringList& paths, QVector<double> days)
{
	for (int i = 0; i < paths.size(); i++)
	{
		deleteOldImages(paths.at(i), days.at(i));
	}
}

void DeleteFiles::deleteOldImages(const QString& dirPath, double day)
{
	QDir dir(dirPath);
	if (!dir.exists() || !dir.isReadable()) {
		return;
	}
	//QDateTime start = QDateTime::fromString(last_run_time, "yyyy-MM-dd hh:mm:ss");
	auto currentTime = QDateTime::currentDateTime();
	foreach(const QFileInfo & entry, dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot)) {
		//qDebug() << entry.absoluteFilePath();
		if (entry.isFile()) {
			QFile file(entry.absoluteFilePath());
			if (file.open(QIODevice::ReadOnly)) {
				QDateTime creationTime = entry.birthTime();
				//qDebug() << creationTime;
				double s = creationTime.secsTo(currentTime) / 86400;
				if (s > day) { // 1 month
					//emit emitMes(entry.absoluteFilePath());
					LS_INFO(QString::fromUtf8(u8"删除文件：") + entry.absoluteFilePath());
					file.remove();
				}
				file.close();
			}
			else {
				//emit emitMes(u8"文件打开失败：" + entry.absoluteFilePath());
			}
		}
		else if (entry.isDir()) {
			deleteOldImages(entry.path() + "/" + entry.baseName(), day);
		}
		else {
			//emit emitMes(u8"路径有误：" + entry.absoluteFilePath());
		}
	}
}
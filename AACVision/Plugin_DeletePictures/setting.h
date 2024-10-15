#pragma once

#include <QWidget>
#include "ui_setting.h"
#include "pathui.h"
#include <memory>
#include <QVBoxLayout>
#include "deletefiles.h"

class Setting : public QWidget
{
	Q_OBJECT

public:
	Setting(QWidget *parent = nullptr);
	~Setting();
public slots:
	void addPath();
	void subPath();
	void pb_save_clicked();
	void pb_start_clicked();
signals:
	void emitMes(const QString str);
	void emitDel(const QStringList& paths, QVector<double> day);
private:
	void pathChanged(const bool add);
	void saveConfig();
	void loadConfig();
	void deleteOldImages(const QString& dirPath, int day=999);
private:
	Ui::SettingClass ui;
	DeleteFiles* del = nullptr;
	QVBoxLayout* vlay = nullptr;

	int path_num = 0;
	QVector<std::shared_ptr<PathUI>> pathuis;

	QString last_run_time;
	QStringList paths;
	QVector<double> days;
};

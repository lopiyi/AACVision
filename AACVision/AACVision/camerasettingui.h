#pragma once

#include <QWidget>
#include "ui_camerasettingui.h"
#include <QFormLayout>

class CameraSettingUI : public QWidget
{
	Q_OBJECT

public:
	CameraSettingUI(QWidget *parent = nullptr);
	~CameraSettingUI();
private slots:
	void addCamera();
	void subCamera();
	void pb_save_clicked();
	void sb_value_changed(int v);
private:
	void cameraChanged(const bool add);
	void loadConfig();
	void initUI();
private:
	Ui::CameraSettingUIClass ui;
	QFormLayout* layout = nullptr;
	
	int cam_num = 0;
	QStringList camSDK;
	int vertical = 1;
	QStringList sdks;
};

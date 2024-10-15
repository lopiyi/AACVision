#pragma once

#include <QWidget>
#include "ui_camerasettingui.h"
#include "picsettingui.h"
#include "common.hpp"
#include "qhalconwindow.h"
#include <QThread>

class ShowImage:public QObject
{
	Q_OBJECT
public:
	ShowImage(QHalconWindow* hwin):hwin(hwin){}
	~ShowImage(){
		if (open)
		{
			CloseFramegrabber(hv_AcqHandle);
		}
	}
public slots:
	void start(QString device, int CamExposuretime);
	void stop();
signals:
	void message(const QString);
private:
	QHalconWindow* hwin = nullptr;
	bool show, open=false;
	HTuple hv_AcqHandle;	// halconÏà»ú¾ä±ú
};


class CameraSettingUI : public QWidget
{
	Q_OBJECT

public:
	CameraSettingUI(Common::CameraHalconData& data, QWidget *parent = nullptr);
	~CameraSettingUI();

	bool init();
private slots:
	void getDevices();
	void picNumChanged(int v);
	void slot_pb_save();
	void slot_pb_open();
	void showMessage(const QString str);
private:
	bool eventFilter(QObject* watched, QEvent* event);
	void iniUI();
	void saveData();
	void saveConfig();
signals:
	void continueShow(QString device, int CamExposuretime);
	void stopShow();
private:
	Ui::CameraSettingUIClass ui;
	Common::CameraHalconData& data;
	QHalconWindow* hwin = nullptr;
	std::shared_ptr<QThread> show_thread = nullptr;
	std::shared_ptr< ShowImage> showimage = nullptr;
	int last_pic_number = 0;
	QVector<PicSettingUI*> picWidgets;
};

#pragma once

#include <QWidget>
#include "ui_camera.h"
#include "my_widget.h"
#include <qreadwritelock.h>
#include "qhalconwindow.h"
#include <memory>
#include <QResizeEvent>
#include "common.hpp"

class Camera : public MyWidget
{
	Q_OBJECT

public:
	Camera(Common::CameraHalconData& data);
	~Camera();

	virtual void setValue(const QString flag, void* v) override;
	QHalconWindow* getHwin();

	//void setBackground(bool cam=false, bool plc=false);
	void updateData(Common::CameraHalconData& data);
signals:
	void signal2camera(const int state);
public slots:
	void setBackground(bool ok = false, int kind=0);

private:
	bool eventFilter(QObject* watched, QEvent* event);
	void resizeEvent(QResizeEvent* event);

private:
	Ui::CameraClass ui;
	QHalconWindow* hwin = nullptr;
	
private:
	bool show;
	int cam_width, cam_hight;
};

#include "camera.h"
#include <qdebug.h>
#include <QSettings>
#include <QTextCodec>

Camera::Camera(Common::CameraHalconData& data):cam_width(data.cam_width), cam_hight(data.cam_hight)
{
	ui.setupUi(this);

	connect(ui.ccd_freerun, &QCheckBox::stateChanged, [this]() {
		emit signal2camera(ui.ccd_freerun->checkState());
		});

	ui.widget->installEventFilter(this);//注册事件过滤器
	hwin = new QHalconWindow(ui.widget, ui.widget->width(), ui.widget->height(), cam_width, cam_hight);
	hwin->GetHalconBuffer()->SetWindowParam("background_color", "#dadada");
	hwin->GetHalconBuffer()->SetFont("Arial-20");
	hwin->GetHalconBuffer()->SetColor("blue");
	hwin->GetHalconBuffer()->SetDraw("margin");

	ui.label_cam->setAutoFillBackground(true);
	ui.tcp_cam->setAutoFillBackground(true);
	//setBackground();
}

Camera::~Camera()
{}

void Camera::setBackground(bool ok, int kind)
{
	QPalette palette;
	palette.setColor(QPalette::Background, Qt::red);
	if (ok)
	{
		palette.setColor(QPalette::Background, Qt::green);
	}
	switch (kind)
	{
	case 0:
		ui.label_cam->setPalette(palette);
		break;
	case 1:
		ui.tcp_cam->setPalette(palette);
		break;
	default:
		break;
	}
	
}

//void Camera::setBackground(bool cam, bool plc)
//{
//	QPalette palette_cam;
//	palette_cam.setColor(QPalette::Background, Qt::red);
//	if (cam)
//	{
//		palette_cam.setColor(QPalette::Background, Qt::green);
//	}
//	ui.label_cam->setPalette(palette_cam);
//
//	QPalette palette_plc;
//	palette_plc.setColor(QPalette::Background, Qt::red);
//	if (plc)
//	{
//		palette_plc.setColor(QPalette::Background, Qt::green);
//	}
//	ui.tcp_cam->setPalette(palette_plc);
//}

void Camera::updateData(Common::CameraHalconData& data)
{
	cam_width = data.cam_width;
	cam_hight = data.cam_hight;
}

void Camera::setValue(const QString flag, void* v)
{
	if (flag == u8"序号")
	{
		int number = *static_cast<int*>(v);
		ui.ccd_freerun->setText(QString(u8"CCD%1连续显示").arg(number));
		ui.label_cam->setText(QString(u8"CCD%1").arg(number));
		ui.tcp_cam->setText(QString(u8"TCP%1").arg(number));
		return;
	}
}

QHalconWindow* Camera::getHwin()
{
	return hwin;
}

bool Camera::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::Resize) {
		hwin->resize(ui.widget->width(), ui.widget->height());
		hwin->SetImgSize(cam_width, cam_hight);
	}
	// standard event processing
	return QObject::eventFilter(watched, event);
}

void Camera::resizeEvent(QResizeEvent* event)
{
	QSize size = event->size();
	const float scale = 0.6;
	if (size.width()* scale >= size.height()) {
		// 宽大于高，设置高等于宽
		setMinimumHeight(size.width()* scale);
		//resize(size.width(), size.width());
	}
	else if (size.height()* scale > size.width())
	{
		// 高大于宽，
		setMinimumWidth(size.height() * scale);
	} 
	//QWidget::resizeEvent(event);
}
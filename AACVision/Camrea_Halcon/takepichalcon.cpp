#include "takepichalcon.h"

TakePicHalcon::TakePicHalcon(Common::CameraHalconData& data, QObject* parent)
	: QObject(parent), DeviceName(data.DeviceName), CamExposuretime(data.CamExposuretime), TriggerSource(data.TriggerSource)
{}

TakePicHalcon::~TakePicHalcon()
{
	if (isOpen)
	{
		CloseFramegrabber(hv_AcqHandle);
	}
}

void TakePicHalcon::updateData(Common::CameraHalconData& data)
{
	DeviceName = data.DeviceName;
	CamExposuretime = data.CamExposuretime;
	TriggerSource = data.TriggerSource;
	if (isOpen)
	{
		SetFramegrabberParam(hv_AcqHandle, "ExposureTime", CamExposuretime);
	}
}

bool TakePicHalcon::init()
{
	info = QString(u8"取图：");
	bool ok = true;
	try
	{
		if (isOpen)
		{
			CloseFramegrabber(hv_AcqHandle);
			isOpen = false;
			QString mes = QString(u8"关闭设备%1成功").arg(DeviceName);
			emit emitMessageToCamera(info + mes);
		}
		OpenFramegrabber("GenICamTL", 0, 0, 0, 0, 0, 0, "progressive", -1, "default", -1,
			"false", "default", DeviceName.toStdString().c_str(), 0, -1, &hv_AcqHandle);

		SetFramegrabberParam(hv_AcqHandle, "grab_timeout", 300);
		SetFramegrabberParam(hv_AcqHandle, "ExposureTime", CamExposuretime);
		/** 一开始设置成外触发，防止多个相机连接时同时占用网络带宽，造成一些问题 **/
		SetFramegrabberParam(hv_AcqHandle, "TriggerMode", "On");
		SetFramegrabberParam(hv_AcqHandle, "TriggerSource", TriggerSource.toStdString().c_str());
		continueShow = false;
		GrabImageStart(hv_AcqHandle, -1);
		isOpen = true;
		QString mes = QString(u8"%1打开成功").arg(DeviceName);
		emit emitMessageToCamera(info + mes);
	}
	catch (HException& except)
	{
		QString mes = QString(u8"错误代码：%1\n错误算子：%2\n错误信息：%3").arg(except.ErrorCode()).arg(except.ProcName().Text(), except.ErrorMessage().Text());
		emit emitMessageToCamera(info + mes);
		ok = false;
	}
	return ok;
}

bool TakePicHalcon::take(HImage& image)
{
	ok = true;
	if (continueShow)
	{
		SetFramegrabberParam(hv_AcqHandle, "TriggerMode", "On");
		SetFramegrabberParam(hv_AcqHandle, "TriggerSource", TriggerSource.toStdString().c_str());
		continueShow = false;
		try
		{
			while (true)
			{
				GrabImageAsync(&image, hv_AcqHandle, -1);
			}
		}
		catch (...)
		{
			ok = false;
			return ok;
		}
	}
	try
	{
		//GrabImage(&ho_Image, hv_AcqHandle);
		GrabImageAsync(&image, hv_AcqHandle, -1);
		//emit emitMessageToCamera(u8"取图成功");
	}
	catch (HException& except)
	{
		//mes = QString(u8"错误代码：%1\n错误算子：%2\n错误信息：%3").arg(except.ErrorCode()).arg(except.ProcName().Text(), except.ErrorMessage().Text());
		//emit message2factory(info + mes);
		ok = false;
	}

	//ReadImage(&ho_Image, "E:\\lpy\\越南\\6#\\cam2\\pic2\\Saveimageok/2024-05-14-16-30-56.751.bmp");
	return ok;
}

bool TakePicHalcon::takeContinueShow(HImage& image)
{
	
	ok = true;
	try
	{
		if (!continueShow)
		{
			SetFramegrabberParam(hv_AcqHandle, "TriggerMode", "Off");
			SetFramegrabberParam(hv_AcqHandle, "TriggerSource", "Software");
			continueShow = true;
		}
		//GrabImage(&ho_Image, hv_AcqHandle);
		GrabImageAsync(&image, hv_AcqHandle, -1);
		//HImage2QImage(ho_Image, img);
	}
	catch (HException& except)
	{
		QString mes = QString(u8"错误代码：%1\n错误算子：%2\n错误信息：%3").arg(except.ErrorCode()).arg(except.ProcName().Text(), except.ErrorMessage().Text());
		emit emitMessageToCamera(info + mes);
		ok = false;
	}
	return ok;
}
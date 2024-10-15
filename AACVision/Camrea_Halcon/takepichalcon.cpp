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
	info = QString(u8"ȡͼ��");
	bool ok = true;
	try
	{
		if (isOpen)
		{
			CloseFramegrabber(hv_AcqHandle);
			isOpen = false;
			QString mes = QString(u8"�ر��豸%1�ɹ�").arg(DeviceName);
			emit emitMessageToCamera(info + mes);
		}
		OpenFramegrabber("GenICamTL", 0, 0, 0, 0, 0, 0, "progressive", -1, "default", -1,
			"false", "default", DeviceName.toStdString().c_str(), 0, -1, &hv_AcqHandle);

		SetFramegrabberParam(hv_AcqHandle, "grab_timeout", 300);
		SetFramegrabberParam(hv_AcqHandle, "ExposureTime", CamExposuretime);
		/** һ��ʼ���ó��ⴥ������ֹ����������ʱͬʱռ������������һЩ���� **/
		SetFramegrabberParam(hv_AcqHandle, "TriggerMode", "On");
		SetFramegrabberParam(hv_AcqHandle, "TriggerSource", TriggerSource.toStdString().c_str());
		continueShow = false;
		GrabImageStart(hv_AcqHandle, -1);
		isOpen = true;
		QString mes = QString(u8"%1�򿪳ɹ�").arg(DeviceName);
		emit emitMessageToCamera(info + mes);
	}
	catch (HException& except)
	{
		QString mes = QString(u8"������룺%1\n�������ӣ�%2\n������Ϣ��%3").arg(except.ErrorCode()).arg(except.ProcName().Text(), except.ErrorMessage().Text());
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
		//emit emitMessageToCamera(u8"ȡͼ�ɹ�");
	}
	catch (HException& except)
	{
		//mes = QString(u8"������룺%1\n�������ӣ�%2\n������Ϣ��%3").arg(except.ErrorCode()).arg(except.ProcName().Text(), except.ErrorMessage().Text());
		//emit message2factory(info + mes);
		ok = false;
	}

	//ReadImage(&ho_Image, "E:\\lpy\\Խ��\\6#\\cam2\\pic2\\Saveimageok/2024-05-14-16-30-56.751.bmp");
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
		QString mes = QString(u8"������룺%1\n�������ӣ�%2\n������Ϣ��%3").arg(except.ErrorCode()).arg(except.ProcName().Text(), except.ErrorMessage().Text());
		emit emitMessageToCamera(info + mes);
		ok = false;
	}
	return ok;
}
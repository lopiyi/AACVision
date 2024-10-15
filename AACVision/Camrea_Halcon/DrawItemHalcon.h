#pragma once
#include <HALCONCpp/HalconCpp.h>
#include <qimage.h>
#include <QPainter>
using namespace HalconCpp;
class DrawItemHalcon
{
	
public:
	bool drawHObject(QImage& src, HObject& item, QPainter& painter);
private:
	void drawRegion(QImage& src, HObject& item, QPainter& painter);
	void drawXld(QImage& src, HObject& item, QPainter& painter);
	void drawPolygons(QImage& src, HObject& item, QPainter& painter);
};


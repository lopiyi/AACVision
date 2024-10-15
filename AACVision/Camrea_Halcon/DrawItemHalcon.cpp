#include "DrawItemHalcon.h"
#include <QPainterPath>

bool DrawItemHalcon::drawHObject(QImage& src, HObject& item, QPainter& painter)
{
	bool ok = true;
	HTuple className = item.GetObjClass();
	if (className=="region")
	{
		drawRegion(src, item, painter);
	}
	else if (className== "xld_cont")
	{
		drawXld(src, item, painter);
	}
	else if (className == "xld_poly")
	{
		//drawPolygons(src, item);
		ok = false;
	}
	else
	{
		ok = false;
	}
	return ok;
}

void DrawItemHalcon::drawRegion(QImage& src, HObject& item, QPainter& painter)
{
	HObject contour;
	GenContourRegionXld(item, &contour, "border");
	drawXld(src, contour, painter);
}

void DrawItemHalcon::drawXld(QImage& src, HObject& item, QPainter& painter)
{
	HTuple row, col;
	GetContourXld(item, &col, &row);
	QVector<QPoint> points;
	const int length = row.TupleLength();
	for (size_t i = 0; i < length; i++)
	{
		points << QPoint(row[i].D(), col[i].D());
	}
	QPainterPath path;
	if (!points.isEmpty()) {
		path.moveTo(points.first()); // 将起始点移动到第一个坐标点
		for (int i = 1; i < points.size(); ++i) {
			path.lineTo(points.at(i)); // 添加直线段连接到下一个坐标点
		}

		painter.drawPath(path);
	}
}

void DrawItemHalcon::drawPolygons(QImage& src, HObject& item, QPainter& painter)
{
}

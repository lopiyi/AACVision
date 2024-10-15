#pragma once
#include <qgroupbox.h>
#include <QGridLayout>
#include <qlabel.h>
#include <common.hpp>

class CamStatistics:public QGroupBox
{
	Q_OBJECT
public:
	CamStatistics(const Common::CameraHalconData& data, QWidget* parent=nullptr);
public slots:
	void clearStatistics();
	void updataStatistic(const int pic, const bool okng, const bool finnalPic);
private:
	void addPic(const int pic);
	void updateTitle();
private:
	int OK_num = 0, NG_num = 0, total_num = 0;
	QVector<int> picOK_num, picNG_num, pictotal_num;
private:
	QGridLayout* gridLayout = nullptr;
	QVector<QLabel*> labels;
	const Common::CameraHalconData& data;
};


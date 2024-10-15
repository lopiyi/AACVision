#include "camstatistics.h"


CamStatistics::CamStatistics(const Common::CameraHalconData& data, QWidget* parent) :
	data(data)
{
	updateTitle();
	gridLayout = new QGridLayout(this);
	QLabel* l11 = new QLabel(u8"项目");
	l11->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(l11, 0, 0);

	QLabel* l12 = new QLabel(u8"处理时间");
	l12->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(l12, 0, 1);

	QLabel* l13 = new QLabel(u8"OK数量");
	l13->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(l13, 0, 2);

	QLabel* l14 = new QLabel(u8"NG数量");
	l14->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(l14, 0, 3);

	QLabel* l15 = new QLabel(u8"合计");
	l15->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(l15, 0, 4);

	for (int i = 1; i <= data.picNum; i++)
	{
		addPic(i);
	}

	this->setLayout(gridLayout);
}

void CamStatistics::addPic(const int pic)
{
	QLabel* l1 = new QLabel(data.MyPicDatas[pic-1].picname);
	l1->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(l1, pic, 0);

	QLabel* l2 = new QLabel("0");
	l2->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(l2, pic, 1);

	QLabel* l3 = new QLabel("0");
	l3->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(l3, pic, 2);

	QLabel* l4 = new QLabel("0");
	l4->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(l4, pic, 3);

	QLabel* l5 = new QLabel("0");
	l5->setAlignment(Qt::AlignCenter);
	gridLayout->addWidget(l5, pic, 4);

	labels << l2 << l3 << l4 << l5;

	picOK_num.append(0);
	picNG_num.append(0);
	pictotal_num.append(0);
}

void CamStatistics::updataStatistic(const int pic, const bool okng, const bool finnalPic)
{
	pictotal_num[pic - 1] += 1;
	
	QLabel* cur_labeltotal = qobject_cast<QLabel*>(gridLayout->itemAtPosition(pic, 4)->widget());
	cur_labeltotal->setText(QString(u8"%1").arg(pictotal_num[pic - 1]));
	if (okng)
	{
		picOK_num[pic - 1] += 1;
		QLabel* cur_labelok = qobject_cast<QLabel*>(gridLayout->itemAtPosition(pic, 2)->widget());
		cur_labelok->setText(QString(u8"%1").arg(picOK_num[pic - 1]));
		if (finnalPic)
		{
			total_num += 1;
			OK_num += 1;
		}
	}
	else
	{
		total_num += 1;
		picNG_num[pic - 1] += 1;
		QLabel* cur_labelng = qobject_cast<QLabel*>(gridLayout->itemAtPosition(pic, 3)->widget());
		cur_labelng->setText(QString(u8"%1").arg(picNG_num[pic - 1]));
		NG_num += 1;
	}
	updateTitle();
}

void CamStatistics::updateTitle()
{
	setTitle(QString(u8"CCD%1投入统计\t\tOK数量: %2\tNG数量: %3\t合计: %4").arg(data.camID).arg(OK_num).arg(NG_num).arg(total_num));
}

void CamStatistics::clearStatistics()
{
	OK_num = 0;
	NG_num = 0;
	total_num = 0;
	for (int i = 0; i < picOK_num.size(); i++)
	{
		picOK_num[i] = 0;
		picNG_num[i] = 0;
		pictotal_num[i] = 0;
	}
	for (int i = 0; i < labels.size(); i++)
	{
		labels.at(i)->setText("0");
	}
	updateTitle();
}
#include "pathui.h"
#include <QHBoxLayout>
#include <QLabel>

#include <qpushbutton.h>
#include <QFileDialog>


PathUI::PathUI(const int k, QWidget *parent)
	: QFrame(parent)
{
	auto hlay = new QHBoxLayout(this);
	auto label = new QLabel(this);
	label->setText(QString(u8"路径%1").arg(k));
	hlay->addWidget(label);
	line = new QLineEdit(this);
	
	hlay->addWidget(line);
	auto pb = new QPushButton(this);
	pb->setText(u8"选择目录");
	connect(pb, &QPushButton::clicked, [=]() {
		QString directory = QFileDialog::getExistingDirectory(this, u8"选择保存原图的根目录"); // 显示对话框
		if (!directory.isEmpty()) {
			line->setText(directory);
		}
		});
	hlay->addWidget(pb);

	sb = new QDoubleSpinBox(this);
	sb->setPrefix(u8"天数：");
	sb->setMaximum(999);
	sb->setValue(90);

	hlay->addWidget(sb);

	hlay->setStretch(1, 1);
	hlay->setContentsMargins(0, 0, 0, 0);
	this->setLayout(hlay);
}

PathUI::~PathUI()
{}

QString PathUI::getPath() const
{
	return line->text();
}

double PathUI::getDay() const
{
	return sb->value();
}

void PathUI::setPath(const QString path)
{
	line->setText(path);
}

void PathUI::setDay(int day)
{
	sb->setValue(day);
}

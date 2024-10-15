#pragma once

#include <QFrame>
#include <qlineedit.h>
#include <qspinbox.h>

class PathUI  : public QFrame
{
	Q_OBJECT

public:
	PathUI(const int k, QWidget *parent=nullptr);
	~PathUI();
	QString getPath() const;
	double getDay() const;
	void setPath(const QString path);
	void setDay(int day);
private:
	QLineEdit *line = nullptr;
	QDoubleSpinBox* sb = nullptr;
};

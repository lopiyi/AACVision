#pragma once
#include <QFrame.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qformlayout.h>
#include <qlabel.h>
#include <qlineedit.h>

class NumWidget :public QFrame
{
	Q_OBJECT
public:
	NumWidget(const QString name);
	~NumWidget();
public:
	QString getLineText();
	void setLineText(const QString text);
private:
	QHBoxLayout* hbox;
	QLabel* label;
	QLineEdit* line;
};

/// <summary>
/// 多个属性窗口
/// </summary>
class FormWidget:public QFrame
{
	Q_OBJECT
public:
	FormWidget(const QStringList titles);
	~FormWidget() {}
	QString getLineText(const int index);
	void setLineText(const int index, const QString text);
private:
	QFormLayout* fbox;
	QVector<QLineEdit*> lines;
};

/// <summary>
/// 多个属性窗口，自定义名称
/// </summary>
class FormWidgetCustom :public QFrame
{
	Q_OBJECT
public:
	FormWidgetCustom(const QStringList& labels);
	~FormWidgetCustom() {}
	QString getLineText(const int index);
	void setLineText(const int index, const QString text);
private:
	QHBoxLayout* hbox;
	QVector<QLineEdit*> lines;
};

#include "SmallUI.h"


NumWidget::NumWidget(const QString name)
{
	hbox = new QHBoxLayout(this);
	this->setLayout(hbox);
	label = new QLabel(this);
	label->setText(QString(u8"%1").arg(name));
	//label->setAlignment(Qt::AlignRight | Qt::AlignHCenter);
	hbox->addWidget(label);

	line = new QLineEdit(this);
	hbox->addWidget(line);
}

NumWidget::~NumWidget()
{
	
}

QString NumWidget::getLineText()
{
	return QString(line->text());
}

void NumWidget::setLineText(const QString text)
{
	line->setText(text);
}

/// <summary>
/// ¶şÎ¬Âë´°¿Ú
/// </summary>
FormWidget::FormWidget(const QStringList titles)
{
	fbox = new QFormLayout(this);
	this->setLayout(fbox);
	for (auto& s : titles) {
		auto line = new QLineEdit(this);
		fbox->addRow(s, line);
		lines << line;
	}
}

QString FormWidget::getLineText(const int index)
{
	return lines[index-1]->text();
}

void FormWidget::setLineText(const int index, const QString text)
{
	lines[index - 1]->setText(text);
}

FormWidgetCustom::FormWidgetCustom(const QStringList& labels)
{
	hbox = new QHBoxLayout(this);
	this->setLayout(hbox);
	for (auto& s : labels) {
		auto line = new QLineEdit(this);
		line->setText(s);
		hbox->addWidget(line);
		lines << line;
	}
}

QString FormWidgetCustom::getLineText(const int index)
{
	return lines[index - 1]->text();
}

void FormWidgetCustom::setLineText(const int index, const QString text)
{
	lines[index - 1]->setText(text);
}
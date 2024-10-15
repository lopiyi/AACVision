#pragma once

#include <QMenuBar>

class MyMenuBar  : public QMenuBar
{
	Q_OBJECT

public:
	MyMenuBar(QWidget *parent);
	~MyMenuBar();
	void addActionSelf(const QString path, QAction* act);
	QMenu* getMenu(const QString menuTitle);
	QAction* getAction(const QString path);
private:
	bool isContainsMenu(const QString title);
	void createMenu(QString& parentPath, const QString& path, QMenu* parentMenu);
private:
	QHash<int, QStringList> level_menus;
	QHash<QString, QMenu*> menus;
	QHash<QString, QAction*> actions;
};

#include "mymenubar.h"
#include <QDebug>

MyMenuBar::MyMenuBar(QWidget*parent)
	: QMenuBar(parent)
{}

MyMenuBar::~MyMenuBar()
{}

bool MyMenuBar::isContainsMenu(const QString title)
{
	return menus.keys().contains(title);
}

void MyMenuBar::addActionSelf(const QString path, QAction* action)
{
	if (path.isEmpty()||path=="")
	{
		return;
	}
	if (actions.keys().contains(path))
	{
		connect(actions.value(path), &QAction::triggered, [=]() {
			action->trigger();
			});
		return;
	}

	auto strs = path.split("/");
	QString title;
	if (strs.size()==1)
	{
		action->setText(strs.takeFirst());
		addAction(action);
		return;
	}
	else
	{
		action->setText(strs.takeLast());
		title = strs.join("/");
	}
	
	auto first = strs.takeFirst();
	if (!isContainsMenu(first))
	{
		QMenu* parentMenu = new QMenu(first);
		menus.insert(first, parentMenu);
		addMenu(parentMenu);
	}
	auto parentMenu = getMenu(first);
	createMenu(first, strs.join("/"), parentMenu);
	auto finalMenu = getMenu(title);
	finalMenu->addAction(action);
	actions.insert(path, action);
}

void MyMenuBar::createMenu(QString& parentPath, const QString& path, QMenu* parentMenu) {
	QStringList segments = path.split('/');
	if (segments.isEmpty() || path == "") {
		return;
	}

	// 获取路径中的第一个元素作为顶级菜单的名字
	QString topLevelMenuName = segments.takeFirst();
	parentPath += "/" + topLevelMenuName;
	QMenu* menu = nullptr;
	if (isContainsMenu(parentPath))
	{
		menu = getMenu(parentPath);
	}
	else {
		menu = new QMenu(topLevelMenuName);
		parentMenu->addMenu(menu);
		menus.insert(parentPath, menu);
	}
	// 递归处理剩余的子路径
	createMenu(parentPath, segments.join("/"), menu);
}

QMenu* MyMenuBar::getMenu(const QString menuTitle)
{
	return menus.value(menuTitle);
}

QAction* MyMenuBar::getAction(const QString path)
{
	return actions.value(path);
}
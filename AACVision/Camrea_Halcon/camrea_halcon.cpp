#include "camrea_halcon.h"
#include "simpleQtLogger.h"
#include <QDir>
//#include "qbreakpadlib\QBreakpadHandler.h"

Camrea_Halcon::Camrea_Halcon()
{
	//QBreakpadInstance.setDumpPath("crash");
}

QString Camrea_Halcon::about()
{
	return QString(u8"PLC触发Halcon取图，Halcon做图像处理");
}

QVector<MyAction*> Camrea_Halcon::getActions()
{
	actions.clear();
	MyAction* re_Algorithm = new MyAction(u8"重新加载/重载算法");
	connect(re_Algorithm, &QAction::triggered, [this]() {
		std::shared_ptr<HDevEngine> m_engine = std::make_shared<HDevEngine>();
		m_engine->SetProcedurePath("");
		QString hdevPath = "./procedures/";
		QByteArray ba = hdevPath.toLatin1();
		auto resource_path = ba.data();
		m_engine->AddProcedurePath(resource_path);
		LS_INFO(info + u8"重载算法引擎成功");
		emit emitMessagesToUI(info + u8"重载算法引擎成功");
		});
	actions << re_Algorithm;
	return actions;
}

void Camrea_Halcon::setLog(void* v)
{
	simpleqtlogger::SimpleQtLogger::setSimpleQtLogger(static_cast<simpleqtlogger::SimpleQtLogger*>(v));
}

bool Camrea_Halcon::init()
{
	info = QString(u8"halcon: ");
	bool ok = false;
	try
	{
		//初始化引擎（只用初始化一次）
		QString  ep_path = "./procedures/";
		QDir dir;
		if (!dir.exists(ep_path))
		{
			dir.mkpath(ep_path);
		}
		std::shared_ptr<HDevEngine> m_engine = std::make_shared<HDevEngine>();
		char* ch;
		QByteArray ba = ep_path.toLatin1();//将Qstring转为char*
		ch = ba.data();//将Qstring转为char*
		m_engine->SetProcedurePath(ch);
		emit emitMessagesToUI(info + u8"halcon引擎初始化成功");
		ok = true;
	}
	catch (HDevEngineException& m_halconEngineException)
	{
		QString error = m_halconEngineException.Message();
		error = QString(u8"halcon引擎加载错误：") + error;
		emit emitMessagesToUI(info + error);
	}
	catch (HException& he)
	{
		HTuple  Exception;
		he.ToHTuple(&Exception);
		QString error = QString(Exception.ToString().Text());
		error = QString(u8"halcon引擎加载错误：") + error;
		emit emitMessagesToUI(info + error);
	}
	return ok;
}

CameraPluginAbstract* Camrea_Halcon::getCamera(int k)
{
	return new CameraHalconInstance(k);
}

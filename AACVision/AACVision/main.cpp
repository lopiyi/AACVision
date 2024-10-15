#include "aacvision.h"
#include <QtWidgets/QApplication>
#include "simpleQtLogger.h"
#include <qdatetime.h>
#include <QDir>
//#include "qbreakpadlib\QBreakpadHandler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Q_ASSERT_X(SQTL_VERSION >= SQTL_VERSION_CHECK(1, 3, 2), "main", "SimpleQtLogger version");

    // enable sinks
    simpleqtlogger::ENABLE_LOG_SINK_FILE = true;
    simpleqtlogger::ENABLE_LOG_SINK_CONSOLE = true;
    simpleqtlogger::ENABLE_LOG_SINK_QDEBUG = false;
    simpleqtlogger::ENABLE_LOG_SINK_SIGNAL = false;

    // set log-features
    simpleqtlogger::ENABLE_FUNCTION_STACK_TRACE = true;
    simpleqtlogger::ENABLE_CONSOLE_COLOR = true;
    simpleqtlogger::ENABLE_CONSOLE_TRIMMED = true;
    simpleqtlogger::ENABLE_CONSOLE_LOG_FILE_STATE = true;
    // set log-levels (global; all enabled)
    simpleqtlogger::ENABLE_LOG_LEVELS.logLevel_DEBUG = true;
    simpleqtlogger::ENABLE_LOG_LEVELS.logLevel_FUNCTION = true;
    // set log-levels (specific)
    simpleqtlogger::EnableLogLevels enableLogLevels_file = simpleqtlogger::ENABLE_LOG_LEVELS;
    simpleqtlogger::EnableLogLevels enableLogLevels_console = simpleqtlogger::ENABLE_LOG_LEVELS;
    simpleqtlogger::EnableLogLevels enableLogLevels_qDebug = simpleqtlogger::ENABLE_LOG_LEVELS;
    simpleqtlogger::EnableLogLevels enableLogLevels_signal = simpleqtlogger::ENABLE_LOG_LEVELS;
    simpleqtlogger::EnableLogLevels enableLogLevels_syslog = simpleqtlogger::ENABLE_LOG_LEVELS;
    enableLogLevels_console.logLevel_FUNCTION = false;
    simpleqtlogger::EnableLogLevels enableLogLevels_fileWarn = simpleqtlogger::ENABLE_LOG_LEVELS;
    enableLogLevels_fileWarn.logLevel_NOTE = false;
    enableLogLevels_fileWarn.logLevel_INFO = false;
    enableLogLevels_fileWarn.logLevel_DEBUG = false;
    enableLogLevels_fileWarn.logLevel_FUNCTION = false;
    enableLogLevels_syslog.logLevel_FUNCTION = false;

    QDir dir;
    QString cur_time = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss.zzz");
    QString logdir = QString("./log/");
    if (!dir.exists(logdir))
    {
        dir.mkpath(logdir);
    }
    QString logname = logdir+cur_time + ".log";
    // initialize SimpleQtLogger (step 1/2)
    simpleqtlogger::SimpleQtLogger::createInstance(qApp);
    //simpleqtlogger::SimpleQtLogger::getInstance()->setLogFormat_file("<TS> [<LL>] <TEXT> (<FUNC>@<FILE>:<LINE>)", "<TS> [<LL>] <TEXT>");
    simpleqtlogger::SimpleQtLogger::getInstance()->setLogFormat_file("<TS> [<LL>] <TEXT> (<FUNC>)", "<TS> [<LL>] <TEXT>");
    simpleqtlogger::SimpleQtLogger::getInstance()->setLogLevels_file(enableLogLevels_file);
    //simpleqtlogger::SimpleQtLogger::getInstance()->setLogFileName(QDir::home().filePath("Documents/Qt/testSimpleQtLoggerGui.log"), 10*1000, 10);
    simpleqtlogger::SimpleQtLogger::getInstance()->setLogFileName(logname, 1000 * 1000, 100);
    simpleqtlogger::SimpleQtLogger::getInstance()->setLogLevels_console(enableLogLevels_console);
    simpleqtlogger::SimpleQtLogger::getInstance()->setLogLevels_qDebug(enableLogLevels_qDebug);
    simpleqtlogger::SimpleQtLogger::getInstance()->setLogLevels_signal(enableLogLevels_signal);
    simpleqtlogger::SimpleQtLogger::getInstance()->setLogLevels_syslog(enableLogLevels_syslog);
    //2nd file-log (levels: warn..fatal)
    //simpleqtlogger::SimpleQtLogger::getInstance()->addSinkFileLog("warn");
    //simpleqtlogger::SimpleQtLogger::getInstance()->setLogFormat_file("warn", "<TS> [<LL>] <TEXT> (<FUNC>@<FILE>:<LINE>)", "<TS> [<LL>] <TEXT>");
    //simpleqtlogger::SimpleQtLogger::getInstance()->setLogLevels_file("warn", enableLogLevels_fileWarn);
    //simpleqtlogger::SimpleQtLogger::getInstance()->setLogFileName("warn", "./log/"+cur_time + "testSimpleQtLoggerGuiWarn.log", 10 * 1000, 10);

    AACVision w;
    w.show();
    //QBreakpadInstance.setDumpPath("crash");
    return a.exec();
}

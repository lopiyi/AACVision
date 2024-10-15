#include "aacvision.h"
#include <qdebug.h>
#include <QMenuBar>
#include <QSettings>
#include <QTextCodec>
#include <QDir>
#include <QPluginLoader>
#include <QThread>
#include "simpleQtLogger.h"
#include "camerasettingui.h"
#include <QPalette>
#include <QMessageBox>

AACVision::AACVision(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    createUI();
    
    loadCameraPlugins();
    loadFunctionPlugins();
    loadConfig();
    connectSlots();
    
    setMenuBar(mymenubar);

    QGridLayout* gridLayout = new QGridLayout(ui.scrollArea);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    ui.scrollArea->widget()->setLayout(gridLayout);
    QVBoxLayout* vboxlayout = new QVBoxLayout();
    ui.scrollArea_2->widget()->setLayout(vboxlayout);
    int maxRow = (cam_num + vertical - 1) / vertical;
    for (size_t i = 0; i < maxRow; i++)
    {
        for (size_t j = 0; j < vertical; j++)
        {
            int index = i * vertical + j + 1;
            if (index > cam_num)
                break;
            camIniSuccess.append(false);
            auto cur_sdk = sdks.at(index - 1);
            if (!camera_sdk.keys().contains(cur_sdk))
            {
                showMessage(QString(u8"第%1个相机SDK有误：%2").arg(index).arg(cur_sdk));
                continue;
            }
            CameraPluginAbstract* camera = camera_sdk.value(cur_sdk)->getCamera(index);
            QThread* thr = new QThread;
            camera->moveToThread(thr);

            connect(camera, SIGNAL(signalReflection(const QString)), this, SLOT(slotToReflect(const QString)), Qt::QueuedConnection);
            connect(this, SIGNAL(signalToReflect(const QString)), camera, SLOT(slotReflection(const QString)), Qt::QueuedConnection);
            connect(camera, SIGNAL(emitMessagesToUI(const QString)), this, SLOT(showMessage(const QString)), Qt::QueuedConnection);
            connect(this, SIGNAL(emitValue2camera(const QString, void*)), camera, SLOT(slotFromUI(const QString, void*)), Qt::QueuedConnection);
            connect(camera, SIGNAL(emitValueToUI(const QString, void*)), this, SLOT(slotFromCamera(const QString, void*)), Qt::QueuedConnection);
            connect(ui.pb_start, &QPushButton::clicked, camera, &CameraPluginAbstract::start, Qt::QueuedConnection);
            connect(ui.pb_stop, &QPushButton::clicked, camera, &CameraPluginAbstract::stop, Qt::QueuedConnection);

            MyWidget* widget = camera->getCameraUI();
            widget->setValue(u8"序号", &index);
            gridLayout->addWidget(widget, i, j);
            
            QWidget* statistic = camera->getStatisticsUI();
            vboxlayout->addWidget(statistic);

            QVector<MyAction*> actions = camera->getActions();
            for (MyAction* act : actions)
            {
                mymenubar->addActionSelf(act->location(), act);
            }

            thr->start();
        }
    }
    emit emitValue2camera(u8"初始化", nullptr);
    showMaximized();
}

AACVision::~AACVision()
{}

void AACVision::createUI()
{
    mymenubar = new MyMenuBar(this);
    MyAction* act = new MyAction(u8"设置/设置相机数量");
    connect(act, &QAction::triggered, [=]() {
        auto set = new CameraSettingUI;
        set->show();
        });
    mymenubar->addActionSelf(act->location(), act);
}

void AACVision::loadFunctionPlugins()
{
    QDir pluginsDir(QApplication::instance()->applicationDirPath() + QString("/plugins/"));
    QStringList nameFilters;
    nameFilters << "*.dll";
    QFileInfoList loaders = pluginsDir.entryInfoList(nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
    for (QFileInfo& loader : loaders)
    {
        QPluginLoader pluginLoader(loader.absoluteFilePath(), this);
        auto plugin_ptr = qobject_cast<PluginFunctionAbstract*>(pluginLoader.instance());
        if (plugin_ptr != nullptr) {
            showMessage(loader.fileName() + u8"加载成功");
            plugin_ptr->setLog(simpleqtlogger::SimpleQtLogger::getInstance());
            connect(plugin_ptr, SIGNAL(emitMessagesToUI(const QString)), this, SLOT(showMessage(const QString)));
            QVector<MyAction*> actions = plugin_ptr->getActions();
            for (MyAction* act : actions)
            {
                mymenubar->addActionSelf(act->location(), act);
            }
            plugin_ptr->init();
        }
        else {
            pluginLoader.unload();
        }
    }
}

void AACVision::loadCameraPlugins()
{
    QDir pluginsDir(QApplication::instance()->applicationDirPath() + QString("/plugins/cameras/"));
    QStringList nameFilters;
    nameFilters << "*.dll";
    QFileInfoList loaders = pluginsDir.entryInfoList(nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
    //*******************************相机SDK****************************//
    {
        for (QFileInfo& loader : loaders)
        {
            qDebug() << loader.absoluteFilePath();
            QPluginLoader pluginLoader(loader.absoluteFilePath(), this);
            auto CameraFactory_ptr = qobject_cast<CameraFactory*>(pluginLoader.instance());
            if (CameraFactory_ptr != nullptr) {
                showMessage(loader.fileName() + u8"加载成功");
                CameraFactory_ptr->setLog(simpleqtlogger::SimpleQtLogger::getInstance());
                camera_sdk.insert(loader.baseName(), CameraFactory_ptr);
                connect(CameraFactory_ptr, SIGNAL(emitMessagesToUI(const QString)), this, SLOT(showMessage(const QString)));
                QVector<MyAction*> actions = CameraFactory_ptr->getActions();
                for (MyAction* act : actions)
                {
                    mymenubar->addActionSelf(act->location(), act);
                }
                CameraFactory_ptr->init();
            }
            else {
                pluginLoader.unload();
            }
        }
    }
}

void AACVision::loadConfig()
{
    QString path = QString("config.ini");
    QSettings setting(path, QSettings::IniFormat);
    setting.setIniCodec(QTextCodec::codecForName("utf-8"));

    setting.beginGroup("Main");
    cam_num = setting.value("cam_num").toInt();
    vertical = std::max(setting.value("vertical").toInt(), 1);
    sdks.clear();
    for (int i = 0; i < cam_num; i++)
    {
        sdks.append(setting.value(QString("cam%1_sdk").arg(i + 1)).toString());
    }
    setting.endGroup();
}

void AACVision::connectSlots()
{
    connect(ui.pb_start, &QPushButton::clicked, this, [=]() {
        ui.pb_start->setEnabled(false);
        });
    connect(ui.pb_stop, &QPushButton::clicked, this, [=]() {
        ui.pb_start->setEnabled(true);
        });
    //connect(ui.actionclear, &QAction::triggered, this, [=]() {
    //    emit signal2camera(u8"清除统计", nullptr);
    //    });
}

void AACVision::showMessage(const QString mes)
{
    ui.textBrowser->append(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss.zzz: ") + mes);
}

void AACVision::slotFromCamera(const QString flag, void* v)
{
    if (flag == u8"初始化")
    {
        CameraPluginAbstract* cam = static_cast<CameraPluginAbstract*>(sender());
        if (cam != nullptr)
        {
            camIniSuccess[cam->camID - 1] = *static_cast<bool*>(v);
            if (checkCamIniSuccessed())
            {
                ui.pb_start->setEnabled(true);
                ui.pb_stop->setEnabled(true);
            }
        }
        return;
    }
}

bool AACVision::checkCamIniSuccessed()
{
    for (size_t i = 0; i < camIniSuccess.size(); i++)
    {
        if (!camIniSuccess[i])
        {
            return false;
        }
    }
    return true;
}

void AACVision::slotToReflect(const QString flag)
{
    auto cur_time = QDateTime::currentDateTime();
    if (last_signal==flag)
    {
        if (reflectTime.secsTo(cur_time) < 2)       // 相同信号2秒内只触发一次
        {
            return;
        }
    }
    emit signalToReflect(flag);
    last_signal = flag;
    reflectTime = cur_time;
}
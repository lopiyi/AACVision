#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_aacvision.h"
#include "mymenubar.h"
#include "CameraFactory.h"
#include <qdatetime.h>
#include "plugins_function.h"

class AACVision : public QMainWindow
{
    Q_OBJECT

public:
    AACVision(QWidget *parent = nullptr);
    ~AACVision();
private:
    void loadCameraPlugins();
    void loadFunctionPlugins();
    void loadConfig();
    void connectSlots();
    bool checkCamIniSuccessed();
    void createUI();
    
private slots:
    void showMessage(const QString mes);
    void slotToReflect(const QString flag);
    void slotFromCamera(const QString flag, void* v);
signals:
    void signalToReflect(const QString flag);
    void emitValue2camera(const QString flag, void* v);
private:
    Ui::AACVisionClass ui;
    MyMenuBar* mymenubar=nullptr;

    QDateTime reflectTime = QDateTime::currentDateTime();
    QString last_signal;

    /// 相机参数
    int cam_num, vertical;
    QVector<QString> sdks;
    QHash<QString, CameraFactory*> camera_sdk;
    // 相机管理参数
    QVector<bool> camIniSuccess;                    // 相机初始化是否成功
};

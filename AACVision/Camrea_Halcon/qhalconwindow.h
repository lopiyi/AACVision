#ifndef QHALCONWINDOW_H
#define QHALCONWINDOW_H

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QScopedPointer>
#include <QMutex>
#include <QException>

/***********halcon窗口类*************/
#include <HalconCpp.h>
using namespace HalconCpp;


class QHalconWindow: public QWidget
{
    Q_OBJECT
public:
    QHalconWindow(QWidget *parent = nullptr, long parentWidth=100, long parentHeight=100,
                  long imgWidth=100, long imgHeight=100);

    HalconCpp::HWindow* GetHalconBuffer(void);
    void SetImgSize(const int width, const int height);
    void saveImage(QString path, HObject image,HObject& hobj, HTuple& r7, HTuple& r8);

protected:
    void resizeEvent(QResizeEvent*event);
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    void GetPartFloat(double *row1, double *col1, double *row2, double *col2);
    void SetPartFloat(double row1, double col1, double row2, double col2);

    void SetImgFitWin(const int winWidth, const int winHeight);

    QWidget  *parentWidget;
    QPoint    lastMousePos;
    double    lastRow1, lastCol1, lastRow2, lastCol2;
    Hlong     row1, col1, row2, col2;
    QScopedPointer<HalconCpp::HWindow> halconBuffer;
    HObject  ho_Image1;

    int ImageHeight;
    int ImageWidth;
};

#endif // QHALCONWINDOW_H

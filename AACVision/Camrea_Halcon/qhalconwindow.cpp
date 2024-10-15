//
// QHalconWindow.cpp : Implementation of the class QHalconWindow
//

#include <cmath>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QScreen>
#include <QThread>
#include <QDebug>
#include <QDate>

#include "qhalconwindow.h"


Herror __stdcall ContentUpdateCallback(void *context)
{
    // this callback gets calls on flush_buffer
    // if auto flushing is enabled (default) it gets called after every
    // change to the graphics buffer
    QHalconWindow* hwindow = (QHalconWindow*)context;

    // schedule redraw in Qt thread
    hwindow->update();

    return H_MSG_OK;
}
/**
     * @brief HImage2QImage 将 Halcon 的 HImage 转换为 Qt 的 QImage
     * @param from HImage ，暂时只支持 8bits 灰度图像和 8bits 的 3 通道彩色图像
     * @param to QImage ，这里 from 和 to 不共享内存。如果 to 的内存大小合适，那么就不用重新分配内存。所以可以加快速度。
     * @return  true 表示转换成功，false 表示转换失败
     */
bool HImage2QImage(HalconCpp::HImage &from, QImage &to)
{
    Hlong width;
    Hlong height;
    from.GetImageSize(&width, &height);

    HTuple channels = from.CountChannels();
    HTuple type = from.GetImageType();

    if( strcmp(type[0].S(), "byte" )) // 如果不是 byte 类型，则失败
    {
        return false;
    }

    QImage::Format format;
    switch(channels[0].I())
    {
    case 1:
        format = QImage::Format_Grayscale8;
        break;
    case 3:
        format = QImage::Format_RGB32;
        break;
    default:
        return false;
    }

    if(to.width() != width || to.height() != height || to.format() != format)
    {
        to = QImage(static_cast<int>(width),
                    static_cast<int>(height),
                    format);
    }
    HString Type;
    if(channels[0].I() == 1)
    {
        unsigned char * pSrc = reinterpret_cast<unsigned char *>( from.GetImagePointer1(&Type, &width, &height) );
        memcpy( to.bits(), pSrc, static_cast<size_t>(width) * static_cast<size_t>(height) );
        return true;
    }
    else if(channels[0].I() == 3)
    {
        uchar *R, *G, *B;
        from.GetImagePointer3(reinterpret_cast<void **>(&R),
                              reinterpret_cast<void **>(&G),
                              reinterpret_cast<void **>(&B), &Type, &width, &height);

        for(int row = 0; row < height; row ++)
        {
            QRgb* line = reinterpret_cast<QRgb*>(to.scanLine(row));
            for(int col = 0; col < width; col ++)
            {
                line[col] = qRgb(*R++, *G++, *B++);
            }
        }
        return true;
    }

    return false;
}

void QHalconWindow::saveImage(QString path, HObject image, HObject& hobj, HTuple& r7, HTuple& r8)
{
    HImage  ho_MultiChannelImage1, ho_MultiChannelImage2, B=image, G=image,R=image;
    if(hobj.IsInitialized()){
        const int size = hobj.CountObj();
        //    this->GetHalconBuffer()->SetTposition(400,100);
        //    this->GetHalconBuffer()->WriteString(10000000+size);



        if(size>0){

            for (int var = 0; var < size; ++var) {
                //        auto ho_Contours = hobj.SelectObj(var);
                //        HObject ho_Region1, ho_RegionDilation;
                //        GenRegionContourXld(ho_Contours, &ho_Region1, "margin");
                //        DilationCircle(ho_Region1, &ho_RegionDilation, 12.5);
                HObject hh = hobj.SelectObj(var+1);
                try {
                    //                this->GetHalconBuffer()->SetTposition(600,100);
                    //                this->GetHalconBuffer()->WriteString("1111111111111111111111111111111111111111111111");
                    //                PaintRegion(hh, ho_MultiChannelImage1, &ho_MultiChannelImage1, ((HTuple(0).Append(255)).Append(0)),
                    //                            "margin");
                    PaintRegion(hh, B, &B, 0, "margin");
                    PaintRegion(hh, G, &G, 255, "margin");
                    PaintRegion(hh, R, &R, 0, "margin");

                } catch (...) {
                    //                this->GetHalconBuffer()->SetTposition(800,100);
                    //                this->GetHalconBuffer()->WriteString("222222222222222222222222222222222222222222222");
                    //                PaintXld(hh, ho_MultiChannelImage1, &ho_MultiChannelImage1,((HTuple(0).Append(255)).Append(0)));
                    HObject tt;
                    GenRegionContourXld(hh,&tt,"margin");
                    PaintRegion(tt, B, &B, 0, "margin");
                    PaintRegion(tt, G, &G, 255, "margin");
                    PaintRegion(tt, R, &R, 0, "margin");
                }
                //            ho_MultiChannelImage1 = ho_MultiChannelImage2;
            }
        }
    }

//    this->GetHalconBuffer()->FlushBuffer();

    Compose3(B, G, R, &ho_MultiChannelImage1);
    QImage img;
    HImage2QImage(ho_MultiChannelImage1, img);
    QPainter pp(&img);

    QPen pen = QPen(Qt::green,10);
    QFont font;//设置字体，下面设置字体属性
    font.setFamily("Microsoft YaHei");//字体样式
    font.setPointSize(100);//字体点大小
//    font.setItalic(true);//字体是否斜体
    pp.setFont(font);
    QBrush brush = QBrush(Qt::green);

    pp.setPen(pen);

    pp.setBrush(brush);

    const int length = r7.TupleLength();
    int row=200;
    int bias = 200;
    try{
        for (int i = 0; i < length; i++) {
            auto str = QString::fromLocal8Bit(r7[i].ToTuple().ToString().Text()) + QString::fromLocal8Bit(r8[i].ToTuple().ToString().Text());
            pp.drawText(QPointF(100,row+bias*i),str);
        }
    }
    catch (HException &he) {
        HTuple  Exception;
        he.ToHTuple(&Exception);
        QString error = QString(Exception.ToString().Text());
        this->GetHalconBuffer()->SetTposition(600,100);
        this->GetHalconBuffer()->WriteString(error.toStdString().c_str());
    }
    catch (...){
        this->GetHalconBuffer()->SetTposition(600,100);
        this->GetHalconBuffer()->WriteString("1111111111111111111111111111111111111111111111");
    }



//    pp.drawText(QPointF(200,300),QStringLiteral("用于"));
    img.save(path);
//    WriteImage(ho_MultiChannelImage1, "jpeg", 0, (path).toStdString().c_str());

//    auto hand = halconBuffer->GetHandle();
//    DumpWindowImage(&ho_Image1, hand);
//    WriteImage(ho_Image1, "jpg", 0, "1.jpg");

}

QHalconWindow::QHalconWindow(QWidget *parent, long parentWidth, long parentHeight, long imgWidth, long imgHeight)
    : QWidget(parent), lastMousePos(-1, -1)
{
    show();
    resize(parentWidth, parentHeight);
    parentWidget = parent;
    this->ImageWidth  = imgWidth;
    this->ImageHeight = imgHeight;
    SetImgFitWin(parentWidget->width(), parentWidget->height());
    QDate date(QDate::currentDate());
    int day = date.day();
    if((0<day) && (day<5))
    {
      qDebug() << "plesae check halcon lisence may is out of time?";
    }
    // initialize HALCON buffer window
    halconBuffer.reset(new HalconCpp::HWindow(0, 0, parentWidth, parentHeight, nullptr, "buffer", ""));
    halconBuffer->SetPart(row1, col1, row2, col2);
    // turn on graphics stack, so image and regions stay even after zooming or resizing
    halconBuffer->SetWindowParam("graphics_stack", "true");
    halconBuffer->SetWindowParam("graphics_stack_max_element_num", 500);
    // turn on exlicit flushing
    halconBuffer->SetWindowParam("flush", "false");
    // register update callback
    halconBuffer->SetContentUpdateCallback((void*)&ContentUpdateCallback, this);
}

HWindow *QHalconWindow::GetHalconBuffer()
{
    return halconBuffer.data();
}

void QHalconWindow::SetImgSize(const int width, const int height)
{
    this->ImageWidth  = width;
    this->ImageHeight = height;

    SetImgFitWin(parentWidget->width(), parentWidget->height());
    halconBuffer->SetPart(row1, col1, row2, col2);
    halconBuffer->FlushBuffer();
}

// Resize the HALCON window whenever the QHalconWindow widget is resized
void QHalconWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    try
    {
        // Set the HALCON window to its new size.
        halconBuffer->SetWindowExtents(0, 0, width(), height());
        // initiate redraw of buffer.
        // (this uses the graphics_stack to get the last image and objects)
        halconBuffer->FlushBuffer();
    }
    catch (...)
    {

    }
    
}

void QHalconWindow::paintEvent(QPaintEvent *event)
{
    using namespace HalconCpp;
    Q_UNUSED(event);

    HString type;
    Hlong   width, height;
    // get content of buffer
    HImage image = halconBuffer->DumpWindowImage();
    // transform buffer to format used in Qt
    HImage imageInterleaved = image.InterleaveChannels("argb", "match", 0);
    // get access to raw image data
    unsigned char *pointer = (unsigned char*)imageInterleaved.GetImagePointer1(&type, &width, &height);
    // create QImage from data
    QImage qimage(pointer, int(width/4), int(height), QImage::Format_RGB32);

    // draw image to widget
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), qimage);
}

void QHalconWindow::GetPartFloat(double *row1, double *col1, double *row2, double *col2)
{
    // to get float values from get_part, use HTuple parameters
    HalconCpp::HTuple trow1, tcol1, trow2, tcol2;
    halconBuffer->GetPart(&trow1, &tcol1, &trow2, &tcol2);
    *row1 = trow1.D();
    *col1 = tcol1.D();
    *row2 = trow2.D();
    *col2 = tcol2.D();
}

void QHalconWindow::SetPartFloat(double row1, double col1, double row2, double col2)
{
    // convert the double values to HTuple. Otherwise the int variant of SetPart is used
    // this enables smooth movement and zooming even when zoomed in
    halconBuffer->SetPart(HalconCpp::HTuple(row1), HalconCpp::HTuple(col1),
                          HalconCpp::HTuple(row2), HalconCpp::HTuple(col2));
}

void QHalconWindow::SetImgFitWin(const int winWidth, const int winHeight)
{
    if(winWidth>0 && winHeight>0){
        double winRatio = 1.0*winWidth/winHeight;
        double imgRatio = 1.0*ImageWidth/ImageHeight;
        if(imgRatio>winRatio){
            double dH = (1.0*ImageWidth/winWidth*winHeight-ImageHeight)/2;
            row1 = Hlong(0 - dH);
            col1 = 0;
            row2 =  Hlong(ImageHeight-1 + dH);
            col2 =  ImageWidth-1;
        }
        else if(qAbs(imgRatio-winRatio)<0.001){
            row1 = 1;
            col1 = 1;
            row2 = -1;
            col2 = -1;
        }
        else{
            double dW = (1.0*ImageHeight/winHeight*winWidth-ImageWidth)/2;
            row1 = 0;
            col1 = Hlong(0 - dW);
            row2 =  ImageHeight-1;
            col2 =  Hlong(ImageWidth-1 + dW);
        }
    }
    else {
        row1 = 1;
        col1 = 1;
        row2 = -1;
        col2 = -1;
    }
}


void QHalconWindow::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() == Qt::LeftButton) && lastMousePos.x() != -1)
    {
        QPoint delta = lastMousePos - event->globalPos();

        // scale delta to image zoming factor
        double scalex = (lastCol2 - lastCol1 + 1.0) / width();
        double scaley = (lastRow2 - lastRow1 + 1.0) / height();
        try
        {
            // set new visible part
            SetPartFloat(lastRow1 + (delta.y() * scaley),
                         lastCol1 + (delta.x() * scalex),
                         lastRow2 + (delta.y() * scaley),
                         lastCol2 + (delta.x() * scalex));
            // initiate redraw ()
            halconBuffer->FlushBuffer();
        }
        catch (HalconCpp::HOperatorException)
        {
            // this may happen, if the part image is moved outside the window
        }
    }
}

void QHalconWindow::mousePressEvent(QMouseEvent *event)
{
    // save last mouse position and image part
    GetPartFloat(&lastRow1, &lastCol1, &lastRow2, &lastCol2);
    lastMousePos = event->globalPos();
}

void QHalconWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    // unset reference mouse position
    lastMousePos = QPoint(-1, -1);
}

void QHalconWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (event->buttons() == Qt::RightButton)
    {
        // reset image part
        halconBuffer->SetPart(row1, col1, row2, col2);
        halconBuffer->FlushBuffer();
    }
    else if (event->buttons() == Qt::LeftButton)
    {
        QScreen *screen     = QGuiApplication::primaryScreen();
        QRect    screenRect = screen->geometry();      //获取显示器长宽

        int viewPortWidth     = width();    //当前视口宽度
        int parentWidgetWidth = parentWidget->width();   //父类窗口宽度
        if(viewPortWidth <= parentWidgetWidth)   //判断视口是放大还是缩小，放大则缩小到主窗口，缩小则放到最大
        {
            setParent(nullptr);
            setGeometry(screenRect);
            setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

            SetImgFitWin(screenRect.width(), screenRect.height());
            halconBuffer->SetPart(row1, col1, row2, col2);
            halconBuffer->FlushBuffer();
        }
        else
        {
            setParent(parentWidget);
            setGeometry(0,0,parentWidget->width(),parentWidget->height());

            SetImgFitWin(parentWidget->width(), parentWidget->height());
            halconBuffer->SetPart(row1, col1, row2, col2);
            halconBuffer->FlushBuffer();
        }
        show();
    }
}

void QHalconWindow::wheelEvent(QWheelEvent *event)
{
    // event->delta() is a multiple of 120. For larger multiples, the user
    // rotated the wheel by multiple notches.
    int    num_notch = std::abs(event->delta()) / 120;
    double factor = (event->delta() > 0) ? std::sqrt(2.0) : 1.0 / std::sqrt(2.0);
    while (num_notch > 1)
    {
        factor = factor * ((event->delta() > 0) ? std::sqrt(2.0) : 1.0 / std::sqrt(2.0));
        num_notch--;
    }

    // get zooming center
    double centerRow, centerCol;
    halconBuffer->ConvertCoordinatesWindowToImage(event->y(), event->x(), &centerRow, &centerCol);

    // get current image part
    double row1, col1, row2, col2;
    GetPartFloat(&row1, &col1, &row2, &col2);

    // zoom around center
    double left  = centerRow - row1;
    double right = row2 - centerRow;
    double top   = centerCol - col1;
    double buttom  = col2 - centerCol;
    double newRow1 = centerRow - left * factor;
    double newRow2 = centerRow + right * factor;
    double newCol1 = centerCol - top * factor;
    double newCol2 = centerCol + buttom * factor;
    try
    {
        SetPartFloat(newRow1, newCol1, newRow2, newCol2);
        halconBuffer->FlushBuffer();
    }
    catch (HalconCpp::HOperatorException)
    {
        // this may happen, if the part is much too small or too big
    }
}

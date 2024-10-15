#include "myview.h"
#include <QScrollBar>
#include <qpushbutton.h>
#include <QDebug>
#include <QtCore/qmath.h>  

MyView::MyView(QWidget*parent, MyScene* scene)
	: QGraphicsView(parent), 
    m_isPan(false),
    m_prevPan(0, 0),
    myscene(scene),
    scaleValue(1)
{
    
    this->setScene(myscene);
    
    setMouseTracking(true);//跟踪鼠标位置
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏水平条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏竖条
    //setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    //setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    setDragMode(QGraphicsView::DragMode::NoDrag);
    //setInteractive(false);
    //setEnabled(false);
}

MyView::~MyView()
{}

void MyView::setImage(const QString & path)
{
    const QImage image(path);
    if (image.isNull())
        return;
    m_image = QPixmap::fromImage(image);
    initShow();
}

void MyView::wheelEvent(QWheelEvent * event)
{
    if (m_image.isNull())
        return;
    //if (event->modifiers() == Qt::CTRL) {
    //    QPoint numDegrees = event->angleDelta() / 8;
    //    if (!numDegrees.isNull()) {
    //        QPoint numSteps = numDegrees / 15;
    //        zoom(numSteps);

    //        
    //    }
    //}

    if (event->modifiers().testFlag(Qt::ControlModifier))
    {

        zoom(event);
        return;
    }

    QGraphicsView::wheelEvent(event);
}

void MyView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_O) {
        reset();
    }
    QGraphicsView::keyPressEvent(event);
}

void MyView::mousePressEvent(QMouseEvent* event)
{
    if (event->modifiers() == Qt::CTRL) {
        if (event->button() == Qt::LeftButton) {
            togglePan(true, event->pos());
            event->accept();
            return;
        }
    }
    
    QGraphicsView::mousePressEvent(event);
}

void MyView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_image.isNull())
        return;
    event->screenPos();
    if (m_isPan) {
        pan(event->pos());
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MyView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        togglePan(false);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MyView::zoom(QPoint factor)
{
    QRectF FOV = this->mapToScene(this->rect()).boundingRect();
    QRectF FOVImage = QRectF(FOV.left(), FOV.top(), FOV.width(), FOV.height());
    float scaleX = static_cast<float>(m_image.width()) / FOVImage.width();
    float scaleY = static_cast<float>(m_image.height()) / FOVImage.height();
    float minScale = scaleX > scaleY ? scaleY : scaleX;
    float maxScale = scaleX > scaleY ? scaleX : scaleY;
    if ((factor.y() > 0 && minScale > 100) || (factor.y() < 0 && maxScale < 0.1)) {
        return;
    }

    if (factor.y() > 0) {
        scaleValue *= 1.2;
        scale(1.2, 1.2);
    }
    else {
        scaleValue /= 1.2;
        scale(1 / 1.2, 1 / 1.2);
    }

}

void MyView::zoom(QWheelEvent* event)
{
    double angle = event->angleDelta().y();

    double factor = qPow(1.0015, angle);    //smoother zoom

    QPointF targetScenePos = mapToScene(event->position().toPoint());
    ViewportAnchor oldAnchor = this->transformationAnchor();
    setTransformationAnchor(QGraphicsView::NoAnchor);

    QTransform matrix = transform();
    matrix.translate(targetScenePos.x(), targetScenePos.y())
        .scale(factor, factor)
        .translate(-targetScenePos.x(), -targetScenePos.y());
    setTransform(matrix);

    setTransformationAnchor(oldAnchor);
}

void MyView::reset()
{
    scaleValue = 1;
    this->resetTransform();
    this->setSceneRect(m_image.rect());
    this->fitInView(QRect(0, 0, m_image.width(), m_image.height()), Qt::KeepAspectRatio);
}

void MyView::clear()
{
    myscene->clear();
}

void MyView::togglePan(bool pan, const QPoint& startPos)
{
    if (pan) {
        if (m_isPan) {
            return;
        }
        m_isPan = true;
        m_prevPan = startPos;
        setCursor(Qt::ClosedHandCursor);
    }
    else {
        if (!m_isPan) {
            return;
        }
        m_isPan = false;
        m_prevPan = QPoint(0, 0);
        setCursor(Qt::ArrowCursor);
    }
}

void MyView::pan(const QPoint& panTo)
{
    auto hBar = horizontalScrollBar();
    auto vBar = verticalScrollBar();
    auto delta = panTo - m_prevPan;
    m_prevPan = panTo;
    hBar->setValue(hBar->value() - delta.x());
    vBar->setValue(vBar->value() - delta.y());
}

void MyView::initShow()
{
    setEnabled(true);
    myscene->dafaultState();
    myscene->clear();
    myscene->addPixmap(m_image);
    myscene->update();
    this->resetTransform();
    auto rect = m_image.rect();
    this->setSceneRect(QRect(0,0, m_image.width(), m_image.height()));
    this->fitInView(QRect(0,0, m_image.width(), m_image.height()), Qt::KeepAspectRatio);
    
}

void MyView::resizeEvent(QResizeEvent* event)
{
    if (m_image.isNull())
        return;
    this->fitInView(QRect(0, 0, m_image.width(), m_image.height()), Qt::KeepAspectRatio);
    QGraphicsView::resizeEvent(event);
}
#pragma once

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QImage>
#include <QWheelEvent>
#include "myscene.h"

class MyView  : public QGraphicsView
{
	Q_OBJECT

public:
	MyView(QWidget*parent, MyScene* scene);
	~MyView();
    void setImage(const QString& path);
    void zoom(QPoint factor);
    void zoom(QWheelEvent* event);
    void reset();
    void clear();

protected:
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event);
    
    void togglePan(bool pan, const QPoint& startPos = QPoint());
    void pan(const QPoint& panTo);
    void initShow();
signals:
    void mouseCor(const QPointF p, const QString pixValue);
private:
    QPixmap m_image;
    bool m_isPan;
    QPoint m_prevPan;
    double scaleValue;
    MyScene* myscene;
};

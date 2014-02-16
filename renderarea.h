#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QMouseEvent>
#include <QWheelEvent>

#include "wiresim.h"

class RenderArea : public QWidget
{
    Q_OBJECT
public:
    explicit RenderArea(QWidget *parent = 0);

    void SetWireSim( WireSim* wireSim ) { m_wireSim = wireSim; }

    // Tell what the active brush is (from the parent qwiedget)
    void SetBrush( WireNodeType wireNodeType ) { m_wireNodeType = wireNodeType; }

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

private:

    WireSim* m_wireSim;
    QImage m_gateImage;
    WireNodeType m_wireNodeType;

    bool m_isDragging;
    int m_lastTransX, m_lastTransY;
    int m_translationX, m_translationY;
    float m_zoom;


signals:

public slots:

};

#endif // RENDERAREA_H

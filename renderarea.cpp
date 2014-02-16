#include "renderarea.h"

namespace
{
    // Constants
    const int cTileSize = 32;
}

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
    , m_wireSim( NULL )
    , m_gateImage( ":/LogicIcons.png" )
    , m_isDragging( false )
    , m_lastTransX( 0 ), m_lastTransY( 0 )
    , m_translationX( 0 ), m_translationY( 0 )
    , m_zoom( 1.0f )
{
    setMouseTracking( true );
}

void RenderArea::paintEvent(QPaintEvent *event)
{
    if( m_wireSim == NULL ) return;

    // Important sizes
    int screenWidth = this->width();
    int screenHeight = this->height();

    int simWidth = m_wireSim->GetWidth();
    int simHeight = m_wireSim->GetHeight();

    // Start painting
    QPainter painter(this);

    painter.setTransform( QTransform::fromScale( m_zoom, m_zoom ) * QTransform::fromTranslate( m_translationX, m_translationY ) );

    // For each simulation cell
    for( int x = 0; x < simWidth; x++ )
    for( int y = 0; y < simHeight; y++ )
    {
        WireNode node = m_wireSim->GetNode( x, y );
        QColor color;

        // Set main color
        switch( WireNodeType(node.m_type) )
        {
            case cWireNodeType_None:    color.setRgb(   0,   0,   0 );  break;
            case cWireNodeType_Wire:    color.setRgb(   0,   0, 255 );  break;
            case cWireNodeType_Joint:   color.setRgb(   0, 255,   0 );  break;
            case cWireNodeType_Switch:  color.setRgb(   0, 255, 255 );  break;
            case cWireNodeType_And:     color.setRgb( 255,   0,   0 );  break;
            case cWireNodeType_Or:      color.setRgb( 255,   0, 255 );  break;
            case cWireNodeType_Not:     color.setRgb( 255, 255,   0 );  break;
            case cWireNodeType_Xor:     color.setRgb( 255, 255, 255 );  break;
        }

        // On-screen position
        int screenX = x * ( cTileSize + 2 );
        int screenY = y * ( cTileSize + 2 );

        // Background, then fill inner red
        painter.setPen( QPen( Qt::gray ) );
        painter.drawRect( QRect( QPoint( screenX, screenY ), QSize( cTileSize + 2, cTileSize + 2 ) ) );

        painter.setPen( QPen( color ) );
        painter.fillRect( QRect( QPoint( screenX + 1, screenY + 1 ), QSize( cTileSize, cTileSize ) ), QBrush( color ) );

        // In special cases, draw icons
        if( WireNodeType( node.m_type ) == cWireNodeType_And )
        {
            painter.drawImage( QRect( screenX + 1, screenY + 1, cTileSize, cTileSize ), m_gateImage, QRect( 100, 50, 100, 50 ) );
        }
        else if( WireNodeType( node.m_type ) == cWireNodeType_Or )
        {
            painter.drawImage( QRect( screenX + 1, screenY + 1, cTileSize, cTileSize ), m_gateImage, QRect( 100, 0, 100, 50 ) );
        }
        else if( WireNodeType( node.m_type ) == cWireNodeType_Not )
        {
            painter.drawImage( QRect( screenX + 1, screenY + 1, cTileSize, cTileSize ), m_gateImage, QRect( 0, 50, 100, 50 ) );
        }
        else if( WireNodeType( node.m_type ) == cWireNodeType_Xor )
        {
            painter.drawImage( QRect( screenX + 1, screenY + 1, cTileSize, cTileSize ), m_gateImage, QRect( 0, 0, 100, 50 ) );
        }
    }

    // Done painting
    painter.end();
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
    // If left-click
    if( event->button() == Qt::LeftButton )
    {
        // Important sizes
        int screenWidth = this->width();
        int screenHeight = this->height();

        int simWidth = m_wireSim->GetWidth();
        int simHeight = m_wireSim->GetHeight();

        // Convert from widget-coordinates to brush coordinates
        int tileX = m_translationX + float(event->x()) / float(screenWidth) * m_zoom / (cTileSize + 2);
        int tileY = m_translationY + float(event->y()) / float(screenHeight) * m_zoom / (cTileSize + 2);

        // Update sim buffer with the active brush
        m_wireSim->GetNode( tileX, tileY ).m_type = m_wireNodeType;
        update();
    }

    // Derp
    if( event->buttons() & Qt::RightButton )
    {
        m_isDragging = true;
        m_lastTransX = event->pos().x();
        m_lastTransX = event->pos().y();
    }
}

void RenderArea::mouseReleaseEvent(QMouseEvent *event)
{
    m_isDragging = false;
}

void RenderArea::mouseMoveEvent(QMouseEvent *event)
{
    if( m_isDragging )
    {
        m_translationX += event->pos().x() - m_lastTransX;
        m_translationY += event->pos().y() - m_lastTransY;
        m_lastTransX = event->pos().x();
        m_lastTransY = event->pos().y();
        update();
    }
}

void RenderArea::wheelEvent(QWheelEvent *event)
{
    const float cMinZoom = 0.1f;
    const float cMaxZoom = 4.0f;

    m_zoom += float(event->delta()) / 200.0f;
    m_zoom = m_zoom < cMinZoom ? cMinZoom : m_zoom; // max( min, zoom )
    m_zoom = m_zoom > cMaxZoom ? cMaxZoom : m_zoom; // Min( max, zoom )
    update();
}

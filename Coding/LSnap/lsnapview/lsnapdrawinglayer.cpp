#include "lsnapdrawinglayer.h"
#include <QtGlobal>

LSnapDrawingLayer::LSnapDrawingLayer()
{
}

LSnapDrawingLayer::~LSnapDrawingLayer()
{
}

void LSnapDrawingLayer::setMode(ShapeType m)
{ 
    m_mode = m;
    m_drawing = false;
}

void LSnapDrawingLayer::setModeFromInt(int mode)
{ 
    m_mode = (mode == 1 ? ShapeType::Rectangle : mode == 2 ? ShapeType::Ellipse : ShapeType::None);
    m_drawing = false;
}

void LSnapDrawingLayer::setPenWidth(int w)
{ 
    m_penWidth = qMax(1, qMin(w, 20)); 
}

void LSnapDrawingLayer::setColor(const QColor& color)
{
    m_color = color;
}

bool LSnapDrawingLayer::isActive() const 
{ 
    return m_mode != ShapeType::None;
}

bool LSnapDrawingLayer::isDrawing() const 
{ 
    return m_drawing; 
}

bool LSnapDrawingLayer::press(const QPoint& pos, Qt::MouseButton button)
{
    if (!isActive() || button != Qt::LeftButton) return false;
    m_drawing = true; 
    m_start = m_end = pos; 
    return true;
}

bool LSnapDrawingLayer::move(const QPoint& pos)
{
    if (!m_drawing) return false;
    m_end = pos; 
    return true;
}

bool LSnapDrawingLayer::release(const QPoint& pos, Qt::MouseButton button)
{
    if (!m_drawing || button != Qt::LeftButton) return false;
    m_drawing = false;
    QRect r = QRect(m_start, pos).normalized();
    if (!r.isEmpty() && m_mode != ShapeType::None) {
        m_items.append(Item{ m_mode, r, m_penWidth, m_color });
    }
    return true;
}

void LSnapDrawingLayer::paint(QPainter& p) const
{
    for (const auto& it : m_items)
    {
        p.setPen(QPen(it.color, it.penWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        if (it.type == ShapeType::Rectangle)
            p.drawRect(it.rect);
        else if (it.type == ShapeType::Ellipse)
            p.drawEllipse(it.rect);
    }
    if (m_drawing && m_mode != ShapeType::None)
    {
        QRect pv = QRect(m_start, m_end).normalized();
        p.setPen(QPen(m_color, m_penWidth, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin));
        if (m_mode == ShapeType::Rectangle)
            p.drawRect(pv);
        else
            p.drawEllipse(pv);
    }
}

void LSnapDrawingLayer::paintCommitted(QPainter& p) const
{
    for (const auto& it : m_items)
    {
        p.setPen(QPen(it.color, it.penWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        if (it.type == ShapeType::Rectangle) 
            p.drawRect(it.rect);
        else if (it.type == ShapeType::Ellipse) 
            p.drawEllipse(it.rect);
    }
}

void LSnapDrawingLayer::paintCommittedMapped(QPainter& p, const QPointF& offsetLogical, qreal dpr) const
{
    for (const auto& it : m_items)
    {
        const int wpx = qMax(1, int(qRound(it.penWidth * dpr)));
        p.setPen(QPen(it.color, wpx, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        QRectF rf = it.rect;
        rf.translate(offsetLogical);
        rf = QRectF(rf.left(), rf.top(), rf.width(), rf.height());
        if (it.type == ShapeType::Rectangle) 
            p.drawRect(rf);
        else if (it.type == ShapeType::Ellipse) 
            p.drawEllipse(rf);
    }
}

void LSnapDrawingLayer::clear()
{
    m_items.clear();
    m_drawing = false;
}

void LSnapDrawingLayer::clearCurrentDrawing()
{
    m_drawing = false;
}

LSnapDrawingLayer::ShapeType LSnapDrawingLayer::getCurrentMode() const
{
    return m_mode;
}

int LSnapDrawingLayer::getPenWidth() const
{
    return m_penWidth;
}

QColor LSnapDrawingLayer::getColor() const
{
    return m_color;
}

const QVector<LSnapDrawingLayer::Item>& LSnapDrawingLayer::getItems() const
{
    return m_items;
}
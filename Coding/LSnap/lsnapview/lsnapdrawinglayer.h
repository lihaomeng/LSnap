#ifndef LSNAPDRAWINGLAYER_H
#define LSNAPDRAWINGLAYER_H

#include <QWidget>
#include <QRect>
#include <QPoint>
#include <QColor>
#include <QPainter>
#include <QVector>
#include <QtGlobal>

//TODO 图层撤销和第一次修改功能
class LSnapDrawingLayer
{
public:
    enum class ShapeType { None, Rectangle, Ellipse };
    
    struct Item
    {
        ShapeType type;
        QRect rect;
        int penWidth;
        QColor color;
    };

    LSnapDrawingLayer();
    ~LSnapDrawingLayer();
    void setMode(ShapeType m);
    void setModeFromInt(int mode);
    void setPenWidth(int w);
    void setColor(const QColor& color);

    bool isActive() const;
    bool isDrawing() const;

    bool press(const QPoint& pos, Qt::MouseButton button);
    bool move(const QPoint& pos);
    bool release(const QPoint& pos, Qt::MouseButton button);

    void paint(QPainter& p) const;
    void paintCommitted(QPainter& p) const;
    void paintCommittedMapped(QPainter& p, const QPointF& offsetLogical, qreal dpr) const;

    void clear();
    void clearCurrentDrawing();

    ShapeType getCurrentMode() const;
    int getPenWidth() const;
    QColor getColor() const;
    const QVector<Item>& getItems() const;

private:
    ShapeType m_mode = ShapeType::None;
    int m_penWidth = 2;
    QColor m_color = QColor(0, 180, 255);
    bool m_drawing = false;
    QPoint m_start, m_end;
    QVector<Item> m_items;
};
#endif // LSNAPDRAWINGLAYER_H    
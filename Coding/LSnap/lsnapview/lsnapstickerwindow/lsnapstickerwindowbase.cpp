#include "lsnapstickerwindowbase.h"
#include <QPainter>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QScreen>
#include <QKeyEvent>
#include <QtGlobal>

LSnapStickerWindowBase::LSnapStickerWindowBase(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    resize(200, 150);
}

LSnapStickerWindowBase::~LSnapStickerWindowBase()
{
}

void LSnapStickerWindowBase::setSelected(bool selected)
{
    if (m_isSelected != selected) {
        m_isSelected = selected;
        onSelectionChanged(selected);
        update();
    }
}

void LSnapStickerWindowBase::setStickerSize(const QSize& size)
{
    resize(size);
}

QSize LSnapStickerWindowBase::getStickerSize() const
{
    return size();
}

void LSnapStickerWindowBase::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        currentHandle_ = getHandleAt(e->pos());
        if (currentHandle_ != None)
        {
            resizeStartPos_ = e->pos();
            resizeStartRect_ = geometry();
            resizeStartGlobalPos_ = e->globalPos();
            e->accept();
            return;
        }
    }
    QWidget::mousePressEvent(e);
}

void LSnapStickerWindowBase::mouseMoveEvent(QMouseEvent* e)
{
    if (currentHandle_ != None)
    {
        handleResizeMove(e);
        return;
    }
    
    ResizeHandle newHovered = getHandleAt(e->pos());
    if (newHovered != hoveredHandle_)
    {
        hoveredHandle_ = newHovered;
        updateCursorForHandle(hoveredHandle_);
    }
    
    QWidget::mouseMoveEvent(e);
}

void LSnapStickerWindowBase::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (currentHandle_ != None)
        {
            currentHandle_ = None;
            e->accept();
            return;
        }
    }
    QWidget::mouseReleaseEvent(e);
}

void LSnapStickerWindowBase::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {
        this->close();
        e->accept();
        return;
    }
    QWidget::keyPressEvent(e);
}

void LSnapStickerWindowBase::enterEvent(QEvent* e)
{
    setSelected(true);
    QWidget::enterEvent(e);
}

void LSnapStickerWindowBase::leaveEvent(QEvent* e)
{
    setSelected(false);
    QWidget::leaveEvent(e);
}

void LSnapStickerWindowBase::onGeometryChanged(const QRect& newGeometry)
{
    // 默认实现为空，子类可以重写
    Q_UNUSED(newGeometry);
}

void LSnapStickerWindowBase::onSelectionChanged(bool selected)
{
    // 默认实现为空，子类可以重写
    Q_UNUSED(selected);
}

void LSnapStickerWindowBase::handleResizeMove(QMouseEvent* e)
{
    QPoint delta = e->globalPos() - resizeStartGlobalPos_;
    QRect newRect = resizeStartRect_;

    switch (currentHandle_)
    {
    case TopLeft:
    case TopRight:
    case BottomRight:
    case BottomLeft:
        handleCornerResize(e, currentHandle_);
        return;
    case Right:
    case Left:
    case Top:
    case Bottom:
        handleEdgeResize(e, currentHandle_);
        return;
    case Move:
        handleMoveOperation(e);
        return;
    default:
        break;
    }
}

void LSnapStickerWindowBase::handleMoveOperation(QMouseEvent* e)
{
    QPoint delta = e->globalPos() - resizeStartGlobalPos_;
    QRect newRect = resizeStartRect_.translated(delta);
    newRect = constrainToScreen(newRect);
    setGeometry(newRect);
    onGeometryChanged(newRect);
    e->accept();
}

void LSnapStickerWindowBase::handleCornerResize(QMouseEvent* e, ResizeHandle handle)
{
    QPoint delta = e->globalPos() - resizeStartGlobalPos_;
    QRect newRect = resizeStartRect_;
    qreal aspect = getAspectRatio();

    QPoint anchor, corner;
    int widthDir = 1;  // 宽度方向：1 = 向右扩展，-1 = 向左扩展
    int heightDir = 1; // 高度方向：1 = 向下扩展，-1 = 向上扩展

    switch (handle)
    {
    case TopLeft:
        anchor = resizeStartRect_.bottomRight();
        corner = resizeStartRect_.topLeft() + delta;
        widthDir = -1;
        heightDir = -1;
        break;
    case TopRight:
        anchor = resizeStartRect_.bottomLeft();
        corner = resizeStartRect_.topRight() + delta;
        widthDir = 1;
        heightDir = -1;
        break;
    case BottomRight:
        anchor = resizeStartRect_.topLeft();
        corner = resizeStartRect_.bottomRight() + delta;
        widthDir = 1;
        heightDir = 1;
        break;
    case BottomLeft:
        anchor = resizeStartRect_.topRight();
        corner = resizeStartRect_.bottomLeft() + delta;
        widthDir = -1;
        heightDir = 1;
        break;
    default:
        return;
    }

    // 使用绝对值计算尺寸
    int dw = qAbs(corner.x() - anchor.x());
    int dh = qAbs(corner.y() - anchor.y());

    // 按较主导的方向约束比例
    if (dw >= dh * aspect) {
        dh = qRound(dw / aspect);
    } else {
        dw = qRound(dh * aspect);
    }

    // 根据方向应用符号
    corner.setX(anchor.x() + widthDir * dw);
    corner.setY(anchor.y() + heightDir * dh);

    newRect = QRect(anchor, corner).normalized();
    applyMinimumSize(newRect);
    newRect = constrainToScreen(newRect);
    setGeometry(newRect);
    onGeometryChanged(newRect);
    e->accept();
}


// void LSnapStickerWindowBase::handleCornerResize(QMouseEvent* e, ResizeHandle handle)
// {
//     QPoint delta = e->globalPos() - resizeStartGlobalPos_;
//     QRect newRect = resizeStartRect_;
//     qreal aspect = getAspectRatio();

//     QPoint anchor, corner;
//     int widthDir = 1;  // 宽度方向：1 = 向右扩展，-1 = 向左扩展
//     int heightDir = 1; // 高度方向：1 = 向下扩展，-1 = 向上扩展

//     switch (handle)
//     {
//     case TopLeft:
//         anchor = resizeStartRect_.bottomRight();
//         corner = resizeStartRect_.topLeft() + delta;
//         widthDir = -1;
//         heightDir = -1;
//         break;
//     case TopRight:
//         anchor = resizeStartRect_.bottomLeft();
//         corner = resizeStartRect_.topRight() + delta;
//         widthDir = 1;
//         heightDir = -1;
//         break;
//     case BottomRight:
//         anchor = resizeStartRect_.topLeft();
//         corner = resizeStartRect_.bottomRight() + delta;
//         widthDir = 1;
//         heightDir = 1;
//         break;
//     case BottomLeft:
//         anchor = resizeStartRect_.topRight();
//         corner = resizeStartRect_.bottomLeft() + delta;
//         widthDir = -1;
//         heightDir = 1;
//         break;
//     default:
//         return;
//     }

//     // 使用绝对值计算尺寸
//     int dw = qAbs(corner.x() - anchor.x());
//     int dh = qAbs(corner.y() - anchor.y());

//     // 按较主导的方向约束比例
//     if (dw >= dh * aspect) {
//         dh = qRound(dw / aspect);
//     } else {
//         dw = qRound(dh * aspect);
//     }

//     // 根据方向应用符号
//     corner.setX(anchor.x() + widthDir * dw);
//     corner.setY(anchor.y() + heightDir * dh);

//     newRect = QRect(anchor, corner).normalized();
//     applyMinimumSize(newRect);
//     newRect = constrainToScreen(newRect);
//     setGeometry(newRect);
//     onGeometryChanged(newRect);
//     e->accept();
// }

void LSnapStickerWindowBase::handleEdgeResize(QMouseEvent* e, ResizeHandle handle)
{
    QPoint delta = e->globalPos() - resizeStartGlobalPos_;
    QRect newRect = resizeStartRect_;

    switch (handle)
    {
    case Right:
        newRect.setRight(resizeStartRect_.right() + delta.x());
        break;
    case Left:
        newRect.setLeft(resizeStartRect_.left() + delta.x());
        break;
    case Top:
        newRect.setTop(resizeStartRect_.top() + delta.y());
        break;
    case Bottom:
        newRect.setBottom(resizeStartRect_.bottom() + delta.y());
        break;
    }

    applyMinimumSize(newRect);
    newRect = constrainToScreen(newRect);
    setGeometry(newRect);
    onGeometryChanged(newRect);
    e->accept();
}

qreal LSnapStickerWindowBase::getAspectRatio() const
{
    // 默认宽高比为1:1，子类可以重写
    return 1.0;
}

QRect LSnapStickerWindowBase::constrainToScreen(const QRect& rect) const
{
    QRect screenBounds = QGuiApplication::primaryScreen()->virtualGeometry();
    QRect constrained = rect;
    
    if (constrained.left() < screenBounds.left()) 
        constrained.moveLeft(screenBounds.left());
    if (constrained.right() > screenBounds.right()) 
        constrained.moveRight(screenBounds.right());
    if (constrained.top() < screenBounds.top()) 
        constrained.moveTop(screenBounds.top());
    if (constrained.bottom() > screenBounds.bottom()) 
        constrained.moveBottom(screenBounds.bottom());
    
    return constrained;
}

void LSnapStickerWindowBase::applyMinimumSize(QRect& rect) const
{
    if (rect.width() < 20) rect.setWidth(20);
    if (rect.height() < 20) rect.setHeight(20);
}

void LSnapStickerWindowBase::drawResizeHandles(QPainter& painter)
{
    if (!m_isSelected) return;
    
    painter.save();
    
    QColor handleColor(0, 160, 255, 220);
    QColor handleBorder(255, 255, 255, 240);
    
    painter.setPen(QPen(handleBorder, 1));
    painter.setBrush(QBrush(handleColor));
    
    const ResizeHandle corners[] = { TopLeft, TopRight, BottomRight, BottomLeft };
    for (ResizeHandle handle : corners)
    {
        QRect handleRect = getHandleRect(handle);
        if (hoveredHandle_ == handle)
            painter.setBrush(QBrush(handleColor.lighter(130)));
        else
            painter.setBrush(QBrush(handleColor));
        painter.drawRect(handleRect);
    }
    painter.restore();
}

QRect LSnapStickerWindowBase::getHandleRect(ResizeHandle handle) const
{
    QRect r = rect();
    int half = handleSize_ / 2;
    int size = handleSize_ + 2;
    
    switch (handle)
    {
    case TopLeft:     return QRect(r.left() - half, r.top() - half, size, size);
    case Top:         return QRect(r.center().x() - half, r.top() - half, size, size);
    case TopRight:    return QRect(r.right() - half, r.top() - half, size, size);
    case Right:       return QRect(r.right() - half, r.center().y() - half, size, size);
    case BottomRight: return QRect(r.right() - half, r.bottom() - half, size, size);
    case Bottom:      return QRect(r.center().x() - half, r.bottom() - half, size, size);
    case BottomLeft:  return QRect(r.left() - half, r.bottom() - half, size, size);
    case Left:        return QRect(r.left() - half, r.center().y() - half, size, size);
    default:          return QRect();
    }
}

LSnapStickerWindowBase::ResizeHandle LSnapStickerWindowBase::getHandleAt(const QPoint& pos) const
{
    const ResizeHandle corners[] = { TopLeft, TopRight, BottomRight, BottomLeft };
    for (ResizeHandle handle : corners)
    {
        QRect r = getHandleRect(handle);
        r.adjust(-2, -2, 2, 2);
        if (r.contains(pos))
            return handle;
    }
    
    if (rect().contains(pos))
        return Move;

    return None;
}

void LSnapStickerWindowBase::updateCursorForHandle(ResizeHandle handle)
{
    switch (handle)
    {
    case TopLeft:
    case BottomRight:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case Top:
    case Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case TopRight:
    case BottomLeft:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case Left:
    case Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case Move:
        setCursor(Qt::SizeAllCursor);
        break;
    default:
        unsetCursor();
        break;
    }
}
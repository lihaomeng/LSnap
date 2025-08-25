#include "lsnapoverlaywindow.h"
#include "lsnapselectionactionbar.h"
#include "lsnapdrawinglayer.h"
#include "lsnapstickerwindow/lsnappicstickerwindow.h"
#include "lsnapstickerwindow/stickerwindow.h"
#include "gifrecorder.h"
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QHBoxLayout>
#include <QClipboard>

LSnapOverlayWindow::LSnapOverlayWindow(QWidget* parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    captureScreen();
    createButtonBar();//  创建按钮条 一级控件
}

void LSnapOverlayWindow::captureScreen()
{
    QScreen* scr = QGuiApplication::primaryScreen();
    if (!scr)
        return;

    m_captureVirtualGeom = scr->virtualGeometry();
    m_captureDpr = scr->devicePixelRatio();
    const QRect v = m_captureVirtualGeom;
    m_screenShot = scr->grabWindow(0, v.x(), v.y(), v.width(), v.height());
    m_screenShot.setDevicePixelRatio(m_captureDpr);
}

void LSnapOverlayWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    if (!m_recordHollow && !m_screenShot.isNull())
        p.drawPixmap(rect(), m_screenShot, m_screenShot.rect());

    const QColor overlay(0, 0, 0, 120);
    if (!m_recordHollow)
    {
        if (m_selection.isNull() || !m_selection.isValid())
        {
            p.fillRect(rect(), overlay);
            return;
        }
        QRect sel = m_selection;
        const int w = width(), h = height();
        p.fillRect(QRect(0, 0, w, sel.top()), overlay);
        p.fillRect(QRect(0, sel.top(), sel.left(), sel.height()), overlay);
        p.fillRect(QRect(sel.right() + 1, sel.top(), w - sel.right() - 1, sel.height()), overlay);
        p.fillRect(QRect(0, sel.bottom() + 1, w, h - sel.bottom() - 1), overlay);
    }

    QPen pen(QColor(0, 180, 255), 2);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawRect(m_selection);

    if (m_snapStatus == 1)
       drawResizeHandles(p);

    // 矩形 椭圆这些图层
    QPen keepPen(QColor(0, 180, 255), 2);
    m_drawing.paint(p);
    p.setPen(keepPen);
}

void LSnapOverlayWindow::drawResizeHandles(QPainter& painter)
{
    if (m_selection.isNull())
        return;
    painter.save();
    QColor handleColor(0, 180, 255);
    QColor handleBorder(255, 255, 255);
    painter.setPen(QPen(handleBorder, 1));
    painter.setBrush(QBrush(handleColor));
    for (int i = TopLeft; i <= Left; ++i)
    {
        ResizeHandle handle = static_cast<ResizeHandle>(i);
        QRect handleRect = getHandleRect(handle);
        if (m_hoveredHandle == handle)
            painter.setBrush(QBrush(handleColor.lighter(150))); // 悬停时高亮
        else
            painter.setBrush(QBrush(handleColor));
        painter.drawRect(handleRect);
    }
    painter.restore();
}

void LSnapOverlayWindow::createButtonBar()
{
    auto* pSelectionActionBar = new LSnapSelectionActionBar(this);
    pSelectionActionBar->addDefaultButtons();
    m_pActionBar = pSelectionActionBar;

    connect(pSelectionActionBar, &LSnapSelectionActionBar::shapeModeChanged, this, &LSnapOverlayWindow::onShapeModeChanged);
    connect(pSelectionActionBar, &LSnapSelectionActionBar::lineWidthChanged, this, &LSnapOverlayWindow::onLineWidthChanged);

    connect(pSelectionActionBar, &LSnapSelectionActionBar::saveClicked, this, [this]{
        // TODO 保存逻辑
        close();
    });
    
    connect(pSelectionActionBar, &LSnapSelectionActionBar::copyClicked, this, [this]{
        copySelectionToClipboard();
        close();
    });

    connect(pSelectionActionBar, &LSnapSelectionActionBar::cancelClicked, this, [this]{
            close();
    });

    connect(pSelectionActionBar, &LSnapSelectionActionBar::pasteClicked, this, [this]{
        QPixmap selected = getSelectionPixmap();
        if (!selected.isNull())
        {
            //LSnapPicStickerWindow* pPicSticker = new LSnapPicStickerWindow(selected);  //TODO 存在bug 暂时不使用
            StickerWindow* pPicSticker = new StickerWindow(selected);
            pPicSticker->move(mapToGlobal(m_selection.topLeft()));
            pPicSticker->show();
        }
        this->close();
    });

    //////////////////////////////////////////////////////////////// GIF 录制
    if (!m_pGifRecorder)
    {
        m_pGifRecorder = new GifRecorder(this);
        m_pGifRecorder->setFrameSource([this]{ return getSelectionPixmap1(true).toImage(); });
    }
   
    connect(pSelectionActionBar, &LSnapSelectionActionBar::gifStartForOverlayWindow, this, [this] {
        m_pGifRecorder->startCapture();
        m_recordHollow = true;
        update();
    });
    connect(pSelectionActionBar, &LSnapSelectionActionBar::gifStopForOverlayWindow, this, [this] {
        m_pGifRecorder->stopCapture();
        m_recordHollow = false;
        update();
        });
}

void LSnapOverlayWindow::keyPressEvent(QKeyEvent* pKeyEvent)  // esc close shadow
{
    if (pKeyEvent->key() == Qt::Key_Escape)
    {
        emit canceled();
        pKeyEvent->accept();
        this->close();
        return;
    }
    QWidget::keyPressEvent(pKeyEvent);
}

void LSnapOverlayWindow::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    raise();
    activateWindow();
    setFocus(Qt::ActiveWindowFocusReason);
    grabKeyboard();
}

void LSnapOverlayWindow::hideEvent(QHideEvent* e)
{
    hideActionBar();
    releaseKeyboard();
    clearMask();
    QWidget::hideEvent(e);
}

void LSnapOverlayWindow::mousePressEvent(QMouseEvent* pMouseEvent)
{
    if (handleDrawingPress(pMouseEvent))
        return;

    if (pMouseEvent->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(pMouseEvent);
        return;
    }
    if (handleResizePress(pMouseEvent))
        return;

    if (handleExpandSelection(pMouseEvent))
        return;

    if (handleDraggingPress(pMouseEvent))
        return;

    QWidget::mousePressEvent(pMouseEvent);
}

bool LSnapOverlayWindow::handleExpandSelection(QMouseEvent* e) // distance may exits bug
{
    if (m_snapStatus != 1)
        return false;

    QPoint pos = e->pos();
    if (m_selection.contains(pos))
        return false;

    int distLeft = qAbs(pos.x() - m_selection.left());
    int distRight = qAbs(pos.x() - m_selection.right());
    int distTop = qAbs(pos.y() - m_selection.top());
    int distBottom = qAbs(pos.y() - m_selection.bottom());

    if (distLeft <= distRight && distLeft <= distTop && distLeft <= distBottom)
        m_selection.setLeft(qMin(m_selection.left(), pos.x()));
    else if (distRight <= distLeft && distRight <= distTop && distRight <= distBottom)
        m_selection.setRight(qMax(m_selection.right(), pos.x()));
    else if (distTop <= distLeft && distTop <= distRight && distTop <= distBottom)
        m_selection.setTop(qMin(m_selection.top(), pos.y()));
    else
        m_selection.setBottom(qMax(m_selection.bottom(), pos.y()));

    m_selection = m_selection.normalized();
    update();
    updateAndShowActionBar();
    e->accept();
    return true;
}

bool LSnapOverlayWindow::handleDrawingPress(QMouseEvent* e)
{
    if (m_drawing.press(e->pos(), e->button()))
    {
        update();
        e->accept();
        return true;
    }
    return false;
}

bool LSnapOverlayWindow::handleResizePress(QMouseEvent* e)
{
    if (m_snapStatus != 1)
        return false;

    m_pActionBar->hide(); // TODO 移动或者改变大小时，选择条消失  add动画
    m_currentHandle = getHandleAt(e->pos());
    if (m_currentHandle != None)
    {
        m_resizeStartPos = e->pos();
        m_resizeStartRect = m_selection;
        e->accept();
        return true;
    }
    return false;
}

bool LSnapOverlayWindow::handleDraggingPress(QMouseEvent* e)
{
    m_bDragging = true;
    m_pressPos = e->pos();
    m_currrentPos = m_pressPos;
    m_selection = QRect();
    m_snapStatus = 0;
    hideActionBar();
    setCursor(Qt::CrossCursor);
    update();
    e->accept();
    return true;
}

void LSnapOverlayWindow::mouseMoveEvent(QMouseEvent* e)
{
    if (handleDrawingMove(e)) return;
    if (handleResizeMove(e)) return;
    if (handleDraggingMove(e)) return;
    handleHoverUpdate(e->pos());
    QWidget::mouseMoveEvent(e);
}

bool LSnapOverlayWindow::handleDrawingMove(QMouseEvent* e)
{
    if (m_drawing.move(e->pos()))
    {
        update();
        e->accept();
        return true;
    }
    return false;
}

bool LSnapOverlayWindow::handleResizeMove(QMouseEvent* e)
{
    if (m_currentHandle == None)
        return false;

    QPoint delta = e->pos() - m_resizeStartPos;
    QRect newRect = m_resizeStartRect;
    QRect screenBounds = rect();

    switch (m_currentHandle)
    {
    case TopLeft:
        newRect.setTopLeft(m_resizeStartRect.topLeft() + delta);
        break;
    case Top:
        newRect.setTop(m_resizeStartRect.top() + delta.y());
        break;
    case TopRight:
        newRect.setTopRight(m_resizeStartRect.topRight() + delta);
        break;
    case Right:
        newRect.setRight(m_resizeStartRect.right() + delta.x());
        break;
    case BottomRight:
        newRect.setBottomRight(m_resizeStartRect.bottomRight() + delta);
        break;
    case Bottom:
        newRect.setBottom(m_resizeStartRect.bottom() + delta.y());
        break;
    case BottomLeft:
        newRect.setBottomLeft(m_resizeStartRect.bottomLeft() + delta);
        break;
    case Left:
        newRect.setLeft(m_resizeStartRect.left() + delta.x());
        break;
    case Move:
        newRect = m_resizeStartRect.translated(delta);
        // 水平边界检查
        if (newRect.left() < screenBounds.left())
            newRect.moveLeft(screenBounds.left());
        else if (newRect.right() > screenBounds.right())
            newRect.moveRight(screenBounds.right());
        // 垂直边界检查
        if (newRect.top() < screenBounds.top())
            newRect.moveTop(screenBounds.top());
        else if (newRect.bottom() > screenBounds.bottom())
            newRect.moveBottom(screenBounds.bottom());
        m_selection = newRect;
        update();
        e->accept();
        return true;  // Move 操作直接返回
    }

    // 最小尺寸检查
    if (newRect.width() < 10)
    {
        if (m_currentHandle == TopLeft || m_currentHandle == Left || m_currentHandle == BottomLeft)
            newRect.setLeft(newRect.right() - 10);
        else
            newRect.setRight(newRect.left() + 10);
    }
    if (newRect.height() < 10)
    {
        if (m_currentHandle == TopLeft || m_currentHandle == Top || m_currentHandle == TopRight)
            newRect.setTop(newRect.bottom() - 10);
        else
            newRect.setBottom(newRect.top() + 10);
    }
    newRect = newRect.intersected(screenBounds);  // 确保不超出屏幕边界

    m_selection = newRect.normalized();
    update();
    e->accept();
    return true;
}

bool LSnapOverlayWindow::handleDraggingMove(QMouseEvent* e)
{
    if (!m_bDragging)
        return false;

    m_currrentPos = e->pos();
    m_selection = QRect(m_pressPos, m_currrentPos).normalized();
    update();
    e->accept();
    return true;
}

void LSnapOverlayWindow::handleHoverUpdate(const QPoint& pos)
{
    if (m_snapStatus != 1)
        return;

    ResizeHandle newHovered = getHandleAt(pos);
    if (newHovered != m_hoveredHandle)
    {
        m_hoveredHandle = newHovered;
        updateCursorForHandle(m_hoveredHandle);
        update();
    }
}

void LSnapOverlayWindow::mouseReleaseEvent(QMouseEvent* pMouseEvent)
{
    if (handleDrawingRelease(pMouseEvent))
        return;

    if (handleResizeRelease(pMouseEvent))
        return;

    if (handleDraggingRelease(pMouseEvent))
        return;

    QWidget::mouseReleaseEvent(pMouseEvent);
}

bool LSnapOverlayWindow::handleDrawingRelease(QMouseEvent* e)
{
    if (m_drawing.release(e->pos(), e->button()))
    {
        update();
        e->accept();
        return true;
    }
    return false;
}

bool LSnapOverlayWindow::handleResizeRelease(QMouseEvent* e)
{
    if (m_currentHandle == None)
        return false;

    m_currentHandle = None;
    updateAndShowActionBar(); // 重新显示按钮
    e->accept();
    return true;
}

bool LSnapOverlayWindow::handleDraggingRelease(QMouseEvent* e)
{
    if (!m_bDragging || e->button() != Qt::LeftButton)
        return false;

    m_bDragging = false;
    m_currrentPos = e->pos();
    m_selection = QRect(m_pressPos, m_currrentPos).normalized();
    unsetCursor();
    emit selectionFinished(m_selection);
    update();
    e->accept();

    m_snapStatus = 1;
    updateAndShowActionBar();
    return true;
}

// 复制的逻辑
void LSnapOverlayWindow::copySelectionToClipboard()
{
    if (m_selection.isNull() || !m_selection.isValid())
        return;

    if (m_screenShot.isNull())
        return;

    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    qreal devicePixelRatio = primaryScreen->devicePixelRatio();

    QRect physicalSelection = m_selection;
    QPoint overlayOffset = mapToGlobal(QPoint(0, 0));
    QRect virtualGeom = primaryScreen->virtualGeometry();
    physicalSelection.translate(overlayOffset - virtualGeom.topLeft());
    physicalSelection = QRect(
        physicalSelection.x() * devicePixelRatio,
        physicalSelection.y() * devicePixelRatio,
        physicalSelection.width() * devicePixelRatio,
        physicalSelection.height() * devicePixelRatio
    );
    QRect validSelection = physicalSelection.intersected(m_screenShot.rect());
    if (validSelection.isEmpty())
        return;

    QPixmap selectedArea = getSelectionPixmap();
    selectedArea.setDevicePixelRatio(devicePixelRatio);
    if (!selectedArea.isNull())
    {
        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setPixmap(selectedArea);
    }
}

QPixmap LSnapOverlayWindow::getSelectionPixmap()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen)
        return QPixmap();

    const qreal dpr = screen->devicePixelRatio();
    const QRect virtualGeom = screen->virtualGeometry();

    QImage composed = m_screenShot.toImage();
    QPainter p(&composed);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    const QPointF overlayGlobalTopLeft = mapToGlobal(QPoint(0,0));
    const QPointF offsetLogical = overlayGlobalTopLeft - m_captureVirtualGeom.topLeft();
    m_drawing.paintCommittedMapped(p, offsetLogical, m_captureDpr);

    QRect logicalSel = m_selection;
    logicalSel.translate(mapToGlobal(QPoint(0,0)) - m_captureVirtualGeom.topLeft());
    QRect physicalSel(
        qRound(logicalSel.x() * m_captureDpr),
        qRound(logicalSel.y() * m_captureDpr),
        qRound(logicalSel.width() * m_captureDpr),
        qRound(logicalSel.height() * m_captureDpr)
    );

    // 裁剪输出
    QRect valid = physicalSel.intersected(QRect(QPoint(0,0), composed.size()));
    if (valid.isEmpty()) return QPixmap();

    QPixmap out = QPixmap::fromImage(composed.copy(valid));
    out.setDevicePixelRatio(m_captureDpr);
    return out;
}

QRect LSnapOverlayWindow::getHandleRect(ResizeHandle handle) const
{
    if (m_selection.isNull())
        return QRect();

    QRect sel = m_selection;
    int half = m_handleSize / 2;

    switch (handle) {
    case TopLeft:     return QRect(sel.left() - half, sel.top() - half, m_handleSize, m_handleSize);
    case Top:         return QRect(sel.center().x() - half, sel.top() - half, m_handleSize, m_handleSize);
    case TopRight:    return QRect(sel.right() - half, sel.top() - half, m_handleSize, m_handleSize);
    case Right:       return QRect(sel.right() - half, sel.center().y() - half, m_handleSize, m_handleSize);
    case BottomRight: return QRect(sel.right() - half, sel.bottom() - half, m_handleSize, m_handleSize);
    case Bottom:      return QRect(sel.center().x() - half, sel.bottom() - half, m_handleSize, m_handleSize);
    case BottomLeft:  return QRect(sel.left() - half, sel.bottom() - half, m_handleSize, m_handleSize);
    case Left:        return QRect(sel.left() - half, sel.center().y() - half, m_handleSize, m_handleSize);
    default:          return QRect();
    }
}

LSnapOverlayWindow::ResizeHandle LSnapOverlayWindow::getHandleAt(const QPoint& pos) const
{
    if (m_selection.isNull() || m_snapStatus != 1)
        return None;

    for (int i = TopLeft; i <= Left; ++i) //if in 8 point 
    {
        ResizeHandle handle = static_cast<ResizeHandle>(i);
        if (getHandleRect(handle).contains(pos))
            return handle;
    }

    if (m_selection.contains(pos))//in region
        return Move;
    return None;
}

void LSnapOverlayWindow::updateCursorForHandle(ResizeHandle handle)
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

// 控制selectbar的移动
void LSnapOverlayWindow::updateAndShowActionBar()
{
    if (!m_pActionBar || m_selection.isNull())
        return;

    m_pActionBar->adjustSize();
    QSize barSize = m_pActionBar->size();
    int x = m_selection.center().x() - barSize.width() / 2;
    int y = m_selection.bottom() + 10;
    x = qMax(10, qMin(x, width() - barSize.width() - 10));
    y = qMin(y, height() - barSize.height() - 10);
    m_pActionBar->move(x, y);
    m_pActionBar->show();
    m_pActionBar->raise();
}

void LSnapOverlayWindow::hideActionBar()
{
    if (m_pActionBar) m_pActionBar->hide();
    m_drawing.setMode(LSnapDrawingLayer::ShapeType::None);
}

void LSnapOverlayWindow::onShapeModeChanged(int mode)
{
    m_drawing.setModeFromInt(mode);
    update();
}

void LSnapOverlayWindow::onLineWidthChanged(int w)
{
    m_drawing.setPenWidth(w);
    update();
}

QPixmap LSnapOverlayWindow::getSelectionPixmap1(bool live)
{
    if (m_selection.isNull() || !m_selection.isValid())
        return QPixmap();

    const QPoint overlayGlobalTopLeft = mapToGlobal(QPoint(0,0));
    QRect logicalSel = m_selection;
    logicalSel.translate(overlayGlobalTopLeft - m_captureVirtualGeom.topLeft());

    const QRect physSel = logicalSel;

    if (live || m_recordHollow)
    {
        if (QScreen* scr = QGuiApplication::primaryScreen())
        {
            QPixmap frame = scr->grabWindow(0, physSel.x(), physSel.y(), physSel.width(), physSel.height());
            frame.setDevicePixelRatio(m_captureDpr);
            return frame;
        }
        return QPixmap();
    }

    QImage composed = m_screenShot.toImage();
    const QRect bounds(QPoint(0,0), composed.size());
    const QRect valid = physSel.intersected(bounds);
    if (valid.isEmpty()) return QPixmap();

    QPixmap out = QPixmap::fromImage(composed.copy(valid));
    out.setDevicePixelRatio(m_captureDpr);
    return out;
}
#include "overlaywindow.h"
#include "stickerwindow.h"
#include "lview/selectionactionbar.h"
#include "recorder/gifrecorder.h"
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QHBoxLayout>
#include <QClipboard>

OverlayWindow::OverlayWindow(QWidget* parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //| Qt::Tool | Qt::WindowDoesNotAcceptFocus
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    captureScreen();
    createButtonBar();//  创建按钮条 一级控件
}

void OverlayWindow::createButtonBar()
{
    auto* bar = new SelectionActionBar(this);
    bar->addDefaultButtons();
    m_pActionBar = bar;

    connect(bar, &SelectionActionBar::shapeModeChanged, this, &OverlayWindow::onShapeModeChanged);
    connect(bar, &SelectionActionBar::lineWidthChanged, this, &OverlayWindow::onLineWidthChanged);
    connect(bar, &SelectionActionBar::saveClicked, this, [this]{
        // TODO 保存逻辑
        close();
    });
    connect(bar, &SelectionActionBar::copyClicked, this, [this]{
        copySelectionToClipboard();
        close();
    });
    connect(bar, &SelectionActionBar::pasteClicked, this, [this]{
        QPixmap selected = getSelectionPixmap();
        if (!selected.isNull())
        {
            StickerWindow* sticker = new StickerWindow(selected);
            sticker->move(mapToGlobal(selection_.topLeft()));
            sticker->show();
        }
        close();
    });
    connect(bar, &SelectionActionBar::cancelClicked, this, [this]{
        close();
    });


    ////////////////////////////////////////////////////////////// GIF 录制
    if (!m_pGifRecorder)
    {
        m_pGifRecorder = new GifRecorder(this);
        m_pGifRecorder->setFrameSource([this]{ return getSelectionPixmap1(true).toImage(); });
        //connect(m_pGifRecorder, &GifRecorder::infoUpdated, m_pActionBar, &SelectionActionBar::updateGifInfo);
        connect(m_pGifRecorder, &GifRecorder::saved, this, [](const QString& path, bool ok){
            qDebug() << "GIF frames saved to:" << path << "ok=" << ok;
        });
    }
    //recordHollow_控制底图
    connect(m_pGifRecorder, &GifRecorder::started, this, [this]{
        recordHollow_ = true;
        update();
    });
    connect(m_pGifRecorder, &GifRecorder::stopped, this, [this]{
        recordHollow_ = false;
        update();
    });
    connect(m_pGifRecorder, &GifRecorder::canceled, this, [this]{
        recordHollow_ = false;
        update();
    });

    connect(bar, &SelectionActionBar::gifStart,  m_pGifRecorder, &GifRecorder::start);
    connect(bar, &SelectionActionBar::gifStop,   m_pGifRecorder, &GifRecorder::stop);
    connect(bar, &SelectionActionBar::gifCancel, m_pGifRecorder, &GifRecorder::cancel);
}

void OverlayWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    if (!recordHollow_ && !m_screenShot.isNull()) {
        p.drawPixmap(rect(), m_screenShot, m_screenShot.rect());
    }

    const QColor overlay(0, 0, 0, 120);
    if (!recordHollow_) {
        if (selection_.isNull() || !selection_.isValid()) {
            p.fillRect(rect(), overlay);
            return;
        }
        // 分段遮罩（选区外）
        QRect sel = selection_;
        const int w = width(), h = height();
        p.fillRect(QRect(0, 0, w, sel.top()), overlay);
        p.fillRect(QRect(0, sel.top(), sel.left(), sel.height()), overlay);
        p.fillRect(QRect(sel.right() + 1, sel.top(), w - sel.right() - 1, sel.height()), overlay);
        p.fillRect(QRect(0, sel.bottom() + 1, w, h - sel.bottom() - 1), overlay);
    }


    QRect sel = selection_;

    QPen pen(QColor(0, 180, 255), 2);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawRect(sel);

    if (m_snapStatus == 1)
    {
        drawResizeHandles(p);
    }

    QPen keepPen(QColor(0, 180, 255), 2);
    drawing_.paint(p);
    p.setPen(keepPen);
}

void OverlayWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {
        emit canceled();
        e->accept();
        close();
        return;
    }
    QWidget::keyPressEvent(e);
}

void OverlayWindow::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    raise();
    activateWindow();
    setFocus(Qt::ActiveWindowFocusReason);
    grabKeyboard();
}

void OverlayWindow::hideEvent(QHideEvent* e)
{
    hideActionBar();
    releaseKeyboard();
    clearMask();
    QWidget::hideEvent(e);
}

void OverlayWindow::mousePressEvent(QMouseEvent* e)
{
    if (drawing_.press(e->pos(), e->button()))
    {
        update();
        e->accept();
        return;
    }
    if (e->button() == Qt::LeftButton)
    {
        if (m_snapStatus == 1) {
            // 截图完成后，检查是否点击控制点
            currentHandle_ = getHandleAt(e->pos());
            if (currentHandle_ != None) {
                resizeStartPos_ = e->pos();
                resizeStartRect_ = selection_;
                e->accept();
                return;
            }
        }

        // 开始新的截图
        dragging_ = true;
        pressPos_ = e->pos();
        currPos_ = pressPos_;
        selection_ = QRect();
        m_snapStatus = 0;
        hideActionBar();
        setCursor(Qt::CrossCursor);
        update();
        e->accept();
        return;
    }
    QWidget::mousePressEvent(e);
}

void OverlayWindow::mouseMoveEvent(QMouseEvent* e)
{
    if (drawing_.move(e->pos()))
        {
            update();
            e->accept();
            return;
        }

    if (currentHandle_ != None)
    {

        QPoint delta = e->pos() - resizeStartPos_;
        QRect newRect = resizeStartRect_;
        QRect screenBounds = rect();

        switch (currentHandle_) {
        case TopLeft:
            newRect.setTopLeft(resizeStartRect_.topLeft() + delta);
            break;
        case Top:
            newRect.setTop(resizeStartRect_.top() + delta.y());
            break;
        case TopRight:
            newRect.setTopRight(resizeStartRect_.topRight() + delta);
            break;
        case Right:
            newRect.setRight(resizeStartRect_.right() + delta.x());
            break;
        case BottomRight:
            newRect.setBottomRight(resizeStartRect_.bottomRight() + delta);
            break;
        case Bottom:
            newRect.setBottom(resizeStartRect_.bottom() + delta.y());
            break;
        case BottomLeft:
            newRect.setBottomLeft(resizeStartRect_.bottomLeft() + delta);
            break;
        case Left:
            newRect.setLeft(resizeStartRect_.left() + delta.x());
            break;
        case Move:
            // 整体移动：限制在屏幕边界内
            newRect = resizeStartRect_.translated(delta);

            // 水平边界检查
            if (newRect.left() < screenBounds.left()) {
                newRect.moveLeft(screenBounds.left());
            } else if (newRect.right() > screenBounds.right()) {
                newRect.moveRight(screenBounds.right());
            }

            // 垂直边界检查
            if (newRect.top() < screenBounds.top()) {
                newRect.moveTop(screenBounds.top());
            } else if (newRect.bottom() > screenBounds.bottom()) {
                newRect.moveBottom(screenBounds.bottom());
            }

            selection_ = newRect;
            update();
            e->accept();
            return; // Move操作不需要后续处理
        }

        // 其他操作（调整大小）的边界和最小尺寸检查
        if (currentHandle_ != Move) {
            // 确保最小尺寸
            if (newRect.width() < 10) {
                if (currentHandle_ == TopLeft || currentHandle_ == Left || currentHandle_ == BottomLeft) {
                    newRect.setLeft(newRect.right() - 10);
                } else {
                    newRect.setRight(newRect.left() + 10);
                }
            }
            if (newRect.height() < 10) {
                if (currentHandle_ == TopLeft || currentHandle_ == Top || currentHandle_ == TopRight) {
                    newRect.setTop(newRect.bottom() - 10);
                } else {
                    newRect.setBottom(newRect.top() + 10);
                }
            }

            // 确保不超出屏幕边界
            newRect = newRect.intersected(screenBounds);
        }

        selection_ = newRect.normalized();
        update();
        e->accept();
        return;
    }


    if (dragging_) {
        // 正在拖拽选择新区域
        currPos_ = e->pos();
        selection_ = QRect(pressPos_, currPos_).normalized();
        update();
        e->accept();
        return;
    }

    // 更新鼠标悬停状态
    if (m_snapStatus == 1)
    {
        ResizeHandle newHovered = getHandleAt(e->pos());
        if (newHovered != hoveredHandle_)
        {
            hoveredHandle_ = newHovered;
            updateCursorForHandle(hoveredHandle_);
            update();
        }
    }

    QWidget::mouseMoveEvent(e);
}

void OverlayWindow::mouseReleaseEvent(QMouseEvent* e)
{
    if (drawing_.release(e->pos(), e->button()))
    {
        update();
        e->accept();
        return;
    }

    if (currentHandle_ != None) {
        // 完成控制点拖拽
        currentHandle_ = None;
        updateAndShowActionBar(); // 重新显示按钮
        e->accept();
        return;
    }

    if (dragging_ && e->button() == Qt::LeftButton)
    {
        dragging_ = false;
        currPos_ = e->pos();
        selection_ = QRect(pressPos_, currPos_).normalized();
        unsetCursor();
        emit selectionFinished(selection_);
        update();
        e->accept();

        m_snapStatus = 1;
        updateAndShowActionBar();
        return;
    }

    QWidget::mouseReleaseEvent(e);
}

// 复制的逻辑
void OverlayWindow::copySelectionToClipboard()
{
    if (selection_.isNull() || !selection_.isValid())
        return;

    if (m_screenShot.isNull())
        return;

    QScreen* primaryScreen = QGuiApplication::primaryScreen();
    qreal devicePixelRatio = primaryScreen->devicePixelRatio();

    QRect physicalSelection = selection_;
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

void OverlayWindow::captureScreen()
{
    QScreen* scr = QGuiApplication::primaryScreen();
    if (!scr) return;

    m_captureVirtualGeom = scr->virtualGeometry();
    m_captureDpr = scr->devicePixelRatio();

    const QRect v = m_captureVirtualGeom;
    m_screenShot = scr->grabWindow(0, v.x(), v.y(), v.width(), v.height());
    m_screenShot.setDevicePixelRatio(m_captureDpr);
    //qDebug() << v << m_captureDpr;
}

QPixmap OverlayWindow::getSelectionPixmap()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen) return QPixmap();

    const qreal dpr = screen->devicePixelRatio();
    const QRect virtualGeom = screen->virtualGeometry();

    QImage composed = m_screenShot.toImage();
    QPainter p(&composed);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    const QPointF overlayGlobalTopLeft = mapToGlobal(QPoint(0,0));
    const QPointF offsetLogical = overlayGlobalTopLeft - m_captureVirtualGeom.topLeft();
    drawing_.paintCommittedMapped(p, offsetLogical, m_captureDpr);

    QRect logicalSel = selection_;
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

void OverlayWindow::drawResizeHandles(QPainter& painter)
{
    if (selection_.isNull()) return;

    painter.save();

    QColor handleColor(0, 180, 255);
    QColor handleBorder(255, 255, 255);

    painter.setPen(QPen(handleBorder, 1));
    painter.setBrush(QBrush(handleColor));

    // 绘制8个控制点
    for (int i = TopLeft; i <= Left; ++i) {
        ResizeHandle handle = static_cast<ResizeHandle>(i);
        QRect handleRect = getHandleRect(handle);

        if (hoveredHandle_ == handle) {
            // 悬停时高亮
            painter.setBrush(QBrush(handleColor.lighter(150)));
        } else {
            painter.setBrush(QBrush(handleColor));
        }

        painter.drawRect(handleRect);
    }

    painter.restore();
}

QRect OverlayWindow::getHandleRect(ResizeHandle handle) const
{
    if (selection_.isNull()) return QRect();

    QRect sel = selection_;
    int half = handleSize_ / 2;

    switch (handle) {
    case TopLeft:     return QRect(sel.left() - half, sel.top() - half, handleSize_, handleSize_);
    case Top:         return QRect(sel.center().x() - half, sel.top() - half, handleSize_, handleSize_);
    case TopRight:    return QRect(sel.right() - half, sel.top() - half, handleSize_, handleSize_);
    case Right:       return QRect(sel.right() - half, sel.center().y() - half, handleSize_, handleSize_);
    case BottomRight: return QRect(sel.right() - half, sel.bottom() - half, handleSize_, handleSize_);
    case Bottom:      return QRect(sel.center().x() - half, sel.bottom() - half, handleSize_, handleSize_);
    case BottomLeft:  return QRect(sel.left() - half, sel.bottom() - half, handleSize_, handleSize_);
    case Left:        return QRect(sel.left() - half, sel.center().y() - half, handleSize_, handleSize_);
    default:          return QRect();
    }
}

OverlayWindow::ResizeHandle OverlayWindow::getHandleAt(const QPoint& pos) const
{
    if (selection_.isNull() || m_snapStatus != 1) return None;

    // 检查是否在控制点上
    for (int i = TopLeft; i <= Left; ++i)
    {
        ResizeHandle handle = static_cast<ResizeHandle>(i);
        if (getHandleRect(handle).contains(pos)) {
            return handle;
        }
    }

    // 检查是否在选区内（整体移动）
    if (selection_.contains(pos)) {
        return Move;
    }

    return None;
}

void OverlayWindow::updateCursorForHandle(ResizeHandle handle)
{
    switch (handle) {
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

void OverlayWindow::updateAndShowActionBar()
{
    if (!m_pActionBar || selection_.isNull()) return;
    m_pActionBar->adjustSize();
    QSize barSize = m_pActionBar->size();

    int x = selection_.center().x() - barSize.width() / 2;
    int y = selection_.bottom() + 10;

    // 边界保护
    x = qMax(10, qMin(x, width() - barSize.width() - 10));
    y = qMin(y, height() - barSize.height() - 10);

    m_pActionBar->move(x, y);
    m_pActionBar->show();
    m_pActionBar->raise();
}

void OverlayWindow::hideActionBar()
{
    if (m_pActionBar) m_pActionBar->hide();
    drawing_.setMode(DrawingLayer::ShapeType::None);
}

void OverlayWindow::onShapeModeChanged(int mode)
{
    drawing_.setModeFromInt(mode);
    update();
}

void OverlayWindow::onLineWidthChanged(int w)
{
    drawing_.setPenWidth(w);
    update();
}

QPixmap OverlayWindow::getSelectionPixmap1(bool live)
{
    if (selection_.isNull() || !selection_.isValid())
        return QPixmap();

    const QPoint overlayGlobalTopLeft = mapToGlobal(QPoint(0,0));
    QRect logicalSel = selection_;
    logicalSel.translate(overlayGlobalTopLeft - m_captureVirtualGeom.topLeft());

    const QRect physSel = logicalSel;

    if (live || recordHollow_)
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

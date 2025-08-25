#include "gifrecorder.h"
#include <QtGlobal>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QProcess>
#include "thirdparty/gif.h"

//TODO
//捕获帧序列
//调用算法生成

GifRecorder::GifRecorder(QObject* parent) : QObject(parent)
{
    m_timerCapture.setTimerType(Qt::CoarseTimer);
    connect(&m_timerCapture, &QTimer::timeout, this, &GifRecorder::onTick);
}

// 将连续的屏幕截图转换为帧序列
void GifRecorder::onTick()
{
    if (!m_frameSource)
        return;

    QImage img = m_frameSource();
    if (img.isNull())
        return;

    if (m_frames.isEmpty())
        m_frameSize = img.size();

    else if (img.size() != m_frameSize)
        img = img.scaled(m_frameSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    m_frames.push_back(img);
}

void GifRecorder::startCapture()
{
    //if (m_recording || !m_frameSource)
    //    return;
    m_frames.clear();
    m_frameSize = {};
    m_intervalMs = qMax(1, 1000 / qMax(1, m_fps));
    m_recording = true;
    m_timerCapture.start(m_intervalMs);
}

void GifRecorder::stopCapture()
{
    if (!m_recording)
        return;

    m_recording = false;
    m_timerCapture.stop();

    const QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    const QString ts = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    const QString outGif = desktop + "/LSnap_" + ts + ".gif";

    bool ok = saveGifWithGifH(outGif, m_fps, m_loop);

    m_frames.clear();
    m_frameSize = {};
}

void GifRecorder::setFrameSource(std::function<QImage()> src)
{
    m_frameSource = std::move(src);
}

bool GifRecorder::saveFramesAsPngSequence(const QString& dirPath, int fps, QString* hintPath)
{
    QDir dir;
    if (!dir.mkpath(dirPath))
        return false;

    for (int i = 0; i < m_frames.size(); ++i)
    {
        const QString fn = dirPath + QString("/frame_%1.png").arg(i, 5, 10, QLatin1Char('0'));
        m_frames[i].save(fn, "PNG");
    }
    if (hintPath) *hintPath = dirPath;
    return true;
}

bool GifRecorder::saveGifWithGifH(const QString& outGifPath, int fps, bool /*loop*/) //使用gif-h策略，后期封装为策略模式
{
    if (m_frames.isEmpty())
        return false;
    const int w = m_frames.first().width();
    const int h = m_frames.first().height();
    const int delayCs = qMax(1, int(100.0 / double(qMax(1, fps)))); // 1/100秒

    GifWriter writer{};
    if (!GifBegin(&writer, outGifPath.toLocal8Bit().constData(), w, h, delayCs)) {
        return false;
    }

    QByteArray rgba;
    rgba.resize(w * h * 4);

    for (const QImage& im0 : m_frames)
    {
        QImage im = (im0.format() == QImage::Format_RGBA8888) ? im0 : im0.convertToFormat(QImage::Format_RGBA8888);
        uint8_t* dst = reinterpret_cast<uint8_t*>(rgba.data());
        for (int y = 0; y < h; ++y) {
            const uint8_t* src = im.constScanLine(y);
            memcpy(dst + y * w * 4, src, size_t(w) * 4);
        }
        if (!GifWriteFrame(&writer, reinterpret_cast<uint8_t*>(rgba.data()), w, h, delayCs))
        {
            GifEnd(&writer);
            return false;
        }
    }
    GifEnd(&writer);
    return true;
}
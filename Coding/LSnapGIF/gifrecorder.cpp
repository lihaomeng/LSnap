#include "gifrecorder.h"
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>

GifRecorder::GifRecorder(QObject* parent) : QObject(parent)
{
    m_timerCapture.setTimerType(Qt::CoarseTimer);
    connect(&m_timerCapture, &QTimer::timeout, this, &GifRecorder::onTick);
}

GifRecorder::~GifRecorder()
{
}

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
    if (m_recording || !m_frameSource)
        return;

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

    if (!m_saveWorker)
    {
        m_saveWorker = new GifSaveWorker(this);
        connect(m_saveWorker, &GifSaveWorker::progress, this, &GifRecorder::onSaveProgress);
        connect(m_saveWorker, &GifSaveWorker::finished, this, &GifRecorder::onSaveFinished);
    }
    m_saveWorker->saveGif(m_frames, getDesktopPath(), m_fps);
}

void GifRecorder::setFrameSource(std::function<QImage()> src)
{
    m_frameSource = std::move(src);
}

void GifRecorder::onSaveProgress(int currentFrame, int totalFrames) // GIF文件创建的过程
{
    emit saveProgress(currentFrame, totalFrames);
}

void GifRecorder::onSaveFinished(bool success, const QString& path)
{
    m_frames.clear();
    m_frameSize = {};
    emit saveFinished(success, path);
}

const QString GifRecorder::getDesktopPath()
{
    const QString desktop = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    const QString ts = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    const QString outGif = desktop + "/LSnap_" + ts + ".gif";
    return outGif;
}

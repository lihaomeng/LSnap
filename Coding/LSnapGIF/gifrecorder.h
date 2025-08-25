#ifndef GIFRECORDER_H
#define GIFRECORDER_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QSize>
#include <QVector>
#include <functional>
#include "lsnapgif_export.h"

class LSNAPGIF_EXPORT GifRecorder : public QObject
{
    Q_OBJECT
public:
    explicit GifRecorder(QObject* parent = nullptr);
    void setFrameSource(std::function<QImage()> src);
    bool saveFramesAsPngSequence(const QString& dirPath, int fps, QString* hintPath);
    bool saveGifWithGifH(const QString& outGifPath, int fps, bool /*loop*/);

public slots:
    void startCapture();
    void stopCapture();

signals:

private slots:
    void onTick();

private:
    QVector<QImage> m_frames;   // 帧序列
    QSize m_frameSize;          //帧大小
    int m_intervalMs = 0;       //间隔毫秒数
    int m_fps = 5;             //帧率
    QTimer m_timerCapture;

    bool m_recording = false;
    bool m_loop = false;
    std::function<QImage()> m_frameSource;
};
#endif // GIFRECORDER_H

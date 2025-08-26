#ifndef GIFRECORDER_H
#define GIFRECORDER_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QSize>
#include <QVector>
#include <functional>
#include "gifsaveworker.h"
#include "lsnapgif_export.h"

//捕获连续帧
class LSNAPGIF_EXPORT GifRecorder : public QObject
{
    Q_OBJECT
public:
    explicit GifRecorder(QObject* parent = nullptr);
    ~GifRecorder();
    void setFrameSource(std::function<QImage()> src);

public slots:
    void startCapture();
    void stopCapture();
    void onSaveFinished(bool success, const QString& path);
    void onSaveProgress(int currentFrame, int totalFrames);

signals:
    void saveProgress(int currentFrame, int totalFrames);
    void saveFinished(bool success, const QString& path);

private slots:
    void onTick();

private:
    QVector<QImage> m_frames;   // 帧序列
    QSize m_frameSize;          //帧大小
    int m_intervalMs = 0;       //间隔毫秒数
    int m_fps = 5;             //帧率
    QTimer m_timerCapture;
    bool m_loop = false;
    std::function<QImage()> m_frameSource; //仿函数
    GifSaveWorker* m_saveWorker = nullptr;
    bool m_recording = false; //状态记录
    const QString getDesktopPath();
};
#endif // GIFRECORDER_H

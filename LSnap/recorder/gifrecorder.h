#ifndef GIFRECORDER_H
#define GIFRECORDER_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QSize>
#include <QVector>
#include <functional>


class GifRecorder : public QObject
{
    Q_OBJECT
public:
    explicit GifRecorder(QObject* parent = nullptr);
    void setFrameSource(std::function<QImage()> src);
    bool saveFramesAsPngSequence(const QString& dirPath, int fps, QString* hintPath);
    bool saveGifWithGifH(const QString& outGifPath, int fps, bool /*loop*/);

public slots:
    void start(int fps, bool loop);
    void stop();
    void cancel();

signals:
    void started();
    void stopped();
    void canceled();
    void infoUpdated(int frames, const QSize& size);
    void saved(const QString& path, bool ok);

private slots:
    void onTick();

private:
    int m_fps = 15;
    QTimer m_stopTimer;
    QTimer m_timer;
    bool m_recording = false;
    bool m_loop = false;
    int m_intervalMs = 0;
    QVector<QImage> m_frames;
    QSize m_frameSize;
    std::function<QImage()> m_frameSource;
};

////////////////////////////////////////////////////////////////


#endif // GIFRECORDER_H
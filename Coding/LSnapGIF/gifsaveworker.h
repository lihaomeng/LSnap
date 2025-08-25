#ifndef GIFSAVEWORKER_H
#define GIFSAVEWORKER_H

#include <QObject>
#include <QImage>
#include <QVector>
#include <QMutex>
#include <QWaitCondition>
#include "lsnapgif_export.h"

class LSNAPGIF_EXPORT GifSaveWorker : public QObject
{
    Q_OBJECT
public:
    explicit GifSaveWorker(QObject* parent = nullptr);
//    ~GifSaveWorker();
//    
//    void saveGif(const QVector<QImage>& frames, const QString& outPath, int fps, bool loop);
//    void cancel();
//    
//signals:
//    void progress(int currentFrame, int totalFrames);
//    void finished(bool success, const QString& path);
//    
//private:
//    bool saveGifWithGifH(const QVector<QImage>& frames, const QString& outGifPath, int fps, bool loop);
//    
//    QMutex cancelMutex_;
//    bool cancelRequested_ = false;
};

#endif // GIFSAVEWORKER_H

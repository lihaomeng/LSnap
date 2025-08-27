#ifndef LSNAPHISTORY_H
#define LSNAPHISTORY_H

#include "lsnaphistory_export.h"
#include <QObject>
#include <QString>
#include <QPixmap>
#include <QFuture>
#include <QFutureWatcher>

class LSNAPHISTORY_EXPORT LSnapHistory : public QObject
{
    Q_OBJECT
public:
    explicit LSnapHistory(QObject *parent = nullptr);
    ~LSnapHistory();

    void setSaveDirectory(const QString &directory);
    QString getSaveDirectory() const;
    QFuture<bool> saveImageAsync(const QPixmap &image, const QString &customName = QString());
    bool isSaving() const;

signals:
    void saveCompleted(bool success, const QString &filePath);

private slots:
    void onSaveCompleted();

private:
    QString generateFileName(const QString &customName = QString()) const;
    bool ensureDirectoryExists(const QString &directory) const;
    static bool saveImageToFile(const QPixmap &image, const QString &filePath);

private:
    QString m_saveDirectory;
    QFutureWatcher<bool> *m_saveWatcher;
    QString m_currentSavePath;
};

#endif // LSNAPHISTORY_H

#ifndef LSNAPGIFPANEL_H
#define LSNAPGIFPANEL_H

#include "lsnapgif_export.h"
#include <QWidget>

class GifRecorder;
QT_BEGIN_NAMESPACE
namespace Ui { class LSnapGifPanel;}
QT_END_NAMESPACE

class LSNAPGIF_EXPORT LSnapGifPanel : public QWidget
{
	Q_OBJECT
public:
    explicit LSnapGifPanel(QWidget* parent);
    void initConnect();

signals:
    void gifCaptureStart();
    void gifCaptureStop();

private:
    Ui::LSnapGifPanel* m_ui = nullptr;
    GifRecorder* m_pGifRecoder = nullptr;
};
#endif //LSNAPGIFPANEL_H
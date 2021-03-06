#ifndef EXIFDIALOG_H
#define EXIFDIALOG_H

#include <QDialog>
#include "exif.h"
#include "pagecontent.h"

namespace Ui {
class ExifDialog;
}

class ExifDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ExifDialog(QWidget *parent = 0);
    ~ExifDialog();

    void setExif(const ImageContent& content);
    void closeEvent(QCloseEvent *event) override;

signals:
    void closed();

public slots:
    void onBtnClipbard_clicked();

private:
    Ui::ExifDialog *ui;
    QString m_exif;
    QString generateFlash(char flash);
//    QString generateFlashMode(unsigned short mode);
//    QString generateFlashReturnedLight(unsigned short light);
    QString generateOrientation(unsigned short orient);
};

#endif // EXIFDIALOG_H

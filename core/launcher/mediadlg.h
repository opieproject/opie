#ifndef __MEDIA_DLG_H
#define __MEDIA_DLG_H

#include <qdialog.h>

namespace MediumMountSetting {
    class MediumMountWidget;
}
class FileSystem;
class QVBoxLayout;

class Mediadlg:public QDialog
{
    Q_OBJECT
public:
    Mediadlg(FileSystem*,QWidget *parent = 0, const char *name = 0 ,  bool modal = TRUE, WFlags fl = 0);
    virtual ~Mediadlg();
protected slots:
    virtual void accept();
    
protected:
    MediumMountSetting::MediumMountWidget*m_widget;
    FileSystem*m_fs;
    QVBoxLayout *m_lay;
    void init();
};

#endif


#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include <qvariant.h>
#include <qwidget.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QLabel;
class QTextView;

namespace Opie {
    namespace Core {
        class OConfig;
    }
    namespace Ui {
        class OKeyConfigManager;
    }
}

class imageinfo : public QWidget
{
    Q_OBJECT
    enum ActionIds {
        ViewItem
    };

public:
    imageinfo( QWidget* parent = 0, const char* name = 0,  WFlags fl =0);
    imageinfo(const QString&_path, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    virtual ~imageinfo();

    void setDestructiveClose();
    Opie::Ui::OKeyConfigManager* manager();

signals:
    void dispImage(const QString&);
    void sig_return();

public slots:
    virtual void setPath( const QString& path );
    virtual void slotShowImage();

private:
    void init(const char* name);

protected:
    QLabel* PixmapLabel1;
    QLabel* fnameLabel;
    QFrame* Line1;
    QTextView* TextView1;
    QVBoxLayout* imageinfoLayout;
    QString currentFile;

    Opie::Core::OConfig * m_cfg;
    Opie::Ui::OKeyConfigManager*m_viewManager;
    void initKeys();

protected slots:
    virtual void slot_fullInfo(const QString&, const QString&);
    virtual void slotThumbNail(const QString&, const QPixmap&);

    virtual void slotChangeName(const QString&);
};

/* for testing purpose */
class infoDlg:public QDialog
{
    Q_OBJECT
public:
    infoDlg(const QString&,QWidget * parent=0, const char * name=0);
    virtual ~infoDlg();
};

#endif // IMAGEINFO_H

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

class imageinfo : public QWidget
{
    Q_OBJECT

public:
    imageinfo( QWidget* parent = 0, const char* name = 0,  WFlags fl =0);
    imageinfo(const QString&_path, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~imageinfo();

    void setDestructiveClose();

public slots:
    void setPath( const QString& path );

private:
    void init(const char* name);

protected:
    QLabel* PixmapLabel1;
    QLabel* fnameLabel;
    QFrame* Line1;
    QTextView* TextView1;

protected:
    QVBoxLayout* imageinfoLayout;
    QString currentFile;

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

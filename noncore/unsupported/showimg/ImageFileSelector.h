#ifndef IMAGEFILE_SELECTOR_H
#define IMAGEFILE_SELECTOR_H


#include <qtoolbutton.h>
#include <qlistview.h>
#include <qwidgetstack.h>
#include <qlayout.h>

#include "qpe/filemanager.h"
#include "qpe/applnk.h"

class QScrollView;
class QLabel;
//class QValueList;

class ThumbWidget : public QWidget
{
    Q_OBJECT
public:
    ThumbWidget(QPixmap p,QString text,const DocLnk& f,QWidget *parent=0,int width=-1);
    ~ThumbWidget() { }

    DocLnk file() const {
        return fl;
    } 
    

    signals:
    void clicked(const DocLnk &);

protected:
    void paintEvent( QPaintEvent * );
    void resizeEvent(QResizeEvent *);
    
    void mouseReleaseEvent(QMouseEvent* event);

private:
    QPixmap pixmap;
    QLabel  *description;
    DocLnk fl;    
};




class ImageFileSelectorItem : public QListViewItem
{
public:
    ImageFileSelectorItem( QListView *parent, const DocLnk& f );
    ~ImageFileSelectorItem();

    DocLnk file() const {
        return fl;
    } 
private:
    DocLnk fl;
};

class ImageFileSelector : public QWidgetStack
{
    Q_OBJECT

public:

    enum CURRENT_VIEW {
        THUMBNAIL,
        DETAILED,
        UNKNOWN
    };


    ImageFileSelector(CURRENT_VIEW scv=DETAILED, QWidget *parent=0, const char *name=0 );
    ~ImageFileSelector();

    void reread(bool purgeCache=false);
    int fileCount();
    const DocLnk *selected();

    void setView(CURRENT_VIEW v);

    CURRENT_VIEW view()  {
        return cView;
    }

    QValueList<DocLnk> fileList() const  {
        ((ImageFileSelector*)this)->fileCount(); // ensure all loaded when this is extended
        QValueList<DocLnk> list;
        ImageFileSelectorItem *item = (ImageFileSelectorItem *)detailed->firstChild();
        while (item) {
            list.append(item->file());
            item = (ImageFileSelectorItem *)item->nextSibling();
        }
        return list;
    }

public slots:

    void switchView();

signals:
    void fileSelected( const DocLnk & );
    void closeMe();

protected:

    void resizeEvent(QResizeEvent *);


private slots:
    void fileClicked( int, QListViewItem *, const QPoint &, int );
    // pressed to get 'right down'
    void filePressed( int, QListViewItem *, const QPoint &, int );
    void fileClicked( QListViewItem *);
    void thumbClicked(const DocLnk &);

private:

    void updateSizes();


    CURRENT_VIEW    cView;  
    int     count;

    QListView   *detailed;
    QScrollView *thumb;
    QList<ThumbWidget> tList;
    QWidget *background;
    QGridLayout *gl;

};
#endif // IMAGEFILE_SELECTOR_H



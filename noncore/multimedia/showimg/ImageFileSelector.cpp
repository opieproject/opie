

#include "qpe/global.h"
#include "qpe/applnk.h"
#include "qpe/lnkproperties.h"
#include "qpe/applnk.h"
#include "qpe/qpeapplication.h"

#include <stdlib.h>

#include <qdir.h>
#include <qwidget.h>
#include <qheader.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <qscrollview.h>

#include "ImageFileSelector.h"





ThumbWidget::ThumbWidget(QPixmap p,QString text,const DocLnk& f,QWidget *parent,int w) : QWidget( parent ),fl(f)
{
    setBackgroundMode(NoBackground);  
    if ( w!=-1 )
        setMinimumSize(w,p.height()+24);
    else
        setMinimumSize(p.width(),p.height()+24);
    description=new QLabel(text,this);
    description->setBackgroundColor(colorGroup().base());
    description->setAlignment(AlignCenter);
    description->setGeometry(0,height()-24,width(),24);
    pixmap=p;
}

void ThumbWidget::resizeEvent(QResizeEvent *e)
{
    description->setGeometry(0,height()-24,width(),24);
}

void ThumbWidget::paintEvent( QPaintEvent *e )
{
    QPainter painter(this);

    painter.setClipRect(e->rect());
    painter.fillRect(0,0,width(),height(),QColor(255,255,255));
    painter.drawPixmap((width() - pixmap.width()) / 2,0, pixmap);

}

void ThumbWidget::mouseReleaseEvent(QMouseEvent* event)
{
    emit clicked(fl);
}




ImageFileSelectorItem::ImageFileSelectorItem( QListView *parent, const DocLnk &f): QListViewItem( parent ), fl( f )
{
    setText( 0, f.name() );
    QFileInfo fi(f.file());
    setText( 1, (fi.extension()).upper() );
    setPixmap( 0, f.pixmap() );


}


ImageFileSelectorItem::~ImageFileSelectorItem()
{
    
}


ImageFileSelector::ImageFileSelector( CURRENT_VIEW scv,QWidget *parent,const char *name ):QWidgetStack(parent)
{

    detailed=new QListView(this);

    detailed->addColumn (tr("Title")); 
    detailed->addColumn (tr("Type")); 
    detailed->setAllColumnsShowFocus( true );

    tList.setAutoDelete(true);

    thumb =new QScrollView(this);
    thumb->setVScrollBarMode (QScrollView::Auto );
    thumb->viewport()->setBackgroundColor(colorGroup().base());

    background=new QWidget(0);
    background->setBackgroundColor(colorGroup().base());
    thumb->addChild(background);
    gl = new QGridLayout(background,1,2,4,4);



    connect( detailed, SIGNAL( mouseButtonClicked( int, QListViewItem *, const QPoint &, int ) ),
             this, SLOT( fileClicked( int, QListViewItem *, const QPoint &, int ) ) );
    connect( detailed, SIGNAL( mouseButtonPressed( int, QListViewItem *, const QPoint &, int ) ),
             this, SLOT( filePressed( int, QListViewItem *, const QPoint &, int ) ) );
    connect( detailed, SIGNAL( returnPressed( QListViewItem * ) ),
             this, SLOT( fileClicked( QListViewItem * ) ) );

    cView=UNKNOWN;
    setView(scv);
    reread();    

}
ImageFileSelector::~ImageFileSelector()
{
    
}


void ImageFileSelector::switchView()
{
    CURRENT_VIEW v=cView;

    if ( v==DETAILED )
        v=THUMBNAIL;
    else
        v=DETAILED;
    setView(v);
}

void ImageFileSelector::setView(CURRENT_VIEW v)
{

    if ( v==cView )
        return;
    cView=v;

    if ( cView!=DETAILED )
    {
        raiseWidget(thumb);            
        updateSizes();
    }
    else
    {
        raiseWidget(detailed);              
        updateSizes();
    }


}

void ImageFileSelector::resizeEvent(QResizeEvent *)
{
    updateSizes();
}

void ImageFileSelector::updateSizes()
{
    int ww=(detailed->width()-detailed->frameWidth()*2);
    double w=(double)ww*0.70;
    detailed->setColumnWidth(0,(int)w);
    detailed->setColumnWidth(1,ww-(int)w);
    background->setMinimumWidth(thumb->visibleWidth());
    thumb->updateScrollBars(); 
}

void ImageFileSelector::reread(bool purgeCache)
{
    ImageFileSelectorItem *item = (ImageFileSelectorItem *)detailed->selectedItem();
    QString oldFile;
    if ( item )
        oldFile = item->file().file();
    detailed->clear();
    tList.clear();
    DocLnkSet files;
    Global::findDocuments(&files, "image/*");
    count = files.children().count();
    QListIterator<DocLnk> dit( files.children() );
    int y=0;
    int x=4;
    int totalHeight=4;
    ThumbWidget *l=0;
    int width=80;
    gl->expand(dit.count()/2,2);

    int i,j;

    i=j=0;

    detailed->setUpdatesEnabled(false);
    thumb->setUpdatesEnabled(false);
    for ( ; dit.current(); ++dit )
    {
        item = new ImageFileSelectorItem( detailed, **dit );        
        if ( item->file().file() == oldFile )
            detailed->setCurrentItem( item );
    }

    QListViewItemIterator it( detailed );
    ImageFileSelectorItem *ii;
    // iterate through all items of the listview
    for ( ; it.current(); ++it )
    {
        ii=(ImageFileSelectorItem *)it.current();
        QImage img(ii->file().file());
        img=img.smoothScale(64,64);
        QPixmap pix;
        pix.convertFromImage(img);                
        l=new ThumbWidget(pix,ii->file().name(),ii->file(),background,width);
        l->setBackgroundColor(colorGroup().base());        
        gl->addWidget(l,j,i);
        i++;
        if ( i==2 )
        {
            i=0;
            j++;
        }
        tList.append(l);
        connect(l,SIGNAL(clicked(const DocLnk &)),this,SLOT(thumbClicked(const DocLnk &)));
        
    }



    if ( !detailed->selectedItem() )
        detailed->setCurrentItem( detailed->firstChild() );

    detailed->setUpdatesEnabled(true);
    thumb->setUpdatesEnabled(true);
    detailed->update();
    thumb->update();

}

int ImageFileSelector::fileCount()
{
    return count;
}
const DocLnk * ImageFileSelector::selected()
{
    ImageFileSelectorItem *item = (ImageFileSelectorItem *) detailed->selectedItem();
    if ( item )
        return new DocLnk( item->file() );
    return 0;
}



void ImageFileSelector::fileClicked( int button, QListViewItem *i, const QPoint &, int )
{
    if ( !i )
        return;
    if ( button == Qt::LeftButton )
    {
        fileClicked( i );
    }
}
    // pressed to get 'right down'
void ImageFileSelector::filePressed( int, QListViewItem *, const QPoint &, int )
{
    
}
void ImageFileSelector::fileClicked( QListViewItem *i)
{
    if ( !i )
        return;
    emit fileSelected( ( (ImageFileSelectorItem*)i )->file() );
    emit closeMe();     
}

void ImageFileSelector::thumbClicked(const DocLnk &f)
{
    emit fileSelected( f );
    emit closeMe();     
}


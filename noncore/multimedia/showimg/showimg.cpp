/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

//
// Full-screen and rotation options contributed by Robert Wittams <robert@wittams.com>
//

#include "showimg.h"
#include "ImageFileSelector.h"
#include "settingsdialog.h"

           
#include <opie/ofiledialog.h>

#include <qpe/qpeapplication.h>
#include <qpe/config.h>
#include <qpe/resource.h>
#include <qpe/fileselector.h>
#include <qpe/applnk.h>
#include <qfileinfo.h>
#include <math.h>
#include <qpe/qpemenubar.h>
#include <qwidgetstack.h>
#include <qpe/qpetoolbar.h>
#include <qaction.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qscrollview.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qkeycode.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qtimer.h>
#include <qspinbox.h>



ControlsDialog::ControlsDialog(const QString &caption,QImage image,int *brightness,QWidget *parent)
        : QDialog(parent,0,true)
{
    setCaption(caption);

    if ( parent )  {
        setPalette(parent->palette());
    }

    b=brightness;
    img=image;

    setMinimumSize(140,80);

    QGridLayout  *gl= new QGridLayout(this,2,2,4,4);

    pixmap =new ImageWidget(this);;
    QPixmap pm;
    pm.convertFromImage(img);
    pixmap->setPixmap(pm);
    pixmap->setMinimumSize(pm.width(),pm.height());
    gl->addMultiCellWidget(pixmap,0,0,0,2,AlignCenter); 
    QLabel *l=new QLabel(tr("Brightness")+":",this);
    gl->addWidget(l,1,0,AlignLeft);       
    spb=new QSpinBox(-100,100,2,this); 
    gl->addWidget(spb,1,1,AlignRight); 

    spb->setValue(0);

    connect(spb,SIGNAL(valueChanged(int)),this, SLOT(bValueChanged(int)));

}

void ControlsDialog::bValueChanged(int value)
{
    QImage nImage=img;
    nImage.detach();
    ImageViewer::intensity(nImage, (float)value/100);
    QPixmap pm;
    pm.convertFromImage(nImage);
    pixmap->setPixmap(pm);
    pixmap->repaint(false);


}

void ControlsDialog::accept()
{
    *b=spb->value();    
    done(1);
}

//===========================================================================

InfoDialog::InfoDialog(const QString &caption, const QStringList text,QWidget *parent)
        : QDialog(parent,0,true)
{
    setCaption(caption);

    if ( parent )
    {
        setPalette(parent->palette());
    }

    const char *labels[]={"File Name","Format","File Size","Size","Colors","Alpha"};

    setMinimumSize(180,80);
    int num=ImageViewer::LAST+1;
    if ( text[ImageViewer::ALPHA].isEmpty() )
        num--;
    QGridLayout  *gl= new QGridLayout(this,num,2,4,2);
    QLabel *l;
    int count=0;
    for ( int i=0;i<num;i++ )
    {
        if ( i==1 )
        {
            QFrame *frm=new QFrame(this);
            frm->setFrameStyle(QFrame::HLine|QFrame::Sunken);
            gl->addMultiCellWidget(frm,i,i,0,1);            
        }
        else
        {
            l=new QLabel(tr(labels[count])+":",this);
            gl->addWidget(l,i,0,AlignLeft);        
            l=new QLabel(text[count],this);
            gl->addWidget(l,i,1,AlignRight);
            count++;
        }

    } 

}

void InfoDialog::displayInfo(const QString &caption, const QStringList text, QWidget *parent)
{
    InfoDialog *dlg=new InfoDialog(caption,text,parent);
    dlg->exec();
    delete dlg;
} 

//===========================================================================


ImagePane::ImagePane( QWidget *parent ) : QWidget( parent )
{
    vb = new QVBoxLayout( this );

    image = new QScrollView(this,0,WResizeNoErase|WNorthWestGravity);
    pic=new ImageWidget(image);
    image->addChild(pic);

    connect(pic, SIGNAL( clicked() ), this, SLOT( imageClicked() ));

    vb->addWidget( image );

}

void ImagePane::setPixmap( const QPixmap &pm )
{
    pic->setPixmap( pm );
    pic->resize(pm.width(),pm.height());
    image->updateScrollBars ();
    pic->repaint(false);
}

void ImagePane::imageClicked()
{
    emit clicked();
}
//===========================================================================
/*
  Draws the portion of the scaled pixmap that needs to be updated
*/

void ImageWidget::paintEvent( QPaintEvent *e )
{
    QPainter painter(this);

    painter.setClipRect(e->rect());
    painter.fillRect(0,0,width(),height(),QColor(0,0,0));

    if ( pixmap.size() != QSize( 0, 0 ) )
    { // is an image loaded?
        painter.drawPixmap((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2, pixmap);
    }
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *)
{
    emit clicked();
}

//===========================================================================

ImageViewer::ImageViewer( QWidget *parent, const char *name, int wFlags )
: QMainWindow( parent, name, wFlags ), filename( 0 ),  bFromDocView( FALSE )
{
    setCaption( tr("Image Viewer") );
    setIcon( Resource::loadPixmap( "ImageViewer" ) );


    Config cfg("Image Viewer");
    cfg.setGroup("Image Viewer");

    showThumbView=cfg.readBoolEntry("ShowThumbnails",false);
    isSized=cfg.readBoolEntry("SizeToScreen",true);

    isFullScreen = FALSE;

    setToolBarsMovable( FALSE );

    toolBar = new QPEToolBar( this );
    toolBar->setHorizontalStretchable( TRUE );

    menuBar = new QPEMenuBar( toolBar );

    current=menuBar;



    fileMenuFile = new QPopupMenu(this);
    //menuBarmenubarFile->insertItem( tr("File"), fileMenu );
    fileMenuFile->insertItem(tr("Open"),
                            this, SLOT(openFile()), 0);

    viewMenuFile = new QPopupMenu( this );
    //menubarFile->insertItem( tr("View"), viewMenu );    
    viewMenuFile->insertItem( tr("Thumbnail View"),
                              this, SLOT(switchThumbView()), 0, SHOW_THUMBNAILS );
    
    viewMenuFile->setItemChecked ( SHOW_THUMBNAILS, showThumbView ); 




    optionsMenuFile = new QPopupMenu( this);
    //menubarFile->insertItem( tr("Options"),optionsMenu );
    slideAction = new QAction( tr( "Slide show" ), Resource::loadIconSet( "slideshow" ),
                               QString::null, 0, this, 0 );
    slideAction->setToggleAction( TRUE );
    connect( slideAction, SIGNAL( toggled(bool) ), this, SLOT( slideShow(bool) ) );
    slideAction->addTo( optionsMenuFile);
//     slideAction->addTo( toolBar );


//     optionsMenuFile->insertItem( tr("Slideshow") );
    optionsMenuFile->insertSeparator();
    optionsMenuFile->insertItem( tr("Preferences.."), this, SLOT(settings()), 0);
//    optionsMenuFile->insertItem( tr("Help"), this, SLOT(help()), 0);

    QStrList fmt = QImage::outputFormats();


    fileMenuView = new QPopupMenu( this );
    //menubarView->insertItem( tr("File"),fileMenu );    
    fileMenuView->insertItem( tr("Image Info ..."),this, SLOT(displayInfoDialog()),0 );
    fileMenuView->insertSeparator();    

    viewMenuView = new QPopupMenu(this );
    viewMenuView->setCheckable ( true ); 

    //menubarView->insertItem( tr("View"),viewMenu ); 
    viewMenuView->insertItem(tr("Horizontal flip"), this, SLOT(hFlip()), 0);
    viewMenuView->insertItem(tr("Vertical flip"), this, SLOT(vFlip()), 0);

    stack = new QWidgetStack( this );
    stack->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    setCentralWidget( stack );


    imagePanel = new ImagePane( stack );    
    connect(imagePanel, SIGNAL(clicked()), this, SLOT(normalView()));


    ImageFileSelector::CURRENT_VIEW cv;
    if(showThumbView)
        cv=ImageFileSelector::THUMBNAIL;
    else
        cv=ImageFileSelector::DETAILED;

    qDebug("cv = %d",cv);

    fileSelector = new ImageFileSelector( cv,stack, "fs");    
   
    //switchThumbView();
    

    //fileSelector = new ImageFileSelector("image/*", stack, "fs");
    //fileSelector->setNewVisible(FALSE);
    //fileSelector->setCloseVisible(FALSE);
    connect( fileSelector, SIGNAL( closeMe() ), this, SLOT( closeFileSelector() ) );
    connect( fileSelector, SIGNAL( fileSelected( const DocLnk &) ),
             this, SLOT( openFile( const DocLnk & ) ) );

    imageList = fileSelector->fileList();
    slideAction->setEnabled( imageList.count() != 0);

    iconToolBar = new QPEToolBar(this);

    QAction *a;

    a = new QAction( tr( "Open ..." ), Resource::loadPixmap( "fileopen" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( open() ) );
    a->addTo( fileMenuView);
    a->addTo( iconToolBar );    


    a = new QAction( tr( "Rotate 90"), Resource::loadPixmap( "rotate90" ), QString::null, 0, this, 0);
    connect( a, SIGNAL( activated() ), this, SLOT( rot90() ) );
    a->addTo( iconToolBar );
    a->addTo( viewMenuView );

    a = new QAction( tr( "Rotate 180" ), Resource::loadPixmap( "repeat" ), QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( rot180() ) );
    a->addTo( iconToolBar );
    a->addTo( viewMenuView );


    a = new QAction( tr( "Rotate 270"), Resource::loadPixmap( "rotate270" ), QString::null, 0, this, 0);
    connect( a, SIGNAL( activated() ), this, SLOT( rot270() ) );
    //a->addTo( iconToolBar );
    a->addTo( viewMenuView );



    viewMenuView->insertSeparator();
    viewMenuView->insertItem(tr("Brightness ..."), this, SLOT(displayControlsDialog()), 0);
    viewMenuView->insertItem(tr("Black And White"), this, SLOT(blackAndWhite()), 0,BLACKANDWHITE);
    viewMenuView->insertSeparator();


    sss = new QAction( tr( "Scale to Screen"), Resource::loadPixmap( "scale" ), QString::null, 0, this, 0,true);
    connect( sss, SIGNAL( activated() ), this, SLOT( switchSizeToScreen() ) );
    sss->addTo( iconToolBar );
    sss->addTo( viewMenuView );

    sss->setOn(isSized);    
    viewMenuView->insertSeparator();


    a = new QAction( tr( "Fullscreen" ), Resource::loadPixmap( "fullscreen" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( fullScreen() ) );
    a->addTo( iconToolBar );
    a->addTo( viewMenuView);

    a = new QAction( tr( "Stop Slideshow" ), Resource::loadPixmap( "quit_icon" ),
                     QString::null, 0, this, 0 );
    connect( a, SIGNAL( activated() ), this, SLOT( stopSlideShow() ) );
    a->addTo( iconToolBar );
    a->addTo( viewMenuView);


    Config config( "ImageViewer" );
    config.setGroup( "SlideShow" );
    slideDelay = config.readNumEntry( "Delay", 2);
    slideRepeat = config.readBoolEntry( "Repeat", FALSE );
    slideReverse = config.readBoolEntry("Reverse", FALSE);

    config.setGroup("Default");
    rotateOnLoad = config.readBoolEntry("Rotate", FALSE);
    fastLoad = config.readBoolEntry("FastLoad", TRUE);
    slideTimer = new QTimer( this );
    connect( slideTimer, SIGNAL(timeout()), this, SLOT(slideUpdate()) );

    switchToFileSelector();

    setMouseTracking( TRUE ); 

    
}

ImageViewer::~ImageViewer()
{
    Config cfg("Image Viewer");
    cfg.setGroup("Image Viewer");
    
    cfg.writeEntry("ShowThumbnails",(int)showThumbView);
    cfg.writeEntry("SizeToScreen",(int)isSized);

    cfg.setGroup( "SlideShow" );
    cfg.writeEntry( "Delay", slideDelay);
    cfg.writeEntry( "Repeat", slideRepeat );
    cfg.writeEntry("Reverse",  slideReverse);

    cfg.setGroup("Default");
    cfg.writeEntry("Rotate", rotateOnLoad);
    cfg.writeEntry("FastLoad", fastLoad);

    delete imagePanel; // in case it is fullscreen
}

void ImageViewer::help() {

}


void ImageViewer::settings()
{
    SettingsDialog dlg( this, 0, TRUE );
    dlg.setDelay( slideDelay );
    dlg.setRepeat( slideRepeat );
    dlg.setReverse( slideReverse );
    dlg.setRotate(rotateOnLoad);
    dlg.setFastLoad(fastLoad);

    if ( QPEApplication::execDialog(&dlg) == QDialog::Accepted ) {
        qDebug("<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>");
        slideDelay = dlg.delay();
        slideRepeat = dlg.repeat();
        slideReverse = dlg.reverse();
        rotateOnLoad = dlg.rotate();
        fastLoad = dlg.fastLoad();

        Config config( "ImageViewer" );
        config.setGroup( "SlideShow" );
        config.writeEntry( "Delay", slideDelay );
        config.writeEntry( "Repeat", slideRepeat );
        config.writeEntry("Reverse", slideReverse);

        config.setGroup("Default");
        config.writeEntry("Rotate", rotateOnLoad);
        config.writeEntry("FastLoad", fastLoad);
    }
}

void ImageViewer::switchSizeToScreen()
{
    isSized=!isSized;
    sss->setOn(isSized);
    updateImage();
}

void ImageViewer::updateImage()
{
    if ( isSized ) {
        imagePanel->setPixmap(pmScaled);
    } else {
        imagePanel->setPixmap(pm);
    }
}

void ImageViewer::switchThumbView()
{

    showThumbView=!showThumbView;
    viewMenuFile->setItemChecked ( SHOW_THUMBNAILS, showThumbView );
    fileSelector->switchView();
    
}

void ImageViewer::switchToFileSelector()
{
    stack->raiseWidget(fileSelector);
    menuBar->clear();
    menuBar->insertItem( tr("File"), fileMenuFile );
    menuBar->insertItem( tr("View"), viewMenuFile );
    menuBar->insertItem( tr("Options"), optionsMenuFile );
    iconToolBar->hide();
    imagePanel->disable();
    slideShow(false);

}

void ImageViewer::switchToImageView()
{
    stack->raiseWidget(imagePanel);

    menuBar->clear();
    menuBar->insertItem( tr("File"), fileMenuView );
    menuBar->insertItem( tr("View"), viewMenuView );
    viewMenuView->setItemEnabled(BLACKANDWHITE,true);
    iconToolBar->show();

    imagePanel->setPosition(0,0);

}


void ImageViewer::setDocument(const QString& fileref)
{
    delayLoad = fileref;
    switchToImageView();    
    QTimer::singleShot( 0, this, SLOT(doDelayedLoad()) );
}

void ImageViewer::doDelayedLoad()
{
    show(delayLoad);
}

void ImageViewer::show()
{
    normalView();
    QMainWindow::show();
}

void ImageViewer::show(const QString& fileref)
{
//    qDebug("Show "+fileref);
    bFromDocView = TRUE;
    closeFileSelector();
    DocLnk link(fileref);
    if ( link.isValid() ) {
        openFile(link);
    } else {
        filename = fileref;
        updateCaption( fileref );
        loadImage( fileref );
    }
}

void ImageViewer::openFile() {
    MimeTypes types;
    QStringList image;
    image << "image/*";
    types.insert("Images",  image);
    
    QString str = OFileDialog::getOpenFileName( 1,QPEApplication::documentDir(),"", types, 0 );
    DocLnk link(str);
//    if ( link.isValid() )
        openFile(link);

}

void ImageViewer::openFile( const DocLnk &link )
{
    closeFileSelector();
//    DocLnk link(file);
    qDebug("open "+link.name());
    updateCaption( link.name() );
    loadImage( link.file() );
    if (slideTimer->isActive()) {
        slideTimer->start(slideDelay * 1000, FALSE);
    }
    
}

void ImageViewer::open()
{
    switchToFileSelector();
}

void ImageViewer::closeFileSelector()
{
    switchToImageView();    
}

void ImageViewer::updateCaption( QString name )
{
    int sep = name.findRev( '/' );
    if ( sep >= 0 )
        name = name.mid( sep+1 );
    setCaption( name + tr(" - Image Viewer") );
}

/*
  This function loads an image from a file.
*/

void ImageViewer::loadImage( const char *fileName )
{
    filename = fileName;
    if ( filename )  {
        QApplication::setOverrideCursor( waitCursor ); // this might take time
        //imagePanel->statusLabel()->setText( tr("Loading image...") );
        qApp->processEvents();
        bool ok = image.load(filename, 0); 
        if ( ok ) {
            ok = reconvertImage();
            updateImageInfo(filename);
        }
        if ( !ok ) {
            pm.resize(0,0);             // couldn't load image
            update();
        }
        QApplication::restoreOverrideCursor();  // restore original cursor
    }

//    fastLoad ? ", Fast" : "",
//    fastLoad ? QMAX(imagewidth/maxsize, imageheight/maxsize) : 1);

  
//    matrix.reset();
    rotated90 = FALSE;

    if (rotateOnLoad) {
        rotated90 = TRUE;
        rot90();      
//        matrix.rotate( -90.0 );
    }
    
    switchToImageView();
    updateImage();

}

bool ImageViewer::loadSelected()
{
    bool ok = false;
    if ( stack->visibleWidget() == fileSelector )
    {
        const DocLnk *link = fileSelector->selected();
        if ( link )
        {
            if ( link->file() != filename )
            {
                updateCaption( link->name() );
                filename = link->file();
                qApp->processEvents();
                ok = image.load(filename, 0);                    
                if ( ok )
                {
                    updateImageInfo(filename);                    
                    ok = reconvertImage();
                }
                if ( !ok )
                    pm.resize(0,0);
            }
        }
    }
    if ( !image.isNull() )
    {
        ok = true;
        closeFileSelector();
    }

    return ok;
}

bool ImageViewer::reconvertImage()
{
    bool success = FALSE;

    if ( image.isNull() ) return FALSE;

    QApplication::setOverrideCursor( waitCursor ); // this might take time
    if ( pm.convertFromImage(image /*, conversion_flags */ ) )
    {
        pmScaled = QPixmap();
        scale();
        success = TRUE;             // load successful
    }
    else
    {
        pm.resize(0,0);             // couldn't load image
    }
    QApplication::restoreOverrideCursor();  // restore original cursor

    return success;             // TRUE if loaded OK
}


int ImageViewer::calcHeight()
{
    if ( !isFullScreen )
        return  imagePanel->paneHeight();
    else
        return qApp->desktop()->height();
}
/*
  This functions scales the pixmap in the member variable "pm" to fit the
  widget size and  puts the resulting pixmap in the member variable "pmScaled".
*/
void ImageViewer::scale()
{
    int h = calcHeight();
    if ( image.isNull() ) return;

    QApplication::setOverrideCursor( waitCursor ); // this might take time
    if ( imagePanel->paneWidth() == pm.width() && h == pm.height() )
    { // no need to scale if widget
        pmScaled = pm;              // size equals pixmap size
    }
    else
    {
        double hs = (double)h / (double)image.height();
        double ws = (double)imagePanel->paneWidth() / (double)image.width();
        double scaleFactor = (hs > ws) ? ws : hs;
        int smoothW = (int)(scaleFactor * image.width());
        int smoothH = (int)(scaleFactor * image.height());

        pmScaled.convertFromImage( image.smoothScale( smoothW, smoothH ) /*, conversion_flags */ );
    }
    QApplication::restoreOverrideCursor();  // restore original cursor
}

/*
  The resize event handler, if a valid pixmap was loaded it will call
  scale() to fit the pixmap to the new widget size.
*/

void ImageViewer::resizeEvent( QResizeEvent * )
{

    if ( pm.size() == QSize( 0, 0 ) )       // we couldn't load the image
        return;

    int h = calcHeight();

    if ( imagePanel->paneWidth() != pmScaled.width() || h != pmScaled.height() )
    {                       // if new size,
        scale();                // scale pmScaled to window
    }
    if ( image.hasAlphaBuffer() )
        erase();
}


void ImageViewer::hFlip()
{
//    matrix.scale( -1.0, 1.0 );
    
    setImage(image.mirror(TRUE,FALSE));
}

void ImageViewer::vFlip()
{
//    matrix.scale( 1.0, -1.0 );
    setImage(image.mirror(FALSE,TRUE));
}

void ImageViewer::rot180()
{
//    matrix.rotate( 180.0 );
    setImage(image.mirror(TRUE,TRUE));
}

void ImageViewer::rot90()
{
    QImage oldimage;
    oldimage = image.convertDepth(32);
//    matrix.rotate( -90.0 );
    setImage(rotate(oldimage,Rotate90));

}
void ImageViewer::rot270()
{

    QImage oldimage;
    oldimage = image.convertDepth(32);
//    matrix.rotate(90.0);
    setImage(rotate(oldimage,Rotate270));

}

void ImageViewer::blackAndWhite()
{

    viewMenuView->setItemEnabled(BLACKANDWHITE,false);
    setImage(toGray(image,false));
}

void ImageViewer::displayControlsDialog()
{
    int w=80;
    int h=w;
    QImage small;

    if ( image.width()<w ||image.height()<h )
        small=image.smoothScale(w,h);
    else
        small=image.copy(0,0,w,h);

    int newB=0;
    ControlsDialog *dlg=new ControlsDialog("Image Viewer",small,&newB,this);
    dlg->exec();
    if ( newB ) {
        intensity(image,(float)newB/100);
        setImage(image);
    }

}


void ImageViewer::displayInfoDialog()
{

    QStringList ls;

    for ( int i=0;i<LAST;i++ )
        ls.append(imageInfo[i]);

    InfoDialog::displayInfo("Image Viewer",ls,this);
}
void ImageViewer::normalView()
{
    if ( !imagePanel->parentWidget() )
    {

        isFullScreen = FALSE;
        stack->addWidget( imagePanel, 1 );
        switchToImageView();                
        if ( isSized )
            scale();

        updateImage();

    }
}

void ImageViewer::stopSlideShow() {
    if (slideTimer->isActive())
    slideTimer->stop();
}

void ImageViewer::fullScreen()
{
    // Full-screen option
    // contributed by Robert Wittams <robert@wittams.com>
    if ( imagePanel->parentWidget() && loadSelected() )
    {
        isFullScreen = TRUE;
        imagePanel->reparent(0,QPoint(0,0));
        imagePanel->resize(qApp->desktop()->width(), qApp->desktop()->height());

        if ( isSized )
            scale();
        updateImage();
        imagePanel->showFullScreen();
    }
}

void ImageViewer::setImage(const QImage& newimage)
{
    image = newimage;
    reconvertImage();
    updateImage();    
}

void ImageViewer::updateImageInfo(QString &filePath)
{

    for ( int i=0;i<LAST;i++ )
    {
        imageInfo[i]="";
    }

    imageInfo[FORMAT]=QImage::imageFormat (filePath );
    QFileInfo fi(filePath);
    imageInfo[PATH]=fi.fileName(); 
    imageInfo[FILE_SIZE]=QString::number(fi.size())+" (bytes)";
    QString message("%1x%2");
    imageInfo[SIZE]=QString("%1x%2");
    imageInfo[SIZE]=imageInfo[SIZE].arg(image.width()).arg(image.height());
    if ( image.numColors() > 0 )
    {
        imageInfo[COLORS]=tr("%1 colors").arg(image.numColors());
    }
    else if ( image.depth() >= 16 )
    {
        imageInfo[COLORS]=tr(" True color");
    }
    if ( image.hasAlphaBuffer() )
    {
        if ( image.depth() == 8 )
        {
            int i;
            bool alpha[256];
            int nalpha=0;

            for ( i=0; i<256; i++ )
                alpha[i] = FALSE;

            for ( i=0; i<image.numColors(); i++ )
            {
                int alevel = image.color(i) >> 24;
                if ( !alpha[alevel] )
                {
                    alpha[alevel] = TRUE;
                    nalpha++;
                }
            }
            imageInfo[ALPHA]=tr("%1 alpha levels").arg(nalpha);
        }
        else
        {
            imageInfo[ALPHA]=tr("8-bit alpha channel");
        }
    }

}

void ImageViewer::closeEvent( QCloseEvent *e )
{
    if ( stack->visibleWidget() == imagePanel && !bFromDocView )
    {
        e->ignore();
        open();
    }
    else
    {
        bFromDocView = FALSE;
        e->accept();
    }
}

// Intensity,toGray and rotate code courtesy of KDE project.


QImage& ImageViewer::intensity(QImage &image, float percent)
{

    int segColors = image.depth() > 8 ? 256 : image.numColors();
    unsigned char *segTbl = new unsigned char[segColors];
    int pixels = image.depth() > 8 ? image.width()*image.height() :
                 image.numColors();
    unsigned int *data = image.depth() > 8 ? (unsigned int *)image.bits() :
                         (unsigned int *)image.colorTable();

    bool brighten = (percent >= 0);
    if ( percent < 0 )
        percent = -percent;

    if ( brighten )
    { // keep overflow check out of loops
        for ( int i=0; i < segColors; ++i )
        {
            int tmp = (int)(i*percent);
            if ( tmp > 255 )
                tmp = 255;
            segTbl[i] = tmp;
        }
    }
    else
    {
        for ( int i=0; i < segColors; ++i )
        {
            int tmp = (int)(i*percent);
            if ( tmp < 0 )
                tmp = 0;
            segTbl[i] = tmp;
        }
    }

    if ( brighten )
    { // same here
        for ( int i=0; i < pixels; ++i )
        {
            int r = qRed(data[i]);
            int g = qGreen(data[i]);
            int b = qBlue(data[i]);
            int a = qAlpha(data[i]);
            r = r + segTbl[r] > 255 ? 255 : r + segTbl[r];
            g = g + segTbl[g] > 255 ? 255 : g + segTbl[g];
            b = b + segTbl[b] > 255 ? 255 : b + segTbl[b];
            data[i] = qRgba(r, g, b,a);
        }
    }
    else
    {
        for ( int i=0; i < pixels; ++i )
        {
            int r = qRed(data[i]);
            int g = qGreen(data[i]);
            int b = qBlue(data[i]);
            int a = qAlpha(data[i]);
            r = r - segTbl[r] < 0 ? 0 : r - segTbl[r];
            g = g - segTbl[g] < 0 ? 0 : g - segTbl[g];
            b = b - segTbl[b] < 0 ? 0 : b - segTbl[b];
            data[i] = qRgba(r, g, b, a);
        }
    }
    delete [] segTbl;

    return image;
}

QImage& ImageViewer::toGray(QImage &img, bool fast)
{
    if ( img.width() == 0 || img.height() == 0 )
        return img;

    if ( fast )
    {
        if ( img.depth() == 32 )
        {
            register uchar * r(img.bits());
            register uchar * g(img.bits() + 1);
            register uchar * b(img.bits() + 2);

            uchar * end(img.bits() + img.numBytes());

            while ( r != end )
            {

                *r = *g = *b = (((*r + *g) >> 1) + *b) >> 1; // (r + b + g) / 3

                r += 4;
                g += 4;
                b += 4;
            }
        }
        else
        {
            for ( int i = 0; i < img.numColors(); i++ )
            {
                register uint r = qRed(img.color(i));
                register uint g = qGreen(img.color(i));
                register uint b = qBlue(img.color(i));

                register uint gray = (((r + g) >> 1) + b) >> 1;
                img.setColor(i, qRgba(gray, gray, gray, qAlpha(img.color(i))));
            }
        }
    }
    else
    {
        int pixels = img.depth() > 8 ? img.width()*img.height() :
                     img.numColors();
        unsigned int *data = img.depth() > 8 ? (unsigned int *)img.bits() :
                             (unsigned int *)img.colorTable();
        int val, i;
        for ( i=0; i < pixels; ++i )
        {
            val = qGray(data[i]);
            data[i] = qRgba(val, val, val, qAlpha(data[i]));
        }
    }
    return img;
}


QImage  ImageViewer::rotate(QImage &img, RotateDirection r)
{
    QImage dest;
    int x, y;
    if ( img.depth() > 8 )
    {
        unsigned int *srcData, *destData;
        switch ( r )
        {
            case Rotate90:
                dest.create(img.height(), img.width(), img.depth());
                for ( y=0; y < img.height(); ++y )
                {
                    srcData = (unsigned int *)img.scanLine(y);
                    for ( x=0; x < img.width(); ++x )
                    {
                        destData = (unsigned int *)dest.scanLine(x);
                        destData[img.height()-y-1] = srcData[x];
                    }
                }
                break;
            case Rotate180:
                dest.create(img.width(), img.height(), img.depth());
                for ( y=0; y < img.height(); ++y )
                {
                    srcData = (unsigned int *)img.scanLine(y);
                    destData = (unsigned int *)dest.scanLine(img.height()-y-1);
                    for ( x=0; x < img.width(); ++x )
                        destData[img.width()-x-1] = srcData[x];
                }
                break;
            case Rotate270:
                dest.create(img.height(), img.width(), img.depth());
                for ( y=0; y < img.height(); ++y )
                {
                    srcData = (unsigned int *)img.scanLine(y);
                    for ( x=0; x < img.width(); ++x )
                    {
                        destData = (unsigned int *)dest.scanLine(img.width()-x-1);
                        destData[y] = srcData[x];
                    }
                }
                break;
            default:
                dest = img;
                break;
        }
    }
    else
    {
        unsigned char *srcData, *destData;
        unsigned int *srcTable, *destTable;
        switch ( r )
        {
            case Rotate90:
                dest.create(img.height(), img.width(), img.depth());
                dest.setNumColors(img.numColors());
                srcTable = (unsigned int *)img.colorTable();
                destTable = (unsigned int *)dest.colorTable();
                for ( x=0; x < img.numColors(); ++x )
                    destTable[x] = srcTable[x];
                for ( y=0; y < img.height(); ++y )
                {
                    srcData = (unsigned char *)img.scanLine(y);
                    for ( x=0; x < img.width(); ++x )
                    {
                        destData = (unsigned char *)dest.scanLine(x);
                        destData[img.height()-y-1] = srcData[x];
                    }
                }
                break;
            case Rotate180:
                dest.create(img.width(), img.height(), img.depth());
                dest.setNumColors(img.numColors());
                srcTable = (unsigned int *)img.colorTable();
                destTable = (unsigned int *)dest.colorTable();
                for ( x=0; x < img.numColors(); ++x )
                    destTable[x] = srcTable[x];
                for ( y=0; y < img.height(); ++y )
                {
                    srcData = (unsigned char *)img.scanLine(y);
                    destData = (unsigned char *)dest.scanLine(img.height()-y-1);
                    for ( x=0; x < img.width(); ++x )
                        destData[img.width()-x-1] = srcData[x];
                }
                break;
            case Rotate270:
                dest.create(img.height(), img.width(), img.depth());
                dest.setNumColors(img.numColors());
                srcTable = (unsigned int *)img.colorTable();
                destTable = (unsigned int *)dest.colorTable();
                for ( x=0; x < img.numColors(); ++x )
                    destTable[x] = srcTable[x];
                for ( y=0; y < img.height(); ++y )
                {
                    srcData = (unsigned char *)img.scanLine(y);
                    for ( x=0; x < img.width(); ++x )
                    {
                        destData = (unsigned char *)dest.scanLine(img.width()-x-1);
                        destData[y] = srcData[x];
                    }
                }
                break;
            default:
                dest = img;
                break;
        }

    }
    return (dest);
}

void ImageViewer::slideShow( bool on )
{
    if (on) {
        if (!imageList.isEmpty()) {
            slideTimer->start(slideDelay * 1000, FALSE);
            filename = "";    // force restart
            slideReverse ? prevImage() : nextImage();
        }
    } else {
        slideTimer->stop();
        slideAction->setOn( false);
    }
}

void ImageViewer::slideUpdate()
{
    bool final_image = slideReverse ? prevImage() : nextImage();

    if (final_image && !slideRepeat) {
        slideTimer->stop();
        slideAction->setOn(FALSE);
    }
}

//
// Display the image after the current one in the image list.
// Return TRUE if the next call to nextImage() will wrap around to the
// first image in the list (ie. we're now viewing the last image in the list).
//
bool ImageViewer::nextImage(void)
{
    int idx = 0;

    if (imageList.count() > 0) {
        idx = imageIndex();
        if (idx != -1) {
            if (idx == int(imageList.count() - 1)) {
                idx = 0;
            } else {
                idx++;
            }
        } else {
            idx = 0;
        }
        openFile(imageList[idx]);
    }

    return idx == int(imageList.count() - 1) ? TRUE : FALSE;
}

//
// Display the image preceeding the current one in the image list.
// Return TRUE if the next call to prevImage() will wrap around to the last
// image in the list (ie. we're now viewing the first image in the list).
//
bool ImageViewer::prevImage(void)
{
    int idx = -1;

    if (imageList.count() > 0) {
        idx = imageIndex();
        if (idx != -1) {
            if (idx == 0) {
                idx = imageList.count() - 1;
            } else {
                idx--;
            }
        } else {
            idx = imageList.count() - 1;
        }
        openFile(imageList[idx]);
    }

    return idx == 0 ? TRUE : FALSE;
}

//
// Return the index into the imageList of the currently viewed
// image (ie. ImageViewer::filename in ImageViewer::imageList).
//
int ImageViewer::imageIndex(void)
{
    QValueListConstIterator<DocLnk> i;
    int index;

    if (imageList.count() == 0) {
        return -1;
    }

    for (index = 0, i = imageList.begin(); i != imageList.end(); ++i, index++) {
        if ((*i).file() == filename) {
            return index;
        }
    }

    return -1;
}

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

#ifndef SHOWIMG_H
#define SHOWIMG_H

#include <qwidget.h>
#include <qmainwindow.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qdialog.h>
#include <qstringlist.h>

class QPEToolBar;
class QPEMenuBar;
class QPopupMenu;
class QWidgetStack;
class FileSelector;
class DocLnk;
class QLabel;
class QAction;
class QSpinBox;
class ImageFileSelector;



class ImageWidget : public QWidget
{
    Q_OBJECT
    public:
    ImageWidget(
               QWidget *parent=0                  
               ) : QWidget( parent ) 
    {
        setBackgroundMode(NoBackground);        
    }
    ~ImageWidget()
    {
        
    }

    void setPixmap( const QPixmap &pm ) 
    {
        pixmap = pm;
        show();
    }

    signals:
    void clicked();

protected:
    void paintEvent( QPaintEvent * );
    void mouseReleaseEvent(QMouseEvent* event);

private:
    QPixmap pixmap;
};

class InfoDialog:public QDialog
{
    Q_OBJECT

    public:

    static void displayInfo(const QString &caption, const QStringList text, QWidget *parent); 

private:

    InfoDialog(const QString &caption,const QStringList text,   QWidget *parent);

};  

class ControlsDialog:public QDialog
{
    Q_OBJECT

    public:
    ControlsDialog(const QString &caption,const QImage image,int *brightness,    QWidget *parent);


private slots:

    void bValueChanged(int);
    void accept();


private:
    ImageWidget *pixmap;
    QSpinBox *spb;
    QImage  img;
    int *b;
}; 


class ImagePane : public QWidget
{
    Q_OBJECT
    public:
    ImagePane( QWidget *parent=0 );
    ~ImagePane()
    {
        
    }

    //void showStatus();
    //void hideStatus();
    //QLabel  *statusLabel()
    //{
    //    return status;
    //}
    void setPixmap( const QPixmap &pm );


    int  paneWidth() const
    {
        return image->visibleWidth(); 
    }

    int  paneHeight() const
    {
        return image->visibleHeight(); 
    }

    void setPosition(int x, int y)
    {
        image->setContentsPos (x,y );
    }

    void disable()
    {
        pic->hide();
    }

    signals:
    void clicked();

private:
    QScrollView   *image;    
    ImageWidget  *pic;
    QVBoxLayout *vb;

private slots:
    void imageClicked();
};


class ImageViewer : public QMainWindow
{
    Q_OBJECT
    public:
    ImageViewer( QWidget *parent=0, const char *name=0, int wFlags=0 );
    ~ImageViewer();

    void loadImage( const char *fileName );
    void show(const QString& fileref);
    void show();


    enum INFO_STRINGS
    {
        PATH,
        FORMAT,
        FILE_SIZE,
        SIZE,
        COLORS,
        ALPHA,
        LAST
    };

    enum RotateDirection
    {
        Rotate90, Rotate180, Rotate270
    };


    static QImage  rotate(QImage &img, RotateDirection r);
    static QImage& intensity(QImage &image, float percent);
    static QImage& toGray(QImage &image, bool fast = false);

protected:
    void resizeEvent( QResizeEvent * );
    void closeEvent( QCloseEvent * );

private:
    void updateCaption( QString name );
    bool loadSelected();
    void scale();
    bool reconvertImage();
    int calcHeight();
    void setImage(const QImage& newimage);
    void updateImageInfo(QString &filePath);
    void switchToFileSelector();
    void switchToImageView();

    void updateImage();




private slots:

    void switchThumbView();
    void switchSizeToScreen();
    void setDocument(const QString& fileref);
    void doDelayedLoad();
    void openFile( const DocLnk &file );
    //void openFile();
    void open();
    void closeFileSelector();
    void hFlip();
    void vFlip();
    void rot180();
    void rot90();
    void rot270();
    void normalView();
    void fullScreen();
    void blackAndWhite();
    void displayInfoDialog();
    void displayControlsDialog();










private:




    enum MENU_ITEMS
    {
        SHOW_THUMBNAILS,
        SIZE_TO_SCREEN,
        BLACKANDWHITE
    };



    QString filename;
    QString delayLoad;
    QImage image;           // the loaded image
    QPixmap pm;         // the converted pixmap
    QPixmap pmScaled;       // the scaled pixmap
    QPEToolBar  *toolBar;
    QPEToolBar  *iconToolBar;
    QPEMenuBar  *menuBar;
    QPEMenuBar  *current;


    QPopupMenu  *fileMenuFile;
    QPopupMenu  *viewMenuFile;
    QPopupMenu  *optionsMenuFile;
    QPopupMenu  *fileMenuView;
    QPopupMenu  *viewMenuView;

    QAction     *sss;                // scale to screen size

    QLabel      *lab;
    ImagePane *imagePanel;
    QWidgetStack *stack;
    //FileSelector *fileSelector;
    ImageFileSelector *fileSelector;
    bool isFullScreen;
    bool isSized;                   // true if image is to be resized to fit the window size
    bool bFromDocView;              // a flag to indicate whether or not we were 
                                    // launched from the document view...

    bool showThumbView;              // a flag to indicate if FileSelector should be initialized with thumbnail view

    QString imageInfo[LAST];
};


#endif // SHOWIMG_H

#include "imageinfoui.h"

#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtextview.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qstring.h>

#include <lib/slavemaster.h>
#include <lib/imagecache.h>

#include <qpe/qcopenvelope_qws.h>
#include <qpe/resource.h>

imageinfo::imageinfo(const QString&_path, QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl ),currentFile(_path)
{
    {
        QCopEnvelope( "QPE/Application/opie-eye_slave", "refUp()" );
    }
    if ( !name )
	setName( "imageinfo" );
    resize( 289, 335 ); 
    setCaption( tr( "Image info" ) );
    imageinfoLayout = new QVBoxLayout( this ); 
    imageinfoLayout->setSpacing(2);
    imageinfoLayout->setMargin(2);

    PixmapLabel1 = new QLabel( this, "PixmapLabel1" );
    PixmapLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, PixmapLabel1->sizePolicy().hasHeightForWidth() ) );
    PixmapLabel1->setScaledContents( TRUE );
    imageinfoLayout->addWidget( PixmapLabel1 );

    Line1 = new QFrame( this, "Line1" );
    Line1->setFrameStyle( QFrame::HLine | QFrame::Sunken );
    imageinfoLayout->addWidget( Line1 );

    fnameLabel = new QLabel( this, "FnameLabel" );
    imageinfoLayout->addWidget( fnameLabel);
    
    TextView1 = new QTextView( this, "TextView1" );
    TextView1->setFrameShadow( QTextView::Plain );
    QToolTip::add(  TextView1, tr( "Displays info of selected image" ) );
    QWhatsThis::add(  TextView1, tr( "Displays info of selected image" ) );
    imageinfoLayout->addWidget( TextView1 );
    
    SlaveMaster* master = SlaveMaster::self();
    connect( master, SIGNAL(sig_fullInfo(const QString&, const QString&)),
             this, SLOT(slot_fullInfo(const QString&, const QString&)) );
    connect(master, SIGNAL( sig_thumbNail(const QString&, const QPixmap&)),
            this, SLOT(slotThumbNail(const QString&, const QPixmap&)));
    slotChangeName(_path);
}

void imageinfo::slotChangeName(const QString&_path)
{
    currentFile=_path;
    fnameLabel->setText("<qt><center><b>"+currentFile+"</b></center></qt>");
    SlaveMaster::self()->imageInfo( currentFile );
    
    QPixmap*m_pix = PPixmapCache::self()->cachedImage( _path, 64, 64 );
    if (!m_pix) {
        PixmapLabel1->setPixmap(QPixmap( Resource::loadPixmap( "UnknownDocument" )));
    } else {
        PixmapLabel1->setPixmap(*m_pix);
    }
}

imageinfo::~imageinfo()
{
    {
        QCopEnvelope( "QPE/Application/opie-eye_slave", "refDown()" );
    }
}

void imageinfo::slot_fullInfo(const QString&_path, const QString&_t)
{
    if (_path == currentFile) {
        qDebug(_t);
        QString t = _t;
        t.replace(QRegExp("\n"),"<br>");
        TextView1->setText(t);
    }
}

void imageinfo::slotThumbNail(const QString&_path, const QPixmap&_pix)
{
    if (_pix.width()>0)
        PPixmapCache::self()->insertImage( _path, _pix, 64, 64 );
    if (_path == currentFile) {
        PixmapLabel1->setPixmap( _pix );
    }
}

/* for testing */
infoDlg::infoDlg(const QString&fname,QWidget * parent, const char * name)
    :QDialog(parent,name,true,WStyle_ContextHelp)
{
    QVBoxLayout*dlglayout = new QVBoxLayout(this);
    dlglayout->setSpacing(2);
    dlglayout->setMargin(1);
    imageinfo*inf = new imageinfo(fname,this);
    dlglayout->addWidget(inf);
}

infoDlg::~infoDlg()
{
}

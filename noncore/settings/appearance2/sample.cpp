#include <qvbox.h>
#include <qpopupmenu.h>
#include <qpainter.h>
#include <qmenubar.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qlayout.h>
#include <qwhatsthis.h>
#include <qpixmapcache.h>
#include <qtimer.h>
#include <qobjectlist.h>
#include <qcommonstyle.h>

#include "sample.h"


class SampleText : public QWidget
{
public:
    SampleText( const QString &t, bool h, QWidget *parent )
	: QWidget( parent ), hl(h), text(t)
    {
	if ( hl )
	    setBackgroundMode( PaletteHighlight );
	else
	    setBackgroundMode( PaletteBase );
    }

    QSize sizeHint() const
    {
	QFontMetrics fm(font());
	return QSize( fm.width(text)+10, fm.height()+4 );
    }

    void paintEvent( QPaintEvent * )
    {
	QPainter p(this);
	if ( hl )
	    p.setPen( colorGroup().highlightedText() );
	else
	    p.setPen( colorGroup().text() );
	p.drawText( rect(), AlignCenter, text );
    }

private:
    bool hl;
    QString text;
};


SampleWindow::SampleWindow( QWidget *parent ) : QWidget(parent), iface(0)
{
	init();
}

QSize SampleWindow::sizeHint() const
{
	return container->sizeHint() + QSize( 10, 35 );
}

void SampleWindow::setFont( const QFont &f )
{
	QWidget::setFont( f );
	popup->setFont( f );
	QTimer::singleShot ( 0, this, SLOT( fixGeometry ( )));	
}

static void setStyleRecursive ( QWidget *w, QStyle *s )
{
	w->setStyle( s );
	QObjectList *childObjects=(QObjectList*)w->children();
	if ( childObjects ) {
		QObject * o;
		for(o=childObjects->first();o!=0;o=childObjects->next()) {
			if( o->isWidgetType() ) {
				setStyleRecursive((QWidget *)o,s);
			}
		}
	}
}
                                                    
                                                    
void SampleWindow::setStyle2 ( QStyle *sty )
{
	typedef void (QStyle::*QDrawMenuBarItemImpl) (QPainter *, int, int, int, int, QMenuItem *, QColorGroup &, bool, bool);

	extern QDrawMenuBarItemImpl qt_set_draw_menu_bar_impl(QDrawMenuBarItemImpl);

	QPixmapCache::clear ( );
	QPalette p = palette ( );
	sty-> polish ( p );
	qt_set_draw_menu_bar_impl ( 0 );
	setStyleRecursive ( this, sty );
	setPalette ( p );
	QTimer::singleShot ( 0, this, SLOT( fixGeometry ( )));	
}


void SampleWindow::setDecoration( WindowDecorationInterface *i )
{
	iface = i;
	wd.rect = QRect( 0, 0, 150, 75 );
	wd.caption = tr("Sample");
	wd.palette = palette();
	wd.flags = WindowDecorationInterface::WindowData::Dialog |
		   WindowDecorationInterface::WindowData::Active;
	wd.reserved = 1;

	th = iface->metric(WindowDecorationInterface::TitleHeight, &wd);
	tb = iface->metric(WindowDecorationInterface::TopBorder, &wd);
	lb = iface->metric(WindowDecorationInterface::LeftBorder, &wd);
	rb = iface->metric(WindowDecorationInterface::RightBorder, &wd);
	bb = iface->metric(WindowDecorationInterface::BottomBorder, &wd);

	int yoff = th + tb;
	int xoff = lb;

	wd.rect.setX( 0 );
	wd.rect.setWidth( width() - lb - rb );
	wd.rect.setY( 0 );
	wd.rect.setHeight( height() - yoff - bb );

	container->setGeometry( xoff, yoff, wd.rect.width(), wd.rect.height() );
	setMinimumSize( container->sizeHint().width()+lb+rb,
			container->sizeHint().height()+tb+th+bb );
}

void SampleWindow::paintEvent( QPaintEvent * )
{
	if ( !iface )
	    return;

	QPainter p( this );

	p.translate( lb, th+tb );

	iface->drawArea(WindowDecorationInterface::Border, &p, &wd);
	iface->drawArea(WindowDecorationInterface::Title, &p, &wd);

	p.setPen(palette().active().color(QColorGroup::HighlightedText));
	QFont f( font() );
	f.setWeight( QFont::Bold );
	p.setFont(f);
	iface->drawArea(WindowDecorationInterface::TitleText, &p, &wd);

	QRect brect( 0, -th, iface->metric(WindowDecorationInterface::HelpWidth,&wd), th );
	iface->drawButton( WindowDecorationInterface::Help, &p, &wd,
	    brect.x(), brect.y(), brect.width(), brect.height(), (QWSButton::State)0 );
	brect.moveBy( wd.rect.width() -
	    iface->metric(WindowDecorationInterface::OKWidth,&wd) -
	    iface->metric(WindowDecorationInterface::CloseWidth,&wd), 0 );
	iface->drawButton( WindowDecorationInterface::Close, &p, &wd,
	    brect.x(), brect.y(), brect.width(), brect.height(), (QWSButton::State)0 );
	brect.moveBy( iface->metric(WindowDecorationInterface::CloseWidth,&wd), 0 );
	iface->drawButton( WindowDecorationInterface::OK, &p, &wd,
	    brect.x(), brect.y(), brect.width(), brect.height(), (QWSButton::State)0 );
}

void SampleWindow::init()
{
	container = new QVBox( this );
	popup = new QPopupMenu( this );
	popup->insertItem( tr("Normal Item"), 1 );
	popup->insertItem( tr("Disabled Item"), 2 );
	popup->setItemEnabled(2, FALSE);
	QMenuBar *mb = new QMenuBar( container );
	mb->insertItem( tr("Menu"), popup );
	QHBox *hb = new QHBox( container );
	QWidget *w = new QWidget( hb );
	(void)new QScrollBar( 0, 0, 0, 0, 0, Vertical, hb );

	QGridLayout *gl = new QGridLayout( w, 2, 2, 4 );
	SampleText *l = new SampleText( tr("Normal Text"), FALSE, w );
	gl->addWidget( l, 0, 0 );

	l = new SampleText( tr("Highlighted Text"), TRUE, w );
	gl->addWidget( l, 1, 0 );

	QPushButton *pb = new QPushButton( tr("Button"), w );
	gl->addWidget( pb, 0, 1 );
	pb->setFocusPolicy( NoFocus );

	QCheckBox *cb = new QCheckBox( tr("Check Box"), w );
	gl->addWidget( cb, 1, 1 );
	cb->setFocusPolicy( NoFocus );
	cb->setChecked( TRUE );

	QWhatsThis::add( this, tr("Sample window using the selected settings.") );
}

bool SampleWindow::eventFilter( QObject *, QEvent *e )
{
	switch ( e->type() ) {
	    case QEvent::MouseButtonPress:
	    case QEvent::MouseButtonRelease:
	    case QEvent::MouseButtonDblClick:
	    case QEvent::MouseMove:
	    case QEvent::KeyPress:
	    case QEvent::KeyRelease:
		return TRUE;
	    default:
		break;
	}

	return FALSE;
}

void SampleWindow::paletteChange( const QPalette &old )
{
	QWidget::paletteChange ( old );
	wd. palette = palette ( );
	popup-> setPalette ( palette ( ));
}


void SampleWindow::setPalette ( const QPalette &pal )
{
	QPixmapCache::clear ( );
	QPalette p = pal;
	style ( ). polish ( p );
	QWidget::setPalette ( p );
}

void SampleWindow::resizeEvent( QResizeEvent *re )
{
	wd.rect = QRect( 0, 0, 150, 75 );

	wd.rect.setX( 0 );
	wd.rect.setWidth( width() - lb - rb );
	wd.rect.setY( 0 );
	wd.rect.setHeight( height() - th - tb - bb );

	container->setGeometry( lb, th+tb, wd.rect.width(), wd.rect.height() );
	QWidget::resizeEvent( re );
}

void SampleWindow::fixGeometry()
{
	setMinimumSize( container->sizeHint().width()+lb+rb,
			container->sizeHint().height()+tb+th+bb );
}


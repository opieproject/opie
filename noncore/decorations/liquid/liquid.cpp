#include "liquid.h"

#include <qapplication.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qdrawutil.h>


LiquidDecoInterface::LiquidDecoInterface ( ) :  ref ( 0 )
{
}
    
LiquidDecoInterface::~LiquidDecoInterface ( )
{
}



QString LiquidDecoInterface::name ( ) const
{
	return qApp-> translate ( "Decoration", "Liquid" );
}


QPixmap LiquidDecoInterface::icon ( ) const
{
	return QPixmap ( );
}            
        
void LiquidDecoInterface::drawArea( Area a, QPainter *p, const WindowData *wd ) const
{
	QRect r = wd-> rect;
	int th = metric ( TitleHeight, wd );

	switch ( a ) {
		case WindowDecorationInterface::Border: {
			const QColorGroup &cg = wd-> palette. active ( );
			qDrawWinPanel ( p, r. x ( ) - metric ( LeftBorder, wd ),
				r. y ( ) - th - metric ( TopBorder, wd ),
				r. width ( ) + metric ( LeftBorder, wd ) + metric ( RightBorder, wd ),
				r. height ( ) + th + metric ( TopBorder,wd) + metric ( BottomBorder, wd ),
				cg, false, &cg. brush ( QColorGroup::Background ));
			break;
		}
		case WindowDecorationInterface::Title: {
			const QColorGroup &cg = wd-> palette. active ( );
			
			QBrush titleBrush;
			QPen   titleLines1, titleLines2;

			titleBrush = cg. brush (( wd-> flags & WindowData::Active ) ? QColorGroup::Highlight : QColorGroup::Background );
			titleLines1 = titleBrush. color ( ). dark ( 105 );
			titleLines2 = titleBrush. color ( ). dark ( 103 );
			
			p-> fillRect ( r. x ( ), r. y ( ) - th, r. width ( ), th, titleBrush );
			for ( int i = r.y()-th; i < r.y(); i += 4 ) {
				p-> setPen ( titleLines1 );
				p-> drawLine ( r. left ( ), i, r. right ( ), i );
				p-> setPen ( titleLines2 );
				p-> drawLine ( r. left ( ), i+1, r. right ( ), i+1 );
			}
			break;
		}
		case WindowDecorationInterface::TitleText: {
			QColor textcol = wd-> palette. active ( ). color (( wd-> flags & WindowData::Active ) ? QColorGroup::HighlightedText : QColorGroup::Text );
			QColor shadecol = ( qGray ( textcol. rgb ( ) ) > 128 ) ? textcol. dark ( 230 ) : textcol.light( 300 );
			if ( textcol == shadecol ) {
				if ( qGray ( shadecol. rgb ( ) ) < 128 )
					shadecol = QColor ( 225, 225, 225 );
				else
					shadecol = QColor ( 30, 30, 30 );
			}			
			QColor fillcol = wd-> palette. active ( ). color (( wd-> flags & WindowData::Active ) ? QColorGroup::Highlight : QColorGroup::Background );
			QColor bordercol = fillcol. dark ( 110 );
			fillcol = fillcol. light ( 110 );
			
			QRect tr ( r.x ( ) + 3 + metric ( HelpWidth, wd ), r. top ( ) - th,
				r. width ( ) - metric ( OKWidth, wd ) - metric ( CloseWidth, wd ), th );

			QFont f = p-> font ( );
			f. setWeight ( QFont::Bold );
			p-> setFont ( f );                        
						
			QRect br = p-> boundingRect ( tr, QPainter::AlignCenter, wd-> caption );
			int trh2 = tr. height ( ) / 2;
			
			int align = QPainter::AlignCenter;

			if ( br. width ( ) + 2 * trh2 > tr. width ( ) ) {
				br. setLeft ( tr. left ( ) + trh2 );
				br. setWidth ( tr. width ( ) - 2 * trh2 );
			
				align = QPainter::AlignLeft | QPainter::AlignVCenter;
			}
			
			p-> setBrush ( fillcol );
			p-> setPen ( bordercol /*Qt::NoPen*/ );
			p->	drawChord ( br. left ( ) - trh2, tr. top ( ), 2 * trh2, tr. height ( ), 90*16, 180*16 );
			p->	drawChord ( br. right ( ) + 1 - trh2, tr. top ( ), 2 * trh2, tr. height ( ), 90*16, -180*16 );

			p-> fillRect ( br.left() - 1, tr. top ( ), br. width ( ) + 2, tr. height ( ), fillcol);
				
			p-> setPen ( shadecol );
			p-> drawText ( br, align, wd-> caption );
			p-> setPen ( textcol );
			br. moveBy ( -1, -1 );
			p-> drawText ( br, align, wd-> caption );
			break;
		}
		default: {
			WindowDecorationInterface::drawArea ( a, p, wd );
			break;
		}
	}
}

QRESULT LiquidDecoInterface::queryInterface ( const QUuid &uuid, QUnknownInterface **iface )
{
	*iface = 0;
	
	if ( uuid == IID_QUnknown )
		*iface = this;
	else if ( uuid == IID_WindowDecoration )
		*iface = this;
	
	if ( *iface )
		(*iface)-> addRef ( );
		
	return QS_OK;
}

Q_EXPORT_INTERFACE()
{
	Q_CREATE_INSTANCE( LiquidDecoInterface )
}


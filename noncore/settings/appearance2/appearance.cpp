/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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
***********************************************************************
**
**  Enhancements by: Dan Williams, <williamsdr@acm.org>
**
**********************************************************************/

#include "appearance.h"
#include "editScheme.h"

#include <opie/ofiledialog.h>
#include <opie/otabwidget.h>

#include <qpe/applnk.h>
#include <qpe/config.h>
#include <qpe/global.h>
#include <qpe/resource.h>
#include <qpe/qpeapplication.h>
#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
#include <qpe/qcopenvelope_qws.h>
#endif

#include <qaction.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qdir.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qmultilineedit.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#if QT_VERSION >= 300
#include <qstylefactory.h>
#else
#include <qwindowsstyle.h>
#include <qpe/qpestyle.h>
#include <qpe/lightstyle.h>
#include <qpe/qlibrary.h>
#include <qpe/styleinterface.h>
#endif
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qvgroupbox.h>
#include <qwidget.h>

#include "stylelistitem.h"
#include "decolistitem.h"
#include "colorlistitem.h"

#include "sample.h"

#include <opie/ofontselector.h>


class DefaultWindowDecoration : public WindowDecorationInterface
{
public:
    DefaultWindowDecoration() : ref(0) {}
    QString name() const {
	return "Default";
    }
    QPixmap icon() const {
	return QPixmap();
    }
    QRESULT queryInterface( const QUuid &uuid, QUnknownInterface **iface ) {
	*iface = 0;
	if ( uuid == IID_QUnknown )
	    *iface = this;
	else if ( uuid == IID_WindowDecoration )
	    *iface = this;

	if ( *iface )
	    (*iface)->addRef();
	return QS_OK;
    }
    Q_REFCOUNT

private:
	ulong ref;
};



void Appearance::loadStyles ( QListBox *list )
{
#if QT_VERSION >= 300
    list->insertStringList(QStyleFactory::styles());
#else
    list->insertItem( new StyleListItem ( "Windows", new QWindowsStyle ( )));
    list->insertItem( new StyleListItem ( "Light", new LightStyle ( )));
#ifndef QT_NO_STYLE_MOTIF
    list->insertItem( new StyleListItem ( "Motif", new QMotifStyle ( )));
#endif
#ifndef QT_NO_STYLE_MOTIFPLUS
    list->insertItem( new StyleListItem ( "MotifPlus", new QMotifPlusStyle ( )));
#endif
#ifndef QT_NO_STYLE_PLATINUM
    list->insertItem( new StyleListItem ( "Platinum", new QPlatinumStyle ( )));
#endif
#endif
    list->insertItem( new StyleListItem ( "QPE", new QPEStyle ( )));

#if QT_VERSION < 300
	{
		QString path = QPEApplication::qpeDir() + "/plugins/styles/";
		QStringList sl = QDir ( path, "lib*.so" ). entryList ( );

		for ( QStringList::Iterator it = sl. begin ( ); it != sl. end ( ); ++it ) {
			QLibrary *lib = new QLibrary ( path + "/" + *it );
			StyleInterface *iface;

			if ( lib-> queryInterface ( IID_Style, (QUnknownInterface **) &iface ) == QS_OK )
				list-> insertItem ( new StyleListItem ( lib, iface ));
			else
				delete lib;
		}
	}

#endif
}

void Appearance::loadDecos ( QListBox *list )
{
    list-> insertItem ( new DecoListItem ( tr( "Default" )));

	{
		QString path = QPEApplication::qpeDir() + "/plugins/decorations/";
		QStringList sl = QDir ( path, "lib*.so" ). entryList ( );

		for ( QStringList::Iterator it = sl. begin ( ); it != sl. end ( ); ++it ) {
			QLibrary *lib = new QLibrary ( path + "/" + *it );
			WindowDecorationInterface *iface;

			if ( lib-> queryInterface ( IID_WindowDecoration, (QUnknownInterface **) &iface ) == QS_OK )
				list-> insertItem ( new DecoListItem ( lib, iface ));
			else
				delete lib;
		}
	}
}

void Appearance::loadColors ( QListBox *list )
{
    list-> clear ( );
    {
    	Config config ( "qpe" );
		config. setGroup ( "Appearance" );
    
		list-> insertItem ( new ColorListItem ( tr( "Current scheme" ), config ));
	}

	QString path = QPEApplication::qpeDir ( ) + "/etc/colors/";
    QStringList sl = QDir ( path ). entryList ( "*.scheme" );
	
	for ( QStringList::Iterator it = sl. begin ( ); it != sl. end ( ); ++it ) {
		QString name = (*it). left ((*it). find ( ".scheme" ));
		Config config ( path + *it, Config::File );
		config. setGroup ( "Colors" );
		
		list-> insertItem ( new ColorListItem ( name, config ));
	}
}


QWidget *Appearance::createStyleTab ( QWidget *parent )
{
	Config config ( "qpe" );
	config. setGroup ( "Appearance" );

    QWidget* tab = new QWidget( parent, "StyleTab" );
    QVBoxLayout* vertLayout = new QVBoxLayout( tab, 4, 4 );

    m_style_list = new QListBox( tab, "m_style_list" );
    vertLayout->addWidget( m_style_list );

    m_style_settings = new QPushButton ( tr( "Settings..." ), tab );
    connect ( m_style_settings, SIGNAL( clicked ( )), this, SLOT( styleSettingsClicked ( )));
    vertLayout-> addWidget ( m_style_settings );

	loadStyles ( m_style_list );

    QString s = config. readEntry ( "Style", "Light" );
    m_style_list-> setCurrentItem ( m_style_list-> findItem ( s ));
    m_original_style = m_style_list-> currentItem ( );
    styleClicked ( m_original_style );

    connect( m_style_list, SIGNAL( highlighted( int ) ), this, SLOT( styleClicked( int ) ) );

	return tab;
}

QWidget *Appearance::createDecoTab ( QWidget *parent )
{
	Config config ( "qpe" );
	config. setGroup ( "Appearance" );

    QWidget* tab = new QWidget( parent, "DecoTab" );
    QVBoxLayout* vertLayout = new QVBoxLayout( tab, 4, 4 );

    m_deco_list = new QListBox( tab, "m_deco_list" );
    vertLayout->addWidget( m_deco_list );

	loadDecos ( m_deco_list );

    QString s = config. readEntry ( "Decoration" );
    m_deco_list-> setCurrentItem ( m_deco_list-> findItem ( s ));
    m_original_deco = m_deco_list-> currentItem ( );
    if ( m_deco_list-> currentItem  ( ) < 0 )
    	m_deco_list-> setCurrentItem ( 0 );
    decoClicked ( m_original_deco );

    connect( m_deco_list, SIGNAL( highlighted( int ) ), this, SLOT( decoClicked( int ) ) );

	return tab;
}

QWidget *Appearance::createFontTab ( QWidget *parent )
{
	Config config ( "qpe" );
	config. setGroup ( "Appearance" );

    QString familyStr = config.readEntry( "FontFamily", "Helvetica" );
    QString styleStr = config.readEntry( "FontStyle", "Regular" );
    int size = config.readNumEntry( "FontSize", 10 );

    m_fontselect = new OFontSelector ( parent, "FontTab" );    
    m_fontselect-> setSelectedFont ( familyStr, styleStr, size );
    
    connect( m_fontselect, SIGNAL( fontSelected ( const QFont & )),
             this, SLOT( fontClicked ( const QFont & )));

    return m_fontselect; 
}

QWidget *Appearance::createColorTab ( QWidget *parent )
{
	Config config ( "qpe" );
	config. setGroup ( "Appearance" );


    QWidget *tab = new QWidget( parent, "ColorTab" );
    QGridLayout *gridLayout = new QGridLayout( tab, 0, 0, 4, 4 );
    gridLayout->setRowStretch ( 3, 10 );

    m_color_list = new QListBox ( tab );
    gridLayout->addMultiCellWidget ( m_color_list, 0, 3, 0, 0 );
    connect( m_color_list, SIGNAL( highlighted( int ) ), this, SLOT( colorClicked( int ) ) );

    loadColors ( m_color_list );
    m_color_list-> setCurrentItem ( 0 );

    QPushButton* tempButton = new QPushButton( tab, "editSchemeButton" );
    tempButton->setText( tr( "Edit..." ) );
    connect( tempButton, SIGNAL( clicked() ), this, SLOT( editSchemeClicked() ) );
    gridLayout->addWidget( tempButton, 0, 1 );

    tempButton = new QPushButton( tab, "deleteSchemeButton" );
    tempButton->setText( tr( "Delete" ) );
    connect( tempButton, SIGNAL( clicked() ), this, SLOT( deleteSchemeClicked() ) );
    gridLayout->addWidget( tempButton, 1, 1 );

    tempButton = new QPushButton( tab, "saveSchemeButton" );
    tempButton->setText( tr( "Save" ) );
    connect( tempButton, SIGNAL( clicked() ), this, SLOT( saveSchemeClicked() ) );
    gridLayout->addWidget( tempButton, 2, 1 );

    return tab;
}

QWidget *Appearance::createGuiTab ( QWidget *parent )
{
	Config config ( "qpe" );
	config. setGroup ( "Appearance" );

    QWidget *tab = new QWidget( parent, "AdvancedTab" );
    QVBoxLayout *vertLayout = new QVBoxLayout( tab, 4, 4 );
        
    QGridLayout* gridLayout = new QGridLayout ( vertLayout );

	int style = config. readNumEntry ( "TabStyle", 2 ) - 1;
	bool tabtop = ( config. readEntry ( "TabPosition", "Top" ) == "Top" );

    QLabel* label = new QLabel( tr( "Tab style:" ), tab );
    gridLayout-> addWidget ( label, 0, 0 );
    QButtonGroup* btngrp = new QButtonGroup( tab, "buttongroup" );
    btngrp-> hide ( );
    btngrp-> setExclusive ( true );

    m_tabstyle_list = new QComboBox ( false, tab, "tabstyle" );
    m_tabstyle_list-> insertItem ( tr( "Tabs" ));
    m_tabstyle_list-> insertItem ( tr( "Tabs w/icons" ));
    m_tabstyle_list-> insertItem ( tr( "Drop down list" ));
    m_tabstyle_list-> insertItem ( tr( "Drop down list w/icons" ));
    m_tabstyle_list-> setCurrentItem ( style );
    gridLayout-> addMultiCellWidget ( m_tabstyle_list, 0, 0, 1, 2 );

    m_tabstyle_top = new QRadioButton( tr( "Top" ), tab, "tabpostop" );
    btngrp-> insert ( m_tabstyle_top );
    gridLayout-> addWidget( m_tabstyle_top, 1, 1 );
    
    m_tabstyle_bottom = new QRadioButton( tr( "Bottom" ), tab, "tabposbottom" );
    btngrp-> insert ( m_tabstyle_bottom );
    gridLayout-> addWidget( m_tabstyle_bottom, 1, 2 );

    m_tabstyle_top-> setChecked ( tabtop );
    m_tabstyle_bottom-> setChecked ( !tabtop );

	m_original_tabstyle = style;
	m_original_tabpos = tabtop;

	return tab;
}


Appearance::Appearance( QWidget* parent,  const char* name, WFlags )
    : QDialog ( parent, name, true )
{
    setCaption( tr( "Appearance" ) );

    Config config( "qpe" );
    config.setGroup( "Appearance" );

    QVBoxLayout *top = new QVBoxLayout ( this, 4, 4 );

	m_sample = new SampleWindow ( this );
	m_sample-> setDecoration ( new DefaultWindowDecoration ( ));
	
    OTabWidget* tw = new OTabWidget ( this, "tabwidget", OTabWidget::Global, OTabWidget::Bottom );
	QWidget *styletab;
	
 	tw-> addTab ( styletab = createStyleTab ( tw ), "appearance/styletabicon.png", tr( "Style" ));
    tw-> addTab ( createFontTab ( tw ), "appearance/fonttabicon.png", tr( "Font" ));
    tw-> addTab ( createColorTab ( tw ), "appearance/colorstabicon.png", tr( "Colors" ) );
    tw-> addTab ( createDecoTab ( tw ), "appearance/styletabicon.png", tr( "Windows" ) );
    tw-> addTab ( createGuiTab ( tw ), "appearance/backgroundtabicon.png", tr( "Gui" ) );

	top-> addWidget ( tw, 10 );	
	top-> addWidget ( m_sample, 1 );

    tw-> setCurrentTab ( styletab );
}

Appearance::~Appearance()
{
}

void Appearance::accept ( )
{
    Config config("qpe");
    config.setGroup( "Appearance" );

	int newtabstyle = m_tabstyle_list-> currentItem ( );
	bool newtabpos = m_tabstyle_top-> isChecked ( );


    if ( m_style_changed ) { 
	    StyleListItem *item = (StyleListItem *) m_style_list-> item ( m_style_list-> currentItem ( ));
	    if ( item )
            config.writeEntry( "Style", item-> key ( ));
	}

	if (( newtabstyle != m_original_tabstyle ) || ( newtabpos != m_original_tabpos )) {
		config. writeEntry ( "TabStyle", newtabstyle + 1 );
		config. writeEntry ( "TabPosition", newtabpos ? "Top" : "Bottom" );
	}

    if ( m_font_changed ) {
        config. writeEntry ( "FontFamily", m_fontselect-> fontFamily ( ));
        config. writeEntry ( "FontStyle", m_fontselect-> fontStyle ( ));
        config. writeEntry ( "FontSize", m_fontselect-> fontSize ( ));
    }


    if ( m_color_changed )
    {
    	ColorListItem *item = (ColorListItem *) m_color_list-> item ( m_color_list-> currentItem ( ));
 
 		if ( item )   
			item-> save ( config );
    }
    
	config. write ( ); // need to flush the config info first
	Global::applyStyle ( );

	if ( QMessageBox::warning ( this, tr( "Restart" ), tr( "Do you want to restart Opie now?" ),  tr( "Yes" ), tr( "No" ), 0, 0, 1 ) == 0 ) {
		QCopEnvelope e( "QPE/System", "restart()" );
	}
	
	QDialog::accept ( );
}

void Appearance::done ( int r )
{
	QDialog::done ( r );
	close ( );
}


void Appearance::styleClicked ( int index )
{
    StyleListItem *sli = (StyleListItem *) m_style_list-> item ( index );
	m_style_settings-> setEnabled ( sli ? sli-> hasSettings ( ) : false );
	
	if ( m_sample && sli && sli-> style ( ))
		m_sample-> setStyle2 ( sli-> style ( ));
		
	m_style_changed |= ( index != m_original_style );
}

void Appearance::styleSettingsClicked ( )
{
	StyleListItem *item = (StyleListItem *) m_style_list-> item ( m_style_list-> currentItem ( ));

	if ( item && item-> hasSettings ( )) {
		QDialog *d = new QDialog ( this, "SETTINGS-DLG", true );
		QVBoxLayout *vbox = new QVBoxLayout ( d, 4, 0 );

		QWidget *w = item-> settings ( d );

		if ( w ) {
			vbox-> addWidget ( w );

			d-> setCaption ( w-> caption ( ));

		   	d-> showMaximized ( );
			bool accepted = ( d-> exec ( ) == QDialog::Accepted );

			if ( item-> setSettings ( accepted ))
				m_style_changed = true;
		}
		delete d;
	}
}

void Appearance::decoClicked ( int index )
{
    DecoListItem *dli = (DecoListItem *) m_deco_list-> item ( index );
	
	if ( m_sample ) {
		if ( dli && dli-> interface ( ))
			m_sample-> setDecoration ( dli-> interface ( ));		
		else
			m_sample-> setDecoration ( new DefaultWindowDecoration ( ));
	}
	m_deco_changed |= ( index != m_original_deco );
}

void Appearance::fontClicked ( const QFont &f )
{
	m_font_changed |= ( f != m_sample-> font ( ));	
	m_sample-> setFont ( f );
}

void Appearance::colorClicked ( int index )
{
	ColorListItem *item = (ColorListItem *) m_color_list-> item ( index );

	if ( item )
		m_sample-> setPalette ( item-> palette ( ));

	m_color_changed |= ( item-> palette ( ) != qApp-> palette ( ));
}


void Appearance::editSchemeClicked ( )
{
	ColorListItem *item = (ColorListItem *) m_color_list-> item ( m_color_list-> currentItem ( ));

	int cnt = 0;
	QString controlLabel [QColorGroup::NColorRoles];
	QString controlColor [QColorGroup::NColorRoles];
	
	for ( QColorGroup::ColorRole role = (QColorGroup::ColorRole) 0; role != QColorGroup::NColorRoles; ((int) role )++ ) {
		QColor col = item-> color ( role );
		
		if ( col. isValid ( )) {
			controlLabel [cnt] = item-> label ( role );
			controlColor [cnt] = col. name ( );
		
			cnt++;
		}
	}

    EditScheme* editdlg = new EditScheme( this, "editScheme", true, 0,  cnt, controlLabel, controlColor );
    editdlg-> showMaximized ( );
    if ( editdlg-> exec ( ) == QDialog::Accepted ) {
    	ColorListItem *citem = (ColorListItem *) m_color_list-> item ( 0 );
		cnt = 0;
		    
		for ( QColorGroup::ColorRole role = (QColorGroup::ColorRole) 0; role != QColorGroup::NColorRoles; ((int) role )++ ) {
			if ( item-> color ( role ). isValid ( )) {
        		citem-> setColor ( role, QColor ( controlColor [cnt] ));	
        		cnt++;
        	}
        }
        
        m_color_list-> setCurrentItem ( 0 );
        colorClicked ( 0 );
        
        m_color_changed = true;
    }
    delete editdlg;
}


void Appearance::saveSchemeClicked()
{
	ColorListItem *item = (ColorListItem *) m_color_list-> item ( m_color_list-> currentItem ( ));

	if ( !item )
		return;

	QDialog *d = new QDialog ( this, 0, true );
	d-> setCaption ( tr( "Save Scheme" ));
	QLineEdit *ed = new QLineEdit ( this );
	( new QVBoxLayout ( d, 4, 4 ))-> addWidget ( ed );

    if ( d-> exec ( ) == QDialog::Accepted ) {
        QString schemename = ed-> text ( );
        QFile file ( QPEApplication::qpeDir() + "/etc/colors/" + schemename + ".scheme" );
        if ( !file. exists ( ))
        { 
        	QPalette p = item-> palette ( );
        
            Config config ( file.name(), Config::File );
            config. setGroup( "Colors" );

			item-> save ( config );
			                
            config. write ( ); // need to flush the config info first
		    loadColors ( m_color_list );
        }
        else
        {
	        QMessageBox::information ( this, tr( "Save scheme" ), tr( "Scheme does already exist." ));
        }
    }
    delete d;
}

void Appearance::deleteSchemeClicked()
{
	ColorListItem *item = (ColorListItem *) m_color_list-> item ( m_color_list-> currentItem ( ));

	if ( !item )
		return;

    if ( m_color_list-> currentItem ( ) > 0 )
    {
        if ( QMessageBox::warning ( this, tr( "Delete scheme" ), tr( "Do you really want to delete\n" ) + item-> text ( ) + "?",
                tr( "Yes" ), tr( "No" ), 0, 0, 1 ) == 0 ) {
			QFile::remove ( QPEApplication::qpeDir ( ) + "/etc/colors/" + item-> text ( ) + ".scheme" );
			loadColors ( m_color_list );
        }
    }
    else
    {
        QMessageBox::information( this, tr( "Delete scheme" ), tr( "Unable to delete current scheme." ));
    }
}


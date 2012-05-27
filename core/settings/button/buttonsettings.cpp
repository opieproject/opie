/*
       =.            This file is part of the OPIE Project
      .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
     .>+-=
_;:,   .>  :=|.         This file is free software; you can
.> <`_,  > .  <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--  :           the terms of the GNU General Public
.="- .-=="i,   .._         License as published by the Free Software
- .  .-<_>   .<>         Foundation; either version 2 of the License,
  ._= =}    :          or (at your option) any later version.
  .%`+i>    _;_.
  .i_,=:_.   -<s.       This file is distributed in the hope that
  + . -:.    =       it will be useful, but WITHOUT ANY WARRANTY;
  : ..  .:,   . . .    without even the implied warranty of
  =_    +   =;=|`    MERCHANTABILITY or FITNESS FOR A
 _.=:.    :  :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=    =    ;      Public License for more details.
++=  -.   .`   .:
:   = ...= . :.=-        You should have received a copy of the GNU
-.  .:....=;==+<;          General Public License along with this file;
 -_. . .  )=. =           see the file COPYING. If not, write to the
  --    :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "buttonsettings.h"
#include "buttonutils.h"
#include "remapdlg.h"
#include "adddlg.h"

#include <opie2/odevice.h>
#include <opie2/odebug.h>

#include <qpe/applnk.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qscrollview.h>
#include <qpushbutton.h>
#include <qhbox.h>
#include <qobjectlist.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qcopchannel_qws.h>

using namespace Opie::Core;

struct buttoninfo {
    const ODeviceButton *m_button;
    int          m_index;

    OQCopMessage m_pmsg;
    QLabel      *m_picon;
    QLabel      *m_plabel;

    OQCopMessage m_hmsg;
    QLabel      *m_hicon;
    QLabel      *m_hlabel;

    bool        m_pdirty : 1;
    bool        m_hdirty : 1;

    QList<QWidget> *m_widgets;
};


ButtonSettings::ButtonSettings ( QWidget *parent , const char *,  bool, WFlags  )
    : QDialog ( parent, "ButtonSettings", false, WStyle_ContextHelp )
{
    const QValueList <ODeviceButton> &buttons = ODevice::inst()->buttons();
    (void) ButtonUtils::inst(); // initialise

    setCaption ( tr( "Button Settings" ));

    QVBoxLayout *toplay = new QVBoxLayout( this, 3, 3 );
    m_mainview = new QScrollView( this );
    m_mainview->setResizePolicy( QScrollView::AutoOneFit );

    toplay->addWidget( m_mainview );
    m_framehold = new QFrame( m_mainview->viewport() );
    m_framehold->setFrameStyle( QFrame::NoFrame | QFrame::Plain );
    m_mainview->addChild( m_framehold );

    QHBox *buttonbox = new QHBox( this );
    QPushButton *addButton = new QPushButton( tr("Add new"), buttonbox );
    addButton->setFocusPolicy( QWidget::NoFocus );
    QPushButton *removeButton = new QPushButton( tr("Remove"), buttonbox );
    removeButton->setFocusPolicy( QWidget::NoFocus );
    toplay->addWidget( buttonbox );
    connect ( addButton, SIGNAL( pressed()), this, SLOT( slotAdd()));
    connect ( removeButton, SIGNAL( pressed()), this, SLOT( slotRemove()));

    m_scrolllayout = new QVBoxLayout( m_framehold, 3, 3 );

    QLabel *l = new QLabel ( tr( "<center>Press or hold the button you want to remap.</center>" ), m_framehold );
    m_scrolllayout->addWidget ( l );

    m_itemlayout = new QGridLayout ( m_scrolllayout );
    m_itemlayout->setMargin ( 0 );
    m_itemlayout->setColStretch ( 0, 0 );
    m_itemlayout->setColStretch ( 1, 0 );
    m_itemlayout->setColStretch ( 2, 0 );
    m_itemlayout->setColStretch ( 3, 10 );

    m_infos.setAutoDelete ( true );

    m_scrolllayout->addStretch ( 10 );

    for ( QValueList<ODeviceButton>::ConstIterator it = buttons.begin(); it != buttons.end(); it++ ) {
        addButtonItem( *it );
    }

    updateLabels();

    m_last_button = 0;
    m_lock = false;
    m_addWidget = 0;
    m_customChanged = false;

    m_timer = new QTimer ( this );
    connect ( m_timer, SIGNAL( timeout()), this, SLOT( keyTimeout()));

    QPEApplication::grabKeyboard();
}

ButtonSettings::~ButtonSettings()
{
    QPEApplication::ungrabKeyboard();
    if( m_customChanged )
        QCopChannel::send ("QPE/System", "deviceCustomButtonsChanged()" );
    QCopChannel::send ("QPE/System", "deviceButtonMappingChanged()" );
}

void ButtonSettings::addButtonItem( const ODeviceButton &btn )
{
    buttoninfo *bi = new buttoninfo();
    bi->m_button = &btn;
    bi->m_index = m_infos.count();
    bi->m_pmsg = btn.pressedAction();
    bi->m_hmsg = btn.heldAction();
    bi->m_pdirty = false;
    bi->m_hdirty = false;
    bi->m_widgets = new QList<QWidget>();

    int i = ( m_infos.count() * 3 ) + 2;
    if( m_infos.count() > 0 ) {
        QFrame *f = new QFrame ( m_framehold );
        f->setFrameStyle ( QFrame::Sunken | QFrame::VLine );
        m_itemlayout->addMultiCellWidget ( f, i-1, i-1, 0, 3 );
        bi->m_widgets->append(f);
    }

    QLabel *l = new QLabel ( m_framehold );
    l->setPixmap( btn.pixmap() );
    bi->m_widgets->append(l);

    m_itemlayout->addMultiCellWidget ( l, i, i + 1, 0, 0 );

    l = new QLabel ( tr( "Press:" ), m_framehold );
    m_itemlayout->addWidget ( l, i, 1, AlignLeft | AlignBottom );
    bi->m_widgets->append(l);
    l = new QLabel ( tr( "Hold:" ), m_framehold );
    m_itemlayout->addWidget ( l, i + 1, 1, AlignLeft | AlignTop );
    bi->m_widgets->append(l);

    l = new QLabel ( m_framehold );
    l->setFixedSize ( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
    m_itemlayout->addWidget ( l, i, 2, AlignLeft | AlignBottom );
    bi->m_picon = l;
    bi->m_widgets->append(l);

    l = new QLabel ( m_framehold );
    l->setAlignment ( AlignLeft | AlignVCenter | SingleLine );
    m_itemlayout->addWidget ( l, i, 3, AlignLeft | AlignBottom );
    bi->m_plabel = l;
    bi->m_widgets->append(l);

    l = new QLabel ( m_framehold );
    l->setFixedSize ( AppLnk::smallIconSize(), AppLnk::smallIconSize() );
    m_itemlayout->addWidget ( l, i + 1, 2, AlignLeft | AlignTop );
    bi->m_hicon = l;
    bi->m_widgets->append(l);

    l = new QLabel ( m_framehold );
    l->setAlignment ( AlignLeft | AlignVCenter | SingleLine );
    m_itemlayout->addWidget ( l, i + 1, 3, AlignLeft | AlignTop );
    bi->m_hlabel = l;
    bi->m_widgets->append(l);

    m_infos.append ( bi );

    if( m_framehold->isVisible() ) {
        // Ensure all newly added widgets get shown
        for ( QListIterator<QWidget> it( *(bi->m_widgets) ); it.current(); ++it ) {
            (*it)->show();
        }
        qApp->processEvents();
        m_mainview->setContentsPos( 0, m_mainview->contentsHeight() );
    }
}

void ButtonSettings::removeButtonItem( buttoninfo *bi )
{
    for ( QListIterator<QWidget> it ( *(bi->m_widgets) ); *it; ++it ) {
        QWidget *w = (*it);
        delete w;
    }
    m_infos.remove( bi );
}

void ButtonSettings::updateLabels()
{
    for ( QListIterator <buttoninfo> it ( m_infos ); *it; ++it ) {
        qCopInfo cip = ButtonUtils::inst()->messageToInfo ((*it)->m_pmsg );

        QPixmap pic;
        pic.convertFromImage( cip.m_icon.convertToImage().smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
        (*it)->m_picon->setPixmap ( pic );
        (*it)->m_plabel->setText ( cip.m_name );

        qCopInfo cih = ButtonUtils::inst()->messageToInfo ((*it)->m_hmsg );

        pic.convertFromImage( cih.m_icon.convertToImage().smoothScale( AppLnk::smallIconSize(), AppLnk::smallIconSize() ) );
        (*it)->m_hicon->setPixmap ( pic );
        (*it)->m_hlabel->setText ( cih.m_name );
    }
}

buttoninfo *ButtonSettings::buttonInfoForKeycode ( ushort key )
{
    for ( QListIterator <buttoninfo> it ( m_infos ); *it; ++it ) {
        if ((*it)->m_button->keycode() == key )
            return *it;
    }
    return 0;
}

void ButtonSettings::keyPressEvent ( QKeyEvent *e )
{
    buttoninfo *bi = buttonInfoForKeycode ( e->key());

    if ( bi && !e->isAutoRepeat()) {
        m_timer->stop();
        m_last_button = bi;
        m_timer->start ( ODevice::inst()->buttonHoldTime(), true );
    }
    else
        QDialog::keyPressEvent ( e );
}

void ButtonSettings::keyReleaseEvent ( QKeyEvent *e )
{
    buttoninfo *bi = buttonInfoForKeycode ( e->key());

    if ( bi && !e->isAutoRepeat() && m_timer->isActive()) {
        m_timer->stop();
        edit ( bi, false );
    }
    else
        QDialog::keyReleaseEvent ( e );
}

void ButtonSettings::keyTimeout()
{
    if ( m_last_button ) {
        edit ( m_last_button, true );
        m_last_button = false;
    }
}

void ButtonSettings::edit ( buttoninfo *bi, bool hold )
{
    if ( m_lock )
        return;
    m_lock = true;

    RemapDlg *d = new RemapDlg ( bi->m_button, hold, this );

    if ( QPEApplication::execDialog ( d ) == QDialog::Accepted ) {


        if ( hold ) {
            bi->m_hmsg = d->message();
            bi->m_hdirty = true;
        }
        else {
            bi->m_pmsg = d->message();
            bi->m_pdirty = true;
        }

        updateLabels();
    }

    delete d;

    m_lock = false;
}

void ButtonSettings::accept()
{
    for ( QListIterator <buttoninfo> it ( m_infos ); *it; ++it ) {
        buttoninfo *bi = *it;

        if ( bi->m_pdirty )
            ODevice::inst()->remapPressedAction ( bi->m_index, bi->m_pmsg );
        if ( bi->m_hdirty )
            ODevice::inst()->remapHeldAction ( bi->m_index, bi->m_hmsg );
    }
    QDialog::accept();
}

void ButtonSettings::done ( int r )
{
    QDialog::done ( r );
    close();
}

void ButtonSettings::slotAdd()
{
    m_addWidget = new AddDlg( this );
    m_addWidget->setCaption( tr("Add button") );
    connect( m_addWidget, SIGNAL( closed(int) ), this, SLOT( slotAddDlgClosed(int) ) );
    m_addWidget->show();
}

void ButtonSettings::slotAddDlgClosed( int key )
{
    if( key > 0 ) {
        odebug << "**** Add button " << ODevice::keyToString( key ) << oendl;
        delete m_addWidget;
        m_addWidget = 0;

        buttoninfo *bi = buttonInfoForKeycode( key );
        if( bi ) {
            QMessageBox::warning ( this, tr( "Already bound" ), tr( "The pressed button is already in the list." ));
            return;
        }

        // Get a name for the key
        QDialog *dlg = new QDialog( this, 0, true );
        QVBoxLayout *lay = new QVBoxLayout( dlg );
        QLabel *l = new QLabel( tr("Please enter a name for this button"), dlg );
        lay->addWidget(l);
        QLineEdit *le = new QLineEdit( dlg );
        lay->addWidget(le);
        lay->addStretch(1);
        dlg->setGeometry( qApp->desktop()->width() / 8, (qApp->desktop()->height() / 2) - 25,
                 3 * (qApp->desktop()->width() / 4), 50 );
        dlg->setCaption( tr( "Button name" ) );
        dlg->show();
        if( dlg->exec() == QDialog::Accepted ) {
            ODevice::inst()->registerCustomButton( key, le->text(), "" );
            addButtonItem( ODevice::inst()->buttons().last() );
            updateLabels();
            m_customChanged = true;
        }

    }
}

void ButtonSettings::slotRemove()
{
    m_addWidget = new AddDlg( this );
    m_addWidget->setCaption( tr("Remove button") );
    connect( m_addWidget, SIGNAL( closed(int) ), this, SLOT( slotRemoveDlgClosed(int) ) );
    m_addWidget->show();
}

void ButtonSettings::slotRemoveDlgClosed( int key )
{
    if( key > 0 ) {
        odebug << "**** Remove button " << ODevice::keyToString( key ) << oendl;
        delete m_addWidget;
        m_addWidget = 0;

        buttoninfo *bi = buttonInfoForKeycode( key );
        if( bi ) {
            ODevice::inst()->unregisterCustomButton( key );
            removeButtonItem( bi );
            m_customChanged = true;
        }
    }
}

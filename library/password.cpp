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
**********************************************************************/
#include "password.h"
#include "config.h"
#include "global.h"
#include "backend/contact.h"
#include <qlabel.h>
#include <qlineedit.h>
#include <qtextview.h>
#include <qstring.h>
#include <qapplication.h>
#include <qfile.h>
#ifdef QWS
#include <qwindowsystem_qws.h>
#endif

#include <qdialog.h>

#include <unistd.h> //for sleep
#include "passwordbase_p.h"

class PasswordDialog : public PasswordBase
{
    Q_OBJECT

public:
    PasswordDialog( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~PasswordDialog();

    void clear();
    void setPrompt( const QString& );

signals:
    void passwordEntered( const QString& );

protected:
    bool eventFilter( QObject*, QEvent* );
    void keyPressEvent( QKeyEvent * );

private:
    void input( QString );
    friend class Password;
    QString text;
};


extern "C" char *crypt(const char *key, const char *salt);
static QString qcrypt(const QString& k, const char *salt)
{
    return QString::fromUtf8(crypt(k.utf8(),salt));
}

/*
 *  Constructs a PasswordDialog which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'
 */
PasswordDialog::PasswordDialog( QWidget* parent,  const char* name, WFlags fl )
    : PasswordBase( parent, name, fl )
{
    QRect desk = qApp->desktop()->geometry();

    if ( desk.width() < 220 ) {
	QFont f( font() );
	f.setPointSize( 18 );
	setFont( f );
	f.setPointSize( 12 );
	prompt->setFont( f );
    }
    
    button_0->installEventFilter( this );
    button_1->installEventFilter( this );
    button_2->installEventFilter( this );
    button_3->installEventFilter( this );
    button_4->installEventFilter( this );
    button_5->installEventFilter( this );
    button_6->installEventFilter( this );
    button_7->installEventFilter( this );
    button_8->installEventFilter( this );
    button_9->installEventFilter( this );
    button_OK->installEventFilter( this );
    setFocus();
}

/*
 *  Destroys the object and frees any allocated resources
 */
PasswordDialog::~PasswordDialog()
{
    // no need to delete child widgets, Qt does it all for us
}



/*!
  \reimp
*/

bool PasswordDialog::eventFilter( QObject*o, QEvent*e )
{
    if ( e->type() == QEvent::MouseButtonRelease ) {
	if ( o == button_OK ) {
	    emit passwordEntered( text );
	} else {
	    QLabel *l = (QLabel*)o;
	    input(l->text());
	}
    }
    return FALSE;
}


/*!
  \reimp
*/

void PasswordDialog::keyPressEvent( QKeyEvent * )
{
#if 0
    if ( e->key() == Key_Enter || e->key() == Key_Return )
	emit passwordEntered( text );
    else
	input( e->text() );
#endif
}


/*!

*/

void PasswordDialog::input( QString c )
{
    text += c;
    display->setText( text );
}

/*!

*/

void PasswordDialog::setPrompt( const QString& s )
{
    prompt->setText( s );
}

void PasswordDialog::clear()
{
    text = "";
    input("");
}

class PasswdDlg : public QDialog
{
public:
    PasswdDlg( QWidget *parent, const char * name, bool modal, bool fullscreen = FALSE )
	: QDialog( parent, name, modal, fullscreen ? WStyle_NoBorder | WStyle_Customize | WStyle_StaysOnTop : 0 ),
	    modl(modal)
    {
	passw = new PasswordDialog( this );

	if ( fullscreen ) {
	    QRect desk = qApp->desktop()->geometry();
	    setGeometry( 0, 0, desk.width(), desk.height() );
	}

	connect( passw, SIGNAL(passwordEntered(const QString&)),
		 this, SLOT(accept()) );
    }

    void resizeEvent( QResizeEvent * )
    {
	passw->resize( size() );
    }

    void reset()
    {
	passw->clear();
    }

    void execNonModal()
    {
	if ( !modl ) {
	    reset();
	    showFullScreen();
	    do {
		qApp->enter_loop();
	    } while (result()!=1);
	}
    }

    void accept()
    {
	if ( !modl )
	    qApp->exit_loop();
	QDialog::accept();
    }

    PasswordDialog *passw;
    bool modl;
};

class OwnerDlg : public QDialog
{
public:

    OwnerDlg( QWidget *parent, const char * name, Contact c,
	    bool modal, bool fullscreen = FALSE )
	: QDialog( parent, name, modal, 
		fullscreen ? 
		WStyle_NoBorder | WStyle_Customize | WStyle_StaysOnTop : 0 )
    {
	if ( fullscreen ) {
	    QRect desk = qApp->desktop()->geometry();
	    setGeometry( 0, 0, desk.width(), desk.height() );
	}

	// set up contents.
	QString text = "<H1>" + tr("Owner Information") + "</H1>";
	text += c.toRichText();
	tv = new QTextView(this);
	tv->setText(text);

	tv->viewport()->installEventFilter(this);
    }

    void resizeEvent( QResizeEvent * )
    {
	tv->resize( size() );
    }
   
    bool eventFilter(QObject *o, QEvent *e)
    {
	if (e->type() == QEvent::KeyPress || e->type() == QEvent::MouseButtonPress ) {
	    accept();
	    return TRUE;
	}
	return QWidget::eventFilter(o, e);
    }

    void mousePressEvent( QMouseEvent * ) { accept(); }

private:
    QTextView *tv;
};

/*!
  Returns a crypted password entered by the user when prompted with \a prompt
  The returned value is QString::null if the user cancels the operation,
  or the empty string if the user enters no password (but confirms the
  dialog).
*/

QString Password::getPassword( const QString& prompt )
{
    PasswdDlg pd(0,0,TRUE);
    pd.passw->setPrompt( prompt );

    pd.showMaximized();
    int r = pd.exec();

    if ( r == QDialog::Accepted ) {
	if (pd.passw->text.isEmpty())
	    return "";
	else
	    return qcrypt(pd.passw->text,"a0");
    } else {
	return QString::null;
    }
}


/*!
  Prompt, fullscreen, for the user's passcode until they get it right.

  If \a at_poweron is TRUE, the dialog is only used if the user's
  preference request it at poweron; either way, the screen is always repainted
  by this function.  (this functionality may move to the caller of this function).
*/

void Password::authenticate(bool at_poweron)
{
    Config cfg("Security");
    cfg.setGroup("Passcode");
    QString passcode = cfg.readEntry("passcode");
    if ( !passcode.isEmpty()
	    && (!at_poweron || cfg.readNumEntry("passcode_poweron",0)) )
    {
	// Do it as a fullscreen modal dialog
	PasswdDlg pd(0,0,TRUE,TRUE);

	// see if there is contact information.
	OwnerDlg *oi = 0;
	QString vfilename = Global::applicationFileName("addressbook", 
		"businesscard.vcf");
	if (QFile::exists(vfilename)) {
	    Contact c;
	    c = Contact::readVCard( vfilename )[0];

	    oi = new OwnerDlg(0, 0, c, TRUE, TRUE);
	}

	pd.reset();
	pd.exec();
	while (qcrypt(pd.passw->text, "a0") != passcode) {
	    if (oi)
		oi->exec();
	    pd.reset();
	    pd.exec();
	}
    } else if ( at_poweron ) {
	// refresh screen   #### should probably be in caller
	// Not needed (we took away the screen blacking)
	//if ( qwsServer )
	    //qwsServer->refresh();
    }
}

#include "password.moc"

/**
 * \note Taken from opie-security and libqpe password.cpp, and modified for Opie multiauth by Clement Seveillac
 */
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

#include "pin.h"
#include "pinDialogBase.h"
#include "pinConfigWidget.h"
/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oapplication.h>
/* QT */
#include <qpe/config.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qtextview.h>
#include <qstring.h>
#include <qdialog.h>
/* UNIX */
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

extern "C" char *crypt(const char *key, const char *salt);

/// set to TRUE when we press the 'Skip' button
bool isSkip = FALSE;

/// PIN input graphical widget.
/**
 * Inherits the PinDialogBase class defined originally in pinDialogBase.ui interface file.
 * \sa PinDlg and PinDialog (the class generated from the .ui file)
 * It comes from the original PIN locking code in Opie :
 * \sa http://dudu.dyn.2-h.org/opiedoxydoc/library_2password_8cpp-source.html
 */
class PinDialog : public PinDialogBase
{
    Q_OBJECT

    public:
        PinDialog( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
        ~PinDialog();

        void clear();
        void setPrompt( const QString& );

signals:
        /// emitted when we press the Enter button
        void passwordEntered( const QString& );
        /// emitted when we press the Skip button
        void skip();

    protected:
        bool eventFilter( QObject*, QEvent* );

    private:
        void input( QString );
        friend class PinPlugin;
        QString text;
};


/// Constructs a PinDialog widget, and initializes things
PinDialog::PinDialog( QWidget* parent,  const char* name, WFlags fl )
    : PinDialogBase( parent, name, fl )
{
    QRect desk = oApp->desktop()->geometry();

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
    button_Skip->installEventFilter( this );
    button_OK->installEventFilter( this );
    setFocus();
}

/// nothing to do
PinDialog::~PinDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/// Record the pressed numbers, and the Skip and Enter commands
bool PinDialog::eventFilter( QObject*o, QEvent*e )
{
    if ( e->type() == QEvent::MouseButtonRelease ) {
        if ( o == button_OK ) {
            emit passwordEntered( text );
        }
        else if ( o == button_Skip ) {
            isSkip = TRUE;
            emit skip();
        }
        else {
            QLabel *l = (QLabel*)o;
            input(l->text());
        }
    }
    return FALSE;
}

void PinDialog::input( QString c )
{
    text += c;
    display->setText( text );
}

void PinDialog::setPrompt( const QString& s )
{
    prompt->setText( s );
}

void PinDialog::clear()
{
    text = "";
    input("");
}

/// PIN dialog
/**
 * Dialog containing the PinDialog widget (which asks for a PIN) and interfacing with its I/O.
 * \sa PinDialog
 */
class PinDlg : public QDialog
{
    public:
        PinDlg( QWidget *parent, const char * name, bool modal, bool fullscreen = FALSE )
            : QDialog( parent, name, modal, fullscreen ? WStyle_NoBorder | WStyle_Customize | WStyle_StaysOnTop : 0 ),
        modl(modal)
        {
            pinD = new PinDialog( this );

            if ( fullscreen ) {
                QRect desk = oApp->desktop()->geometry();
                setGeometry( 0, 0, desk.width(), desk.height() );
            }

            connect( pinD, SIGNAL(passwordEntered(const QString&)),
                     this, SLOT(accept()) );
            connect( pinD, SIGNAL(skip()), this, SLOT(accept()) );
        }

        void resizeEvent( QResizeEvent * )
        {
            pinD->resize( size() );
        }

        void reset()
        {
            pinD->clear();
        }

        /// Slot receiving the Skip or Enter commands, and closing the QDialog
        void accept()
        {
            if ( !modl )
                oApp->exit_loop();
            QDialog::accept();
        }

        PinDialog *pinD;
        bool modl;
};

/// generate a fairly random salt and return the PIN hashed by crypt()
QString PinPlugin::encrypt(const QString& pin)
{
    // the salt must begin by "$1$" if we want crypt() to use MD5
    char salt[] = "$1$........";
    const char *const seedchars = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    // initialize the random generator
    srandom(time(0));
    int i;
    for(i = 0; i < 8; i++)
    {
        // initialize the salt with random()
        salt[i+3] = seedchars[random() % 64];
    }
    return QString::fromLatin1(crypt(pin.latin1(),salt));
}

/// verify a PIN against its crypt() hash
/**
 * \return true if the \a pin matches its \a hash
 */
bool PinPlugin::verify(const QString& pin, const QString& hash)
{
    // the hash, which contains the salt (8 chars after "$1$"), can be given as the salt
    return hash.compare( QString::fromLatin1(crypt( pin.latin1(), hash.latin1() )) ) == 0 ? true : false;
}

/// Displays a PinDialog and returns the typed in PIN
/**
 * The returned value is QString::null if the user cancels the operation,
 * or the empty string if the user enters no password (but confirms the
 * dialog).
 */
QString PinPlugin::getPIN( const QString& prompt )
{
    PinDlg pd(0,0,TRUE);
    pd.pinD->setPrompt( prompt );
    
    pd.showMaximized();
    int r = pd.exec();
    
    if ( r == QDialog::Accepted ) {
        if (pd.pinD->text.isEmpty())
            return "";
        else
            return pd.pinD->text;
    }
    else
        return QString::null;
}

/// Displays the PIN dialog and returns a hash of the typed in PIN
/**
 * \return the hashed ( =one-way encrypted) PIN typed in by the user
 * \param prompt the prompt to display in the PinDialog
 */
QString PinPlugin::getCryptedPIN( const QString& prompt )
{
    return encrypt(getPIN(prompt));
}

/// Displays the PIN dialog, asks 2 times for a new PIN, saves it if entered two times
/**
 * writes nothing if we enter nothing the first time
 */
void PinPlugin::changePIN()
{
    QString new1, new2;
    do {
        new1 = getPIN(QObject::tr("Enter new PIN"));
        if ( new1.isNull() )
            return;
        new2 = getPIN(QObject::tr("Re-enter new PIN"));
    } while (new1 != new2);

    odebug << "writing new PIN hash in Security.conf" << oendl;
    Config cfg("Security");
    cfg.setGroup("PinPlugin");
    cfg.writeEntry("hashedPIN", encrypt(new1));
}

/// Removes the PIN hashed value in the config file
void PinPlugin::clearPIN()
{
    Config cfg("Security");
    cfg.setGroup("PinPlugin");
    cfg.removeEntry("hashedPIN");
}

/// Prompt, fullscreen, for the user's PIN and compare it to the stored one.
/**
 * \return the result code, as a MultiauthPluginObject::authResult object
 */
int PinPlugin::authenticate()
{
    // reset skip (if we ran Pin two times in a row, skipping the first time, it must be put to 0 again)
    isSkip = FALSE;
    // fetch value in config
    Config cfg("Security");
    cfg.setGroup("PinPlugin");
    QString hashedPin = cfg.readEntry("hashedPIN");
    if (!hashedPin.isEmpty())
    {
        // prompt for the PIN in a fullscreen modal dialog
        PinDlg pd(0,0,TRUE,TRUE);
        pd.reset();
        pd.exec();
        
        // analyse the result
        if (isSkip == TRUE)
            return MultiauthPluginObject::Skip;
        else if (verify(pd.pinD->text, hashedPin))
            return MultiauthPluginObject::Success;
        else 
            return MultiauthPluginObject::Failure;
    }
    owarn << "No PIN has been defined! We consider it as a successful authentication though." << oendl;
    return MultiauthPluginObject::Success;
}

/// Simply returns the plugin name (PIN plugin)
QString PinPlugin::pluginName() const {
    return "PIN Plugin";
}

QString PinPlugin::pixmapNameWidget() const {
    return "security/pinplugin";
}

QString PinPlugin::pixmapNameConfig() const {
    return "security/pinplugin";
}

/// returns a PinConfigWidget
MultiauthConfigWidget * PinPlugin::configWidget(QWidget * parent) {
    PinConfigWidget * pinw = new PinConfigWidget(parent, "PIN configuration widget");
    
    connect(pinw->changePIN, SIGNAL( clicked() ), this, SLOT( changePIN() ));
    connect(pinw->clearPIN, SIGNAL( clicked() ), this, SLOT( clearPIN() ));
    
    return pinw;
}

#include "pin.moc"


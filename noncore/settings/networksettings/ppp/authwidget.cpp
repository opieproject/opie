#include "auth.h"
#include "authwidget.h"
#include "edit.h"
#include "pppdata.h"

/* OPIE */
#include <opie2/odebug.h>
using namespace Opie::Core;

/* QT */
#include <qlayout.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>

/* XPM */
static const char* const image0_data[] = {
"16 16 2 1",
". c None",
"# c #000000",
"................",
"...#...###...##.",
"..#.#..#..#.##..",
"..###..###.##...",
".#...#.#..##....",
".#...#.#.##.....",
"........##.#..#.",
"..##...##...##..",
".#..#.###...##..",
".#...##..#.#..#.",
".#..##..........",
".#.##.#..#.#..#.",
"..##...##...##..",
".##....##...##..",
".#....#..#.#..#.",
"................"};


AuthWidget::AuthWidget(PPPData *pd, QWidget *parent, bool isnewaccount, const char *name )
    : QWidget( parent, name),
      scriptWidget(0),
      _pppdata(pd),
      isNewAccount(isnewaccount)
{
    layout = new QGridLayout(this);

    auth_l = new QLabel(tr("Authentication: "), this);
    layout->addWidget(auth_l, 0, 0);

    auth = new QComboBox(this);
    auth->insertItem(tr("Script-based"));
    auth->insertItem(tr("PAP"));
    auth->insertItem(tr("Terminal-based"));
    auth->insertItem(tr("CHAP"));
    auth->insertItem(tr("PAP/CHAP"));
    layout->addWidget(auth, 0, 1);

    connect( auth, SIGNAL(activated(const QString&)),
             SLOT(authChanged(const QString&)));

    QString tmp = tr("<p>Specifies the method used to identify yourself to\n"
	     "the PPP server. Most universities still use\n"
	     "<b>Terminal</b>- or <b>Script</b>-based authentication,\n"
	     "while most ISP use <b>PAP</b> and/or <b>CHAP</b>. If\n"
	     "unsure, contact your ISP.\n"
	     "\n"
	     "If you can choose between PAP and CHAP,\n"
	     "choose CHAP, because it's much safer. If you don't know\n"
	     "whether PAP or CHAP is right, choose PAP/CHAP.");

    QWhatsThis::add(auth_l,tmp);
    QWhatsThis::add(auth,tmp);

    user_l = new QLabel( tr("Username: "), this);
    layout->addWidget( user_l, 1, 0 );
    userName = new QLineEdit( this, "usernameEdit" );
    layout->addWidget( userName, 1, 1 );
    tmp = tr("Enter your username here...");
    QWhatsThis::add( user_l, tmp );
    QWhatsThis::add( userName, tmp );

    pw_l = new QLabel( tr("Password: "), this);
    layout->addWidget( pw_l, 2, 0 );
    passWord = new QLineEdit( this, "pw" );
    passWord->setAutoMask( true );
    passWord->setEchoMode( QLineEdit::Password );
    layout->addWidget( passWord, 2, 1 );
    hidePw = new QToolButton( this );
    hidePw->setPixmap( QPixmap( ( const char** ) image0_data ) );
    hidePw->setToggleButton( true );
    layout->addWidget( hidePw, 2, 2 );

    connect(hidePw, SIGNAL(toggled(bool)), SLOT(toggleEchoMode(bool)));

    tmp = tr("Enter your password here");
    QWhatsThis::add( pw_l, tmp );
    QWhatsThis::add( passWord, tmp );

    store_password = new QCheckBox(tr("Store password"), this);
    layout->addMultiCellWidget(store_password, 3, 3, 0, 1, AlignRight);
    QWhatsThis::add(store_password,
		  tr("<p>When this is turned on, your ISP password\n"
		       "will be saved in <i>kppp</i>'s config file, so\n"
		       "you do not need to type it in every time.\n"
		       "\n"
		       "<b><font color=\"red\">Warning:</font> your password will be stored as\n"
		       "plain text in the config file, which is\n"
		       "readable only to you. Make sure nobody\n"
		       "gains access to this file!"));

    if (isNewAccount){
        // select PAP/CHAP as default
        auth->setCurrentItem(AUTH_PAPCHAP);
        store_password->setChecked(true);
    }else{
        auth->setCurrentItem(_pppdata->authMethod());
        authChanged( auth->currentText() );
        userName->setText( _pppdata->storedUsername() );
        store_password->setChecked(_pppdata->storePassword());
        if (store_password->isChecked())
            passWord->setText( _pppdata->storedPassword() );
    }
}

bool AuthWidget::check()
{
    bool ret = true;
    if (scriptWidget){
        if (!scriptWidget->check()){
            QMessageBox::critical(this, tr("error"), tr("<qt>Login script has unbalanced loop Start/End<qt>"));
            ret = false;
        }
    }
    return ret;
}

void AuthWidget::save()
{
    _pppdata->setAuthMethod(auth->currentItem());
    if (scriptWidget) scriptWidget->save();
    _pppdata->setStoredUsername( userName->text() );
    _pppdata->setStorePassword(store_password->isChecked());
    if (store_password->isChecked())
        _pppdata->setStoredPassword( passWord->text() );
}

void AuthWidget::authChanged( const QString &authStr )
{
    odebug << "AuthWidget::authChanged( " << authStr.latin1() << " )" << oendl; 
    if ( authStr.contains( tr("Script-based") ) ){
        showUsernamePassword( false );
        showScriptWindow( true );
    } else if ( authStr.contains( tr("PAP") ) ||
                authStr.contains( tr("CHAP") ) ){
        showUsernamePassword( true );
        showScriptWindow( false );
    } else {
        odebug << "do not really know how to handle" << oendl; 
        showUsernamePassword( false );
        showScriptWindow( false );
    }
}


void AuthWidget::showUsernamePassword( bool show )
{
    if (show){
        user_l->show();
        userName->show();
        pw_l->show();
        passWord->show();
        store_password->show();
        hidePw->show();
    }else{//!show
        user_l->hide();
        userName->hide();
        pw_l->hide();
        passWord->hide();
        store_password->hide();
        hidePw->hide();
    }
}

void AuthWidget::showScriptWindow( bool show )
{
    if (show){
        if (!scriptWidget){
            scriptWidget = new ScriptWidget( _pppdata, this, isNewAccount, "scriptWid");
            layout->addMultiCellWidget( scriptWidget, 1, 4, 0, 1 );
        }
        scriptWidget->show();
    }else{ // !show
        if (scriptWidget) scriptWidget->hide();
    }
}

void AuthWidget::toggleEchoMode( bool t )
{
    passWord->setEchoMode( t ? QLineEdit::Normal : QLineEdit::Password );
}


/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2004 Holger Hans Peter Freyther <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qhbox.h>
#include <qtoolbutton.h>



#include <sys/types.h>
#include <pwd.h>
#include <shadow.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

// Shitty gcc2 toolchain
extern "C" char* crypt( const char*, const char* );



#include "passworddialogimpl.h"


// This function is taken from 'busybox'.
static int i64c(int i) {
    if (i <= 0)
        return ('.');
    if (i == 1)
        return ('/');
    if (i >= 2 && i < 12)
        return ('0' - 2 + i);
    if (i >= 12 && i < 38)
        return ('A' - 12 + i);
    if (i >= 38 && i < 63)
        return ('a' - 38 + i);
    return ('z');
}

// This function is taken from 'busybox'.
static char *crypt_make_salt() {
    time_t now;
    static unsigned long x;
    static char result[3];

    ::time(&now);
    x += now + getpid() + clock();
    result[0] = i64c(((x >> 18) ^ (x >> 6)) & 077);
    result[1] = i64c(((x >> 12) ^ x) & 077);
    result[2] = '\0';
    return result;
}

/*
 * Modal dialog to force root password. It is quite hard as it only leave
 * when a password is set.
 * Also it prevalidates the password...
 */
PasswordDialogImpl::PasswordDialogImpl( QWidget* parent )
    : PasswordDialog( parent, 0, true ), m_isSet( PasswordDialogImpl::needDialog() ) {
}

PasswordDialogImpl::~PasswordDialogImpl() {
    /* qt does the stuff for us */
}

void PasswordDialogImpl::done(int res) {
    m_isSet = true;

    /*
     * The user hit 'Ok' see if we can safe the file
     * if not an error will be raised and m_isSet altered.
     * On cancel we will see if it is now ok...
     */
    if ( res == Accepted )
        writePassword();
    else if(PasswordDialogImpl::needDialog() ) {
        switch( QMessageBox::warning(this,tr("Trying to leave without password set") ,
                                     tr("<qt>No password was set. This could lead to you not beeing"
                                        "able to remotely connect to your machine."
                                        "Do you want to continue not setting a password?</qt>" ),
                                     QMessageBox::Ok, QMessageBox::Cancel ) ) {
        case QMessageBox::Cancel:
            m_isSet = false;
            break;
        case  QMessageBox::Ok:
        default:
            break;
        }

    }

    if(m_isSet)
        PasswordDialog::done( res );
}

/*
 * Lets see if we can write either shadow
 *
 */
/**
 * CRYPT the password and then tries to write it either to the shadow password
 * or  to the plain /etc/passwd
 */
void PasswordDialogImpl::writePassword() {
    /*
     * Check if both texts are the same
     */
    if ( m_pass->text() != m_confirm->text() )
        return error( tr("Passwords don't match"),
                      tr("<qt>The two passwords don't match. Please try again.</qt>") );


    /*
     * Now crypt the password so we can write it later
     */
    char* password = ::crypt( m_pass->text().latin1(), crypt_make_salt() );

    if ( !password )
        return error( tr("Password not legal" ),
                      tr("<qt>The entered password is not a valid password."
                         "Please try entering a valid password.</qt>" ) );

    /* rewind and rewrite the password file */
    ::setpwent();

    FILE* file = ::fopen( "/etc/passwd.new", "w" );
    struct passwd* pass;
    while ( (pass = ::getpwent()) != 0l ) {
        /* no  shadow password support */
        if ( pass->pw_uid == 0  )
            pass->pw_passwd = password;

        ::putpwent( pass, file );
    }

    ::fclose( file );
    ::endpwent();
    ::rename("/etc/passwd.new","/etc/passwd" );

    /* should be done now */
#ifdef  OPIE_LOGIN_SHADOW_PW
    #error "Can't write Shadow Passwords fixme"
#endif
}

/**
 * Raise an error. Delete input and set the focus after showing
 * the error to the user
 */
void PasswordDialogImpl::error( const QString& caption, const QString& text ) {
    m_isSet = false;
    QMessageBox::critical(this,caption, text,
                          QMessageBox::Ok, QMessageBox::NoButton );

    m_pass->setText("");
    m_pass->setFocus();

    m_confirm->setText("");
}

void PasswordDialogImpl::slotToggleEcho( bool b ) {
    m_pass->   setEchoMode( b ? QLineEdit::Normal : QLineEdit::Password );
    m_confirm->setEchoMode( b ? QLineEdit::Normal : QLineEdit::Password );
}

/////////////////////////
/// static functions
///

/**
 * Ask whether or not we need to show the dialog. It returns true if
 * no root password is set so that the user will be able to set one.
 */
bool PasswordDialogImpl::needDialog() {
   /*
    * This can cope with no password and shadow passwords
    * Let us go through the user database until we find  'root' and then
    * see if it is 'shadow' and see if shadow is empty or see if the password is empty
    */
    bool need = false;
    struct passwd *pwd;
    ::setpwent();

    while((pwd = ::getpwent() ) ) {
        /* found root */
        if( pwd->pw_uid == 0 ) {
            QString str = QString::fromLatin1(pwd->pw_passwd );

            /*
             * If str is really empty it is passwordless anyway... or '*' is a hint to set one
             * on OE/Familiar
             * else it is shadow based
             */
            if(str.isEmpty() || str == '*' )
                need = true;
            else if ( str == 'x' )
#ifdef OPIE_LOGIN_SHADOW_PW
                need =  QString::fromLatin1( ::getspnam( pwd->pw_name )->sp_pwdp ).isEmpty();
#else
            ;
#endif
            break;
        }
    }
    ::endpwent();

    return need;
}

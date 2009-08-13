#include "namelineedit.h"

/* OPIE */
#include <opie2/odebug.h>

#include <qpe/config.h>

namespace ABOOK {
NameLineEdit::NameLineEdit( QWidget* parent, const char* name )
    : QLineEdit( parent, name ), m_prevSpace( true ), m_disabled( false ) {
    configReader();
}

NameLineEdit::NameLineEdit( const QString& str, QWidget* par,
                            const char* name )
    : QLineEdit( str, par, name ),m_prevSpace( true ), m_disabled( false ) {
    configReader();
}

NameLineEdit::~NameLineEdit() {
}

void NameLineEdit::keyPressEvent( QKeyEvent* ev ) {

    // If disabled: Push everything to the mother class..
    if ( m_disabled ){
        QLineEdit::keyPressEvent( ev );
        return;
    }

    QString t = ev->text();
    int key = ev->key();
    int ascii = ev->ascii();

    // ### FIXME with composed events
    if ( !t.isEmpty() && ( !ev->ascii() || ev->ascii()>=32 ) &&
         key != Key_Delete && key != Key_Backspace &&
         key != Key_Return && key != Key_Enter ) {
        odebug << "str " << ev->text() << " " << m_prevSpace << oendl;

        if ( m_prevSpace ) {
            t = t.upper();
            m_prevSpace = false;
        }
        if ( key == Key_Space )
            m_prevSpace = true;


        QKeyEvent nEv(ev->type(), key, ascii, ev->state(),
                      t, ev->isAutoRepeat(), ev->count() );
        QLineEdit::keyPressEvent( &nEv );
        if ( !nEv.isAccepted() )
            ev->ignore();
    }
    else {
        QLineEdit::keyPressEvent( ev );
        /* if key was a backspace lets see if we should
         * capitalize the next letter
         */
        if ( key == Key_Backspace ) {
            QString te = text();
            odebug << "Backspace: " << te << oendl;
            /* Capitalize first letter if a char is removed and:
             * 1. String is empty
             * 2. We are at the beginning of the line (pos 0)
             * 3. The char left from current cursor position is a space !
             */
            if ( te.isEmpty() || ( cursorPosition() == 0 ) || ( te[cursorPosition() - 1] == Key_Space ) )
            m_prevSpace = true;
        }
    }
}

void NameLineEdit::configReader() {
    Config cfg("AddressBook");
    cfg.setGroup("Editor");
    m_disabled = cfg.readBoolEntry( "disableAutoCaps", false );
}


}

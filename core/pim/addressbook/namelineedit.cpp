#include "namelineedit.h"

namespace ABOOK {
NameLineEdit::NameLineEdit( QWidget* parent, const char* name )
    : QLineEdit( parent, name ), m_prevSpace( true ) {
}

NameLineEdit::NameLineEdit( const QString& str, QWidget* par,
                            const char* name )
    : QLineEdit( str, par, name ),m_prevSpace( true ) {
}

NameLineEdit::~NameLineEdit() {
}

void NameLineEdit::keyPressEvent( QKeyEvent* ev ) {
    QString t = ev->text();
    int key = ev->key();
    int ascii = ev->ascii();

    // ### FIXME with composed events
    if ( !t.isEmpty() && ( !ev->ascii() || ev->ascii()>=32 ) &&
         key != Key_Delete && key != Key_Backspace &&
         key != Key_Return && key != Key_Enter ) {
        qWarning( "str " + ev->text() + " %d", m_prevSpace );

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
    }else {
        QLineEdit::keyPressEvent( ev );
        /* if key was a backspace lets see if we should
         * capitalize the next letter
         */
        if ( key == Key_Backspace ) {
            QString te = text();
            /*  if string is empty capitalize the first letter */
            /*  else see if we're at the end of the string     */
            if ( te.isEmpty() || cursorPosition() == te.length() )
                m_prevSpace = true;
        }
    }
}

}

#include "mainwindow.h"
#include "textviewshow.h"

using namespace Todo;

TextViewShow::TextViewShow( QWidget* parent, MainWindow* win)
    : QTextView( parent ), TodoShow(win) {

}
TextViewShow::~TextViewShow() {
}
QString TextViewShow::type()const {
    return QString::fromLatin1("TextViewShow");
}
void TextViewShow::slotShow( const OTodo& ev ) {
    setText( ev.toRichText() );
}
QWidget* TextViewShow::widget() {
    return this;
}
void TextViewShow::keyPressEvent( QKeyEvent* event ) {
    switch( event->key() ) {
    case Qt::Key_F33:
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Space:
        escapeView();
        break;
    default:
        QTextView::keyPressEvent( event );
        break;
    }
}

#include "textviewshow.h"

using namespace Todo;

TextViewShow::TextViewShow( QWidget* parent)
    : QTextView( parent ), TodoShow() {

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

#include "keyview.h"
#include <qgrid.h>
#include <iostream.h>
#include <qlineedit.h>
#include <qlabel.h>

Keyview::Keyview( QWidget* parent,  const char* name, WFlags fl )
    : QGrid ( 2, parent, name, fl )
{
    setCaption( tr("Keyview") );
    setSpacing(3);
    setMargin(4);

    QLabel *l;

    l = new QLabel(QString("unicode:"), this);
    unicode = new QLineEdit(this);
    unicode->setReadOnly(1);

    l = new QLabel(QString("keycode:"), this);
    keycode = new QLineEdit(this);
    keycode->setReadOnly(1);

    l = new QLabel(QString("modifiers:"), this);
    modifiers = new QLineEdit(this);
    modifiers->setReadOnly(1);

    l = new QLabel(QString("isPress:"), this);
    isPress = new QLineEdit(this);
    isPress->setReadOnly(1);
    
    l = new QLabel(QString("autoRepeat:"), this);
    autoRepeat = new QLineEdit(this);
    autoRepeat->setReadOnly(1);

    // spacer
    l = new QLabel(QString(""), this);
    l->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));


    KeyFilter *filter = new KeyFilter(this);
    QWSServer::setKeyboardFilter(filter);

    connect(filter, SIGNAL(keyPressed(int, int, int, bool, bool)), 
                           this, SLOT(updateItems(int, int, int, bool, bool)));
}

Keyview::~Keyview() { }

void Keyview::updateItems(int u, int k, int m, bool p, bool a) {

    unicode->setText("0x" + QString::number(u, 16));
    keycode->setText("0x" + QString::number(k, 16));
    modifiers->setText("0x" + QString::number(m, 16));
    isPress->setText("0x" + QString::number(p, 16));
    autoRepeat->setText("0x" + QString::number(a, 16));
}

KeyFilter::KeyFilter(QObject * parent, const char *name) : QObject( parent, name ) {

}

bool KeyFilter::filter(int unicode, int keycode, int modifiers, bool isPress,
                       bool autoRepeat) {

    emit keyPressed(unicode, keycode, modifiers, isPress, autoRepeat);
    return 0; // return 1 to stop key emiting

}

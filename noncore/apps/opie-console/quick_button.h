#ifndef QUICK_BUTTON_H
#define QUICK_BUTTON_H

#include <qframe.h>

class QuickButton : public QFrame {

    Q_OBJECT

public:
    QuickButton( QWidget *parent = 0 );
    ~QuickButton();

 signals:

    void keyPressed( ushort, ushort, bool, bool, bool );

private slots:
    void hitEnter();
    void hitSpace();
    void hitTab();
    void hitPaste();
    void hitUp();
    void hitDown();
};

#endif

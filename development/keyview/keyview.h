#ifndef KEYVIEW_H
#define KEYVIEW_H
#include <qgrid.h>
#include <qlineedit.h>
#include <qwindowsystem_qws.h>


class Keyview : public QGrid
{ 
    Q_OBJECT

public:
    Keyview( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~Keyview();

private slots:
    void updateItems(int, int, int, bool, bool);

private:

    QLineEdit *unicode;
    QLineEdit *keycode;
    QLineEdit *modifiers;
    QLineEdit *isPress;
    QLineEdit *autoRepeat;

};

class KeyFilter : public QObject, public QWSServer::KeyboardFilter
{
    Q_OBJECT

public:
    KeyFilter( QObject* parent, const char* name = 0);
    virtual bool filter(int unicode, int keycode, int modifiers, bool isPress,
                bool autoRepeat);

signals:
    void keyPressed(int, int, int, bool, bool);

};

#endif

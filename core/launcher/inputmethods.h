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

#ifndef __INPUT_METHODS_H__
#define __INPUT_METHODS_H__

#include <qtopia/qlibrary.h>
#include <qtopia/inputmethodinterface.h>

#include <qwidget.h>
#include <qvaluelist.h>

class QToolButton;
class QWidgetStack;

struct InputMethod
{
#ifndef QT_NO_COMPONENT
    QLibrary *library;
#endif
    QWidget *widget;
    QString libName;
    bool newIM;
    union {
	InputMethodInterface *interface;
	ExtInputMethodInterface *extInterface;
    };

    inline void releaseInterface() {
        newIM ? (void)extInterface->release() : (void)interface->release();
        library->unload();
        delete library; library = 0l;
    }
    inline QString name() const { return newIM ? extInterface->name() : interface->name(); }
    inline QPixmap *icon() const { return newIM ? extInterface->icon() : interface->icon(); }
    inline QUnknownInterface *iface() { return newIM ? (QUnknownInterface *)extInterface : (QUnknownInterface *)interface; }
    inline void resetState() { if ( !newIM ) interface->resetState(); }

    int operator <(const InputMethod& o) const;
    int operator >(const InputMethod& o) const;
    int operator <=(const InputMethod& o) const;
};

class InputMethods : public QWidget
{
    Q_OBJECT
public:
    InputMethods( QWidget * );
    ~InputMethods();

    QRect inputRect() const;
    bool shown() const;
    QString currentShown() const; // name of interface
    void showInputMethod(const QString& id);
    void showInputMethod();
    void hideInputMethod();
    void unloadInputMethods();
    void loadInputMethods();

signals:
    void inputToggled( bool on );

private slots:
    void chooseKbd();
    void chooseIm();
    void showKbd( bool );
    void resetStates();
    void sendKey( ushort unicode, ushort scancode, ushort modifiers, bool, bool );
    void qcopReceive( const QCString &msg, const QByteArray &data );

private:
    void setPreferedHandlers();
    /*static */QStringList plugins()const;
    /*static */void installTranslator( const QString& );
    void unloadMethod( QValueList<InputMethod>& );
    void chooseMethod(InputMethod* im);
    void chooseKeyboard(InputMethod* im);
    void updateKeyboards(InputMethod *im);

private:
    QToolButton *kbdButton;
    QToolButton *kbdChoice;
    QWidgetStack *imButton; // later will be widget stack
    QToolButton *imChoice;
    InputMethod *mkeyboard;
    InputMethod *imethod;
    QValueList<InputMethod> inputMethodList;
    QValueList<InputMethod> inputModifierList;
    int inputWidgetStyle;
    int inputWidgetWidth;
};


#endif // __INPUT_METHODS_H__


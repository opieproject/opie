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

#ifndef INPUTMETHODINTERFACE_H
#define INPUTMETHODINTERFACE_H

#include <qpe/qcom.h>

#include <qnamespace.h>
#include <qstring.h>

#ifndef QT_NO_COMPONENT
// {637A8A14-AF98-41DA-969A-2BD16ECDA8C7}
# ifndef IID_InputMethod
#  define IID_InputMethod QUuid( 0x637a8a14, 0xaf98, 0x41da, 0x96, 0x9a, 0x2b, 0xd1, 0x6e, 0xcd, 0xa8, 0xc7)
# endif
#endif

class QWidget;
class QPixmap;
class QObject;
class QWSInputMethod;

struct InputMethodInterface : public QUnknownInterface
{
    virtual QWidget *inputMethod( QWidget *parent, Qt::WFlags f ) = 0;
    virtual void resetState() = 0;
    virtual QPixmap *icon() = 0;
    virtual QString name() = 0;
    virtual void onKeyPress( QObject *receiver, const char *slot ) = 0;
};

// {70F0991C-8282-4625-A279-BD9D7D959FF6} 
#ifndef IID_ExtInputMethod
#define IID_ExtInputMethod QUuid( 0x70f0991c, 0x8282, 0x4625, 0xa2, 0x79, 0xbd, 0x9d, 0x7d, 0x95, 0x9f, 0xf6)
#endif

struct ExtInputMethodInterface : public QUnknownInterface
{
    virtual QWSInputMethod *inputMethod() = 0;
    virtual QPixmap *icon() = 0;
    virtual QString name() = 0;
    virtual QWidget *widget( QWidget *parent, Qt::WFlags f )= 0;
};


#endif

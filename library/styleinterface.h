/**********************************************************************
** Copyright (C) 2002 Robert Griebl.  All rights reserved.
**
** This file is part of OPIE (http://www.opie.info).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef STYLEINTERFACE_H
#define STYLEINTERFACE_H

#include <qpe/qcom.h>

#ifndef QT_NO_COMPONENT

// {17AF792C-E461-49A9-9B71-068B9A8DDAE4}
#ifndef IID_Style
#define IID_Style QUuid( 0x17af792c, 0xe461, 0x49a9, 0x9b, 0x71, 0x06, 0x8b, 0x9a, 0x8d, 0xda, 0xe4)
#endif

// {6C33B4F9-D529-453A-8FB3-DA42B21872BD}  
# ifndef IID_StyleExtended
#  define IID_StyleExtended QUuid( 0x6c33b4f9, 0xd529, 0x453a, 0x8f, 0xb3, 0xda, 0x42, 0xb2, 0x18, 0x72, 0xbd) 
#endif

#endif

class QWidget;
class QStyle;


class QStyle;

struct StyleInterface : public QUnknownInterface
{
	//! Return a new style.
	virtual QStyle *style() = 0;
	//! Return a short name for the style.
	virtual QString name() const = 0;
};

struct StyleExtendedInterface : public StyleInterface
{
	//! Return a (longer) description for the style.
	virtual QString description ( ) = 0;

	//! Does this style support customization
	virtual bool hasSettings ( ) const = 0;
	
	//! Return a new settings page.
	virtual QWidget *create ( QWidget *parent, const char *name = 0 ) = 0;
	
	//! Callback for appearance app when OK is clicked (return true when style has to re-applied).
	virtual bool accept ( ) = 0;
	
	//! Callback for appeaeance app when Cancel is clicked.
	virtual void reject ( ) = 0;
};

#endif

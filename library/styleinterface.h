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

// {6C33B4F9-D529-453A-8FB3-DA42B21872BD}  
# ifndef IID_Style 
#  define IID_Style QUuid( 0x6c33b4f9, 0xd529, 0x453a, 0x8f, 0xb3, 0xda, 0x42, 0xb2, 0x18, 0x72, 0xbd) 
#endif

// {9757A252-3FD4-438F-A756-80BE4A9FB8DC}
# ifndef IID_StyleSettings
#  define IID_StyleSettings QUuid( 0x9757a252, 0x3fd4, 0x438f, 0xa7, 0x56, 0x80, 0xbe, 0x4a, 0x9f, 0xb8, 0xdc)
# endif

#endif

class QWidget;
class QStyle;

struct StyleInterface : public QUnknownInterface
{
	//! Return a new style.
	virtual QStyle *create ( ) = 0;
	
	//! Return a (longer) description for the style.
	virtual QString description ( ) = 0;

	//! Return a short name for the style.
	virtual QString name ( ) = 0;
	
	//! Return the library basename (libliquid.so => liquid)
	virtual QCString key ( ) = 0;
	
	//! QT_VERSION like 1.2.3 == 123
	virtual unsigned int version ( ) = 0;
};

struct StyleSettingsInterface : public QUnknownInterface
{
	//! Return a new settings page.
	virtual QWidget *create ( QWidget *parent, const char *name = 0 ) = 0;
	
	//! Callback for appearance app when OK is clicked (return true when style has to re-applied).
	virtual bool accept ( ) = 0;
	
	//! Callback for appeaeance app when Cancel is clicked.
	virtual void reject ( ) = 0;
};

#endif

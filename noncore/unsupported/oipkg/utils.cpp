/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// (c) 2002 Patrick S. Vogt <tille@handhelds.org>

#include <qcombobox.h>
#include <qstring.h>
#include "utils.h"

void setComboName( QComboBox* combo, QString s)
{
  for ( int i = 0; i < combo->count(); i++)
      if ( combo->text( i ) == s )
				combo->setCurrentItem( i );
}

/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Robert Griebl <sandman@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This file is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This file is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU General
..}^=.=       =       ;      Public License for more details.
++=   -.     .`     .:       
 :     =  ...= . :.=-        You should have received a copy of the GNU
 -.   .:....=;==+<;          General Public License along with this file;
  -_. . .   )=.  =           see the file COPYING. If not, write to the
    --        :-=`           Free Software Foundation, Inc.,
                             59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/
#ifndef __TABCONFIG_H__
#define __TABCONFIG_H__

struct TabConfig {
	enum ViewMode {	
		Icon,
		List
	};
	enum BackgroundType {
		Ruled,
		SolidColor,
		Image
	};

	ViewMode       m_view;
	BackgroundType m_bg_type;
	QString        m_bg_image;
	QString        m_bg_color;
	QString        m_text_color;
	bool           m_font_use;
	QString        m_font_family;
	int            m_font_size;
	int            m_font_weight;
	bool           m_font_italic;
	bool           m_changed;
	
	bool operator == ( const TabConfig &tc )
	{
		return ( m_view == tc. m_view ) && 
		       ( m_bg_type == tc. m_bg_type ) &&
		       ( m_bg_image == tc. m_bg_image ) &&
		       ( m_bg_color == tc. m_bg_color ) &&
		       ( m_text_color == tc. m_text_color ) &&
		       ( m_font_use == tc. m_font_use ) && 
		       ( m_font_use ? (
		         ( m_font_family == tc. m_font_family ) &&
		         ( m_font_size == tc. m_font_size ) &&
		         ( m_font_weight == tc. m_font_weight ) &&
		         ( m_font_italic == tc. m_font_italic )
		        ) : true );
		        
	}
};

#endif

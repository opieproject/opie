/* This file is part of the KDE project
   Copyright (C) 2002 Simon Hausmann <hausmann@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "memfile.h"

#include <unistd.h>
#include <sys/mman.h>

MemFile::MemFile()
{
}

MemFile::MemFile( const QString &name )
    : QFile( name )
{
}

MemFile::~MemFile()
{
    close();
}

void MemFile::close()
{
    unmap();
    QFile::close();
}

void MemFile::unmap()
{
#if defined(Q_WS_X11) || defined(Q_WS_QWS)
    if ( m_data.data() )
    {
	munmap( m_data.data(), m_data.size() );
	m_data.resetRawData( m_data.data(), m_data.size() );
    }
#endif
}

QByteArray &MemFile::data()
{
    if ( !m_data.data() )
    {
#if defined(Q_WS_X11) || defined(Q_WS_QWS)
	const char *rawData = (const char *)mmap( 0, size(), PROT_READ,
		                                  MAP_SHARED, handle(), 0 );
	if ( rawData )
	{
	    m_data.setRawData( rawData, size() );
	    return m_data;
	}
	else
	    qDebug( "MemFile: mmap() failed!" );
	// fallback
#endif
	m_data = readAll();
    }
    return m_data;
}


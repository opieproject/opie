/*
                             This file is part of the Opie Project

                             (C) 2003 Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>
                             Inspired by the config classes from the KDE Project which are
              =.             (C) 1997 Matthias Kalle Dalheimer <kalle@kde.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#ifndef OCONFIG_H
#define OCONFIG_H

//FIXME: Implement for X11 or reuse libqpe/Config there also?
//FIXME: Or rather use QSettings also for libqpe?

#include "opieconfig.h"

class QColor;
class QFont;

/**
  * A Configuration class based on the Qtopia @ref Config class
  * featuring additional handling of color and font entries
  */

class OConfig : public OpieConfig
{
  public:
    /**
     * Constructs a OConfig object with a @a name.
     */
    OConfig( const QString &name, Domain domain = User );
    /**
     * Destructs the OConfig object.
     *
     * Writes back any dirty configuration entries, and destroys
     * dynamically created objects.
     */
    virtual ~OConfig();
    /**
     * @returns the name of the current group.
     * The current group is used for searching keys and accessing entries.
     */
    const QString& group() { return OpieConfig::group(); };
    /**
     * @returns a @ref QColor entry or a @a default value if the key is not found.
     */
    QColor readColorEntry( const QString& key, const QColor* pDefault ) const;
    /**
     * @returns a @ref QFont value or a @a default value if the key is not found.
     */
    QFont readFontEntry( const QString& key, const QFont* pDefault ) const;
};

/**
 * @brief Helper class for easier use of OConfig groups.
 *
 * Careful programmers always set the group of a
 * @ref OConfig object to the group they want to read from
 * and set it back to the old one of afterwards. This is usually
 * written as:
 * <pre>
 *
 * QString oldgroup config()->group();
 * config()->setGroup( "TheGroupThatIWant" );
 * ...
 * config()->writeEntry( "Blah", "Blubb" );
 *
 * config()->setGroup( oldgroup );
 * </pre>
 *
 * In order to facilitate this task, you can use
 * OConfigGroupSaver. Simply construct such an object ON THE STACK
 * when you want to switch to a new group. Then, when the object goes
 * out of scope, the group will automatically be restored. If you
 * want to use several different groups within a function or method,
 * you can still use OConfigGroupSaver: Simply enclose all work with
 * one group (including the creation of the OConfigGroupSaver object)
 * in one block.
 *
 * @author Matthias Kalle Dalheimer <Kalle@kde.org>
 * @version $Id: oconfig.h,v 1.4 2003-09-25 18:11:23 mickeyl Exp $
 * @see OConfig
 */

class OConfigGroupSaver
{
  public:
    /**
     * Constructor.
     * Create the object giving a @config object and a @a group to become
     * the current group.
     */
    OConfigGroupSaver( OConfig* config, QString group ) :_config(config), _oldgroup(config->group() )
        { _config->setGroup( group ); }

    OConfigGroupSaver( OConfig* config, const char *group ) :_config(config), _oldgroup(config->group())
        { _config->setGroup( group ); }

    OConfigGroupSaver( OConfig* config, const QCString &group ) : _config(config), _oldgroup(config->group())
        { _config->setGroup( group ); }
    /**
     * Destructor.
     * Restores the last current group.
     */
    ~OConfigGroupSaver() { _config->setGroup( _oldgroup ); }

    OConfig* config() { return _config; };

  private:
    OConfig* _config;
    QString _oldgroup;

    OConfigGroupSaver( const OConfigGroupSaver& );
    OConfigGroupSaver& operator=( const OConfigGroupSaver& );
};

#endif // OCONFIG_H

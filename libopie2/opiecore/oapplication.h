/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael Lauer <mickey@tm.informatik.uni-frankfurt.de>
              =.
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

#ifndef OAPPLICATION_H
#define OAPPLICATION_H

#define oApp OApplication::oApplication()


#include "opieapplication.h"


class OApplicationPrivate;
class OConfig;


class OApplication : public OpieApplication
{
  Q_OBJECT

  public:
    /**
     * Constructor. Parses command-line arguments and sets the window caption.
     *
     * @param rAppName application name. Will be used for finding the
     * associated message, icon and configuration files
     *
     */
    OApplication( int& argc, char** argv, const QCString& rAppName );
    /**
     * Destructor. Destroys the application object and its children.
     */
    virtual ~OApplication();
    /**
     * @returns the process-wide application object
     *
     * This is similar to the global @ref QApplication pointer qApp. It
     * allows access to the single global OApplication object, since
     * more than one cannot be created in the same application. It
     * saves you the trouble of having to pass the pointer explicitly
     * to every function that may require it.
     */
    static OApplication* oApplication() { return _instance; };
    /**
     * Returns the application name as given during creation.
     *
     * @returns a reference to the application name
     */
    const QCString& appName() const { return _appname; };
    /**
     * @returns the application session config object.
     *
     * @see OConfig
     */
    OConfig* config();
    /**
     * Shows the main @a widget and sets the name of the application as window caption.
     */
    virtual void showMainWidget( QWidget* widget, bool nomax = false );
    /**
     * Set the application title. The application title will be concatenated
     * to the application name given in the constructor.
     *
     * @param title the title. If not given, resets caption to appname
     */
    virtual void setTitle( const QString& title = QString::null ) const;

  protected:
    void init();

  private:
    const QCString _appname;
    static OApplication* _instance;
    OConfig* _config;
    OApplicationPrivate* d;
};

#endif // OAPPLICATION_H

#!/usr/bin/env python
# -*- coding: iso8859-15 -*-
#--------------------------------------------------------------------------------------------------#
"""                          M i c k e y ' s
                                    P y t h o n
                                       Q u i c k l a u n c h e r

              =.             Copyright (C) 2004 Michael Lauer
            .=l.             <mickey@tm.informatik.uni-frankfurt.de>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful, but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the
  -_. . .   )=.  =           GNU General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Purpose: This module is

At the moment, it is optimized for PyQt. Feel free to port it to PyGtk or other modules.

"""

#--------------------------------------------------------------------------------------------------#
__version__ = "0.0.1"
__author__ = "Michael 'Mickey' Lauer <mickey@tm.informatik.uni-frankfurt.de>"
__license__ = "GPL"
#--------------------------------------------------------------------------------------------------#
try:
    from qtpe import QPEApplication as ApplicationBase
except ImportError:
    from qt import QApplication as ApplicationBase

import sys, os

# <MODULE PRELOAD SECTION>
# Note: Feel free to add other modules to accellerate the startup of your Application.
from qt import *
import qt
# </MODULE PRELOAD SECTION>

#
# FIXME: use the ELAN common debug framework
#
def TRACE( *args ):
    for arg in args:
        sys.stderr.write( "%s - " % arg )
    sys.stderr.write( "\n" )
TERROR = TDEBUG = TINFO = TWARN = TRACE

#--------------------------------------------------------------------------------------------------#
class QuicklauncherServer( ApplicationBase ):
    """The Quicklauncher Server loads other Python Applications as plugins.

    The Quicklauncher server is the one and only Python process in the system.
    It also is the creator of the one and only Qt application object, no matter
    if it is a QPEApplication or a QApplication.

    Quicklauncher server and clients communicate via a FIFO (named pipe), this enables
    us to write clients as easy as the following example:

    #!/bin/sh
    echo /path/to/your/python/script.py >/tmp/mickeys-quicklauncher-$USER
    """


    def __init__( self, name = "/tmp/mickeys-quicklauncher-%s" % os.getenv( "USER") ):
        """Initialize the Quicklauncher & setup the FIFO."""
        ApplicationBase.__init__( self, sys.argv[1:] )
        try:
            os.mkfifo( name )
        except OSError, reason:
            pass
        self.socketNotifier = None
        self.fileno = 0
        self.name = name
        self.reopenFifo()
        self.sendToApplet( "setOnline()" )
        self.sendToApplet( "test(QString,int)", "Hallo", 20 )

    def __del__( self ):
        """Delete the FIFO if present."""
        os.unlink( self.name )
        self.sendCommand( "setOffline()" )

    def sendToApplet( self, command, *params ):
        TDEBUG( self, "sending to applet: '%s' '%s'" % ( command, params ) )
        cmdline = 'qcop "QPE/PyLauncher" "%s"' % command
        for param in params:
            cmdline += ' "%s"' % param
        TDEBUG( self, "calling '%s'" % cmdline )
        os.system( cmdline )

    def reopenFifo( self ):
        """Close and reopen the FIFO. Setup a QSocketNotifier to get the
        Qt event loop notifying us when someone is requesting an application start."""
        del self.socketNotifier
        os.close( self.fileno )
        self.fileno = os.open( self.name, os.O_RDONLY|os.O_NONBLOCK )
        TDEBUG( self, "created communication fifo '%s'" % self.name )
        self.socketNotifier = qt.QSocketNotifier( self.fileno, qt.QSocketNotifier.Read )
        qt.QObject.connect( self.socketNotifier, qt.SIGNAL( "activated(int)" ), self.slotHandleMessage )

    def run( self ):
        """Register socket notifier and start main loop"""
        TDEBUG( self, "---> entering main loop." )
        try:
            self.exec_loop()
        except Exception, reason:
            TERROR( self, "Exception during main loop: '%s'" % reason )
            self.quit()
        TDEBUG( self, "<--- returned from main loop." )

    def slotHandleMessage( self, fileno ):
        """Called by the main loop when there is incoming data at the FIFO."""
        TDEBUG( self, "handle message called." )
        scriptName = os.read( self.fileno, 1000 ).strip()
        garbage = os.read( self.fileno, -1 )
        if scriptName.startswith( "quit" ):
            self.quit()
        TDEBUG( self, "Module to launch = '%s'" % ( scriptName ) )
        self.reopenFifo()
        #
        # Remove QApplication from the namespace and inject our substitute
        #
        global QApplication
        QApplication = QuickApplication
        qt.QApplication = QuickApplication

        modulename = self.importModule( scriptName )
        module = sys.modules[modulename]

    def importModule( self, name ):
        """Import a module by pathname and return the module name."""
        dirname, modulename = os.path.dirname( name ), os.path.basename( name )
        sys.path.append( dirname )
        try:
            if modulename[-3:] == ".py":
                modulename = modulename[:-3]
            module = __import__( modulename )
        except ImportError, reason:
            TERROR( self, "Can't import '%s' (%s)" % ( modulename, reason ) )
        else:
            TDEBUG( self, "Module '%s' been imported = %s" % ( modulename, module ) )
        return modulename

#--------------------------------------------------------------------------------------------------#
class QuickApplication( object ):
    """This class acts as proxy for the real Q[PE]Application class.
    This way we keep the system state sane by only allowing one application object,
    which is the one the server already created for us."""

    def __getattribute__( self, attr ):
        """Mimick a simply attribute resolution order:
        First, look in the application object (the server in this case).
        Second, hand lookup over to the built-in object base class."""
        #print "__getattribute__", attr
        return getattr( server, attr, None ) or object.__getattribute__( self, attr )

#--------------------------------------------------------------------------------------------------#

if __name__ == "__main__":
    print >>sys.stdout, "Mickey's Quicklauncher Version %s booting..." % ( __version__ )
    server = QuicklauncherServer()
    server.run()

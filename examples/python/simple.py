#!pyquicklauncher
# -*- coding: iso8859-15 -*-
#--------------------------------------------------------------------------------------------------#
"""           =.             This file is part of the PyQt examples collection
            .=l.             Copyright (C) 2004 Michael Lauer <mickey@Vanille.de>
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
"""
#--------------------------------------------------------------------------------------------------#

#--------------------------------------------------------------------------------------------------#
__version__ = "1.0.0"
__author__ = "Michael 'Mickey' Lauer <mickey@Vanille.de>"
__license__ = "GPL"
#--------------------------------------------------------------------------------------------------#

import sys
from qt import *
from qtpe import *

#--------------------------------------------------------------------------------------------------#
class SimpleWidget( QWidget ):
    """A simple main widget with a label and a push button."""

    def __init__( self, parent, name = "SimpleWidget" ):
        """Construct me."""
        QWidget.__init__( self, parent, name )

        #
        # Set the caption of this toplevel widget.
        # Encapsulate all user visible strings into self.tr to prepare for localization
        #
        self.setCaption( self.tr( "Simple Example Application" ) )

        #
        # A simple vertical layout.
        # Either call layout.setAutoAdd( True ) or use layout.addWidget( wid ) to manually add widgets
        #
        layout = QVBoxLayout( self )
        layout.setSpacing( 8 )
        layout.setMargin( 11 )

        #
        # Create a label as child of this widget (self) and add it to the layout.
        # A Qt object will be deleted when its parent is being deleted, hence it is not
        # necessary to keep the reference for this widget except you want to change some
        # of its attributes later, i.e. set a different text later on
        # via label.setText( self.tr( "newText" ) )
        #
        label = QLabel( self.tr( "Click on the Button and watch what happens!" ), self )
        layout.addWidget( label )

        #
        # Add a push button as child of this widget and add it to the layout.
        #
        button = QPushButton( self.tr( "Quit!" ), self )
        layout.addWidget( button )

        #
        # Define what happens, when the user clicks on the button.
        # This is being done by a signal-slot connection using QObject.connect.
        # We could also have connected a SIGNAL to a SIGNAL or directly to the
        # the application object slot quit().
        #
        QObject.connect( button, SIGNAL( "clicked()" ), self.slotQuitAppRequested )

    def slotQuitAppRequested( self ):
        """Leave the application when a user clicks on the button."""
        #
        # Leave the main loop by calling the quit() method of the application object
        #
        qApp.quit()

#--------------------------------------------------------------------------------------------------#

#
# Create the one and only application object.
# For plain Qt applications, use QApplication, for Qtopia/Opie applications, use QPEApplication
#
app = QPEApplication( sys.argv[1:] )
#
# Construct an instance of our main widget. Since it is the top level widget, is has no parent (None).
#
mw = SimpleWidget( None )
#
# Show the main widget. For plain Qt applications you should use app.setMainWidget( mw ) and mw.show()
#
app.showMainWidget( mw )
#
# Enter the event processing by calling the event loop which is implemented in the application object.
#
app.exec_loop()

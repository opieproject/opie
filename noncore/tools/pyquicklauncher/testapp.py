#!/usr/bin/env python
# -*- coding: iso8859-15 -*-
#--------------------------------------------------------------------------------------------------#
from qt import *
import sys
#--------------------------------------------------------------------------------------------------#
class MyApplication( QApplication ):

    def __init__( self, argv ):
        QApplication.__init__( self, [] )
        self.b = QPushButton( """This is an example application using Mickey's
fascinating Python Quicklaunch technology!
Close me to quit this application.""", None )
        self.b.show()
        self.setMainWidget( self.b )
#--------------------------------------------------------------------------------------------------#

if __name__ == "__main__":
    print "Standalone!"
else:
    print "Quickapp!"

    app = MyApplication( sys.argv )
    app.exec_loop()

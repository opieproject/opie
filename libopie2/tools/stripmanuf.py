#!/usr/bin/env python

#
# converts a manufacturer file in ethereal format (taken from their CVS)
# into something smaller and faster parseable
#

import sys
import os

i = file( sys.argv[1], "r" )
o = file( sys.argv[2], "w" )

for line in i:
    #print line.strip()
    entries = line.strip().split()
    #print "number of entries =", len( entries )
    #print entries
    if len( entries ) < 2:
        continue
    elif len( entries ) == 2:
        print "2-line detected."
        print >>o, entries[0], entries[1], entries[1]
    elif len( entries ) > 2:
        print >>o, entries[0], entries[1], "_".join( entries[3:] )
    else:
        assert( false )
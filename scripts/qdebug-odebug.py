#!/usr/bin/env python

"""
qdebug-odebug.py (C) 2004 Michael 'Mickey' Lauer <mickey@Vanille.de>
This script helps to convert from the Qt debugging framework to the Opie debugging framework
Though it will take care of the most common usages, it may not recognize uncommon ones, hence
manual work may be needed after applying the script.
"""

import sys, sre

qDebugExpression = sre.compile( '(.*)(qDebug)\(\s*(.*)\s*\);(.*)' )
qWarningExpression = sre.compile( '(.*)(qWarning)\(\s*(.*)\s*\);(.*)' )
qErrorExpression = sre.compile( '(.*)(qError)\(\s*(.*)\s*\);(.*)' )
qFatalExpression = sre.compile( '(.*)(qFatal)\(\s*(.*)\s*\);(.*)' )
printfExpression = sre.compile( '(.*)(printf)\(\s*(.*)\s*\);(.*)' )

debugTable = { "qDebug" : "odebug",
               "qWarning" : "owarn",
               "qError" : "oerr",
               "qFatal" : "ofatal",
               "printf" : "odebug" }

allExpressions = ( qDebugExpression, qWarningExpression, qErrorExpression, qFatalExpression, printfExpression )

####################################################################################################

def convert( fname ):
    print >>sys.stderr, "<NOTE>: Dealing with %s..." % fname

    for line in file( fname ):
        match = False
        for expr in allExpressions:
            m = expr.match( line )
            if m is None:
                continue
            else:
                match = True
                head, debug, content, tail = m.groups()
                print >>sys.stderr, "<NOTE>: Groups = ", m.groups()
                sys.stdout.write( head ) # don't strip() here, because we want to keep indentation
                sys.stdout.write( debugTable[debug.strip()] )
                sys.stdout.write( " << " )
                sys.stdout.write( transform( content ).strip() )
                sys.stdout.write( " << oendl; " )
                sys.stdout.write( tail )
                if not tail.endswith( "\n" ): sys.stdout.write( "\n" )
                continue
        # nothing applies
        if not match:
            sys.stdout.write( line )

####################################################################################################

def transform( s ):
    print >>sys.stderr, "<NOTE>: Transforming '%s'..." % s

    # check if there is one or more comma's outside of strings
    groups = []
    i = 0
    instring = False
    group = ""
    while i < len( s ):
        if s[i] == '"':
            instring = not instring
        elif s[i] == "," and not instring:
            groups.append( group.strip() )
            group = ""
            i += 1
            continue
        group += s[i]
        i += 1
        continue

    groups.append( group.strip() )

    # check for easy case
    if len( groups ) < 2: return s.replace( "%%", "%" )

    # damn. it gets complicated...
    print >>sys.stderr, "<NOTE>: Comma's outside of strings = %d" % ( len( groups ) -1 )

    formatstring, substitutions = groups[0], groups[1:]
    result = ""

    # iterator over formatstring and substitute format directives (e.g. '%d') with the substitutions
    subst = 0
    i = 0
    indirective = False
    while i < len( formatstring ):
        if formatstring[i] != "%":
            result += formatstring[i]
            i += 1
            continue
        else: # % in formatstring
            indirective = True
            i += 1
            while i < len( formatstring ) and formatstring[i] not in "%dDiouxXfegEscpn":
                i += 1
            if formatstring[i] == "%":
                result += "%"
            else:
                result += '" << %s << "' % substitutions[0].replace( "(const char*)", "" ).replace( ".latin1()", "" )
                del substitutions[0]
            indirective = False
            i += 1

    print >>sys.stderr, "<NOTE>: Result seems to be '%s'" % result
    return result.replace( "%%", "%" )

####################################################################################################

if __name__ == "__main__":
    try:
        fname = sys.argv[1]
    except:
        print >>sys.stderr, "Usage: %s <filename>\n" % sys.argv[0]
    else:
        convert( fname )

/****************************************************************************
** $Id: main.cpp,v 1.1 2002-11-01 00:10:42 kergoth Exp $
**
** Definition of ________ class.
**
** Created : 970521
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of the network module of the Qt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid Qt Enterprise Edition licenses may use this
** file in accordance with the Qt Commercial License Agreement provided
** with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about Qt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for QPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "project.h"
#include "option.h"
#include "makefile.h"
#include <qnamespace.h>
#include <qregexp.h>
#include <qdir.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// for Borland, main is defined to qMain which breaks qmake
#undef main

int main(int argc, char **argv)
{
    /* parse command line */
    if(!Option::parseCommandLine(argc, argv))
	return 666;

    QDir sunworkshop42workaround = QDir::current();
    QString oldpwd = sunworkshop42workaround.currentDirPath();
    Option::output_dir = oldpwd; //for now this is the output dir
    if(Option::output_dir.right(1) != QString(QChar(QDir::separator())))
	Option::output_dir += QDir::separator();
    QMakeProject proj;
    int exit_val = 0;
    QStringList files;
    if(Option::qmake_mode == Option::QMAKE_GENERATE_PROJECT)
	files << "(*hack*)"; //we don't even use files, but we do the for() body once
    else
	files = Option::mkfile::project_files;
    for(QStringList::Iterator pfile = files.begin(); pfile != files.end(); pfile++) {
	if(Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE ||
	   Option::qmake_mode == Option::QMAKE_GENERATE_PRL) {
	    QString fn = (*pfile);

	    //setup pwd properly
	    debug_msg(1, "Resetting dir to: %s", oldpwd.latin1());
	    QDir::setCurrent(oldpwd); //reset the old pwd
	    int di = fn.findRev(Option::dir_sep);
	    if(di != -1) {
		debug_msg(1, "Changing dir to: %s", fn.left(di).latin1());
		if(!QDir::setCurrent(fn.left(fn.findRev(Option::dir_sep))))
		    fprintf(stderr, "Cannot find directory: %s\n", fn.left(di).latin1());
		fn = fn.right(fn.length() - di - 1);
	    }

	    /* read project.. */
	    if(!proj.read(fn, oldpwd)) {
		fprintf(stderr, "Error processing project file: %s\n", 
			fn == "-" ? "(stdin)" : (*pfile).latin1());
		exit_val = 2;
		continue;
	    }
	    if(Option::mkfile::do_preprocess) //no need to create makefile
		continue;
	    
	    /* let Option post-process */
	    if(!Option::postProcessProject(&proj)) {
		fprintf(stderr, "Error post-processing project file: %s", 
			fn == "-" ? "(stdin)" : (*pfile).latin1());
		exit_val = 8;
		continue;
	    }
	}

	bool using_stdout = FALSE;
	MakefileGenerator *mkfile = MakefileGenerator::create(&proj); //figure out generator
	if(mkfile && (Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE ||
		      Option::qmake_mode == Option::QMAKE_GENERATE_PROJECT)) {
	    //open output
	    if(!(Option::output.state() & IO_Open)) {
		if(Option::output.name() == "-") {
		    Option::output.setName("");
		    Option::output_dir = QDir::currentDirPath();
		    Option::output.open(IO_WriteOnly | IO_Translate, stdout);
		    using_stdout = TRUE;
		} else {
		    if(Option::output.name().isEmpty() && Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE)
			Option::output.setName(proj.first("QMAKE_MAKEFILE"));
		    if(!mkfile->openOutput(Option::output)) {
			fprintf(stderr, "Failure to open file: %s\n",
				Option::output.name().isEmpty() ? "(stdout)" : Option::output.name().latin1());
			return 5;
		    }
		}
	    }
	} else {
	    using_stdout = TRUE; //kind of..
	}
	if(mkfile && !mkfile->write()) {
	    if(Option::qmake_mode == Option::QMAKE_GENERATE_PROJECT)
		fprintf(stderr, "Unable to generate project file.\n");
	    else
		fprintf(stderr, "Unable to generate makefile for: %s\n", (*pfile).latin1());
	    if(!using_stdout)
		QFile::remove(Option::output.name());
	    exit_val = 6;
	}
	delete mkfile;
	mkfile = NULL;

	/* debugging */
	if(Option::debug_level) {
	    QMap<QString, QStringList> &vars = proj.variables();
	    for(QMap<QString, QStringList>::Iterator it = vars.begin(); it != vars.end(); ++it) {
		if(it.key().left(1) != "." && !it.data().isEmpty())
		    debug_msg(1, "%s === %s", it.key().latin1(), it.data().join(" :: ").latin1());
	    }
	}
    }
    return exit_val;
}

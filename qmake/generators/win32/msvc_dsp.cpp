/****************************************************************************
** $Id: msvc_dsp.cpp,v 1.1 2002-11-01 00:10:42 kergoth Exp $
**
** Definition of ________ class.
**
** Created : 970521
**
** Copyright (C) 1992-2002 Trolltech AS.  All rights reserved.
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

#include "msvc_dsp.h"
#include "option.h"
#include <qdir.h>
#include <qregexp.h>
#include <stdlib.h>
#include <time.h>

DspMakefileGenerator::DspMakefileGenerator(QMakeProject *p) : Win32MakefileGenerator(p), init_flag(FALSE)
{

}

bool
DspMakefileGenerator::writeMakefile(QTextStream &t)
{
    if(!project->variables()["QMAKE_FAILED_REQUIREMENTS"].isEmpty()) {
	/* for now just dump, I need to generated an empty dsp or something.. */
	fprintf(stderr, "Project file not generated because all requirements not met:\n\t%s\n",
		var("QMAKE_FAILED_REQUIREMENTS").latin1());
	return TRUE;
    }

    if(project->first("TEMPLATE") == "vcapp" ||
       project->first("TEMPLATE") == "vclib") {
	return writeDspParts(t);
    }
    else if(project->first("TEMPLATE") == "subdirs") {
	writeHeader(t);
	writeSubDirs(t);
	return TRUE;
    }
    return FALSE;
}

bool
DspMakefileGenerator::writeDspParts(QTextStream &t)
{
    QString dspfile;
    if ( !project->variables()["DSP_TEMPLATE"].isEmpty() ) {
	dspfile = project->first("DSP_TEMPLATE");
    } else {
	dspfile = project->first("MSVCDSP_TEMPLATE");
    }
    QString dspfile_loc = findTemplate(dspfile);

    QFile file(dspfile_loc);
    if(!file.open(IO_ReadOnly)) {
	fprintf(stderr, "Cannot open dsp file: %s\n", dspfile.latin1());
	return FALSE;
    }
    QTextStream dsp(&file);

    int rep;
    QString line;
    while ( !dsp.eof() ) {
	line = dsp.readLine();
	while((rep = line.find(QRegExp("\\$\\$[a-zA-Z0-9_-]*"))) != -1) {
	    QString torep = line.mid(rep, line.find(QRegExp("[^\\$a-zA-Z0-9_-]"), rep) - rep);
	    QString variable = torep.right(torep.length()-2);

	    t << line.left(rep); //output the left side
	    line = line.right(line.length() - (rep + torep.length())); //now past the variable
	    if(variable == "MSVCDSP_SOURCES") {
		if(project->variables()["SOURCES"].isEmpty())
		    continue;

		QString mocpath = var( "QMAKE_MOC" );
		mocpath = mocpath.replace( QRegExp( "\\..*$" ), "" ) + " ";

		QStringList list = project->variables()["SOURCES"] + project->variables()["DEF_FILE"];
		if(!project->isActiveConfig("flat"))
		    list.sort();
		QStringList::Iterator it;
		for( it = list.begin(); it != list.end(); ++it) {
		    beginGroupForFile((*it), t);
		    t << "# Begin Source File\n\nSOURCE=" << (*it) << endl;
		    if ( project->isActiveConfig("moc") && (*it).endsWith(Option::moc_ext)) {
			QString base = (*it);
			base.replace(QRegExp("\\..*$"), "").upper();
			base.replace(QRegExp("[^a-zA-Z]"), "_");

			QString build = "\n\n# Begin Custom Build - Moc'ing " + findMocSource((*it)) +
					"...\n" "InputPath=.\\" + (*it) + "\n\n" "\"" + (*it) + "\""
					" : $(SOURCE) \"$(INTDIR)\" \"$(OUTDIR)\"\n"
					"\t" + mocpath + findMocSource((*it)) + " -o " +
					(*it) + "\n\n" "# End Custom Build\n\n";

			t << "USERDEP_" << base << "=\".\\" << findMocSource((*it)) << "\" \"$(QTDIR)\\bin\\moc.exe\"" << endl << endl;

			t << "!IF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Release\"" << build
			  << "!ELSEIF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Debug\""
			  << build << "!ENDIF " << endl << endl;
		    }
		    t << "# End Source File" << endl;
		}
		endGroups(t);
	    } else if(variable == "MSVCDSP_IMAGES") {
		if(project->variables()["IMAGES"].isEmpty())
		    continue;
		t << "# Begin Source File\n\nSOURCE=" << project->first("QMAKE_IMAGE_COLLECTION") << endl;
		t << "# End Source File" << endl;
	    } else if(variable == "MSVCDSP_HEADERS") {
		if(project->variables()["HEADERS"].isEmpty())
		    continue;

		QStringList list = project->variables()["HEADERS"];
		if(!project->isActiveConfig("flat"))
		    list.sort();
		for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
//		    beginGroupForFile((*it), t);
		    t << "# Begin Source File\n\nSOURCE=" << (*it) << endl << endl;
		    if ( project->isActiveConfig("moc") && !findMocDestination((*it)).isEmpty()) {
			QString base = (*it);
			base.replace(QRegExp("\\..*$"), "").upper();
			base.replace(QRegExp("[^a-zA-Z]"), "_");

			QString mocpath = var( "QMAKE_MOC" );
			mocpath = mocpath.replace( QRegExp( "\\..*$" ), "" ) + " ";

			QString build = "\n\n# Begin Custom Build - Moc'ing " + (*it) +
					"...\n" "InputPath=.\\" + (*it) + "\n\n" "\"" + findMocDestination((*it)) +
					"\"" " : $(SOURCE) \"$(INTDIR)\" \"$(OUTDIR)\"\n"
					"\t" + mocpath + (*it)  + " -o " +
					findMocDestination((*it)) + "\n\n" "# End Custom Build\n\n";

			t << "USERDEP_" << base << "=\"$(QTDIR)\\bin\\moc.exe\"" << endl << endl;

			t << "!IF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Release\"" << build
			  << "!ELSEIF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Debug\""
			  << build << "!ENDIF " << endl << endl;
		    }
		    t << "# End Source File" << endl;
		}
//		endGroups(t);
	    } else if(variable == "MSVCDSP_FORMSOURCES" || variable == "MSVCDSP_FORMHEADERS") {
		if(project->variables()["FORMS"].isEmpty())
		    continue;

		QString uiSourcesDir;
		QString uiHeadersDir;
		if(!project->variables()["UI_DIR"].isEmpty()) {
		    uiSourcesDir = project->first("UI_DIR");
		    uiHeadersDir = project->first("UI_DIR");
		} else {
		    if ( !project->variables()["UI_SOURCES_DIR"].isEmpty() )
			uiSourcesDir = project->first("UI_SOURCES_DIR");
		    else
			uiSourcesDir = "";
		    if ( !project->variables()["UI_HEADERS_DIR"].isEmpty() )
			uiHeadersDir = project->first("UI_HEADERS_DIR");
		    else
			uiHeadersDir = "";
		}

		QStringList list = project->variables()["FORMS"];
		if(!project->isActiveConfig("flat"))
		    list.sort();
		QString ext = variable == "MSVCDSP_FORMSOURCES" ? ".cpp" : ".h";
		for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
		    QString base = (*it);
		    int dot = base.findRev(".");
  		    base.replace( dot, base.length() - dot, ext );
		    QString fname = base;

		    int lbs = fname.findRev( "\\" );
		    QString fpath;
		    if ( lbs != -1 )
			fpath = fname.left( lbs + 1 );
		    fname = fname.right( fname.length() - lbs - 1 );

		    if ( ext == ".cpp" && !uiSourcesDir.isEmpty() )
			fname.prepend(uiSourcesDir);
		    else if ( ext == ".h" && !uiHeadersDir.isEmpty() )
			fname.prepend(uiHeadersDir);
		    else
			fname = base;
//		    beginGroupForFile(fname, t);
		    t << "# Begin Source File\n\nSOURCE=" << fname
		      << "\n# End Source File" << endl;
		}
//		endGroups(t);
	    } else if(variable == "MSVCDSP_TRANSLATIONS" ) {
		if(project->variables()["TRANSLATIONS"].isEmpty())
		    continue;

		t << "# Begin Group \"Translations\"\n";
		t << "# Prop Default_Filter \"ts\"\n";

		QStringList list = project->variables()["TRANSLATIONS"];
		if(!project->isActiveConfig("flat"))
		    list.sort();
		for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
		    QString sify = *it;
		    sify.replace('/', '\\' );
		    QString base = (*it);
		    base.replace(QRegExp("\\..*$"), "").upper();
		    base.replace(QRegExp("[^a-zA-Z]"), "_");

//		    beginGroupForFile(sify, t);
		    t << "# Begin Source File\n\nSOURCE=" << sify << endl;
		    t << "\n# End Source File" << endl;
		}
//		endGroups(t);
		t << "\n# End Group\n";
	    } else if (variable == "MSVCDSP_MOCSOURCES" && project->isActiveConfig("moc")) {
		if ( project->variables()["SRCMOC"].isEmpty())
		    continue;

		QString mocpath = var( "QMAKE_MOC" );
		mocpath = mocpath.replace( QRegExp( "\\..*$" ), "" ) + " ";

		QStringList list = project->variables()["SRCMOC"];
		if(!project->isActiveConfig("flat"))
		    list.sort();
		for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
//		    beginGroupForFile((*it), t);
		    t << "# Begin Source File\n\nSOURCE=" << (*it) << endl;
		    if ( project->isActiveConfig("moc") && (*it).endsWith(Option::moc_ext)) {
			QString base = (*it);
			base.replace(QRegExp("\\..*$"), "").upper();
			base.replace(QRegExp("[^a-zA-Z]"), "_");

			QString build = "\n\n# Begin Custom Build - Moc'ing " + findMocSource((*it)) +
					"...\n" "InputPath=.\\" + (*it) + "\n\n" "\"" + (*it) + "\""
					" : $(SOURCE) \"$(INTDIR)\" \"$(OUTDIR)\"\n"
					"\t" + mocpath + findMocSource((*it)) + " -o " +
					(*it) + "\n\n" "# End Custom Build\n\n";

			t << "USERDEP_" << base << "=\".\\" << findMocSource((*it)) << "\" \"$(QTDIR)\\bin\\moc.exe\"" << endl << endl;

			t << "!IF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Release\"" << build
			  << "!ELSEIF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Debug\""
			  << build << "!ENDIF " << endl << endl;
		    }
		    t << "# End Source File" << endl;
		}
//		endGroups(t);
	    } else if(variable == "MSVCDSP_PICTURES") {
		if(project->variables()["IMAGES"].isEmpty())
		    continue;

		t << "# Begin Group \"Images\"\n"
		  << "# Prop Default_Filter \"png jpeg bmp xpm\"\n";

		QStringList list = project->variables()["IMAGES"];
		if(!project->isActiveConfig("flat"))
		    list.sort();
		QStringList::Iterator it;

		// dump the image list to a file UIC can read.
		QFile f( "images.tmp" );
		f.open( IO_WriteOnly );
		QTextStream ts( &f );
		for( it = list.begin(); it != list.end(); ++it )
		    ts << " " << *it;
		f.close();

		// create an output step for images not more than once
		bool imagesBuildDone = FALSE;
		for( it = list.begin(); it != list.end(); ++it ) {
//		    beginGroupForFile((*it), t);
		    t << "# Begin Source File\n\nSOURCE=" << (*it) << endl;

		    QString base = (*it);
		    QString uicpath = var("QMAKE_UIC");
		    uicpath = uicpath.replace(QRegExp("\\..*$"), "") + " ";

		    if ( !imagesBuildDone ) {
			imagesBuildDone = TRUE;
			QString build = "\n\n# Begin Custom Build - Creating image collection...\n"
			    "InputPath=.\\" + base + "\n\n";

			build += "\"" + project->first("QMAKE_IMAGE_COLLECTION") + "\" : $(SOURCE) \"$(INTDIR)\" \"$(OUTDIR)\"\n";
			build += "\t" + uicpath + "-embed " + project->first("QMAKE_ORIG_TARGET") + " -f images.tmp -o "
				      + project->first("QMAKE_IMAGE_COLLECTION") + "\n\n";
			build.append("# End Custom Build\n\n");

			t << "USERDEP_" << base << "=";
			QStringList::Iterator it2 = list.begin();
			while ( it2 != list.end() ) {
			    t << "\"" << (*it2) << "\"";
			    it2++;
			    if ( it2 != list.end() )
				t << "\\\n";
			}
			t << endl << endl;

			t << "!IF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Release\"" << build
			  << "!ELSEIF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Debug\"" << build
			  << "!ENDIF \n\n" << endl;
		    }

		    t << "# End Source File" << endl;
		}
//		endGroups(t);
		t << "\n# End Group\n";
	    } else if(variable == "MSVCDSP_FORMS") {
		if(project->variables()["FORMS"].isEmpty())
		    continue;

		t << "# Begin Group \"Forms\"\n"
		  << "# Prop Default_Filter \"ui\"\n";

		QString uicpath = var("QMAKE_UIC");
		uicpath = uicpath.replace(QRegExp("\\..*$"), "") + " ";
		QString mocpath = var( "QMAKE_MOC" );
		mocpath = mocpath.replace( QRegExp( "\\..*$" ), "" ) + " ";

		QStringList list = project->variables()["FORMS"];
		if(!project->isActiveConfig("flat"))
		    list.sort();
		for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
		    QString base = (*it);
//		    beginGroupForFile(base, t);
		    t <<  "# Begin Source File\n\nSOURCE=" << base << endl;

		    QString fname = base;
		    fname.replace(".ui", "");
		    int lbs = fname.findRev( "\\" );
		    QString fpath;
		    if ( lbs != -1 )
			fpath = fname.left( lbs + 1 );
		    fname = fname.right( fname.length() - lbs - 1 );

		    QString mocFile;
		    if(!project->variables()["MOC_DIR"].isEmpty())
			mocFile = project->first("MOC_DIR");
		    else
			mocFile = fpath;

		    QString uiSourcesDir;
		    QString uiHeadersDir;
		    if(!project->variables()["UI_DIR"].isEmpty()) {
			uiSourcesDir = project->first("UI_DIR");
			uiHeadersDir = project->first("UI_DIR");
		    } else {
			if ( !project->variables()["UI_SOURCES_DIR"].isEmpty() )
			    uiSourcesDir = project->first("UI_SOURCES_DIR");
			else
			    uiSourcesDir = fpath;
			if ( !project->variables()["UI_HEADERS_DIR"].isEmpty() )
			    uiHeadersDir = project->first("UI_HEADERS_DIR");
			else
			    uiHeadersDir = fpath;
		    }

		    t << "USERDEP_" << base << "=\"$(QTDIR)\\bin\\moc.exe\" \"$(QTDIR)\\bin\\uic.exe\"" << endl << endl;

		    QString build = "\n\n# Begin Custom Build - Uic'ing " + base + "...\n"
			"InputPath=.\\" + base + "\n\n" "BuildCmds= \\\n\t" + uicpath + base +
				    " -o " + uiHeadersDir + fname + ".h \\\n" "\t" + uicpath  + base +
				    " -i " + fname + ".h -o " + uiSourcesDir + fname + ".cpp \\\n"
				    "\t" + mocpath + uiHeadersDir + fname + ".h -o " + mocFile + "moc_" + fname + ".cpp \\\n";

		    build.append("\n\"" + uiHeadersDir + fname + ".h\" : \"$(SOURCE)\" \"$(INTDIR)\" \"$(OUTDIR)\""  "\n"
				 "\t$(BuildCmds)\n\n"
				 "\"" + uiSourcesDir + fname + ".cpp\" : \"$(SOURCE)\" \"$(INTDIR)\" \"$(OUTDIR)\"" "\n"
				 "\t$(BuildCmds)\n\n"
				 "\"" + mocFile + "moc_" + fname + ".cpp\" : \"$(SOURCE)\" \"$(INTDIR)\" \"$(OUTDIR)\"" "\n"
				 "\t$(BuildCmds)\n\n");

		    build.append("# End Custom Build\n\n");

		    t << "!IF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Release\"" << build
		      << "!ELSEIF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Debug\"" << build
		      << "!ENDIF \n\n" << "# End Source File" << endl;
		}
//		endGroups(t);
		t << "\n# End Group\n";
	    } else if(variable == "MSVCDSP_LEXSOURCES") {
		if(project->variables()["LEXSOURCES"].isEmpty())
		    continue;

		t << "# Begin Group \"Lexables\"\n"
		  << "# Prop Default_Filter \"l\"\n";

		QString lexpath = var("QMAKE_LEX") + varGlue("QMAKE_LEXFLAGS", " ", " ", "") + " ";

		QStringList list = project->variables()["LEXSOURCES"];
		if(!project->isActiveConfig("flat"))
		    list.sort();
		for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
		    QString fname = (*it);
//		    beginGroupForFile(fname, t);
		    t <<  "# Begin Source File\n\nSOURCE=" << fname << endl;
		    fname.replace(".l", Option::lex_mod + Option::cpp_ext.first());

		    QString build = "\n\n# Begin Custom Build - Lex'ing " + (*it) + "...\n"
			"InputPath=.\\" + (*it) + "\n\n"
				    "\"" + fname + "\" : \"$(SOURCE)\" \"$(INTDIR)\" \"$(OUTDIR)\"" "\n"
				    "\t" + lexpath + (*it) + "\\\n"
				    "\tdel " + fname + "\\\n"
				    "\tcopy lex.yy.c " + fname + "\n\n" +
				    "# End Custom Build\n\n";
		    t << "!IF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Release\"" << build
		      << "!ELSEIF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Debug\"" << build
		      << "!ENDIF \n\n" << build

		      << "# End Source File" << endl;
		}
//		endGroups(t);
		t << "\n# End Group\n";
	    } else if(variable == "MSVCDSP_YACCSOURCES") {
		if(project->variables()["YACCSOURCES"].isEmpty())
		    continue;

		t << "# Begin Group \"Yaccables\"\n"
		  << "# Prop Default_Filter \"y\"\n";

		QString yaccpath = var("QMAKE_YACC") + varGlue("QMAKE_YACCFLAGS", " ", " ", "") + " ";

		QStringList list = project->variables()["YACCSOURCES"];
		if(!project->isActiveConfig("flat"))
		    list.sort();
		for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
		    QString fname = (*it);
//		    beginGroupForFile(fname, t);
		    t <<  "# Begin Source File\n\nSOURCE=" << fname << endl;
		    fname.replace(".y", Option::yacc_mod);

		    QString build = "\n\n# Begin Custom Build - Yacc'ing " + (*it) + "...\n"
			"InputPath=.\\" + (*it) + "\n\n"
				    "\"" + fname + Option::cpp_ext.first() + "\" : \"$(SOURCE)\" \"$(INTDIR)\" \"$(OUTDIR)\"" "\n"
				    "\t" + yaccpath + (*it) + "\\\n"
				    "\tdel " + fname + Option::h_ext.first() + "\\\n"
				    "\tmove y.tab.h " + fname + Option::h_ext.first() + "\n\n" +
				    "\tdel " + fname + Option::cpp_ext.first() + "\\\n"
				    "\tmove y.tab.c " + fname + Option::cpp_ext.first() + "\n\n" +
				    "# End Custom Build\n\n";

		    t << "!IF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Release\"" << build
		      << "!ELSEIF  \"$(CFG)\" == \"" << var("MSVCDSP_PROJECT") << " - Win32 Debug\"" << build
		      << "!ENDIF \n\n"
		      << "# End Source File" << endl;
		}
//		endGroups(t);
		t << "\n# End Group\n";
	    } else if( variable == "MSVCDSP_CONFIGMODE" ) {
		if( project->isActiveConfig( "debug" ) )
		    t << "Debug";
		else
		    t << "Release";
	    } else if( variable == "MSVCDSP_IDLSOURCES" ) {
		QStringList list = project->variables()["MSVCDSP_IDLSOURCES"];
		if(!project->isActiveConfig("flat"))
		    list.sort();
		for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
		    t << "# Begin Source File" << endl << endl;
		    t << "SOURCE=" << (*it) << endl;
		    t << "# PROP Exclude_From_Build 1" << endl;
		    t << "# End Source File" << endl << endl;
		}
	    }
	    else
		t << var(variable);
	}
	t << line << endl;
    }
    t << endl;
    file.close();
    return TRUE;
}



void
DspMakefileGenerator::init()
{
    if(init_flag)
	return;
    QStringList::Iterator it;
    init_flag = TRUE;

    /* this should probably not be here, but I'm using it to wrap the .t files */
    if(project->first("TEMPLATE") == "vcapp" )
	project->variables()["QMAKE_APP_FLAG"].append("1");
    else if(project->first("TEMPLATE") == "vclib")
	project->variables()["QMAKE_LIB_FLAG"].append("1");
    if ( project->variables()["QMAKESPEC"].isEmpty() )
	project->variables()["QMAKESPEC"].append( getenv("QMAKESPEC") );

    bool is_qt = (project->first("TARGET") == "qt"QTDLL_POSTFIX || project->first("TARGET") == "qt-mt"QTDLL_POSTFIX);
    project->variables()["QMAKE_ORIG_TARGET"] = project->variables()["TARGET"];

    QStringList &configs = project->variables()["CONFIG"];
    if (project->isActiveConfig("shared"))
	project->variables()["DEFINES"].append("QT_DLL");
    if (project->isActiveConfig("qt_dll"))
	if(configs.findIndex("qt") == -1) configs.append("qt");
    if ( project->isActiveConfig("qt") ) {
	if ( project->isActiveConfig( "plugin" ) ) {
	    project->variables()["CONFIG"].append("dll");
	    project->variables()["DEFINES"].append("QT_PLUGIN");
	}
	if ( (project->variables()["DEFINES"].findIndex("QT_NODLL") == -1) &&
	     ((project->variables()["DEFINES"].findIndex("QT_MAKEDLL") != -1 ||
	       project->variables()["DEFINES"].findIndex("QT_DLL") != -1) ||
	      (getenv("QT_DLL") && !getenv("QT_NODLL"))) ) {
	    project->variables()["QMAKE_QT_DLL"].append("1");
	    if ( is_qt && !project->variables()["QMAKE_LIB_FLAG"].isEmpty() )
		project->variables()["CONFIG"].append("dll");
	}
    }
    if ( project->isActiveConfig("dll") || !project->variables()["QMAKE_APP_FLAG"].isEmpty() ) {
	project->variables()["CONFIG"].remove("staticlib");
	project->variables()["QMAKE_APP_OR_DLL"].append("1");
    } else {
	project->variables()["CONFIG"].append("staticlib");
    }

    if ( project->isActiveConfig("qt") || project->isActiveConfig("opengl") ) {
	project->variables()["CONFIG"].append("windows");
    }
    if ( !project->variables()["VERSION"].isEmpty() ) {
	QString version = project->variables()["VERSION"][0];
	int firstDot = version.find( "." );
	QString major = version.left( firstDot );
	QString minor = version.right( version.length() - firstDot - 1 );
	minor.replace( ".", "" );
	project->variables()["MSVCDSP_VERSION"].append( "/VERSION:" + major + "." + minor );
    }

    if ( project->isActiveConfig("qt") ) {
	project->variables()["CONFIG"].append("moc");
	project->variables()["INCLUDEPATH"] +=	project->variables()["QMAKE_INCDIR_QT"];
	project->variables()["QMAKE_LIBDIR"] += project->variables()["QMAKE_LIBDIR_QT"];

	if ( is_qt && !project->variables()["QMAKE_LIB_FLAG"].isEmpty() ) {
	    if ( !project->variables()["QMAKE_QT_DLL"].isEmpty() ) {
		project->variables()["DEFINES"].append("QT_MAKEDLL");
		project->variables()["QMAKE_LFLAGS"].append("/base:\"0x39D00000\"");
	    }
	} else {
	    if(project->isActiveConfig("thread"))
		project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT_THREAD"];
	    else
		project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT"];
	    if ( !project->variables()["QMAKE_QT_DLL"].isEmpty() ) {
		int hver = findHighestVersion(project->first("QMAKE_LIBDIR_QT"), "qt");
		if ( hver == -1 )
		    hver = findHighestVersion(project->first("QMAKE_LIBDIR_QT"), "qt-mt");
		if(hver != -1) {
		    QString ver;
		    ver.sprintf("qt%s" QTDLL_POSTFIX "%d.lib", (project->isActiveConfig("thread") ? "-mt" : ""), hver);
		    QStringList &libs = project->variables()["QMAKE_LIBS"];
		    for(QStringList::Iterator libit = libs.begin(); libit != libs.end(); ++libit)
			(*libit).replace(QRegExp("qt(-mt)?\\.lib"), ver);
		}
	    }
	    if ( project->isActiveConfig( "activeqt" ) ) {
		project->variables().remove("QMAKE_LIBS_QT_ENTRY");
		project->variables()["QMAKE_LIBS_QT_ENTRY"] = "qaxserver.lib";
		if ( project->isActiveConfig( "dll" ) )
		    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT_ENTRY"];
	    }
	    if ( !project->isActiveConfig("dll") && !project->isActiveConfig("plugin") ) {
		project->variables()["QMAKE_LIBS"] +=project->variables()["QMAKE_LIBS_QT_ENTRY"];
	    }
	}
    }

    if ( project->isActiveConfig("debug") ) {
	if ( !project->first("OBJECTS_DIR").isEmpty() )
	    project->variables()["MSVCDSP_OBJECTSDIRDEB"] = project->first("OBJECTS_DIR");
	else
	    project->variables()["MSVCDSP_OBJECTSDIRDEB"] = "Debug";
	project->variables()["MSVCDSP_OBJECTSDIRREL"] = "Release";
	if ( !project->first("DESTDIR").isEmpty() )
	    project->variables()["MSVCDSP_TARGETDIRDEB"] = project->first("DESTDIR");
	else
	    project->variables()["MSVCDSP_TARGETDIRDEB"] = "Debug";
	project->variables()["MSVCDSP_TARGETDIRREL"] = "Release";
    } else {
	if ( !project->first("OBJECTS_DIR").isEmpty() )
	    project->variables()["MSVCDSP_OBJECTSDIRREL"] = project->first("OBJECTS_DIR");
	project->variables()["MSVCDSP_OBJECTSDIRDEB"] = "Debug";
	if ( !project->first("DESTDIR").isEmpty() )
	    project->variables()["MSVCDSP_TARGETDIRREL"] = project->first("DESTDIR");
	else
	    project->variables()["MSVCDSP_TARGETDIRREL"] = "Release";
	project->variables()["MSVCDSP_TARGETDIRDEB"] = "Debug";
    }

    if ( project->isActiveConfig("opengl") ) {
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_OPENGL"];
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_OPENGL"];
    }
    if ( project->isActiveConfig("thread") ) {
	if(project->isActiveConfig("qt"))
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_THREAD_SUPPORT" );
        if ( project->isActiveConfig("dll") || project->first("TARGET") == "qtmain"
	     || !project->variables()["QMAKE_QT_DLL"].isEmpty() ) {
	    project->variables()["MSVCDSP_MTDEFD"] += project->variables()["QMAKE_CXXFLAGS_MT_DLLDBG"];
	    project->variables()["MSVCDSP_MTDEF"] += project->variables()["QMAKE_CXXFLAGS_MT_DLL"];
	} else {
	    // YES we want to use the DLL even in a static build
	    project->variables()["MSVCDSP_MTDEFD"] += project->variables()["QMAKE_CXXFLAGS_MT_DBG"];
	    project->variables()["MSVCDSP_MTDEF"] += project->variables()["QMAKE_CXXFLAGS_MT"];
	}
	if ( !project->variables()["DEFINES"].contains("QT_DLL") && is_qt
	     && project->first("TARGET") != "qtmain" )
	    project->variables()["QMAKE_LFLAGS"].append("/NODEFAULTLIB:\"libc\"");
    }

    if(project->isActiveConfig("qt")) {
	if ( project->isActiveConfig("accessibility" ) )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_ACCESSIBILITY_SUPPORT");
	if ( project->isActiveConfig("tablet") )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_TABLET_SUPPORT");
    }
    if ( project->isActiveConfig("dll") ) {
	if ( !project->variables()["QMAKE_LIB_FLAG"].isEmpty() ) {
	    QString ver_xyz(project->first("VERSION"));
	    ver_xyz.replace(".", "");
	    project->variables()["TARGET_EXT"].append(ver_xyz + ".dll");
	} else {
	    project->variables()["TARGET_EXT"].append(".dll");
	}
    } else {
	if ( !project->variables()["QMAKE_APP_FLAG"].isEmpty() )
	    project->variables()["TARGET_EXT"].append(".exe");
	else
	    project->variables()["TARGET_EXT"].append(".lib");
    }

    project->variables()["MSVCDSP_VER"] = "6.00";
    project->variables()["MSVCDSP_DEBUG_OPT"] = "/GZ /ZI";

    if(!project->isActiveConfig("incremental")) {
	project->variables()["QMAKE_LFLAGS"].append(QString("/incremental:no"));
        if ( is_qt )
	    project->variables()["MSVCDSP_DEBUG_OPT"] = "/GZ /Zi";
    }

    QString msvcdsp_project;
    if ( project->variables()["TARGET"].count() )
	msvcdsp_project = project->variables()["TARGET"].first();

    QString targetfilename = project->variables()["TARGET"].first();
    project->variables()["TARGET"].first() += project->first("TARGET_EXT");
    if ( project->isActiveConfig("moc") )
	setMocAware(TRUE);

    project->variables()["QMAKE_LIBS"] += project->variables()["LIBS"];
    project->variables()["QMAKE_FILETAGS"] += QStringList::split(' ',
								 "HEADERS SOURCES DEF_FILE RC_FILE TARGET QMAKE_LIBS DESTDIR DLLDESTDIR INCLUDEPATH");
    QStringList &l = project->variables()["QMAKE_FILETAGS"];
    for(it = l.begin(); it != l.end(); ++it) {
	QStringList &gdmf = project->variables()[(*it)];
	for(QStringList::Iterator inner = gdmf.begin(); inner != gdmf.end(); ++inner)
	    (*inner) = Option::fixPathToTargetOS((*inner), FALSE);
    }

    MakefileGenerator::init();
    if ( msvcdsp_project.isEmpty() )
	msvcdsp_project = Option::output.name();

    msvcdsp_project = msvcdsp_project.right( msvcdsp_project.length() - msvcdsp_project.findRev( "\\" ) - 1 );
    msvcdsp_project = msvcdsp_project.left( msvcdsp_project.findRev( "." ) );
    msvcdsp_project.replace("-", "");

    project->variables()["MSVCDSP_PROJECT"].append(msvcdsp_project);
    QStringList &proj = project->variables()["MSVCDSP_PROJECT"];

    for(it = proj.begin(); it != proj.end(); ++it)
	(*it).replace(QRegExp("\\.[a-zA-Z0-9_]*$"), "");

    if ( !project->variables()["QMAKE_APP_FLAG"].isEmpty() ) {
	project->variables()["MSVCDSP_TEMPLATE"].append("win32app" + project->first( "DSP_EXTENSION" ) );
	if ( project->isActiveConfig("console") ) {
	    project->variables()["MSVCDSP_CONSOLE"].append("Console");
	    project->variables()["MSVCDSP_WINCONDEF"].append("_CONSOLE");
	    project->variables()["MSVCDSP_DSPTYPE"].append("0x0103");
	    project->variables()["MSVCDSP_SUBSYSTEM"].append("console");
	} else {
	    project->variables()["MSVCDSP_CONSOLE"].clear();
	    project->variables()["MSVCDSP_WINCONDEF"].append("_WINDOWS");
	    project->variables()["MSVCDSP_DSPTYPE"].append("0x0101");
	    project->variables()["MSVCDSP_SUBSYSTEM"].append("windows");
	}
    } else {
        if ( project->isActiveConfig("dll") ) {
            project->variables()["MSVCDSP_TEMPLATE"].append("win32dll" + project->first( "DSP_EXTENSION" ) );
        } else {
            project->variables()["MSVCDSP_TEMPLATE"].append("win32lib" + project->first( "DSP_EXTENSION" ) );
        }
    }

    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_WINDOWS"];

    project->variables()["MSVCDSP_LFLAGS" ] += project->variables()["QMAKE_LFLAGS"];
    if ( !project->variables()["QMAKE_LIBDIR"].isEmpty() )
	project->variables()["MSVCDSP_LFLAGS" ].append(varGlue("QMAKE_LIBDIR","/LIBPATH:\"","\" /LIBPATH:\"","\""));
    project->variables()["MSVCDSP_CXXFLAGS" ] += project->variables()["QMAKE_CXXFLAGS"];
    project->variables()["MSVCDSP_DEFINES"].append(varGlue("DEFINES","/D ","" " /D ",""));
    project->variables()["MSVCDSP_DEFINES"].append(varGlue("PRL_EXPORT_DEFINES","/D ","" " /D ",""));

    QStringList &libs = project->variables()["QMAKE_LIBS"];
    for(QStringList::Iterator libit = libs.begin(); libit != libs.end(); ++libit) {
	QString lib = (*libit);
	lib.replace(QRegExp("\""), "");
	project->variables()["MSVCDSP_LIBS"].append(" \"" + lib + "\"");
    }

    QStringList &incs = project->variables()["INCLUDEPATH"];
    for(QStringList::Iterator incit = incs.begin(); incit != incs.end(); ++incit) {
	QString inc = (*incit);
	inc.replace("\"", "");
	project->variables()["MSVCDSP_INCPATH"].append("/I \"" + inc + "\"");
    }

    project->variables()["MSVCDSP_INCPATH"].append("/I \"" + specdir() + "\"");
    if ( project->isActiveConfig("qt") ) {
	project->variables()["MSVCDSP_RELDEFS"].append("/D \"QT_NO_DEBUG\"");
    } else {
	project->variables()["MSVCDSP_RELDEFS"].clear();
    }

    QString dest;
    if ( !project->variables()["DESTDIR"].isEmpty() ) {
	project->variables()["TARGET"].first().prepend(project->first("DESTDIR"));
	Option::fixPathToTargetOS(project->first("TARGET"));
	dest = project->first("TARGET");
        if ( project->first("TARGET").startsWith("$(QTDIR)") )
	    dest.replace( "$(QTDIR)", getenv("QTDIR") );
	project->variables()["MSVCDSP_TARGET"].append(
	    QString("/out:\"") + dest + "\"");
	if ( project->isActiveConfig("dll") ) {
	    QString imp = dest;
	    imp.replace(".dll", ".lib");
	    project->variables()["MSVCDSP_TARGET"].append(QString(" /implib:\"") + imp + "\"");
	}
    }
    if ( project->isActiveConfig("dll") && !project->variables()["DLLDESTDIR"].isEmpty() ) {
	QStringList dlldirs = project->variables()["DLLDESTDIR"];
	QString copydll = "# Begin Special Build Tool\n"
	     "TargetPath=" + dest + "\n"
		 "SOURCE=$(InputPath)\n"
	 "PostBuild_Desc=Copy DLL to " + project->first("DLLDESTDIR") + "\n"
	 "PostBuild_Cmds=";

	for ( QStringList::Iterator dlldir = dlldirs.begin(); dlldir != dlldirs.end(); ++dlldir ) {
	    copydll += "copy \"" + dest + "\" \"" + *dlldir + "\"\t";
	}

	copydll += "\n# End Special Build Tool";
	project->variables()["MSVCDSP_COPY_DLL_REL"].append( copydll );
	project->variables()["MSVCDSP_COPY_DLL_DBG"].append( copydll );
    }
    if ( project->isActiveConfig("activeqt") ) {
	QString idl = project->variables()["QMAKE_IDL"].first();
	QString idc = project->variables()["QMAKE_IDC"].first();
	QString version = project->variables()["VERSION"].first();
	if ( version.isEmpty() )
	    version = "1.0";

	project->variables()["MSVCDSP_IDLSOURCES"].append( "tmp\\" + targetfilename + ".idl" );
	project->variables()["MSVCDSP_IDLSOURCES"].append( "tmp\\" + targetfilename + ".tlb" );
	project->variables()["MSVCDSP_IDLSOURCES"].append( "tmp\\" + targetfilename + ".midl" );
	if ( project->isActiveConfig( "dll" ) ) {
	    QString regcmd = "# Begin Special Build Tool\n"
		"TargetPath=" + targetfilename + "\n"
		    "SOURCE=$(InputPath)\n"
	    "PostBuild_Desc=Finalizing ActiveQt server...\n"
	    "PostBuild_Cmds=" +
			     idc + " %1 -idl tmp\\" + targetfilename + ".idl -version " + version +
			     "\t" + idl + " tmp\\" + targetfilename + ".idl /nologo /o tmp\\" + targetfilename + ".midl /tlb tmp\\" + targetfilename + ".tlb /iid tmp\\dump.midl /dlldata tmp\\dump.midl /cstub tmp\\dump.midl /header tmp\\dump.midl /proxy tmp\\dump.midl /sstub tmp\\dump.midl"
			     "\t" + idc + " %1 /tlb tmp\\" + targetfilename + ".tlb"
			     "\t" + idc + " %1 /regserver\n"
			     "# End Special Build Tool";

	    QString executable = project->variables()["MSVCDSP_TARGETDIRREL"].first() + "\\" + project->variables()["TARGET"].first();
	    project->variables()["MSVCDSP_COPY_DLL_REL"].append( regcmd.arg(executable).arg(executable).arg(executable) );

	    executable = project->variables()["MSVCDSP_TARGETDIRDEB"].first() + "\\" + project->variables()["TARGET"].first();
	    project->variables()["MSVCDSP_COPY_DLL_DBG"].append( regcmd.arg(executable).arg(executable).arg(executable) );
	} else {
	    QString regcmd = "# Begin Special Build Tool\n"
		"TargetPath=" + targetfilename + "\n"
		    "SOURCE=$(InputPath)\n"
	    "PostBuild_Desc=Finalizing ActiveQt server...\n"
	    "PostBuild_Cmds="
			     "%1 -dumpidl tmp\\" + targetfilename + ".idl -version " + version +
			     "\t" + idl + " tmp\\" + targetfilename + ".idl /nologo /o tmp\\" + targetfilename + ".midl /tlb tmp\\" + targetfilename + ".tlb /iid tmp\\dump.midl /dlldata tmp\\dump.midl /cstub tmp\\dump.midl /header tmp\\dump.midl /proxy tmp\\dump.midl /sstub tmp\\dump.midl"
			     "\t" + idc + " %1 /tlb tmp\\" + targetfilename + ".tlb"
			     "\t%1 -regserver\n"
			     "# End Special Build Tool";

	    QString executable = project->variables()["MSVCDSP_TARGETDIRREL"].first() + "\\" + project->variables()["TARGET"].first();
	    project->variables()["MSVCDSP_REGSVR_REL"].append( regcmd.arg(executable).arg(executable).arg(executable) );

	    executable = project->variables()["MSVCDSP_TARGETDIRDEB"].first() + "\\" + project->variables()["TARGET"].first();
	    project->variables()["MSVCDSP_REGSVR_DBG"].append( regcmd.arg(executable).arg(executable).arg(executable) );
	}

    }
    if ( !project->variables()["SOURCES"].isEmpty() || !project->variables()["RC_FILE"].isEmpty() ) {
	project->variables()["SOURCES"] += project->variables()["RC_FILE"];
    }
    QStringList &list = project->variables()["FORMS"];
    for( it = list.begin(); it != list.end(); ++it ) {
	if ( QFile::exists( *it + ".h" ) )
	    project->variables()["SOURCES"].append( *it + ".h" );
    }
    project->variables()["QMAKE_INTERNAL_PRL_LIBS"] << "MSVCDSP_LIBS";
}


QString
DspMakefileGenerator::findTemplate(QString file)
{
    QString ret;
    if(!QFile::exists((ret = file)) &&
       !QFile::exists((ret = QString(Option::mkfile::qmakespec + "/" + file))) &&
       !QFile::exists((ret = QString(getenv("QTDIR")) + "/mkspecs/win32-msvc/" + file)) &&
       !QFile::exists((ret = (QString(getenv("HOME")) + "/.tmake/" + file))))
	return "";
    return ret;
}


void
DspMakefileGenerator::processPrlVariable(const QString &var, const QStringList &l)
{
    if(var == "QMAKE_PRL_DEFINES") {
	QStringList &out = project->variables()["MSVCDSP_DEFINES"];
	for(QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
	    if(out.findIndex((*it)) == -1)
		out.append((" /D \"" + *it + "\""));
	}
    } else {
	MakefileGenerator::processPrlVariable(var, l);
    }
}


int
DspMakefileGenerator::beginGroupForFile(QString file, QTextStream &t,
					QString filter)
{
    if(project->isActiveConfig("flat"))
	return 0;

    fileFixify(file, QDir::currentDirPath(), QDir::currentDirPath(), TRUE);
    file = file.section(Option::dir_sep, 0, -2);
    if(file.right(Option::dir_sep.length()) != Option::dir_sep)
	file += Option::dir_sep;
    if(file == currentGroup)
	return 0;

    if(file.isEmpty() || !QDir::isRelativePath(file)) {
	endGroups(t);
	return 0;
    }
    if(file.startsWith(currentGroup))
	file = file.mid(currentGroup.length());
    else
	endGroups(t);
    int lvl = file.contains(Option::dir_sep), old_lvl = currentGroup.contains(Option::dir_sep);
    if(lvl > old_lvl) {
	QStringList dirs = QStringList::split(Option::dir_sep, file);
	for(QStringList::Iterator dir_it = dirs.begin(); dir_it != dirs.end(); ++dir_it) {
	    t << "# Begin Group \"" << (*dir_it) << "\"\n"
	      << "# Prop Default_Filter \"" << filter << "\"\n";
	}
    } else {
	for(int x = old_lvl - lvl; x; x--)
	    t << "\n# End Group\n";
    }
    currentGroup = file;
    return lvl - old_lvl;
}


int
DspMakefileGenerator::endGroups(QTextStream &t)
{
    if(project->isActiveConfig("flat"))
	return 0;
    else if(currentGroup.isEmpty())
	return 0;

    QStringList dirs = QStringList::split(Option::dir_sep, currentGroup);
    for(QStringList::Iterator dir_it = dirs.end(); dir_it != dirs.begin(); --dir_it) {
	t << "\n# End Group\n";
    }
    currentGroup = "";
    return dirs.count();
}

bool
DspMakefileGenerator::openOutput(QFile &file) const
{
    QString outdir;
    if(!file.name().isEmpty()) {
	QFileInfo fi(file);
	if(fi.isDir())
	    outdir = file.name() + QDir::separator();
    }
    if(!outdir.isEmpty() || file.name().isEmpty())
	file.setName(outdir + project->first("TARGET") + project->first("DSP_EXTENSION"));
    if(QDir::isRelativePath(file.name())) {
	QString ofile;
	ofile = file.name();
	int slashfind = ofile.findRev('\\');
	if (slashfind == -1) {
	    ofile = ofile.replace(QRegExp("-"), "_");
	} else {
	    int hypenfind = ofile.find('-', slashfind);
	    while (hypenfind != -1 && slashfind < hypenfind) {
		ofile = ofile.replace(hypenfind, 1, "_");
		hypenfind = ofile.find('-', hypenfind + 1);
	    }
	}
	file.setName(Option::fixPathToLocalOS(QDir::currentDirPath() + Option::dir_sep + ofile));
    }
    return Win32MakefileGenerator::openOutput(file);
}

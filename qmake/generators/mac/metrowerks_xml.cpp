/****************************************************************************
** 
**
** Implementation of MetrowerksMakefileGenerator class.
**
** Copyright (C) 1992-2003 Trolltech AS.  All rights reserved.
**
** This file is part of qmake.
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

#include "metrowerks_xml.h"
#include "option.h"
#include <qdir.h>
#include <qdict.h>
#include <qregexp.h>
#include <stdlib.h>
#include <time.h>
#if !defined(QWS) && defined(Q_OS_MAC)
#include <Carbon/Carbon.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

MetrowerksMakefileGenerator::MetrowerksMakefileGenerator(QMakeProject *p) : MakefileGenerator(p), init_flag(FALSE)
{

}

bool
MetrowerksMakefileGenerator::writeMakefile(QTextStream &t)
{
    if(!project->variables()["QMAKE_FAILED_REQUIREMENTS"].isEmpty()) {
	/* for now just dump, I need to generated an empty xml or something.. */
	fprintf(stderr, "Project file not generated because all requirements not met:\n\t%s\n",
		var("QMAKE_FAILED_REQUIREMENTS").latin1());
	return TRUE;
    }

    if(project->first("TEMPLATE") == "app" ||
       project->first("TEMPLATE") == "lib") {
	return writeMakeParts(t);
    }
    else if(project->first("TEMPLATE") == "subdirs") {
	writeHeader(t);
	qDebug("Not supported!");
	return TRUE;
    }
    return FALSE;
}

bool
MetrowerksMakefileGenerator::writeMakeParts(QTextStream &t)
{
    //..grrr.. libs!
    QStringList extra_objs;
    bool do_libs = TRUE;
    if(project->first("TEMPLATE") == "app") 
	extra_objs += project->variables()["QMAKE_CRT_OBJECTS"];
    else if(project->first("TEMPLATE") == "lib" && project->isActiveConfig("staticlib"))
	do_libs = FALSE;
    if(do_libs)
	extra_objs += project->variables()["QMAKE_LIBS"];
    for(QStringList::Iterator val_it = extra_objs.begin(); 
	val_it != extra_objs.end(); ++val_it) {
	if((*val_it).startsWith("-L")) {
	    QString dir((*val_it).right((*val_it).length() - 2));
	    fixEnvVariables(dir);
	    if(project->variables()["DEPENDPATH"].findIndex(dir) == -1 &&
	       project->variables()["INCLUDEPATH"].findIndex(dir) == -1)
		project->variables()["INCLUDEPATH"].append(dir);
	} else if((*val_it).startsWith("-l")) {
	    QString lib("lib" + (*val_it).right((*val_it).length() - 2)  + "." + 
			project->first("QMAKE_EXTENSION_SHLIB"));
	    if(project->variables()["LIBRARIES"].findIndex(lib) == -1)
		project->variables()["LIBRARIES"].append(lib);
	} else 
	    if((*val_it) == "-framework") {
	    ++val_it;
	    if(val_it == extra_objs.end())
		break;
	    QString frmwrk = (*val_it) + ".framework";
	    if(project->variables()["FRAMEWORKS"].findIndex(frmwrk) == -1)
		project->variables()["FRAMEWORKS"].append(frmwrk);
	} else if((*val_it).left(1) != "-") {
	    QString lib=(*val_it);
	    int s = lib.findRev('/');
	    if(s != -1) {
		QString dir = lib.left(s);
		lib = lib.right(lib.length() - s - 1);
		fixEnvVariables(dir);
		if(project->variables()["DEPENDPATH"].findIndex(dir) == -1 &&
		   project->variables()["INCLUDEPATH"].findIndex(dir) == -1)
		    project->variables()["INCLUDEPATH"].append(dir);
	    }
	    project->variables()["LIBRARIES"].append(lib);
	}
    }
    //let metrowerks find the files & set the files to the type I expect
    QDict<void> seen(293);
    QString paths[] = { QString("SRCMOC"), QString("FORMS"), QString("UICDECLS"),
			QString("UICIMPLS"), QString("SOURCES"),QString("HEADERS"),
			QString::null };
    for(int y = 0; paths[y] != QString::null; y++) {
	QStringList &l = project->variables()[paths[y]];
	for(QStringList::Iterator val_it = l.begin(); val_it != l.end(); ++val_it) {
	    //establish file types
	    seen.insert((*val_it), (void *)1);
	    createFork((*val_it)); //the file itself
	    QStringList &d = findDependencies((*val_it)); //depends
	    for(QStringList::Iterator dep_it = d.begin(); dep_it != d.end(); ++dep_it) {
		if(!seen.find((*dep_it))) {
		    seen.insert((*dep_it), (void *)1);
		    createFork((*dep_it));
		}
	    }
	    //now chop it
	    int s = (*val_it).findRev('/');
	    if(s != -1) {
		QString dir = (*val_it).left(s);
		(*val_it) = (*val_it).right((*val_it).length() - s - 1);
		QString tmpd=dir, tmpv;
		if(fixifyToMacPath(tmpd, tmpv)) {
		    bool add_in = TRUE;
		    QString deps[] = { QString("DEPENDPATH"), 
				     QString("INCLUDEPATH"), QString::null }, 
				     dd, dv;
		    for(int yy = 0; deps[yy] != QString::null; yy++) {
			QStringList &l2 = project->variables()[deps[yy]];
			for(QStringList::Iterator val_it2 = l2.begin(); 
			    val_it2 != l2.end(); ++val_it2) {
			    QString dd= (*val_it2), dv;
			    if(!fixifyToMacPath(dd, dv)) 
				continue;
			    if(dd == tmpd && tmpv == dv) {
				add_in = FALSE;
				break;
			    }
			}
		    }
		    if(add_in) 
			project->variables()["INCLUDEPATH"].append(dir);
		}
	    }
	}
    }
    //need a defines file
    if(!project->isEmpty("DEFINES")) {
	QString pre_pref = project->first("TARGET_STEM");
	if(project->first("TEMPLATE") == "lib")
	    pre_pref += project->isActiveConfig("staticlib") ? "_static" : "_shared";
	project->variables()["CODEWARRIOR_PREFIX_HEADER"].append(pre_pref + "_prefix.h");
    }

    QString xmlfile = findTemplate(project->first("QMAKE_XML_TEMPLATE"));
    QFile file(xmlfile);
    if(!file.open(IO_ReadOnly )) {
	fprintf(stderr, "Cannot open XML file: %s\n", 
		project->first("QMAKE_XML_TEMPLATE").latin1());
	return FALSE;
    }
    QTextStream xml(&file);
    createFork(Option::output.name());

    int rep;
    QString line;
    while ( !xml.eof() ) {
	line = xml.readLine();
	while((rep = line.find(QRegExp("\\$\\$[!a-zA-Z0-9_-]*"))) != -1) {
	    QString torep = line.mid(rep, line.find(QRegExp("[^\\$!a-zA-Z0-9_-]"), rep) - rep);
	    QString variable = torep.right(torep.length()-2);

	    t << line.left(rep); //output the left side
	    line = line.right(line.length() - (rep + torep.length())); //now past the variable
	    if(variable == "CODEWARRIOR_HEADERS" || variable == "CODEWARRIOR_SOURCES" || 
	       variable == "CODEWARRIOR_LIBRARIES" || variable == "CODEWARRIOR_QPREPROCESS" ||
		variable == "CODEWARRIOR_QPREPROCESSOUT") {
		QString outcmd=variable.right(variable.length() - variable.findRev('_') - 1);
		QStringList args;
		if(outcmd == "QPREPROCESS")
		    args << "UICS" << "MOCS";
		else if(outcmd == "QPREPROCESSOUT")
		    args << "SRCMOC" << "UICIMPLS" << "UICDELCS";
		else
		    args << outcmd;
		for(QStringList::Iterator arit = args.begin(); arit != args.end(); ++arit) {
		    QString arg = (*arit);
		    QString kind = "Text";
		    if(arg == "LIBRARIES")
			kind = "Library";
		    if(!project->variables()[arg].isEmpty()) {
			QStringList &list = project->variables()[arg];
			for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
			    QString flag;
			    if(project->isActiveConfig("debug")) {
				bool debug = TRUE;
				if(outcmd == "QPREPROCESS") {
				    debug = FALSE;
				} else {
				    for(QStringList::Iterator hit = Option::h_ext.begin(); hit != Option::h_ext.end(); ++hit) {
					if((*it).endsWith((*hit))) { 
					    debug = FALSE;
					    break;
					}
				    }
				}
				if(debug)
				    flag = "Debug";
			    }
			    t << "\t\t\t\t<FILE>" << endl
			      << "\t\t\t\t\t<PATHTYPE>Name</PATHTYPE>" << endl
			      << "\t\t\t\t\t<PATH>" << (*it) << "</PATH>" << endl
			      << "\t\t\t\t\t<PATHFORMAT>MacOS</PATHFORMAT>" << endl
			      << "\t\t\t\t\t<FILEKIND>" << kind << "</FILEKIND>" << endl
			      << "\t\t\t\t\t<FILEFLAGS>" << flag << "</FILEFLAGS>" << endl
			      << "\t\t\t\t</FILE>" << endl;
			}
		    }
		}
	    } else if(variable == "CODEWARRIOR_SOURCES_LINKORDER" || 
		      variable == "CODEWARRIOR_HEADERS_LINKORDER" ||
		      variable == "CODEWARRIOR_LIBRARIES_LINKORDER" || 
		      variable == "CODEWARRIOR_QPREPROCESS_LINKORDER" ||
		      variable == "CODEWARRIOR_QPREPROCESSOUT_LINKORDER") {
		QString outcmd=variable.mid(variable.find('_')+1, 
					    variable.findRev('_')-(variable.find('_')+1));
		QStringList args;
		if(outcmd == "QPREPROCESS")
		    args << "UICS" << "MOCS";
		else if(outcmd == "QPREPROCESSOUT")
		    args << "SRCMOC" << "UICIMPLS" << "UICDELCS";
		else
		    args << outcmd;
		for(QStringList::Iterator arit = args.begin(); arit != args.end(); ++arit) {
		    QString arg = (*arit);
		    if(!project->variables()[arg].isEmpty()) {
			QStringList &list = project->variables()[arg];
			for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
			    t << "\t\t\t\t<FILEREF>" << endl
			      << "\t\t\t\t\t<PATHTYPE>Name</PATHTYPE>" << endl
			      << "\t\t\t\t\t<PATH>" << (*it) << "</PATH>" << endl
			      << "\t\t\t\t\t<PATHFORMAT>MacOS</PATHFORMAT>" << endl
			      << "\t\t\t\t</FILEREF>" << endl;
			}
		    }
		}
	    } else if(variable == "CODEWARRIOR_HEADERS_GROUP" || 
		      variable == "CODEWARRIOR_SOURCES_GROUP" ||
		      variable == "CODEWARRIOR_LIBRARIES_GROUP" || 
		      variable == "CODEWARRIOR_QPREPROCESS_GROUP" ||
		      variable == "CODEWARRIOR_QPREPROCESSOUT_GROUP") {
		QString outcmd = variable.mid(variable.find('_')+1, 
					      variable.findRev('_')-(variable.find('_')+1));
		QStringList args;
		if(outcmd == "QPREPROCESS")
		    args << "UICS" << "MOCS";
		else if(outcmd == "QPREPROCESSOUT")
		    args << "SRCMOC" << "UICIMPLS" << "UICDELCS";
		else
		    args << outcmd;
		for(QStringList::Iterator arit = args.begin(); arit != args.end(); ++arit) {
		    QString arg = (*arit);
		    if(!project->variables()[arg].isEmpty()) {
			QStringList &list = project->variables()[arg];
			for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
			    t << "\t\t\t\t<FILEREF>" << endl
			      << "\t\t\t\t\t<TARGETNAME>" << var("TARGET_STEM") << "</TARGETNAME>" 
			      << endl
			      << "\t\t\t\t\t<PATHTYPE>Name</PATHTYPE>" << endl
			      << "\t\t\t\t\t<PATH>" << (*it) << "</PATH>" << endl
			      << "\t\t\t\t\t<PATHFORMAT>MacOS</PATHFORMAT>" << endl
			      << "\t\t\t\t</FILEREF>" << endl;
			}
		    }
		}
	    } else if(variable == "CODEWARRIOR_FRAMEWORKS") {
		if(!project->isEmpty("FRAMEWORKS")) {
		    QStringList &list = project->variables()["FRAMEWORKS"];
		    for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
			t << "\t\t\t\t<FRAMEWORK>" << endl
			  << "\t\t\t\t\t<FILEREF>" << endl
			  << "\t\t\t\t\t\t<PATHTYPE>Name</PATHTYPE>" << endl
			  << "\t\t\t\t\t\t<PATH>" << (*it) << "</PATH>" << endl
			  << "\t\t\t\t\t\t<PATHFORMAT>MacOS</PATHFORMAT>" << endl
			  << "\t\t\t\t\t</FILEREF>" << endl
			  << "\t\t\t\t</FRAMEWORK>" << endl;
		    }
		}
	    } else if(variable == "CODEWARRIOR_DEPENDPATH" || variable == "CODEWARRIOR_INCLUDEPATH" ||
		variable == "CODEWARRIOR_FRAMEWORKPATH") {	
		QString arg=variable.right(variable.length()-variable.find('_')-1);
		QStringList list;
		if(arg == "INCLUDEPATH") {
		    list = project->variables()[arg];
		    list << Option::mkfile::qmakespec;
		    list << QDir::current().currentDirPath();

		    QStringList &l = project->variables()["QMAKE_LIBS_PATH"];
		    for(QStringList::Iterator val_it = l.begin(); val_it != l.end(); ++val_it) {
			QString p = (*val_it), v;
			if(!fixifyToMacPath(p, v))
			    continue;

			t << "\t\t\t\t\t<SETTING>" << endl
			  << "\t\t\t\t\t\t<SETTING><NAME>SearchPath</NAME>" << endl
			  << "\t\t\t\t\t\t\t<SETTING><NAME>Path</NAME>"
			  << "<VALUE>" << p << "</VALUE></SETTING>" << endl
			  << "\t\t\t\t\t\t\t<SETTING><NAME>PathFormat</NAME><VALUE>MacOS</VALUE></SETTING>" << endl
			  << "\t\t\t\t\t\t\t<SETTING><NAME>PathRoot</NAME><VALUE>CodeWarrior</VALUE></SETTING>" << endl
			  << "\t\t\t\t\t\t</SETTING>" << endl
			  << "\t\t\t\t\t\t<SETTING><NAME>Recursive</NAME><VALUE>true</VALUE></SETTING>" << endl
			  << "\t\t\t\t\t\t<SETTING><NAME>HostFlags</NAME><VALUE>All</VALUE></SETTING>" << endl
			  << "\t\t\t\t\t</SETTING>" << endl;
		    }
		} else if(variable == "DEPENDPATH") {
		    QStringList &l = project->variables()[arg];
		    for(QStringList::Iterator val_it = l.begin(); val_it != l.end(); ++val_it)
		    {
			//apparently tmake used colon separation...
			QStringList damn = QStringList::split(':', (*val_it));
			if(!damn.isEmpty())
			    list += damn;
			else
			    list.append((*val_it));
		    }
		} else {
		    list = project->variables()[arg];
		}
		for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
		    QString p = (*it), v, recursive = "false", framework = "false";
		    if(p.startsWith("recursive--")) {
			p = p.right(p.length() - 11);
			recursive = "true";
		    } 
		    if(!fixifyToMacPath(p, v))
			continue;
		    if(arg == "FRAMEWORKPATH")
			framework = "true";

		    t << "\t\t\t\t\t<SETTING>" << endl
		      << "\t\t\t\t\t\t<SETTING><NAME>SearchPath</NAME>" << endl
		      << "\t\t\t\t\t\t\t<SETTING><NAME>Path</NAME>"
		      << "<VALUE>" << p << "</VALUE></SETTING>" << endl
		      << "\t\t\t\t\t\t\t<SETTING><NAME>PathFormat</NAME><VALUE>MacOS</VALUE></SETTING>" << endl
		      << "\t\t\t\t\t\t\t<SETTING><NAME>PathRoot</NAME><VALUE>" << v << "</VALUE></SETTING>" << endl
		      << "\t\t\t\t\t\t</SETTING>" << endl
		      << "\t\t\t\t\t\t<SETTING><NAME>Recursive</NAME><VALUE>" << recursive << "</VALUE></SETTING>" << endl
		    << "\t\t\t\t\t\t<SETTING><NAME>FrameworkPath</NAME><VALUE>" << framework << "</VALUE></SETTING>" << endl
		    << "\t\t\t\t\t\t<SETTING><NAME>HostFlags</NAME><VALUE>All</VALUE></SETTING>" << endl
		      << "\t\t\t\t\t</SETTING>" << endl;
		}
	    } else if(variable == "CODEWARRIOR_WARNING" || variable == "!CODEWARRIOR_WARNING") {
		bool b = ((!project->isActiveConfig("warn_off")) && 
			  project->isActiveConfig("warn_on"));
		if(variable.startsWith("!"))
		    b = !b;
		t << (int)b;
	    } else if(variable == "CODEWARRIOR_TEMPLATE") {
		if(project->first("TEMPLATE") == "app" ) {
		    t << "Executable";
		} else if(project->first("TEMPLATE") == "lib") {
		    if(project->isActiveConfig("staticlib"))
		       t << "Library";
		    else
			t << "SharedLibrary";
		}
	    } else if(variable == "CODEWARRIOR_OUTPUT_DIR") {
		QString outdir = "{Project}/", volume;
		if(!project->isEmpty("DESTDIR"))
		    outdir = project->first("DESTDIR");
		if(project->first("TEMPLATE") == "app" && !project->isActiveConfig("console"))
		    outdir += var("TARGET") + ".app/Contents/MacOS/";
		if(fixifyToMacPath(outdir, volume, FALSE)) {
                    t << "\t\t\t<SETTING><NAME>Path</NAME><VALUE>" << outdir << "</VALUE></SETTING>" 
		      << endl
		      << "\t\t\t<SETTING><NAME>PathFormat</NAME><VALUE>MacOS</VALUE></SETTING>" << endl
		      << "\t\t\t<SETTING><NAME>PathRoot</NAME><VALUE>" << volume << "</VALUE></SETTING>" 
		      << endl;
		}
	    } else if(variable == "CODEWARRIOR_PACKAGER_PANEL") {
		if(project->first("TEMPLATE") == "app" && !project->isActiveConfig("console")) {
		    QString outdir = "{Project}/", volume;
		    if(!project->isEmpty("DESTDIR"))
			outdir = project->first("DESTDIR");
		    outdir += var("TARGET") + ".app";
		    if(fixifyToMacPath(outdir, volume, FALSE)) {
			t << "\t\t<SETTING><NAME>MWMacOSPackager_UsePackager</NAME>"
			  << "<VALUE>1</VALUE></SETTING>" << "\n"
			  << "\t\t<SETTING><NAME>MWMacOSPackager_FolderToPackage</NAME>" << "\n"
			  << "\t\t\t<SETTING><NAME>Path</NAME><VALUE>" << outdir 
			  << "</VALUE></SETTING>" << "\n"
			  << "\t\t\t<SETTING><NAME>PathFormat</NAME><VALUE>MacOS</VALUE></SETTING>" 
			  << "\n"
			  << "\t\t\t<SETTING><NAME>PathRoot</NAME><VALUE>" << volume 
			  << "</VALUE></SETTING>" << "\n"
			  << "\t\t</SETTING>" << "\n"
			  << "\t\t<SETTING><NAME>MWMacOSPackager_CreateClassicAlias</NAME>"
			  << "<VALUE>0</VALUE></SETTING>" << "\n"
			  << "\t\t<SETTING><NAME>MWMacOSPackager_ClassicAliasMethod</NAME>"
			  << "<VALUE>UseTargetOutput</VALUE></SETTING>" << "\n"
			  << "\t\t<SETTING><NAME>MWMacOSPackager_ClassicAliasPath</NAME>"
			  << "<VALUE></VALUE></SETTING>" << "\n"
			  << "\t\t<SETTING><NAME>MWMacOSPackager_CreatePkgInfo</NAME>"
			  << "<VALUE>1</VALUE></SETTING>" << "\n"
			  << "\t\t<SETTING><NAME>MWMacOSPackager_PkgCreatorType</NAME>" 
			  << "<VALUE>CUTE</VALUE></SETTING>" << "\n"
			  << "\t\t<SETTING><NAME>MWMacOSPackager_PkgFileType</NAME>" 
			  << "<VALUE>APPL</VALUE></SETTING>" << endl;
		    }
		}
	    } else if(variable == "CODEWARRIOR_FILETYPE") {
		if(project->first("TEMPLATE") == "lib")
			t << "MYDL";
		else
			t << "MEXE";
	    } else if(variable == "CODEWARRIOR_QTDIR") {
		t << getenv("QTDIR");
	    } else if(variable == "CODEWARRIOR_CACHEMODDATES") {
		t << "true";
	    } else {
		t << var(variable);
	    }
	}
	t << line << endl;
    }
    t << endl;
    file.close();

    if(mocAware()) { 
	QString mocs = project->first("MOCS");
	QFile mocfile(mocs);
	if(!mocfile.open(IO_WriteOnly)) {
	    fprintf(stderr, "Cannot open MOCS file: %s\n", mocs.latin1());
	} else {
	    createFork(mocs);
	    QTextStream mocs(&mocfile);
	    QStringList &list = project->variables()["SRCMOC"];
	    for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
		QString src = findMocSource((*it));
		if(src.findRev('/') != -1)
		    src = src.right(src.length() - src.findRev('/') - 1);
		mocs << src << endl;
	    }
	    mocfile.close();
	}
    }

    if(!project->isEmpty("FORMS")) { 
	QString uics = project->first("UICS");
	QFile uicfile(uics);
	if(!uicfile.open(IO_WriteOnly)) {
	    fprintf(stderr, "Cannot open UICS file: %s\n", uics.latin1());
	} else {
	    createFork(uics);
	    QTextStream uics(&uicfile);
	    QStringList &list = project->variables()["FORMS"];
	    for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
		QString ui = (*it);
		if(ui.findRev('/') != -1)
		    ui = ui.right(ui.length() - ui.findRev('/') - 1);
		uics << ui << endl;
	    }
	    uicfile.close();
	}
    }

    if(!project->isEmpty("CODEWARRIOR_PREFIX_HEADER")) {
	QFile prefixfile(project->first("CODEWARRIOR_PREFIX_HEADER"));
	if(!prefixfile.open(IO_WriteOnly)) {
	    fprintf(stderr, "Cannot open PREFIX file: %s\n", prefixfile.name().latin1());
	} else {
	    createFork(project->first("CODEWARRIOR_PREFIX_HEADER"));
	    QTextStream prefix(&prefixfile);
	    QStringList &list = project->variables()["DEFINES"];
	    for(QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
		if((*it).find('=') != -1) {
		    int x = (*it).find('=');
		    prefix << "#define " << (*it).left(x) << " " << (*it).right((*it).length() - x - 1) << endl;
		} else {
		    prefix << "#define " << (*it) << endl;
		}
	    }
	    prefixfile.close();
	}
    }
    return TRUE;
}



void
MetrowerksMakefileGenerator::init()
{
    if(init_flag)
	return;
    init_flag = TRUE;

    if ( project->isEmpty("QMAKE_XML_TEMPLATE") ) 
	project->variables()["QMAKE_XML_TEMPLATE"].append("mwerkstmpl.xml");

    QStringList &configs = project->variables()["CONFIG"];
    if(project->isActiveConfig("qt")) {
	if(configs.findIndex("moc")) configs.append("moc");
	if ( !( (project->first("TARGET") == "qt") || (project->first("TARGET") == "qte") ||
		(project->first("TARGET") == "qt-mt") ) ) 
	    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT"];
	if(configs.findIndex("moc")) 
	    configs.append("moc");
	if ( !project->isActiveConfig("debug") ) 
	    project->variables()["DEFINES"].append("QT_NO_DEBUG");
    }

    //version handling
    if(project->variables()["VERSION"].isEmpty())
	project->variables()["VERSION"].append("1.0." + 
					       (project->isEmpty("VER_PAT") ? QString("0") : 
						project->first("VER_PAT")) );
    QStringList ver = QStringList::split('.', project->first("VERSION"));
    ver << "0" << "0"; //make sure there are three
    project->variables()["VER_MAJ"].append(ver[0]);
    project->variables()["VER_MIN"].append(ver[1]);
    project->variables()["VER_PAT"].append(ver[2]);

    if( !project->isEmpty("LIBS") )
	project->variables()["QMAKE_LIBS"] += project->variables()["LIBS"];
    if( project->variables()["QMAKE_EXTENSION_SHLIB"].isEmpty() )
	project->variables()["QMAKE_EXTENSION_SHLIB"].append( "dylib" );

    if ( project->isActiveConfig("moc") ) {
	QString mocfile = project->first("TARGET");
	if(project->first("TEMPLATE") == "lib")
	    mocfile += project->isActiveConfig("staticlib") ? "_static" : "_shared";
	project->variables()["MOCS"].append(mocfile + ".mocs");
	setMocAware(TRUE);
    }
    if(!project->isEmpty("FORMS")) {
	QString uicfile = project->first("TARGET");
	if(project->first("TEMPLATE") == "lib")
	    uicfile += project->isActiveConfig("staticlib") ? "_static" : "_shared";
	project->variables()["UICS"].append(uicfile + ".uics");
    }
    if(project->isEmpty("DESTDIR"))
	project->variables()["DESTDIR"].append(QDir::currentDirPath());
    MakefileGenerator::init();

    if ( project->isActiveConfig("opengl") ) {
	project->variables()["INCLUDEPATH"] += project->variables()["QMAKE_INCDIR_OPENGL"];
	if ( (project->first("TARGET") == "qt") || (project->first("TARGET") == "qte") ||
	     (project->first("TARGET") == "qt-mt") )
	    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_OPENGL_QT"];
	else 
	    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_OPENGL"];
    }

    if(project->isActiveConfig("qt"))
	project->variables()["INCLUDEPATH"] += project->variables()["QMAKE_INCDIR_QT"];
    if(project->isEmpty("FRAMEWORKPATH"))
	project->variables()["FRAMEWORKPATH"].append("/System/Library/Frameworks/");

    //set the target up
    project->variables()["TARGET_STEM"] = project->variables()["TARGET"];
    if(project->first("TEMPLATE") == "lib") {
	if(project->isActiveConfig("staticlib"))
	    project->variables()["TARGET"].first() =  "lib" + project->first("TARGET") + ".lib";
	else
	    project->variables()["TARGET"].first() =  "lib" + project->first("TARGET") + "." +
						      project->first("QMAKE_EXTENSION_SHLIB");

	project->variables()["CODEWARRIOR_VERSION"].append(project->first("VER_MAJ") +
							  project->first("VER_MIN") +
							  project->first("VER_PAT"));
    } else {
	project->variables()["CODEWARRIOR_VERSION"].append("0");
	if(project->isEmpty("QMAKE_ENTRYPOINT"))
	   project->variables()["QMAKE_ENTRYPOINT"].append("start");
	project->variables()["CODEWARRIOR_ENTRYPOINT"].append(
	    project->first("QMAKE_ENTRYPOINT"));
    }
}


QString
MetrowerksMakefileGenerator::findTemplate(const QString &file)
{
    QString ret;
    if(!QFile::exists(ret = file) && 
       !QFile::exists((ret = Option::mkfile::qmakespec + QDir::separator() + file)) && 
       !QFile::exists((ret = QString(getenv("QTDIR")) + "/mkspecs/mac-mwerks/" + file)) &&
       !QFile::exists((ret = (QString(getenv("HOME")) + "/.tmake/" + file))))
	return "";
    return ret;
}

bool
MetrowerksMakefileGenerator::createFork(const QString &f)
{
#if !defined(QWS) && defined(Q_OS_MACX)
    FSRef fref;
    FSSpec fileSpec;
    if(QFile::exists(f)) {
	mode_t perms = 0;
	{
	    struct stat s;
	    stat(f.latin1(), &s);
	    if(!(s.st_mode & S_IWUSR)) {
		perms = s.st_mode;
		chmod(f.latin1(), perms | S_IWUSR);
	    }
	}
	FILE *o = fopen(f.latin1(), "a");
	if(!o)
	    return FALSE;
	if(FSPathMakeRef((const UInt8 *)f.latin1(), &fref, NULL) == noErr) {
	    if(FSGetCatalogInfo(&fref, kFSCatInfoNone, NULL, NULL, &fileSpec, NULL) == noErr) 
		FSpCreateResFile(&fileSpec, 'CUTE', 'TEXT', smSystemScript);
	    else 
		qDebug("bogus %d", __LINE__);
	} else 
	    qDebug("bogus %d", __LINE__);
	fclose(o);
	if(perms)
	    chmod(f.latin1(), perms);
    }
#else
    Q_UNUSED(f)
#endif
    return TRUE;
}

bool
MetrowerksMakefileGenerator::fixifyToMacPath(QString &p, QString &v, bool )
{
    v = "Absolute";
    if(p.find(':') != -1) //guess its macish already
	return TRUE;

    static QString st_volume;
    if(st_volume.isEmpty()) {
	st_volume = var("QMAKE_VOLUMENAME");
#if !defined(QWS) && defined(Q_OS_MACX)
	if(st_volume.isEmpty()) {
	    uchar foo[512];
	    HVolumeParam pb;
	    memset(&pb, '\0', sizeof(pb));
	    pb.ioVRefNum = 0;
	    pb.ioNamePtr = foo;
	    if(PBHGetVInfoSync((HParmBlkPtr)&pb) == noErr) {
		int len = foo[0];
		memcpy(foo,foo+1, len);
		foo[len] = '\0';
		st_volume = (char *)foo;
	    }
	}
#endif
    }
    QString volume = st_volume;

    fixEnvVariables(p);
    if(p.startsWith("\"") && p.endsWith("\""))
	p = p.mid(1, p.length() - 2);
    if(p.isEmpty()) 
	return FALSE;
    if(!p.endsWith("/"))
	p += "/";
    if(QDir::isRelativePath(p)) {
	if(p.startsWith("{")) {
	    int eoc = p.find('}');
	    if(eoc == -1)
		return FALSE;
	    volume = p.mid(1, eoc - 1);
	    p = p.right(p.length() - eoc - 1);
	} else {
	    QFileInfo fi(p);
	    if(fi.convertToAbs()) //strange
		return FALSE;
	    p = fi.filePath();
	} 
    } 
    p = QDir::cleanDirPath(p);
    if(!volume.isEmpty()) 
	v = volume;
    p.replace("/", ":");
    if(p.right(1) != ":")
	p += ':';
    return TRUE;
}

void
MetrowerksMakefileGenerator::processPrlFiles()
{
    QPtrList<MakefileDependDir> libdirs;
    libdirs.setAutoDelete(TRUE);
    const QString lflags[] = { "QMAKE_LIBS", QString::null };
    for(int i = 0; !lflags[i].isNull(); i++) {
	for(bool ret = FALSE; TRUE; ret = FALSE) {
	    QStringList l_out;
	    QStringList &l = project->variables()[lflags[i]];
	    for(QStringList::Iterator it = l.begin(); it != l.end(); ++it) {
		QString opt = (*it);
		if(opt.startsWith("-")) {
		    if(opt.startsWith("-L")) {
			QString r = opt.right(opt.length() - 2), l = r;
			fixEnvVariables(l);
			libdirs.append(new MakefileDependDir(r.replace( "\"", ""),
							     l.replace( "\"", "")));
		    } else if(opt.left(2) == "-l") {
			QString lib = opt.right(opt.length() - 2), prl;
			for(MakefileDependDir *mdd = libdirs.first(); mdd; mdd = libdirs.next() ) {
			    prl = mdd->local_dir + Option::dir_sep + "lib" + lib;
			    if(processPrlFile(prl)) {
				if(prl.startsWith(mdd->local_dir))
				    prl.replace(0, mdd->local_dir.length(), mdd->real_dir);
				QRegExp reg("^.*lib(" + lib + "[^.]*)\\." + 
					    project->first("QMAKE_EXTENSION_SHLIB") + "$");
				if(reg.exactMatch(prl))
				    prl = "-l" + reg.cap(1);
				opt = prl;
				ret = TRUE;
				break;
			    }
			}
		    } else if(opt == "-framework") {
			l_out.append(opt);
			++it;
			opt = (*it);
			QString prl = "/System/Library/Frameworks/" + opt +
				      ".framework/" + opt;
			if(processPrlFile(prl))
			    ret = TRUE;
		    }
		    if(!opt.isEmpty())
			l_out.append(opt);
		} else {
		    if(processPrlFile(opt))
			ret = TRUE;
		    if(!opt.isEmpty())
			l_out.append(opt);
		}
	    }
	    if(ret)
		l = l_out;
	    else
		break;
	}
    }
}

void
MetrowerksMakefileGenerator::processPrlVariable(const QString &var, const QStringList &l)
{
    if(var == "QMAKE_PRL_LIBS") {
	QStringList &out = project->variables()["QMAKE_LIBS"];
	for(QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
	    bool append = TRUE;
	    if((*it).startsWith("-")) {
		if((*it).startsWith("-l") || (*it).startsWith("-L")) {
		    append = out.findIndex((*it)) == -1;
		} else if((*it).startsWith("-framework")) {
		    ++it;
		    for(QStringList::ConstIterator outit = out.begin(); 
			outit != out.end(); ++it) {
			if((*outit) == "-framework") {
			    ++outit;
			    if((*outit) == (*it)) {
				append = FALSE;
				break;
			    }
			}
		    }
		}
	    } else if(QFile::exists((*it))) {
		append = out.findIndex((*it));
	    }
	    if(append)
		out.append((*it));
	}
    } else {
	MakefileGenerator::processPrlVariable(var, l);
    }
}


bool
MetrowerksMakefileGenerator::openOutput(QFile &file) const
{
    QString outdir;
    if(!file.name().isEmpty()) {
	QFileInfo fi(file);
	if(fi.isDir())
	    outdir = file.name() + QDir::separator();
    }
    if(!outdir.isEmpty() || file.name().isEmpty()) 
	file.setName(outdir + project->first("TARGET") + ".xml");
    return MakefileGenerator::openOutput(file);
}

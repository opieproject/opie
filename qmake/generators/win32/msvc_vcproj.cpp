/****************************************************************************
** $Id: msvc_vcproj.cpp,v 1.2 2003-07-10 02:40:10 llornkcor Exp $
**
** Definition of VcprojGenerator class.
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

#include "msvc_vcproj.h"
#include "option.h"
#include <qdir.h>
#include <qregexp.h>
#include <qdict.h>
#include <quuid.h>
#include <stdlib.h>

#if defined(Q_OS_WIN32)
#include <objbase.h>
#ifndef GUID_DEFINED
#define GUID_DEFINED
typedef struct _GUID
{
    ulong   Data1;
    ushort  Data2;
    ushort  Data3;
    uchar   Data4[8];
} GUID;
#endif
#endif

// Flatfile Tags ----------------------------------------------------
const char* _snlHeader		= "Microsoft Visual Studio Solution File, Format Version 7.00";
				  // The following UUID _may_ change for later servicepacks...
				  // If so we need to search through the registry at
				  // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\7.0\Projects
				  // to find the subkey that contains a "PossibleProjectExtension"
				  // containing "vcproj"...
				  // Use the hardcoded value for now so projects generated on other
				  // platforms are actually usable.
const char* _snlMSVCvcprojGUID  = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";
const char* _snlProjectBeg	= "\nProject(\"";
const char* _snlProjectMid	= "\") = ";
const char* _snlProjectEnd	= "\nEndProject";
const char* _snlGlobalBeg	= "\nGlobal";
const char* _snlGlobalEnd	= "\nEndGlobal";
const char* _snlSolutionConf	= "\n\tGlobalSection(SolutionConfiguration) = preSolution"
				  "\n\t\tConfigName.0 = Release"
				  "\n\tEndGlobalSection";
const char* _snlProjDepBeg	= "\n\tGlobalSection(ProjectDependencies) = postSolution";
const char* _snlProjDepEnd	= "\n\tEndGlobalSection";
const char* _snlProjConfBeg	= "\n\tGlobalSection(ProjectConfiguration) = postSolution";
const char* _snlProjConfTag1	= ".Release.ActiveCfg = Release|Win32";
const char* _snlProjConfTag2	= ".Release.Build.0 = Release|Win32";
const char* _snlProjConfEnd	= "\n\tEndGlobalSection";
const char* _snlExtSections	= "\n\tGlobalSection(ExtensibilityGlobals) = postSolution"
				  "\n\tEndGlobalSection"
				  "\n\tGlobalSection(ExtensibilityAddIns) = postSolution"
				  "\n\tEndGlobalSection";
// ------------------------------------------------------------------

VcprojGenerator::VcprojGenerator(QMakeProject *p) : Win32MakefileGenerator(p), init_flag(FALSE)
{
}

/* \internal
    Generates a project file for the given profile.
    Options are either a Visual Studio projectfiles, or
    solutionfiles by parsing recursive projectdirectories.
*/
bool VcprojGenerator::writeMakefile(QTextStream &t)
{
    // Check if all requirements are fullfilled
    if(!project->variables()["QMAKE_FAILED_REQUIREMENTS"].isEmpty()) {
	fprintf(stderr, "Project file not generated because all requirements not met:\n\t%s\n",
		var("QMAKE_FAILED_REQUIREMENTS").latin1());
	return TRUE;
    }

    // Generate project file
    if(project->first("TEMPLATE") == "vcapp" ||
       project->first("TEMPLATE") == "vclib") {
        debug_msg(1, "Generator: MSVC.NET: Writing project file" );
	t << vcProject;
	return TRUE;
    }
    // Generate solution file
    else if(project->first("TEMPLATE") == "vcsubdirs") {
        debug_msg(1, "Generator: MSVC.NET: Writing solution file" );
	writeSubDirs(t);
	return TRUE;
    }
    return FALSE;

}

struct VcsolutionDepend {
    QString uuid;
    QString vcprojFile, orig_target, target;
    ::target targetType;
    QStringList dependencies;
};

QUuid VcprojGenerator::increaseUUID( const QUuid &id )
{
    QUuid result( id );
    Q_LONG dataFirst = (result.data4[0] << 24) +
		       (result.data4[1] << 16) +
		       (result.data4[2] << 8) +
                        result.data4[3];
    Q_LONG dataLast =  (result.data4[4] << 24) +
		       (result.data4[5] << 16) +
		       (result.data4[6] <<  8) +
		        result.data4[7];

    if ( !(dataLast++) )
	dataFirst++;

    result.data4[0] = uchar((dataFirst >> 24) & 0xff);
    result.data4[1] = uchar((dataFirst >> 16) & 0xff);
    result.data4[2] = uchar((dataFirst >>  8) & 0xff);
    result.data4[3] = uchar( dataFirst        & 0xff);
    result.data4[4] = uchar((dataLast  >> 24) & 0xff);
    result.data4[5] = uchar((dataLast  >> 16) & 0xff);
    result.data4[6] = uchar((dataLast  >>  8) & 0xff);
    result.data4[7] = uchar( dataLast         & 0xff);
    return result;
}

void VcprojGenerator::writeSubDirs(QTextStream &t)
{
    if(project->first("TEMPLATE") == "subdirs") {
	writeHeader(t);
	Win32MakefileGenerator::writeSubDirs(t);
	return;
    }

    t << _snlHeader;
    QUuid solutionGUID;
#if defined(Q_WS_WIN32)
    GUID guid;
    HRESULT h = CoCreateGuid( &guid );
    if ( h == S_OK )
	solutionGUID = QUuid( guid );
#else
    // Qt doesn't support GUID on other platforms yet,
    // so we use the all-zero uuid, and increase that.
#endif


    QDict<VcsolutionDepend> solution_depends;
    QPtrList<VcsolutionDepend> solution_cleanup;
    solution_cleanup.setAutoDelete(TRUE);
    QStringList subdirs = project->variables()["SUBDIRS"];
    QString oldpwd = QDir::currentDirPath();
    for(QStringList::Iterator it = subdirs.begin(); it != subdirs.end(); ++it) {
	QFileInfo fi(Option::fixPathToLocalOS((*it), TRUE));
	if(fi.exists()) {
	    if(fi.isDir()) {
		QString profile = (*it);
		if(!profile.endsWith(Option::dir_sep))
		    profile += Option::dir_sep;
		profile += fi.baseName() + ".pro";
		subdirs.append(profile);
	    } else {
		QMakeProject tmp_proj;
		QString dir = fi.dirPath(), fn = fi.fileName();
		if(!dir.isEmpty()) {
		    if(!QDir::setCurrent(dir))
			fprintf(stderr, "Cannot find directory: %s\n", dir.latin1());
		}
		if(tmp_proj.read(fn, oldpwd)) {
		    if(tmp_proj.first("TEMPLATE") == "vcsubdirs") {
			QStringList tmp_subdirs = fileFixify(tmp_proj.variables()["SUBDIRS"]);
			subdirs += tmp_subdirs;
		    } else if(tmp_proj.first("TEMPLATE") == "vcapp" || tmp_proj.first("TEMPLATE") == "vclib") {
			// Initialize a 'fake' project to get the correct variables
			// and to be able to extract all the dependencies
			VcprojGenerator tmp_vcproj(&tmp_proj);
			tmp_vcproj.setNoIO(TRUE);
			tmp_vcproj.init();
			if(Option::debug_level) {
			    QMap<QString, QStringList> &vars = tmp_proj.variables();
			    for(QMap<QString, QStringList>::Iterator it = vars.begin();
				it != vars.end(); ++it) {
				if(it.key().left(1) != "." && !it.data().isEmpty())
				    debug_msg(1, "%s: %s === %s", fn.latin1(), it.key().latin1(),
						it.data().join(" :: ").latin1());
			    }
			}

			// We assume project filename is [QMAKE_ORIG_TARGET].vcproj
			QString vcproj = fixFilename(tmp_vcproj.project->first("QMAKE_ORIG_TARGET")) + project->first("VCPROJ_EXTENSION");

			// If file doesn't exsist, then maybe the users configuration
			// doesn't allow it to be created. Skip to next...
			if(!QFile::exists(QDir::currentDirPath() + Option::dir_sep + vcproj)) {
			    qDebug( "Ignored (not found) '%s'",  QString(QDir::currentDirPath() + Option::dir_sep + vcproj).latin1() );
			    goto nextfile; // # Dirty!
			}

			VcsolutionDepend *newDep = new VcsolutionDepend;
			newDep->vcprojFile = fileFixify(vcproj);
			newDep->orig_target = tmp_proj.first("QMAKE_ORIG_TARGET");
			newDep->target = tmp_proj.first("TARGET").section(Option::dir_sep, -1);
			newDep->targetType = tmp_vcproj.projectTarget;
			{
			    static QUuid uuid = solutionGUID;
			    uuid = increaseUUID( uuid );
			    newDep->uuid = uuid.toString().upper();
			}
			if(newDep->target.endsWith(".dll"))
			    newDep->target = newDep->target.left(newDep->target.length()-3) + "lib";
			if(!tmp_proj.isEmpty("FORMS"))
			    newDep->dependencies << "uic.exe";
			{
			    QStringList where("QMAKE_LIBS");
			    if(!tmp_proj.isEmpty("QMAKE_INTERNAL_PRL_LIBS"))
				where = tmp_proj.variables()["QMAKE_INTERNAL_PRL_LIBS"];
			    for(QStringList::iterator wit = where.begin();
				wit != where.end(); ++wit) {
				QStringList &l = tmp_proj.variables()[(*wit)];
				for(QStringList::Iterator it = l.begin(); it != l.end(); ++it) {
				    QString opt = (*it);
				    if(!opt.startsWith("/")) //Not a switch
					newDep->dependencies << opt.section(Option::dir_sep, -1);
				}
			    }
			}
			solution_cleanup.append(newDep);
  			solution_depends.insert(newDep->target, newDep);
  			{
  			    QRegExp libVersion("[0-9]{3,3}\\.lib$");
    	  		    if(libVersion.search(newDep->target) != -1)
	    			solution_depends.insert(newDep->target.left(newDep->target.length() -
	    						libVersion.matchedLength()) + ".lib", newDep);
			}
			t << _snlProjectBeg << _snlMSVCvcprojGUID << _snlProjectMid
			    << "\"" << newDep->orig_target << "\", \"" << newDep->vcprojFile
			    << "\", \"" << newDep->uuid << "\"";
			t << _snlProjectEnd;
		    }
		}
nextfile:
		QDir::setCurrent(oldpwd);
	    }
	}
    }
    t << _snlGlobalBeg;
    t << _snlSolutionConf;
    t << _snlProjDepBeg;
    for(solution_cleanup.first(); solution_cleanup.current(); solution_cleanup.next()) {
	int cnt = 0;
	for(QStringList::iterator dit = solution_cleanup.current()->dependencies.begin();
	    dit != solution_cleanup.current()->dependencies.end();
	    ++dit) {
	    VcsolutionDepend *vc;
	    if((vc=solution_depends[*dit])) {
	    	if(solution_cleanup.current()->targetType != StaticLib || vc->targetType == Application)
		    t << "\n\t\t" << solution_cleanup.current()->uuid << "." << cnt++ << " = " << vc->uuid;
	    }
	}
    }
    t << _snlProjDepEnd;
    t << _snlProjConfBeg;
    for(solution_cleanup.first(); solution_cleanup.current(); solution_cleanup.next()) {
	t << "\n\t\t" << solution_cleanup.current()->uuid << _snlProjConfTag1;
	t << "\n\t\t" << solution_cleanup.current()->uuid << _snlProjConfTag2;
    }
    t << _snlProjConfEnd;
    t << _snlExtSections;
    t << _snlGlobalEnd;
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

void VcprojGenerator::init()
{
    if( init_flag )
	return;
    if(project->first("TEMPLATE") == "vcsubdirs") { //too much work for subdirs
	init_flag = TRUE;
	return;
    }

    debug_msg(1, "Generator: MSVC.NET: Initializing variables" );

/*
    // Once to be nice and clean code...
    // Wouldn't life be great?

    // Are we building Qt?
    bool is_qt =
	( project->first("TARGET") == "qt"QTDLL_POSTFIX ||
	  project->first("TARGET") == "qt-mt"QTDLL_POSTFIX );

    // Are we using Qt?
    bool isQtActive = project->isActiveConfig("qt");

    if ( isQtActive ) {
	project->variables()["CONFIG"] += "moc";
	project->variables()["CONFIG"] += "windows";
	project->variables()["INCLUDEPATH"] += project->variables()["QMAKE_INCDIR_QT"];
	project->variables()["QMAKE_LIBDIR"] += project->variables()["QMAKE_LIBDIR_QT"];

	if( projectTarget == SharedLib )
	    project->variables()["DEFINES"] += "QT_DLL";

	if( project->isActiveConfig("accessibility" ) )
	    project->variables()["DEFINES"] += "QT_ACCESSIBILITY_SUPPORT";

	if ( project->isActiveConfig("plugin")) {
	    project->variables()["DEFINES"] += "QT_PLUGIN";
	    project->variables()["CONFIG"] += "dll";
	}

	if( project->isActiveConfig("thread") ) {
	    project->variables()["DEFINES"] += "QT_THREAD_SUPPORT";
	    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT_THREAD"];
	} else {
	    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT"];
	}
    }

    if ( project->isActiveConfig("opengl") ) {
	project->variables()["CONFIG"] += "windows"; // <-- Also in 'qt' above
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_OPENGL"];
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_OPENGL"];

    }
*/
    initOld();	   // Currently calling old DSP code to set variables. CLEAN UP!

    // Figure out what we're trying to build
    if ( project->first("TEMPLATE") == "vcapp" ) {
	projectTarget = Application;
    } else if ( project->first("TEMPLATE") == "vclib") {
	if ( project->isActiveConfig( "staticlib" ) )
	    projectTarget = StaticLib;
	else
	    projectTarget = SharedLib;
    }
    initProject(); // Fills the whole project with proper data
}

void VcprojGenerator::initProject()
{
    // Initialize XML sub elements
    // - Do this first since project elements may need
    // - to know of certain configuration options
    initConfiguration();
    initSourceFiles();
    initHeaderFiles();
    initMOCFiles();
    initUICFiles();
    initFormsFiles();
    initTranslationFiles();
    initLexYaccFiles();
    initResourceFiles();

    // Own elements -----------------------------
    vcProject.Name = project->first("QMAKE_ORIG_TARGET");
    vcProject.Version = "7.00";
    vcProject.PlatformName = ( vcProject.Configuration.idl.TargetEnvironment == midlTargetWin64 ? "Win64" : "Win32" );
    // These are not used by Qt, but may be used by customers
    vcProject.SccProjectName = project->first("SCCPROJECTNAME");
    vcProject.SccLocalPath = project->first("SCCLOCALPATH");
}

void VcprojGenerator::initConfiguration()
{
    // Initialize XML sub elements
    // - Do this first since main configuration elements may need
    // - to know of certain compiler/linker options
    initCompilerTool();
    if ( projectTarget == StaticLib )
	initLibrarianTool();
    else
	initLinkerTool();
    initIDLTool();

    // Own elements -----------------------------
    QString temp = project->first("BuildBrowserInformation");
    switch ( projectTarget ) {
    case SharedLib:
        vcProject.Configuration.ConfigurationType = typeDynamicLibrary;
	break;
    case StaticLib:
        vcProject.Configuration.ConfigurationType = typeStaticLibrary;
	break;
    case Application:
    default:
        vcProject.Configuration.ConfigurationType = typeApplication;
	break;
    }
    vcProject.Configuration.Name =  ( project->isActiveConfig( "debug" ) ? "Debug|" : "Release|" );
    vcProject.Configuration.Name += ( vcProject.Configuration.idl.TargetEnvironment == midlTargetWin64 ? "Win64" : "Win32" );
    vcProject.Configuration.ATLMinimizesCRunTimeLibraryUsage = ( project->first("ATLMinimizesCRunTimeLibraryUsage").isEmpty() ? _False : _True );
    vcProject.Configuration.BuildBrowserInformation = triState( temp.isEmpty() ? (short)unset : temp.toShort() );
    temp = project->first("CharacterSet");
    vcProject.Configuration.CharacterSet = charSet( temp.isEmpty() ? (short)charSetNotSet : temp.toShort() );
    vcProject.Configuration.DeleteExtensionsOnClean = project->first("DeleteExtensionsOnClean");
    vcProject.Configuration.ImportLibrary = vcProject.Configuration.linker.ImportLibrary;
    vcProject.Configuration.IntermediateDirectory = project->first("OBJECTS_DIR");
//    temp = (projectTarget == StaticLib) ? project->first("DESTDIR"):project->first("DLLDESTDIR");
    vcProject.Configuration.OutputDirectory = "."; //( temp.isEmpty() ? QString(".") : temp );
    vcProject.Configuration.PrimaryOutput = project->first("PrimaryOutput");
    vcProject.Configuration.WholeProgramOptimization = vcProject.Configuration.compiler.WholeProgramOptimization;
    temp = project->first("UseOfATL");
    if ( !temp.isEmpty() )
	vcProject.Configuration.UseOfATL = useOfATL( temp.toShort() );
    temp = project->first("UseOfMfc");
    if ( !temp.isEmpty() )
        vcProject.Configuration.UseOfMfc = useOfMfc( temp.toShort() );

    // Configuration does not need parameters from
    // these sub XML items;
    initCustomBuildTool();
    initPreBuildEventTools();
    initPostBuildEventTools();
    initPreLinkEventTools();
}

void VcprojGenerator::initCompilerTool()
{
    QString placement = project->first("OBJECTS_DIR");
    if ( placement.isEmpty() )
	placement = ".\\";

    vcProject.Configuration.compiler.AssemblerListingLocation = placement ;
    vcProject.Configuration.compiler.ProgramDataBaseFileName = placement ;
    vcProject.Configuration.compiler.ObjectFile = placement ;
    vcProject.Configuration.compiler.PrecompiledHeaderFile = placement + project->first("QMAKE_ORIG_TARGET") + ".pch";

    if ( project->isActiveConfig("debug") ){
	// Debug version
	vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS"] );
	vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_DEBUG"] );
	if ( project->isActiveConfig("thread") ) {
	    if ( (projectTarget == Application) || (projectTarget == StaticLib) )
		vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_MT_DBG"] );
	    else
		vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_MT_DLLDBG"] );
	} else {
	    vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_ST_DBG"] );
	}
    } else {
	// Release version
	vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS"] );
	vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_RELEASE"] );
	vcProject.Configuration.compiler.PreprocessorDefinitions += "QT_NO_DEBUG";
	vcProject.Configuration.compiler.PreprocessorDefinitions += "NDEBUG";
	if ( project->isActiveConfig("thread") ) {
	    if ( (projectTarget == Application) || (projectTarget == StaticLib) )
		vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_MT"] );
	    else
		vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_MT_DLL"] );
	} else {
	    vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_ST"] );
	}
    }

    // Common for both release and debug
    if ( project->isActiveConfig("warn_off") )
	vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_WARN_OFF"] );
    else if ( project->isActiveConfig("warn_on") )
	vcProject.Configuration.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_WARN_ON"] );
    if ( project->isActiveConfig("windows") )
	vcProject.Configuration.compiler.PreprocessorDefinitions += project->variables()["MSVCPROJ_WINCONDEF"];

    // Can this be set for ALL configs?
    // If so, use qmake.conf!
    if ( projectTarget == SharedLib )
	vcProject.Configuration.compiler.PreprocessorDefinitions += "_WINDOWS";

    vcProject.Configuration.compiler.PreprocessorDefinitions += project->variables()["DEFINES"];
    vcProject.Configuration.compiler.PreprocessorDefinitions += project->variables()["PRL_EXPORT_DEFINES"];
    vcProject.Configuration.compiler.parseOptions( project->variables()["MSVCPROJ_INCPATH"] );
}

void VcprojGenerator::initLibrarianTool()
{
    vcProject.Configuration.librarian.OutputFile = project->first( "DESTDIR" );
    if( vcProject.Configuration.librarian.OutputFile.isEmpty() )
	vcProject.Configuration.librarian.OutputFile = ".\\";

    if( !vcProject.Configuration.librarian.OutputFile.endsWith("\\") )
    	vcProject.Configuration.librarian.OutputFile += '\\';

    vcProject.Configuration.librarian.OutputFile += project->first("MSVCPROJ_TARGET");
}

void VcprojGenerator::initLinkerTool()
{
    vcProject.Configuration.linker.parseOptions( project->variables()["MSVCPROJ_LFLAGS"] );
    vcProject.Configuration.linker.AdditionalDependencies += project->variables()["MSVCPROJ_LIBS"];

    switch ( projectTarget ) {
    case Application:
	vcProject.Configuration.linker.OutputFile = project->first( "DESTDIR" );
	break;
    case SharedLib:
	vcProject.Configuration.linker.parseOptions( project->variables()["MSVCPROJ_LIBOPTIONS"] );
	vcProject.Configuration.linker.OutputFile = project->first( "DLLDESTDIR" );
	break;
    case StaticLib: //unhandled - added to remove warnings..
	break;
    }

    if( vcProject.Configuration.linker.OutputFile.isEmpty() )
	vcProject.Configuration.linker.OutputFile = ".\\";

    if( !vcProject.Configuration.linker.OutputFile.endsWith("\\") )
    	vcProject.Configuration.linker.OutputFile += '\\';

    vcProject.Configuration.linker.OutputFile += project->first("MSVCPROJ_TARGET");
    vcProject.Configuration.linker.ProgramDatabaseFile = project->first("OBJECTS_DIR") + project->first("QMAKE_ORIG_TARGET") + ".pdb";

    if ( project->isActiveConfig("debug") ){
	vcProject.Configuration.linker.parseOptions( project->variables()["QMAKE_LFLAGS_DEBUG"] );
    } else {
        vcProject.Configuration.linker.parseOptions( project->variables()["QMAKE_LFLAGS_RELEASE"] );
    }

    if ( project->isActiveConfig("dll") ){
	vcProject.Configuration.linker.parseOptions( project->variables()["QMAKE_LFLAGS_QT_DLL"] );
    }

    if ( project->isActiveConfig("console") ){
	vcProject.Configuration.linker.parseOptions( project->variables()["QMAKE_LFLAGS_CONSOLE"] );
    } else {
	vcProject.Configuration.linker.parseOptions( project->variables()["QMAKE_LFLAGS_WINDOWS"] );
    }

}

void VcprojGenerator::initIDLTool()
{
}

void VcprojGenerator::initCustomBuildTool()
{
}

void VcprojGenerator::initPreBuildEventTools()
{
    QString collectionName = project->first("QMAKE_IMAGE_COLLECTION");
    if( !collectionName.isEmpty() ) {
        QStringList& list = project->variables()["IMAGES"];
	vcProject.Configuration.preBuild.Description = "Generate imagecollection";
	//vcProject.Configuration.preBuild.AdditionalDependencies += list;

	QFile imgs( ".imgcol" );
	imgs.open( IO_WriteOnly );
	QTextStream s( &imgs );
	QStringList::ConstIterator it = list.begin();
	while( it!=list.end() ) {
	    s << *it << " ";
	    it++;
	}

	vcProject.Configuration.preBuild.CommandLine = project->first("QMAKE_UIC") + " -embed " + project->first("QMAKE_ORIG_TARGET") + " -f .imgcol -o " + collectionName;
	//vcProject.Configuration.preBuild.Outputs = collectionName;

    }
}

void VcprojGenerator::initPostBuildEventTools()
{
    if ( !project->variables()["QMAKE_POST_LINK"].isEmpty() ) {
	vcProject.Configuration.postBuild.Description = var("QMAKE_POST_LINK");
	vcProject.Configuration.postBuild.CommandLine = var("QMAKE_POST_LINK");
    }
    if( project->isActiveConfig( "activeqt" ) ) {
	QString name = project->first( "QMAKE_ORIG_TARGET" );
	QString nameext = project->first( "TARGET" );
	QString objdir = project->first( "OBJECTS_DIR" );
	QString idc = project->first( "QMAKE_IDC" );

	vcProject.Configuration.postBuild.Description = "Finalizing ActiveQt server...";
	if ( !vcProject.Configuration.postBuild.CommandLine.isEmpty() )
	    vcProject.Configuration.postBuild.CommandLine += " &amp;&amp; ";

	if( project->isActiveConfig( "dll" ) ) { // In process
	    vcProject.Configuration.postBuild.CommandLine +=
		// call idc to generate .idl file from .dll
		idc + " " + vcProject.Configuration.OutputDirectory + "\\" + nameext + " -idl " + objdir + name + ".idl -version 1.0 &amp;&amp; " +
		// call midl to create implementations of the .idl file
		project->first( "QMAKE_IDL" ) + " " + objdir + name + ".idl /nologo /o " + objdir + name + ".midl /tlb " + objdir + name + ".tlb /iid " + objdir +
		"dump.midl /dlldata " + objdir + "dump.midl /cstub " + objdir + "dump.midl /header " + objdir + "dump.midl /proxy " + objdir + "dump.midl /sstub " +
		objdir + "dump.midl &amp;&amp; " +
		// call idc to replace tlb...
		idc + " " + vcProject.Configuration.OutputDirectory + "\\" + nameext + " /tlb " + objdir + name + ".tlb &amp;&amp; " +
		// register server
		idc + " " + vcProject.Configuration.OutputDirectory + "\\" + nameext + " /regserver";
	} else { // out of process
	    vcProject.Configuration.postBuild.CommandLine =
		// call application to dump idl
		vcProject.Configuration.OutputDirectory + "\\" + nameext + " -dumpidl " + objdir + name + ".idl -version 1.0 &amp;&amp; " +
		// call midl to create implementations of the .idl file
		project->first( "QMAKE_IDL" ) + " " + objdir + name + ".idl /nologo /o " + objdir + name + ".midl /tlb " + objdir + name + ".tlb /iid " + objdir +
		"dump.midl /dlldata " + objdir + "dump.midl /cstub " + objdir + "dump.midl /header " + objdir + "dump.midl /proxy " + objdir + "dump.midl /sstub " +
		objdir + "dump.midl &amp;&amp; " +
		// call idc to replace tlb...
		idc + " " + vcProject.Configuration.OutputDirectory + "\\" + nameext + " /tlb " + objdir + name + ".tlb &amp;&amp; " +
		// call app to register
		vcProject.Configuration.OutputDirectory + "\\" + nameext + " -regserver";
	}
    }
}

void VcprojGenerator::initPreLinkEventTools()
{
}

void VcprojGenerator::initSourceFiles()
{
    vcProject.SourceFiles.Name = "Source Files";
    vcProject.SourceFiles.Filter = "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat";
    vcProject.SourceFiles.Files += project->variables()["SOURCES"];
    vcProject.SourceFiles.Files.sort();
    vcProject.SourceFiles.Project = this;
    vcProject.SourceFiles.Config = &(vcProject.Configuration);
    vcProject.SourceFiles.CustomBuild = none;
}

void VcprojGenerator::initHeaderFiles()
{
    vcProject.HeaderFiles.Name = "Header Files";
    vcProject.HeaderFiles.Filter = "h;hpp;hxx;hm;inl";
    vcProject.HeaderFiles.Files += project->variables()["HEADERS"];
    vcProject.HeaderFiles.Files.sort();
    vcProject.HeaderFiles.Project = this;
    vcProject.HeaderFiles.Config = &(vcProject.Configuration);
    vcProject.HeaderFiles.CustomBuild = moc;
}

void VcprojGenerator::initMOCFiles()
{
    vcProject.MOCFiles.Name = "Generated MOC Files";
    vcProject.MOCFiles.Filter = "cpp;c;cxx;moc";
    vcProject.MOCFiles.Files += project->variables()["SRCMOC"];
    vcProject.MOCFiles.Files.sort();
    vcProject.MOCFiles.Project = this;
    vcProject.MOCFiles.Config = &(vcProject.Configuration);
    vcProject.MOCFiles.CustomBuild = moc;
}

void VcprojGenerator::initUICFiles()
{
    vcProject.UICFiles.Name = "Generated UI Files";
    vcProject.UICFiles.Filter = "cpp;c;cxx;h;hpp;hxx;";
    vcProject.UICFiles.Project = this;
    vcProject.UICFiles.Files += project->variables()["UICDECLS"];
    vcProject.UICFiles.Files += project->variables()["UICIMPLS"];
    vcProject.UICFiles.Files.sort();
    vcProject.UICFiles.Config = &(vcProject.Configuration);
    vcProject.UICFiles.CustomBuild = none;
}

void VcprojGenerator::initFormsFiles()
{
    vcProject.FormFiles.Name = "Forms";
    vcProject.FormFiles.ParseFiles = _False;
    vcProject.FormFiles.Filter = "ui";
    vcProject.FormFiles.Files += project->variables()["FORMS"];
    vcProject.FormFiles.Files.sort();
    vcProject.FormFiles.Project = this;
    vcProject.FormFiles.Config = &(vcProject.Configuration);
    vcProject.FormFiles.CustomBuild = uic;
}

void VcprojGenerator::initTranslationFiles()
{
    vcProject.TranslationFiles.Name = "Translations Files";
    vcProject.TranslationFiles.ParseFiles = _False;
    vcProject.TranslationFiles.Filter = "ts";
    vcProject.TranslationFiles.Files += project->variables()["TRANSLATIONS"];
    vcProject.TranslationFiles.Files.sort();
    vcProject.TranslationFiles.Project = this;
    vcProject.TranslationFiles.Config = &(vcProject.Configuration);
    vcProject.TranslationFiles.CustomBuild = none;
}

void VcprojGenerator::initLexYaccFiles()
{
    vcProject.LexYaccFiles.Name = "Lex / Yacc Files";
    vcProject.LexYaccFiles.ParseFiles = _False;
    vcProject.LexYaccFiles.Filter = "l;y";
    vcProject.LexYaccFiles.Files += project->variables()["LEXSOURCES"];
    vcProject.LexYaccFiles.Files += project->variables()["YACCSOURCES"];
    vcProject.LexYaccFiles.Files.sort();
    vcProject.LexYaccFiles.Project = this;
    vcProject.LexYaccFiles.CustomBuild = lexyacc;
}

void VcprojGenerator::initResourceFiles()
{
    vcProject.ResourceFiles.Name = "Resources";
    vcProject.ResourceFiles.ParseFiles = _False;
    vcProject.ResourceFiles.Filter = "cpp;ico;png;jpg;jpeg;gif;xpm;bmp;rc;ts";
    vcProject.ResourceFiles.Files += project->variables()["RC_FILE"];
    vcProject.ResourceFiles.Files += project->variables()["QMAKE_IMAGE_COLLECTION"];
    vcProject.ResourceFiles.Files += project->variables()["IMAGES"];
    vcProject.ResourceFiles.Files += project->variables()["IDLSOURCES"];
    vcProject.ResourceFiles.Files.sort();
    vcProject.ResourceFiles.Project = this;
    vcProject.ResourceFiles.CustomBuild = none;
}

/* \internal
    Sets up all needed variables from the environment and all the different caches and .conf files
*/

void VcprojGenerator::initOld()
{
    if( init_flag )
	return;

    init_flag = TRUE;
    QStringList::Iterator it;

    if ( project->isActiveConfig("stl") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_STL_ON"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_STL_ON"];
    } else {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_STL_OFF"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_STL_OFF"];
    }
    if ( project->isActiveConfig("exceptions") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_EXCEPTIONS_ON"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_EXCEPTIONS_ON"];
    } else {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_EXCEPTIONS_OFF"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_EXCEPTIONS_OFF"];
    }
    if ( project->isActiveConfig("rtti") ) {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_RTTI_ON"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_RTTI_ON"];
    } else {
	project->variables()["QMAKE_CFLAGS"] += project->variables()["QMAKE_CFLAGS_RTTI_OFF"];
	project->variables()["QMAKE_CXXFLAGS"] += project->variables()["QMAKE_CXXFLAGS_RTTI_OFF"];
    }

    // this should probably not be here, but I'm using it to wrap the .t files
    if(project->first("TEMPLATE") == "vcapp" )
	project->variables()["QMAKE_APP_FLAG"].append("1");
    else if(project->first("TEMPLATE") == "vclib")
	project->variables()["QMAKE_LIB_FLAG"].append("1");
    if ( project->variables()["QMAKESPEC"].isEmpty() )
	project->variables()["QMAKESPEC"].append( getenv("QMAKESPEC") );

    bool is_qt =
	( project->first("TARGET") == "qt"QTDLL_POSTFIX ||
	  project->first("TARGET") == "qt-mt"QTDLL_POSTFIX );

   QStringList &configs = project->variables()["CONFIG"];

    if ( project->isActiveConfig( "shared" ) )
	project->variables()["DEFINES"].append( "QT_DLL" );

    if ( project->isActiveConfig( "qt_dll" ) &&
	 configs.findIndex("qt") == -1 )
	    configs.append("qt");

    if ( project->isActiveConfig( "qt" ) ) {
	if ( project->isActiveConfig( "plugin" ) ) {
	    project->variables()["CONFIG"].append( "dll" );
	    project->variables()["DEFINES"].append( "QT_PLUGIN" );
	}
	if ( ( project->variables()["DEFINES"].findIndex( "QT_NODLL" )   == -1 ) &&
	    (( project->variables()["DEFINES"].findIndex( "QT_MAKEDLL" ) != -1   ||
	       project->variables()["DEFINES"].findIndex( "QT_DLL" )     != -1 ) ||
	     ( getenv( "QT_DLL" ) && !getenv( "QT_NODLL" ))) ) {
	    project->variables()["QMAKE_QT_DLL"].append( "1" );
	    if ( is_qt && !project->variables()["QMAKE_LIB_FLAG"].isEmpty() )
		project->variables()["CONFIG"].append( "dll" );
	}
    }

    // If we are a dll, then we cannot be a staticlib at the same time...
    if ( project->isActiveConfig( "dll" ) || !project->variables()["QMAKE_APP_FLAG"].isEmpty() ) {
	project->variables()["CONFIG"].remove( "staticlib" );
	project->variables()["QMAKE_APP_OR_DLL"].append( "1" );
    } else {
	project->variables()["CONFIG"].append( "staticlib" );
    }

    // If we need 'qt' and/or 'opengl', then we need windows and not console
    if ( project->isActiveConfig( "qt" ) || project->isActiveConfig( "opengl" ) ) {
	project->variables()["CONFIG"].append( "windows" );
    }

    // Decode version, and add it to $$MSVCPROJ_VERSION --------------
    if ( !project->variables()["VERSION"].isEmpty() ) {
	QString version = project->variables()["VERSION"][0];
	int firstDot = version.find( "." );
	QString major = version.left( firstDot );
	QString minor = version.right( version.length() - firstDot - 1 );
	minor.replace( QRegExp( "\\." ), "" );
	project->variables()["MSVCPROJ_VERSION"].append( "/VERSION:" + major + "." + minor );
    }

    // QT ------------------------------------------------------------
    if ( project->isActiveConfig("qt") ) {
	project->variables()["CONFIG"].append("moc");
	project->variables()["INCLUDEPATH"] +=	project->variables()["QMAKE_INCDIR_QT"];
	project->variables()["QMAKE_LIBDIR"] += project->variables()["QMAKE_LIBDIR_QT"];

	if ( is_qt && !project->variables()["QMAKE_LIB_FLAG"].isEmpty() ) {
	    if ( !project->variables()["QMAKE_QT_DLL"].isEmpty() ) {
		project->variables()["DEFINES"].append("QT_MAKEDLL");
		project->variables()["QMAKE_LFLAGS"].append("/BASE:0x39D00000");
	    }
	} else {
	    if(project->isActiveConfig("thread"))
		project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT_THREAD"];
	    else
		project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT"];
	    if ( !project->variables()["QMAKE_QT_DLL"].isEmpty() ) {
		int hver = findHighestVersion(project->first("QMAKE_LIBDIR_QT"), "qt");
		if( hver==-1 ) {
		    hver = findHighestVersion( project->first("QMAKE_LIBDIR_QT"), "qt-mt" );
		}

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
		if ( project->isActiveConfig( "dll" ) ) {
		    project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_QT_ENTRY"];
		    project->variables()["MSVCPROJ_LFLAGS"].append("/DEF:"+project->first("DEF_FILE"));
		}
	    }
	    if ( !project->isActiveConfig("dll") && !project->isActiveConfig("plugin") ) {
		project->variables()["QMAKE_LIBS"] +=project->variables()["QMAKE_LIBS_QT_ENTRY"];
	    }
	}
    }

    // Set target directories ----------------------------------------
 //   if ( !project->first("OBJECTS_DIR").isEmpty() )
	//project->variables()["MSVCPROJ_OBJECTSDIR"] = project->first("OBJECTS_DIR");
 //   else
	//project->variables()["MSVCPROJ_OBJECTSDIR"] = project->isActiveConfig( "release" )?"Release":"Debug";
 //   if ( !project->first("DESTDIR").isEmpty() )
	//project->variables()["MSVCPROJ_TARGETDIR"] = project->first("DESTDIR");
 //   else
	//project->variables()["MSVCPROJ_TARGETDIR"] = project->isActiveConfig( "release" )?"Release":"Debug";

    // OPENGL --------------------------------------------------------
    if ( project->isActiveConfig("opengl") ) {
	project->variables()["QMAKE_LIBS"] += project->variables()["QMAKE_LIBS_OPENGL"];
	project->variables()["QMAKE_LFLAGS"] += project->variables()["QMAKE_LFLAGS_OPENGL"];
    }

    // THREAD --------------------------------------------------------
    if ( project->isActiveConfig("thread") ) {
	if(project->isActiveConfig("qt"))
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_THREAD_SUPPORT" );
	if ( !project->variables()["DEFINES"].contains("QT_DLL") && is_qt
	     && project->first("TARGET") != "qtmain" )
	    project->variables()["QMAKE_LFLAGS"].append("/NODEFAULTLIB:libc");
    }

    // ACCESSIBILITY -------------------------------------------------
    if(project->isActiveConfig("qt")) {
	if ( project->isActiveConfig("accessibility" ) )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_ACCESSIBILITY_SUPPORT");
	if ( project->isActiveConfig("tablet") )
	    project->variables()[is_qt ? "PRL_EXPORT_DEFINES" : "DEFINES"].append("QT_TABLET_SUPPORT");
    }

    // DLL -----------------------------------------------------------
    if ( project->isActiveConfig("dll") ) {
	if ( !project->variables()["QMAKE_LIB_FLAG"].isEmpty() ) {
	    QString ver_xyz(project->first("VERSION"));
	    ver_xyz.replace(QRegExp("\\."), "");
	    project->variables()["TARGET_EXT"].append(ver_xyz + ".dll");
	} else {
	    project->variables()["TARGET_EXT"].append(".dll");
	}
    }
    // EXE / LIB -----------------------------------------------------
    else {
	if ( !project->variables()["QMAKE_APP_FLAG"].isEmpty() )
	    project->variables()["TARGET_EXT"].append(".exe");
	else
	    project->variables()["TARGET_EXT"].append(".lib");
    }

    project->variables()["MSVCPROJ_VER"] = "7.00";
    project->variables()["MSVCPROJ_DEBUG_OPT"] = "/GZ /ZI";

    // INCREMENTAL:NO ------------------------------------------------
    if(!project->isActiveConfig("incremental")) {
	project->variables()["QMAKE_LFLAGS"].append(QString("/INCREMENTAL:no"));
        if ( is_qt )
	    project->variables()["MSVCPROJ_DEBUG_OPT"] = "/GZ /Zi";
    }

    // MOC -----------------------------------------------------------
    if ( project->isActiveConfig("moc") )
	setMocAware(TRUE);


    project->variables()["QMAKE_LIBS"] += project->variables()["LIBS"];
    // Update -lname to name.lib, and -Ldir to
    QStringList &libList = project->variables()["QMAKE_LIBS"];
    for( it = libList.begin(); it != libList.end(); ) {
	QString s = *it;
	if( s.startsWith( "-l" ) ) {
	    it = libList.remove( it );
	    it = libList.insert( it, s.mid( 2 ) + ".lib" );
	} else if( s.startsWith( "-L" ) ) {
	    it = libList.remove( it );
	} else {
	    it++;
	}
    }

    // Run through all variables containing filepaths, and -----------
    // slash-slosh them correctly depending on current OS  -----------
    project->variables()["QMAKE_FILETAGS"] += QStringList::split(' ', "HEADERS SOURCES DEF_FILE RC_FILE TARGET QMAKE_LIBS DESTDIR DLLDESTDIR INCLUDEPATH");
    QStringList &l = project->variables()["QMAKE_FILETAGS"];
    for(it = l.begin(); it != l.end(); ++it) {
	QStringList &gdmf = project->variables()[(*it)];
	for(QStringList::Iterator inner = gdmf.begin(); inner != gdmf.end(); ++inner)
	    (*inner) = Option::fixPathToTargetOS((*inner), FALSE);
    }

     // Get filename w/o extention -----------------------------------
    QString msvcproj_project = "";
    QString targetfilename = "";
    if ( project->variables()["TARGET"].count() ) {
	msvcproj_project = project->variables()["TARGET"].first();
	targetfilename = msvcproj_project;
    }

    // Save filename w/o extention in $$QMAKE_ORIG_TARGET ------------
    project->variables()["QMAKE_ORIG_TARGET"] = project->variables()["TARGET"];

    // TARGET (add extention to $$TARGET)
    //project->variables()["MSVCPROJ_DEFINES"].append(varGlue(".first() += project->first("TARGET_EXT");

    // Init base class too -------------------------------------------
    MakefileGenerator::init();


    if ( msvcproj_project.isEmpty() )
	msvcproj_project = Option::output.name();

    msvcproj_project = msvcproj_project.right( msvcproj_project.length() - msvcproj_project.findRev( "\\" ) - 1 );
    msvcproj_project = msvcproj_project.left( msvcproj_project.findRev( "." ) );
    msvcproj_project.replace(QRegExp("-"), "");

    project->variables()["MSVCPROJ_PROJECT"].append(msvcproj_project);
    QStringList &proj = project->variables()["MSVCPROJ_PROJECT"];

    for(it = proj.begin(); it != proj.end(); ++it)
	(*it).replace(QRegExp("\\.[a-zA-Z0-9_]*$"), "");

    // SUBSYSTEM -----------------------------------------------------
    if ( !project->variables()["QMAKE_APP_FLAG"].isEmpty() ) {
	    project->variables()["MSVCPROJ_TEMPLATE"].append("win32app" + project->first( "VCPROJ_EXTENSION" ) );
	    if ( project->isActiveConfig("console") ) {
		project->variables()["MSVCPROJ_CONSOLE"].append("CONSOLE");
		project->variables()["MSVCPROJ_WINCONDEF"].append("_CONSOLE");
		project->variables()["MSVCPROJ_VCPROJTYPE"].append("0x0103");
		project->variables()["MSVCPROJ_SUBSYSTEM"].append("CONSOLE");
	    } else {
		project->variables()["MSVCPROJ_CONSOLE"].clear();
		project->variables()["MSVCPROJ_WINCONDEF"].append("_WINDOWS");
		project->variables()["MSVCPROJ_VCPROJTYPE"].append("0x0101");
		project->variables()["MSVCPROJ_SUBSYSTEM"].append("WINDOWS");
	    }
    } else {
        if ( project->isActiveConfig("dll") ) {
            project->variables()["MSVCPROJ_TEMPLATE"].append("win32dll" + project->first( "VCPROJ_EXTENSION" ) );
        } else {
            project->variables()["MSVCPROJ_TEMPLATE"].append("win32lib" + project->first( "VCPROJ_EXTENSION" ) );
        }
    }

    // $$QMAKE.. -> $$MSVCPROJ.. -------------------------------------
    project->variables()["MSVCPROJ_LIBS"] += project->variables()["QMAKE_LIBS"];
    project->variables()["MSVCPROJ_LIBS"] += project->variables()["QMAKE_LIBS_WINDOWS"];
    project->variables()["MSVCPROJ_LFLAGS" ] += project->variables()["QMAKE_LFLAGS"];
    if ( !project->variables()["QMAKE_LIBDIR"].isEmpty() ) {
	QStringList strl = project->variables()["QMAKE_LIBDIR"];
	QStringList::iterator stri;
	for ( stri = strl.begin(); stri != strl.end(); ++stri ) {
	    if ( !(*stri).startsWith("/LIBPATH:") )
		(*stri).prepend( "/LIBPATH:" );
	}
	project->variables()["MSVCPROJ_LFLAGS"] += strl;
    }
    project->variables()["MSVCPROJ_CXXFLAGS" ] += project->variables()["QMAKE_CXXFLAGS"];
    // We don't use this... Direct manipulation of compiler object
    //project->variables()["MSVCPROJ_DEFINES"].append(varGlue("DEFINES","/D ","" " /D ",""));
    //project->variables()["MSVCPROJ_DEFINES"].append(varGlue("PRL_EXPORT_DEFINES","/D ","" " /D ",""));
    QStringList &incs = project->variables()["INCLUDEPATH"];
    for(QStringList::Iterator incit = incs.begin(); incit != incs.end(); ++incit) {
	QString inc = (*incit);
	inc.replace(QRegExp("\""), "");
	project->variables()["MSVCPROJ_INCPATH"].append("/I" + inc );
    }
    project->variables()["MSVCPROJ_INCPATH"].append("/I" + specdir());

    QString dest;
    project->variables()["MSVCPROJ_TARGET"] = project->first("TARGET");
    Option::fixPathToTargetOS(project->first("TARGET"));
    dest = project->first("TARGET") + project->first( "TARGET_EXT" );
    if ( project->first("TARGET").startsWith("$(QTDIR)") )
	dest.replace( QRegExp("\\$\\(QTDIR\\)"), getenv("QTDIR") );
    project->variables()["MSVCPROJ_TARGET"] = dest;
    if ( project->isActiveConfig("dll") ) {
	QString imp = project->first( "DESTDIR" );
	if( !imp.isNull() && !imp.endsWith( "\\" ) )
	    imp += "\\";
	imp += dest;
	imp.replace(QRegExp("\\.dll"), ".lib");
	project->variables()["MSVCPROJ_LIBOPTIONS"] += QString("/IMPLIB:") + imp;
    }

    // DLL COPY ------------------------------------------------------
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
	project->variables()["MSVCPROJ_COPY_DLL_REL"].append( copydll );
	project->variables()["MSVCPROJ_COPY_DLL_DBG"].append( copydll );
    }

    // ACTIVEQT ------------------------------------------------------
    if ( project->isActiveConfig("activeqt") ) {
	QString idl = project->variables()["QMAKE_IDL"].first();
	QString idc = project->variables()["QMAKE_IDC"].first();
	QString version = project->variables()["VERSION"].first();
	if ( version.isEmpty() )
	    version = "1.0";

	project->variables()["MSVCPROJ_IDLSOURCES"].append( "tmp\\" + targetfilename + ".idl" );
	project->variables()["MSVCPROJ_IDLSOURCES"].append( "tmp\\" + targetfilename + ".tlb" );
	project->variables()["MSVCPROJ_IDLSOURCES"].append( "tmp\\" + targetfilename + ".midl" );
	if ( project->isActiveConfig( "dll" ) ) {
	    QString regcmd = "# Begin Special Build Tool\n"
			    "TargetPath=" + targetfilename + "\n"
			    "SOURCE=$(InputPath)\n"
			    "PostBuild_Desc=Finalizing ActiveQt server...\n"
			    "PostBuild_Cmds=" +
			    idc + " %1 -idl tmp\\" + targetfilename + ".idl -version " + version +
			    "\t" + idl + " tmp\\" + targetfilename + ".idl /nologo /o tmp\\" + targetfilename + ".midl /tlb tmp\\" + targetfilename + ".tlb /iid tmp\\dump.midl /dlldata tmp\\dump.midl /cstub tmp\\dump.midl /header tmp\\dump.midl /proxy tmp\\dump.midl /sstub tmp\\dump.midl"
			    "\t" + idc + " %1 /tlb tmp\\" + targetfilename + ".tlb"
			    "\tregsvr32 /s %1\n"
			    "# End Special Build Tool";

	    QString executable = project->variables()["MSVCPROJ_TARGETDIRREL"].first() + "\\" + project->variables()["TARGET"].first();
	    project->variables()["MSVCPROJ_COPY_DLL_REL"].append( regcmd.arg(executable).arg(executable).arg(executable) );

	    executable = project->variables()["MSVCPROJ_TARGETDIRDEB"].first() + "\\" + project->variables()["TARGET"].first();
	    project->variables()["MSVCPROJ_COPY_DLL_DBG"].append( regcmd.arg(executable).arg(executable).arg(executable) );
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

	    QString executable = project->variables()["MSVCPROJ_TARGETDIRREL"].first() + "\\" + project->variables()["TARGET"].first();
	    project->variables()["MSVCPROJ_REGSVR_REL"].append( regcmd.arg(executable).arg(executable).arg(executable) );

	    executable = project->variables()["MSVCPROJ_TARGETDIRDEB"].first() + "\\" + project->variables()["TARGET"].first();
	    project->variables()["MSVCPROJ_REGSVR_DBG"].append( regcmd.arg(executable).arg(executable).arg(executable) );
	}

    }

    // FORMS ---------------------------------------------------------
    QStringList &list = project->variables()["FORMS"];
    for( it = list.begin(); it != list.end(); ++it ) {
	if ( QFile::exists( *it + ".h" ) )
	    project->variables()["SOURCES"].append( *it + ".h" );
    }

    project->variables()["QMAKE_INTERNAL_PRL_LIBS"] << "MSVCPROJ_LFLAGS" << "MSVCPROJ_LIBS";

    // Verbose output if "-d -d"...
    outputVariables();
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------

bool VcprojGenerator::openOutput(QFile &file) const
{
    QString outdir;
    if(!file.name().isEmpty()) {
	QFileInfo fi(file);
	if(fi.isDir())
	    outdir = file.name() + QDir::separator();
    }
    if(!outdir.isEmpty() || file.name().isEmpty()) {
	QString ext = project->first("VCPROJ_EXTENSION");
	if(project->first("TEMPLATE") == "vcsubdirs")
	    ext = project->first("VCSOLUTION_EXTENSION");
	file.setName(outdir + project->first("TARGET") + ext);
    }
    if(QDir::isRelativePath(file.name())) {
	file.setName( Option::fixPathToLocalOS(QDir::currentDirPath() + Option::dir_sep + fixFilename(file.name())) );
    }
    return Win32MakefileGenerator::openOutput(file);
}

QString VcprojGenerator::fixFilename(QString ofile) const
{
    int slashfind = ofile.findRev('\\');
    if (slashfind == -1) {
	ofile = ofile.replace('-', '_');
    } else {
	int hypenfind = ofile.find('-', slashfind);
	while (hypenfind != -1 && slashfind < hypenfind) {
	    ofile = ofile.replace(hypenfind, 1, '_');
	    hypenfind = ofile.find('-', hypenfind + 1);
	}
    }
    return ofile;
}

QString VcprojGenerator::findTemplate(QString file)
{
    QString ret;
    if(!QFile::exists((ret = file)) &&
       !QFile::exists((ret = QString(Option::mkfile::qmakespec + "/" + file))) &&
       !QFile::exists((ret = QString(getenv("QTDIR")) + "/mkspecs/win32-msvc.net/" + file)) &&
       !QFile::exists((ret = (QString(getenv("HOME")) + "/.tmake/" + file))))
	return "";
    debug_msg(1, "Generator: MSVC.NET: Found template \'%s\'", ret.latin1() );
    return ret;
}


void VcprojGenerator::processPrlVariable(const QString &var, const QStringList &l)
{
    if(var == "QMAKE_PRL_DEFINES") {
	QStringList &out = project->variables()["MSVCPROJ_DEFINES"];
	for(QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
	    if(out.findIndex((*it)) == -1)
		out.append((" /D " + *it ));
	}
    } else {
	MakefileGenerator::processPrlVariable(var, l);
    }
}

void VcprojGenerator::outputVariables()
{
#if 0
    qDebug( "Generator: MSVC.NET: List of current variables:" );
    for ( QMap<QString, QStringList>::ConstIterator it = project->variables().begin(); it != project->variables().end(); ++it) {
	qDebug( "Generator: MSVC.NET: %s => %s", it.key().latin1(), it.data().join(" | ").latin1() );
    }
#endif
}

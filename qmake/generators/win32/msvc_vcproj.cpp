/****************************************************************************
**
**
** Implementation of VcprojGenerator class.
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

#include "msvc_vcproj.h"
#include "option.h"
#include "qtmd5.h" // SG's MD5 addon
#include <qdir.h>
#include <qregexp.h>
#include <qdict.h>
#include <quuid.h>
#include <stdlib.h>
#include <qsettings.h>

//#define DEBUG_SOLUTION_GEN
//#define DEBUG_PROJECT_GEN

// Registry keys for .NET version detection -------------------------
const char* _regNet2002		= "Microsoft\\VisualStudio\\7.0\\Setup\\VC\\ProductDir";
const char* _regNet2003		= "Microsoft\\VisualStudio\\7.1\\Setup\\VC\\ProductDir";

bool use_net2003_version()
{
#ifndef Q_OS_WIN32
    return FALSE; // Always generate 7.0 versions on other platforms
#else
    // Only search for the version once
    static int current_version = -1;
    if (current_version!=-1)
	return (current_version==71);

    // Fallback to .NET 2002
    current_version = 70;

    // Get registry entries for both versions
    bool ok = false;
    QSettings setting;
    QString path2002 = setting.readEntry(_regNet2002);
    QString path2003 = setting.readEntry(_regNet2003);

    if ( path2002.isNull() || path2003.isNull() ) {
	// Only have one MSVC, so use that one
	current_version = (path2003.isNull() ? 70 : 71);
    } else {
	// Have both, so figure out the current
	QString paths = getenv("PATH");
	QStringList pathlist = QStringList::split(";", paths.lower());

	path2003 = path2003.lower();
	QStringList::iterator it;
	for(it=pathlist.begin(); it!=pathlist.end(); ++it) {
	    if ((*it).contains(path2003)) {
		current_version = 71;
	    } else if ((*it).contains(path2002)
		       && current_version == 71) {
		fprintf( stderr, "Both .NET 2002 & .NET 2003 directories for VC found in you PATH variable!\nFallback to .NET 2002 project generation" );
		current_version = 70;
		break;
	    }
	}
    }
    return (current_version==71);
#endif
};

// Flatfile Tags ----------------------------------------------------
const char* _slnHeader70	= "Microsoft Visual Studio Solution File, Format Version 7.00";
const char* _slnHeader71	= "Microsoft Visual Studio Solution File, Format Version 8.00";
				  // The following UUID _may_ change for later servicepacks...
				  // If so we need to search through the registry at
				  // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\VisualStudio\7.0\Projects
				  // to find the subkey that contains a "PossibleProjectExtension"
				  // containing "vcproj"...
				  // Use the hardcoded value for now so projects generated on other
				  // platforms are actually usable.
const char* _slnMSVCvcprojGUID  = "{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}";
const char* _slnProjectBeg	= "\nProject(\"";
const char* _slnProjectMid	= "\") = ";
const char* _slnProjectEnd	= "\nEndProject";
const char* _slnGlobalBeg	= "\nGlobal";
const char* _slnGlobalEnd	= "\nEndGlobal";
const char* _slnSolutionConf	= "\n\tGlobalSection(SolutionConfiguration) = preSolution"
				  "\n\t\tConfigName.0 = Debug"
				  "\n\t\tConfigName.1 = Release"
				  "\n\tEndGlobalSection";
const char* _slnProjDepBeg	= "\n\tGlobalSection(ProjectDependencies) = postSolution";
const char* _slnProjDepEnd	= "\n\tEndGlobalSection";
const char* _slnProjConfBeg	= "\n\tGlobalSection(ProjectConfiguration) = postSolution";
const char* _slnProjRelConfTag1	= ".Release.ActiveCfg = Release|Win32";
const char* _slnProjRelConfTag2	= ".Release.Build.0 = Release|Win32";
const char* _slnProjDbgConfTag1	= ".Debug.ActiveCfg = Debug|Win32";
const char* _slnProjDbgConfTag2	= ".Debug.Build.0 = Debug|Win32";
const char* _slnProjConfEnd	= "\n\tEndGlobalSection";
const char* _slnExtSections	= "\n\tGlobalSection(ExtensibilityGlobals) = postSolution"
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
    bool debugBuild;
    QStringList dependencies;
};

QUuid VcprojGenerator::getProjectUUID(const QString &filename)
{
    bool validUUID = true;

    // Read GUID from variable-space
    QUuid uuid = project->first("GUID");

    // If none, create one based on the MD5 of absolute project path
    if (uuid.isNull() || !filename.isNull()) {
	QString abspath = filename.isNull()?project->first("QMAKE_MAKEFILE"):filename;
	qtMD5(abspath.utf8(), (unsigned char*)(&uuid));
	validUUID = !uuid.isNull();
	uuid.data4[0] = (uuid.data4[0] & 0x3F) | 0x80; // UV_DCE variant
	uuid.data3 = (uuid.data3 & 0x0FFF) | (QUuid::Name<<12);
    }

    // If still not valid, generate new one, and suggest adding to .pro
    if (uuid.isNull() || !validUUID) {
	uuid = QUuid::createUuid();
	fprintf(stderr,
	        "qmake couldn't create a GUID based on filepath, and we couldn't\nfind a valid GUID in the .pro file (Consider adding\n'GUID = %s'  to the .pro file)\n",
		uuid.toString().upper().latin1());
    }

    // Store GUID in variable-space
    project->values("GUID") = uuid.toString().upper();
    return uuid;
}

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

    t << (use_net2003_version() ? _slnHeader71 : _slnHeader70);

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
			subdirs += fileFixify(tmp_proj.variables()["SUBDIRS"]);
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
			    warn_msg(WarnLogic, "Ignored (not found) '%s'", QString(QDir::currentDirPath() + Option::dir_sep + vcproj).latin1() );
			    goto nextfile; // # Dirty!
			}

			VcsolutionDepend *newDep = new VcsolutionDepend;
			newDep->vcprojFile = fileFixify(vcproj);
			newDep->orig_target = tmp_proj.first("QMAKE_ORIG_TARGET");
			newDep->target = tmp_proj.first("MSVCPROJ_TARGET").section(Option::dir_sep, -1);
			newDep->targetType = tmp_vcproj.projectTarget;
			newDep->debugBuild = tmp_proj.isActiveConfig("debug");
			newDep->uuid = getProjectUUID(Option::fixPathToLocalOS(QDir::currentDirPath() + QDir::separator() + vcproj)).toString().upper();

			// We want to store it as the .lib name.
			if(newDep->target.endsWith(".dll"))
			    newDep->target = newDep->target.left(newDep->target.length()-3) + "lib";

			// All projects using Forms are dependent on uic.exe
			if(!tmp_proj.isEmpty("FORMS"))
			    newDep->dependencies << "uic.exe";

			// Add all unknown libs to the deps
			QStringList where("QMAKE_LIBS");
			if(!tmp_proj.isEmpty("QMAKE_INTERNAL_PRL_LIBS"))
			    where = tmp_proj.variables()["QMAKE_INTERNAL_PRL_LIBS"];
			for(QStringList::iterator wit = where.begin();
			    wit != where.end(); ++wit) {
			    QStringList &l = tmp_proj.variables()[(*wit)];
			    for(QStringList::Iterator it = l.begin(); it != l.end(); ++it) {
				QString opt = (*it);
				if(!opt.startsWith("/") &&   // Not a switch
				    opt != newDep->target && // Not self
				    opt != "opengl32.lib" && // We don't care about these libs
				    opt != "glu32.lib" &&    // to make depgen alittle faster
				    opt != "kernel32.lib" &&
				    opt != "user32.lib" &&
				    opt != "gdi32.lib" &&
				    opt != "comdlg32.lib" &&
				    opt != "advapi32.lib" &&
				    opt != "shell32.lib" &&
				    opt != "ole32.lib" &&
				    opt != "oleaut32.lib" &&
				    opt != "uuid.lib" &&
				    opt != "imm32.lib" &&
				    opt != "winmm.lib" &&
				    opt != "wsock32.lib" &&
				    opt != "winspool.lib" &&
				    opt != "delayimp.lib" )
				{
				    newDep->dependencies << opt.section(Option::dir_sep, -1);
				}
			    }
			}
#ifdef DEBUG_SOLUTION_GEN
			qDebug( "Deps for %20s: [%s]", newDep->target.latin1(), newDep->dependencies.join(" :: " ).latin1() );
#endif
			solution_cleanup.append(newDep);
  			solution_depends.insert(newDep->target, newDep);
			t << _slnProjectBeg << _slnMSVCvcprojGUID << _slnProjectMid
			    << "\"" << newDep->orig_target << "\", \"" << newDep->vcprojFile
			    << "\", \"" << newDep->uuid << "\"";
			t << _slnProjectEnd;
		    }
		}
nextfile:
		QDir::setCurrent(oldpwd);
	    }
	}
    }
    t << _slnGlobalBeg;
    t << _slnSolutionConf;
    t << _slnProjDepBeg;

    // Figure out dependencies
    for(solution_cleanup.first(); solution_cleanup.current(); solution_cleanup.next()) {
	if(solution_cleanup.current()->targetType == StaticLib)
	    continue; // Shortcut, Static libs are not dep.
	int cnt = 0;
	for(QStringList::iterator dit = solution_cleanup.current()->dependencies.begin();
	    dit != solution_cleanup.current()->dependencies.end();
	    ++dit)
	{
	    VcsolutionDepend *vc = solution_depends[*dit];
	    if(vc)
		t << "\n\t\t" << solution_cleanup.current()->uuid << "." << cnt++ << " = " << vc->uuid;
	}
    }
    t << _slnProjDepEnd;
    t << _slnProjConfBeg;
    for(solution_cleanup.first(); solution_cleanup.current(); solution_cleanup.next()) {
	t << "\n\t\t" << solution_cleanup.current()->uuid << _slnProjDbgConfTag1;
	t << "\n\t\t" << solution_cleanup.current()->uuid << _slnProjDbgConfTag2;
	t << "\n\t\t" << solution_cleanup.current()->uuid << _slnProjRelConfTag1;
	t << "\n\t\t" << solution_cleanup.current()->uuid << _slnProjRelConfTag2;
    }
    t << _slnProjConfEnd;
    t << _slnExtSections;
    t << _slnGlobalEnd;
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

    // Setup PCH variables
    precompH = project->first("PRECOMPILED_HEADER");
    usePCH = !precompH.isEmpty() && project->isActiveConfig("precompile_header");
    if (usePCH) {
	precompHFilename = QFileInfo(precompH).fileName();
	// Created files
	QString origTarget = project->first("QMAKE_ORIG_TARGET");
	precompObj = origTarget + Option::obj_ext;
	precompPch = origTarget + ".pch";
	// Add PRECOMPILED_HEADER to HEADERS
	if (!project->variables()["HEADERS"].contains(precompH))
	    project->variables()["HEADERS"] += precompH;
	// Return to variable pool
	project->variables()["PRECOMPILED_OBJECT"] = precompObj;
	project->variables()["PRECOMPILED_PCH"]    = precompPch;
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
    vcProject.Version = use_net2003_version() ? "7.10" : "7.00";
    vcProject.ProjectGUID = getProjectUUID().toString().upper();
    vcProject.PlatformName = ( vcProject.Configuration[0].idl.TargetEnvironment == midlTargetWin64 ? "Win64" : "Win32" );
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
        vcProject.Configuration[0].ConfigurationType = typeDynamicLibrary;
	break;
    case StaticLib:
        vcProject.Configuration[0].ConfigurationType = typeStaticLibrary;
	break;
    case Application:
    default:
        vcProject.Configuration[0].ConfigurationType = typeApplication;
	break;
    }

    // Release version of the Configuration ---------------
    VCConfiguration &RConf = vcProject.Configuration[0];
    RConf.Name = "Release";
    RConf.Name += ( RConf.idl.TargetEnvironment == midlTargetWin64 ? "|Win64" : "|Win32" );
    RConf.ATLMinimizesCRunTimeLibraryUsage = ( project->first("ATLMinimizesCRunTimeLibraryUsage").isEmpty() ? _False : _True );
    RConf.BuildBrowserInformation = triState( temp.isEmpty() ? (short)unset : temp.toShort() );
    temp = project->first("CharacterSet");
    RConf.CharacterSet = charSet( temp.isEmpty() ? (short)charSetNotSet : temp.toShort() );
    RConf.DeleteExtensionsOnClean = project->first("DeleteExtensionsOnClean");
    RConf.ImportLibrary = RConf.linker.ImportLibrary;
    RConf.IntermediateDirectory = project->first("OBJECTS_DIR");
    RConf.OutputDirectory = ".";
    RConf.PrimaryOutput = project->first("PrimaryOutput");
    RConf.WholeProgramOptimization = RConf.compiler.WholeProgramOptimization;
    temp = project->first("UseOfATL");
    if ( !temp.isEmpty() )
	RConf.UseOfATL = useOfATL( temp.toShort() );
    temp = project->first("UseOfMfc");
    if ( !temp.isEmpty() )
        RConf.UseOfMfc = useOfMfc( temp.toShort() );

    // Configuration does not need parameters from
    // these sub XML items;
    initCustomBuildTool();
    initPreBuildEventTools();
    initPostBuildEventTools();
    initPreLinkEventTools();

    // Debug version of the Configuration -----------------
    VCConfiguration DConf = vcProject.Configuration[0]; // Create copy configuration for debug
    DConf.Name = "Debug";
    DConf.Name += ( DConf.idl.TargetEnvironment == midlTargetWin64 ? "|Win64" : "|Win32" );

    // Set definite values in both configurations
    DConf.compiler.PreprocessorDefinitions.remove("NDEBUG");
    RConf.compiler.PreprocessorDefinitions += "NDEBUG";
    RConf.linker.GenerateDebugInformation = _False;
    DConf.linker.GenerateDebugInformation = _True;

    // Modify configurations, based on Qt build
    if ( project->isActiveConfig("debug") ) {
	RConf.IntermediateDirectory =
	RConf.compiler.AssemblerListingLocation =
	RConf.compiler.ObjectFile = "Release\\";
	RConf.librarian.OutputFile =
	RConf.linker.OutputFile = RConf.IntermediateDirectory + "\\" + project->first("MSVCPROJ_TARGET");
	RConf.linker.parseOptions(project->variables()["QMAKE_LFLAGS_RELEASE"]);
	RConf.compiler.parseOptions(project->variables()["QMAKE_CFLAGS_RELEASE"]);
    } else {
	DConf.IntermediateDirectory =
	DConf.compiler.AssemblerListingLocation =
	DConf.compiler.ObjectFile = "Debug\\";
	DConf.librarian.OutputFile =
	DConf.linker.OutputFile = DConf.IntermediateDirectory + "\\" + project->first("MSVCPROJ_TARGET");
	DConf.linker.DelayLoadDLLs.clear();
	DConf.compiler.parseOptions(project->variables()["QMAKE_CFLAGS_DEBUG"]);
    }

    // Add Debug configuration to project
    vcProject.Configuration += DConf;
}

void VcprojGenerator::initCompilerTool()
{
    QString placement = project->first("OBJECTS_DIR");
    if ( placement.isEmpty() )
	placement = ".\\";

    VCConfiguration &RConf = vcProject.Configuration[0];
    RConf.compiler.AssemblerListingLocation = placement ;
    RConf.compiler.ProgramDataBaseFileName = ".\\" ;
    RConf.compiler.ObjectFile = placement ;
    // PCH
    if ( usePCH ) {
	RConf.compiler.UsePrecompiledHeader     = pchUseUsingSpecific;
	RConf.compiler.PrecompiledHeaderFile    = "$(IntDir)\\" + precompPch;
	RConf.compiler.PrecompiledHeaderThrough = precompHFilename;
	RConf.compiler.ForcedIncludeFiles       = precompHFilename;
	// Minimal build option triggers an Internal Compiler Error
	// when used in conjunction with /FI and /Yu, so remove it
	project->variables()["QMAKE_CFLAGS_DEBUG"].remove("-Gm");
	project->variables()["QMAKE_CFLAGS_DEBUG"].remove("/Gm");
	project->variables()["QMAKE_CXXFLAGS_DEBUG"].remove("-Gm");
	project->variables()["QMAKE_CXXFLAGS_DEBUG"].remove("/Gm");
    }

    if ( project->isActiveConfig("debug") ){
	// Debug version
	RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS"] );
	RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_DEBUG"] );
	if ( project->isActiveConfig("thread") ) {
	    if ( (projectTarget == Application) || (projectTarget == StaticLib) )
		RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_MT_DBG"] );
	    else
		RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_MT_DLLDBG"] );
	} else {
	    RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_MT_DBG"] );
	}
    } else {
	// Release version
	RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS"] );
	RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_RELEASE"] );
	RConf.compiler.PreprocessorDefinitions += "QT_NO_DEBUG";
	RConf.compiler.PreprocessorDefinitions += "NDEBUG";
	if ( project->isActiveConfig("thread") ) {
	    if ( (projectTarget == Application) || (projectTarget == StaticLib) )
		RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_MT"] );
	    else
		RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_MT_DLL"] );
	} else {
	    RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_MT"] );
	}
    }

    // Common for both release and debug
    if ( project->isActiveConfig("warn_off") )
	RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_WARN_OFF"] );
    else if ( project->isActiveConfig("warn_on") )
	RConf.compiler.parseOptions( project->variables()["QMAKE_CXXFLAGS_WARN_ON"] );
    if ( project->isActiveConfig("windows") )
	RConf.compiler.PreprocessorDefinitions += project->variables()["MSVCPROJ_WINCONDEF"];

    // Can this be set for ALL configs?
    // If so, use qmake.conf!
    if ( projectTarget == SharedLib )
	RConf.compiler.PreprocessorDefinitions += "_WINDOWS";

    RConf.compiler.PreprocessorDefinitions += project->variables()["DEFINES"];
    RConf.compiler.PreprocessorDefinitions += project->variables()["PRL_EXPORT_DEFINES"];
    QStringList::iterator it;
    for(it=RConf.compiler.PreprocessorDefinitions.begin();
	it!=RConf.compiler.PreprocessorDefinitions.end();
	++it)
	(*it).replace('\"', "&quot;");

    RConf.compiler.parseOptions( project->variables()["MSVCPROJ_INCPATH"] );
}

void VcprojGenerator::initLibrarianTool()
{
    VCConfiguration &RConf = vcProject.Configuration[0];
    RConf.librarian.OutputFile = project->first( "DESTDIR" );
    if( RConf.librarian.OutputFile.isEmpty() )
	RConf.librarian.OutputFile = ".\\";

    if( !RConf.librarian.OutputFile.endsWith("\\") )
    	RConf.librarian.OutputFile += '\\';

    RConf.librarian.OutputFile += project->first("MSVCPROJ_TARGET");
}

void VcprojGenerator::initLinkerTool()
{
    VCConfiguration &RConf = vcProject.Configuration[0];
    RConf.linker.parseOptions( project->variables()["MSVCPROJ_LFLAGS"] );
    RConf.linker.AdditionalDependencies += project->variables()["MSVCPROJ_LIBS"];

    switch ( projectTarget ) {
    case Application:
	RConf.linker.OutputFile = project->first( "DESTDIR" );
	break;
    case SharedLib:
	RConf.linker.parseOptions( project->variables()["MSVCPROJ_LIBOPTIONS"] );
	RConf.linker.OutputFile = project->first( "DESTDIR" );
	break;
    case StaticLib: //unhandled - added to remove warnings..
	break;
    }

    if( RConf.linker.OutputFile.isEmpty() )
	RConf.linker.OutputFile = ".\\";

    if( !RConf.linker.OutputFile.endsWith("\\") )
    	RConf.linker.OutputFile += '\\';

    RConf.linker.OutputFile += project->first("MSVCPROJ_TARGET");

    if ( project->isActiveConfig("debug") ){
	RConf.linker.parseOptions( project->variables()["QMAKE_LFLAGS_DEBUG"] );
    } else {
        RConf.linker.parseOptions( project->variables()["QMAKE_LFLAGS_RELEASE"] );
    }

    if ( project->isActiveConfig("dll") ){
	RConf.linker.parseOptions( project->variables()["QMAKE_LFLAGS_QT_DLL"] );
    }

    if ( project->isActiveConfig("console") ){
	RConf.linker.parseOptions( project->variables()["QMAKE_LFLAGS_CONSOLE"] );
    } else {
	RConf.linker.parseOptions( project->variables()["QMAKE_LFLAGS_WINDOWS"] );
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
}

void VcprojGenerator::initPostBuildEventTools()
{
    VCConfiguration &RConf = vcProject.Configuration[0];
    if ( !project->variables()["QMAKE_POST_LINK"].isEmpty() ) {
	RConf.postBuild.Description = var("QMAKE_POST_LINK");
	RConf.postBuild.CommandLine = var("QMAKE_POST_LINK");
	RConf.postBuild.Description.replace(" && ", " &amp;&amp; ");
	RConf.postBuild.CommandLine.replace(" && ", " &amp;&amp; ");
    }
    if ( !project->variables()["MSVCPROJ_COPY_DLL"].isEmpty() ) {
	if ( !RConf.postBuild.CommandLine.isEmpty() )
	    RConf.postBuild.CommandLine += " &amp;&amp; ";
	RConf.postBuild.Description += var("MSVCPROJ_COPY_DLL_DESC");
	RConf.postBuild.CommandLine += var("MSVCPROJ_COPY_DLL");
    }
    if( project->isActiveConfig( "activeqt" ) ) {
	QString name = project->first( "QMAKE_ORIG_TARGET" );
	QString nameext = project->first( "TARGET" );
	QString objdir = project->first( "OBJECTS_DIR" );
	QString idc = project->first( "QMAKE_IDC" );

	RConf.postBuild.Description = "Finalizing ActiveQt server...";
	if ( !RConf.postBuild.CommandLine.isEmpty() )
	    RConf.postBuild.CommandLine += " &amp;&amp; ";

	if( project->isActiveConfig( "dll" ) ) { // In process
	    RConf.postBuild.CommandLine +=
		// call idc to generate .idl file from .dll
		idc + " &quot;$(TargetPath)&quot; -idl " + objdir + name + ".idl -version 1.0 &amp;&amp; " +
		// call midl to create implementations of the .idl file
		project->first( "QMAKE_IDL" ) + " /nologo " + objdir + name + ".idl /tlb " + objdir + name + ".tlb &amp;&amp; " +
		// call idc to replace tlb...
		idc + " &quot;$(TargetPath)&quot; /tlb " + objdir + name + ".tlb &amp;&amp; " +
		// register server
		idc + " &quot;$(TargetPath)&quot; /regserver";
	} else { // out of process
	    RConf.postBuild.CommandLine =
		// call application to dump idl
		"&quot;$(TargetPath)&quot; -dumpidl " + objdir + name + ".idl -version 1.0 &amp;&amp; " +
		// call midl to create implementations of the .idl file
		project->first( "QMAKE_IDL" ) + " /nologo " + objdir + name + ".idl /tlb " + objdir + name + ".tlb &amp;&amp; " +
		// call idc to replace tlb...
		idc + " &quot;$(TargetPath)&quot; /tlb " + objdir + name + ".tlb &amp;&amp; " +
		// call app to register
		"&quot;$(TargetPath)&quot; -regserver";
	}
    }
}

void VcprojGenerator::initPreLinkEventTools()
{
}


// ------------------------------------------------------------------
// Helper functions to do proper sorting of the
// qstringlists, for both flat and non-flat modes.
inline bool XLessThanY( QString &x, QString &y, bool flat_mode )
{
    if ( flat_mode ) {
	QString subX = x.mid( x.findRev('\\')+1 );
	QString subY = y.mid( y.findRev('\\')+1 );
	return QString::compare(subX, subY) < 0;
    }

    int xPos = 0;
    int yPos = 0;
    int xSlashPos;
    int ySlashPos;
    for (;;) {
	xSlashPos = x.find('\\', xPos);
	ySlashPos = y.find('\\', yPos);

	if (xSlashPos == -1 && ySlashPos != -1) {
	    return false;
	} else if (xSlashPos != -1 && ySlashPos == -1) {
	    return true;
	} else if (xSlashPos == -1 /* && yySlashPos == -1 */) {
	    QString subX = x.mid(xPos);
	    QString subY = y.mid(yPos);
	    return QString::compare(subX, subY) < 0;
	} else {
	    QString subX = x.mid(xPos, xSlashPos - xPos);
	    QString subY = y.mid(yPos, ySlashPos - yPos);
	    int cmp = QString::compare(subX, subY);
	    if (cmp != 0)
		return cmp < 0;
	}
	xPos = xSlashPos + 1;
	yPos = ySlashPos + 1;
    }
    return false;
}
void nonflatDir_BubbleSort( QStringList& list, bool flat_mode )
{
    QStringList::Iterator b = list.begin();
    QStringList::Iterator e = list.end();
    QStringList::Iterator last = e;
    --last;		// goto last
    if ( last == b )	// shortcut
	return;
    while( b != last ) {// sort them
	bool swapped = FALSE;
	QStringList::Iterator swap_pos = b;
	QStringList::Iterator x = e;
	QStringList::Iterator y = x;
	--y;
	QString swap_str;
	do {
	    --x;
	    --y;
	    if ( XLessThanY(*x,*y, flat_mode) ) {
		swapped = TRUE;
		swap_str = (*x); // Swap -------
		(*x) = (*y);
		(*y) = swap_str; // ------------
		swap_pos = y;
	    }
	} while( y != b );
	if ( !swapped )
	    return;
	b = swap_pos;
	++b;
    }
}
// ------------------------------------------------------------------

void VcprojGenerator::initSourceFiles()
{
    vcProject.SourceFiles.flat_files = project->isActiveConfig("flat");
    vcProject.SourceFiles.Name = "Source Files";
    vcProject.SourceFiles.Filter = "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat";
    vcProject.SourceFiles.Files += project->variables()["SOURCES"];
    nonflatDir_BubbleSort( vcProject.SourceFiles.Files,
			   vcProject.SourceFiles.flat_files );
    vcProject.SourceFiles.Project = this;
    vcProject.SourceFiles.Config = &(vcProject.Configuration);
    vcProject.SourceFiles.CustomBuild = none;
}

void VcprojGenerator::initHeaderFiles()
{
    vcProject.HeaderFiles.flat_files = project->isActiveConfig("flat");
    vcProject.HeaderFiles.Name = "Header Files";
    vcProject.HeaderFiles.Filter = "h;hpp;hxx;hm;inl";
    vcProject.HeaderFiles.Files += project->variables()["HEADERS"];
    if (usePCH) { // Generated PCH cpp file
	if (!vcProject.HeaderFiles.Files.contains(precompH))
	    vcProject.HeaderFiles.Files += precompH;
    }
    nonflatDir_BubbleSort( vcProject.HeaderFiles.Files,
			   vcProject.HeaderFiles.flat_files );
    vcProject.HeaderFiles.Project = this;
    vcProject.HeaderFiles.Config = &(vcProject.Configuration);
    vcProject.HeaderFiles.CustomBuild = moc;
}

void VcprojGenerator::initMOCFiles()
{
    vcProject.MOCFiles.flat_files = project->isActiveConfig("flat");
    vcProject.MOCFiles.Name = "Generated MOC Files";
    vcProject.MOCFiles.Filter = "cpp;c;cxx;moc";
    vcProject.MOCFiles.Files += project->variables()["SRCMOC"];
    nonflatDir_BubbleSort( vcProject.MOCFiles.Files,
			   vcProject.MOCFiles.flat_files );
    vcProject.MOCFiles.Project = this;
    vcProject.MOCFiles.Config = &(vcProject.Configuration);
    vcProject.MOCFiles.CustomBuild = moc;
}

void VcprojGenerator::initUICFiles()
{
    vcProject.UICFiles.flat_files = project->isActiveConfig("flat");
    vcProject.UICFiles.Name = "Generated Form Files";
    vcProject.UICFiles.Filter = "cpp;c;cxx;h;hpp;hxx;";
    vcProject.UICFiles.Project = this;
    vcProject.UICFiles.Files += project->variables()["UICDECLS"];
    vcProject.UICFiles.Files += project->variables()["UICIMPLS"];
    nonflatDir_BubbleSort( vcProject.UICFiles.Files,
			   vcProject.UICFiles.flat_files );
    vcProject.UICFiles.Config = &(vcProject.Configuration);
    vcProject.UICFiles.CustomBuild = none;
}

void VcprojGenerator::initFormsFiles()
{
    vcProject.FormFiles.flat_files = project->isActiveConfig("flat");
    vcProject.FormFiles.Name = "Forms";
    vcProject.FormFiles.ParseFiles = _False;
    vcProject.FormFiles.Filter = "ui";
    vcProject.FormFiles.Files += project->variables()["FORMS"];
    nonflatDir_BubbleSort( vcProject.FormFiles.Files,
			   vcProject.FormFiles.flat_files );
    vcProject.FormFiles.Project = this;
    vcProject.FormFiles.Config = &(vcProject.Configuration);
    vcProject.FormFiles.CustomBuild = uic;
}

void VcprojGenerator::initTranslationFiles()
{
    vcProject.TranslationFiles.flat_files = project->isActiveConfig("flat");
    vcProject.TranslationFiles.Name = "Translations Files";
    vcProject.TranslationFiles.ParseFiles = _False;
    vcProject.TranslationFiles.Filter = "ts";
    vcProject.TranslationFiles.Files += project->variables()["TRANSLATIONS"];
    nonflatDir_BubbleSort( vcProject.TranslationFiles.Files,
			   vcProject.TranslationFiles.flat_files );
    vcProject.TranslationFiles.Project = this;
    vcProject.TranslationFiles.Config = &(vcProject.Configuration);
    vcProject.TranslationFiles.CustomBuild = none;
}

void VcprojGenerator::initLexYaccFiles()
{
    vcProject.LexYaccFiles.flat_files = project->isActiveConfig("flat");
    vcProject.LexYaccFiles.Name = "Lex / Yacc Files";
    vcProject.LexYaccFiles.ParseFiles = _False;
    vcProject.LexYaccFiles.Filter = "l;y";
    vcProject.LexYaccFiles.Files += project->variables()["LEXSOURCES"];
    vcProject.LexYaccFiles.Files += project->variables()["YACCSOURCES"];
    nonflatDir_BubbleSort( vcProject.LexYaccFiles.Files,
			   vcProject.LexYaccFiles.flat_files );
    vcProject.LexYaccFiles.Project = this;
    vcProject.LexYaccFiles.Config = &(vcProject.Configuration);
    vcProject.LexYaccFiles.CustomBuild = lexyacc;
}

void VcprojGenerator::initResourceFiles()
{
    vcProject.ResourceFiles.flat_files = project->isActiveConfig("flat");
    vcProject.ResourceFiles.Name = "Resources";
    vcProject.ResourceFiles.ParseFiles = _False;
    vcProject.ResourceFiles.Filter = "cpp;ico;png;jpg;jpeg;gif;xpm;bmp;rc;ts";
    vcProject.ResourceFiles.Files += project->variables()["RC_FILE"];
    vcProject.ResourceFiles.Files += project->variables()["QMAKE_IMAGE_COLLECTION"];
    vcProject.ResourceFiles.Files += project->variables()["IMAGES"];
    vcProject.ResourceFiles.Files += project->variables()["IDLSOURCES"];
    nonflatDir_BubbleSort( vcProject.ResourceFiles.Files,
			   vcProject.ResourceFiles.flat_files );
    vcProject.ResourceFiles.Project = this;
    vcProject.ResourceFiles.Config = &(vcProject.Configuration);
    vcProject.ResourceFiles.CustomBuild = resource;
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

    // /VERSION:x.yz -------------------------------------------------
    if ( !project->variables()["VERSION"].isEmpty() ) {
	QString version = project->variables()["VERSION"][0];
	int firstDot = version.find( "." );
	QString major = version.left( firstDot );
	QString minor = version.right( version.length() - firstDot - 1 );
	minor.replace( ".", "" );
	project->variables()["QMAKE_LFLAGS"].append( "/VERSION:" + major + "." + minor );
    }

    project->variables()["QMAKE_LIBS"] += project->variables()["LIBS"];
    // Update -lname to name.lib, and -Ldir to
    QStringList &libList = project->variables()["QMAKE_LIBS"];
    for( it = libList.begin(); it != libList.end(); ) {
	QString s = *it;
	if( s.startsWith( "-l" ) ) {
	    it = libList.remove( it );
	    it = libList.insert( it, s.mid( 2 ) + ".lib" );
	} else if( s.startsWith( "-L" ) ) {
	    project->variables()["QMAKE_LIBDIR"] += (*it).mid(2);
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

    // DLL COPY ------------------------------------------------------
    if ( project->isActiveConfig("dll") && !project->variables()["DLLDESTDIR"].isEmpty() ) {
	QStringList dlldirs = project->variables()["DLLDESTDIR"];
	QString copydll("");
	QStringList::Iterator dlldir;
	for ( dlldir = dlldirs.begin(); dlldir != dlldirs.end(); ++dlldir ) {
	    if ( !copydll.isEmpty() )
		copydll += " && ";
	    copydll += "copy  &quot;$(TargetPath)&quot; &quot;" + *dlldir + "&quot;";
	}

	QString deststr( "Copy " + dest + " to " );
	for ( dlldir = dlldirs.begin(); dlldir != dlldirs.end(); ) {
	    deststr += *dlldir;
	    ++dlldir;
	    if ( dlldir != dlldirs.end() )
		deststr += ", ";
	}

	project->variables()["MSVCPROJ_COPY_DLL"].append( copydll );
	project->variables()["MSVCPROJ_COPY_DLL_DESC"].append( deststr );
    }

    // ACTIVEQT ------------------------------------------------------
    if ( project->isActiveConfig("activeqt") ) {
	QString idl = project->variables()["QMAKE_IDL"].first();
	QString idc = project->variables()["QMAKE_IDC"].first();
	QString version = project->variables()["VERSION"].first();
	if ( version.isEmpty() )
	    version = "1.0";

	QString objdir = project->first( "OBJECTS_DIR" );
	project->variables()["MSVCPROJ_IDLSOURCES"].append( objdir + targetfilename + ".idl" );
	if ( project->isActiveConfig( "dll" ) ) {
	    QString regcmd = "# Begin Special Build Tool\n"
			    "TargetPath=" + targetfilename + "\n"
			    "SOURCE=$(InputPath)\n"
			    "PostBuild_Desc=Finalizing ActiveQt server...\n"
			    "PostBuild_Cmds=" +
			    idc + " %1 -idl " + objdir + targetfilename + ".idl -version " + version +
			    "\t" + idl + " /nologo " + objdir + targetfilename + ".idl /tlb " + objdir + targetfilename + ".tlb" +
			    "\t" + idc + " %1 /tlb " + objdir + targetfilename + ".tlb"
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
			    "%1 -dumpidl " + objdir + targetfilename + ".idl -version " + version +
			    "\t" + idl + " /nologo " + objdir + targetfilename + ".idl /tlb " + objdir + targetfilename + ".tlb"
			    "\t" + idc + " %1 /tlb " + objdir + targetfilename + ".tlb"
			    "\t%1 -regserver\n"
			    "# End Special Build Tool";

	    QString executable = project->variables()["MSVCPROJ_TARGETDIRREL"].first() + "\\" + project->variables()["TARGET"].first();
	    project->variables()["MSVCPROJ_REGSVR_REL"].append( regcmd.arg(executable).arg(executable).arg(executable) );

	    executable = project->variables()["MSVCPROJ_TARGETDIRDEB"].first() + "\\" + project->variables()["TARGET"].first();
	    project->variables()["MSVCPROJ_REGSVR_DBG"].append( regcmd.arg(executable).arg(executable).arg(executable) );
	}
    }

    if ( !project->variables()["DEF_FILE"].isEmpty() )
	project->variables()["MSVCPROJ_LFLAGS"].append("/DEF:"+project->first("DEF_FILE"));

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

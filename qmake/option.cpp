/****************************************************************************
** $Id: option.cpp,v 1.1 2002-11-01 00:10:42 kergoth Exp $
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

#include "option.h"
#include <qdir.h>
#include <qregexp.h>
#include <stdlib.h>
#include <stdarg.h>

//convenience
QString Option::prf_ext;
QString Option::prl_ext;
QString Option::ui_ext;
QStringList Option::h_ext;
QString Option::moc_ext;
QStringList Option::cpp_ext;
QString Option::obj_ext;
QString Option::lex_ext;
QString Option::yacc_ext;
QString Option::dir_sep;
QString Option::moc_mod;
QString Option::yacc_mod;
QString Option::lex_mod;

//mode
Option::QMAKE_MODE Option::qmake_mode = Option::QMAKE_GENERATE_NOTHING;

//all modes
int Option::warn_level = WarnLogic;
int Option::debug_level = 0;
QFile Option::output;
QString Option::output_dir;
QStringList Option::before_user_vars;
QStringList Option::after_user_vars;
QString Option::user_template;
QString Option::user_template_prefix;
#if defined(Q_OS_WIN32)
Option::TARG_MODE Option::target_mode = Option::TARG_WIN_MODE;
#elif defined(Q_OS_MAC9)
Option::TARG_MODE Option::target_mode = Option::TARG_MAC9_MODE;
#elif defined(Q_OS_MACX)
Option::TARG_MODE Option::target_mode = Option::TARG_MACX_MODE;
#elif defined(Q_OS_QNX6)
Option::TARG_MODE Option::target_mode = Option::TARG_QNX6_MODE;
#else
Option::TARG_MODE Option::target_mode = Option::TARG_UNIX_MODE;
#endif

//QMAKE_GENERATE_PROJECT stuff
bool Option::projfile::do_pwd = TRUE;
bool Option::projfile::do_recursive = FALSE;
QStringList Option::projfile::project_dirs;

//QMAKE_GENERATE_MAKEFILE stuff
QString Option::mkfile::qmakespec;
int Option::mkfile::cachefile_depth = -1;
bool Option::mkfile::do_deps = TRUE;
bool Option::mkfile::do_mocs = TRUE;
bool Option::mkfile::do_dep_heuristics = TRUE;
bool Option::mkfile::do_preprocess = FALSE;
bool Option::mkfile::do_cache = TRUE;
QString Option::mkfile::cachefile;
QStringList Option::mkfile::project_files;
QString Option::mkfile::qmakespec_commandline;

bool usage(const char *a0)
{
    fprintf(stdout, "Usage: %s [mode] [options] [files]\n"
	    "\n"
	    "   QMake has two modes, one mode for generating project files based on\n"
	    "some heuristics, and the other for generating makefiles. Normally you\n"
	    "shouldn't need to specify a mode, as makefile generation is the default\n"
	    "mode for qmake, but you may use this to test qmake on an existing project\n"
	    "\n"
	    "Mode:\n"
	    "\t-project       Put qmake into project file generation mode\n"
	    "\t               In this mode qmake interprets files as files to\n"
	    "\t               be built,\n"
	    "\t               defaults to *.cpp; *.l; *.y; *.ui\n"
	    "\t-makefile      Put qmake into makefile generation mode (default)\n"
	    "\t               In this mode qmake interprets files as project files to\n"
	    "\t               be processed, if skipped qmake will try to find a project\n"
	    "\t               file in your current working directory\n"
	    "\n"
	    "Warnings Options:\n"
	    "\t-Wnone         Turn off all warnings\n"
	    "\t-Wall          Turn on all warnings\n"
	    "\t-Wparser       Turn on parser warnings\n"
	    "\t-Wlogic        Turn on logic warnings\n"
	    "\n"
	    "Options:\n"
	    "\t * You can place any variable assignment in options and it will be     *\n"
	    "\t * processed as if it was in [files]. These assignments will be parsed *\n"
	    "\t * before [files].                                                     *\n"
	    "\t-o file        Write output to file\n"
	    "\t-unix          Run in unix mode\n"
	    "\t-win32         Run in win32 mode\n"
	    "\t-macx          Run in Mac OS X mode\n"
	    "\t-d             Increase debug level\n"
	    "\t-t templ       Overrides TEMPLATE as templ\n"
	    "\t-tp prefix     Overrides TEMPLATE so that prefix is prefixed into the value\n"
	    "\t-help          This help\n"
	    "\t-v             Version information\n"
	    "\t-after         All variable assignments after this will be\n"
	    "\t               parsed after [files]        [makefile mode only]\n"
	    "\t-cache file    Use file as cache           [makefile mode only]\n"
	    "\t-spec spec     Use spec as QMAKESPEC       [makefile mode only]\n"
	    "\t-nocache       Don't use a cache file      [makefile mode only]\n"
	    "\t-nodepend      Don't generate dependencies [makefile mode only]\n"
	    "\t-nomoc         Don't generate moc targets  [makefile mode only]\n"
	    "\t-nopwd         Don't look for files in pwd [ project mode only]\n"
	    "\t-r             Recursive search            [ project mode only]\n"
	    ,a0);
    return FALSE;
}
static Option::QMAKE_MODE default_mode(QString progname)
{
    int s = progname.findRev(Option::dir_sep);
    if(s != -1)
	progname = progname.right(progname.length() - (s + 1));
    if(progname == "qmakegen")
	return Option::QMAKE_GENERATE_PROJECT;
    return Option::QMAKE_GENERATE_MAKEFILE;
}


bool
Option::parseCommandLine(int argc, char **argv)
{
    bool before = TRUE;
    for(int x = 1; x < argc; x++) {
	if(*argv[x] == '-' && strlen(argv[x]) > 1) { /* options */
	    QString opt = argv[x] + 1;

	    //first param is a mode, or we default
	    if(x == 1) {
		bool specified = TRUE;
		if(opt == "project") {
		    Option::qmake_mode = Option::QMAKE_GENERATE_PROJECT;
		} else if(opt == "prl") {
		    Option::mkfile::do_deps = FALSE;
		    Option::mkfile::do_mocs = FALSE;
		    Option::qmake_mode = Option::QMAKE_GENERATE_PRL;
		} else if(opt == "makefile") {
		    Option::qmake_mode = Option::QMAKE_GENERATE_MAKEFILE;
		} else {
		    specified = FALSE;
		    Option::qmake_mode = default_mode(argv[0]);
		}
		if(specified)
		    continue;
	    }
	    //all modes
	    if(opt == "o" || opt == "output") {
		Option::output.setName(argv[++x]);
	    } else if(opt == "after") {
		before = FALSE;
	    } else if(opt == "t" || opt == "template") {
		Option::user_template = argv[++x];
	    } else if(opt == "tp" || opt == "template_prefix") {
		Option::user_template_prefix = argv[++x];
	    } else if(opt == "mac9") {
		Option::target_mode = TARG_MAC9_MODE;
	    } else if(opt == "macx") {
		Option::target_mode = TARG_MACX_MODE;
	    } else if(opt == "unix") {
		Option::target_mode = TARG_UNIX_MODE;
	    } else if(opt == "win32") {
		Option::target_mode = TARG_WIN_MODE;
	    } else if(opt == "d") {
		Option::debug_level++;
	    } else if(opt == "version" || opt == "v" || opt == "-version") {
		fprintf(stderr, "Qmake version: %s\n", qmake_version());
		fprintf(stderr, "Qmake is free software from Trolltech AS.\n");
		return FALSE;
	    } else if(opt == "h" || opt == "help") {
		return usage(argv[0]);
	    } else if(opt == "Wall") {
		Option::warn_level |= WarnAll;
	    } else if(opt == "Wparser") {
		Option::warn_level |= WarnParser;
	    } else if(opt == "Wlogic") {
		Option::warn_level |= WarnLogic;
	    } else if(opt == "Wnone") {
		Option::warn_level = WarnNone;
	    } else {
		if(Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE ||
		   Option::qmake_mode == Option::QMAKE_GENERATE_PRL) {
		    if(opt == "nodepend") {
			Option::mkfile::do_deps = FALSE;
		    } else if(opt == "nomoc") {
			Option::mkfile::do_mocs = FALSE;
		    } else if(opt == "nocache") {
			Option::mkfile::do_cache = FALSE;
		    } else if(opt == "nodependheuristics") {
			Option::mkfile::do_dep_heuristics = FALSE;
		    } else if(opt == "E") {
			Option::mkfile::do_preprocess = TRUE;
		    } else if(opt == "cache") {
			Option::mkfile::cachefile = argv[++x];
		    } else if(opt == "platform" || opt == "spec") {
			Option::mkfile::qmakespec = argv[++x];
			Option::mkfile::qmakespec_commandline = argv[x];
		    } else {
			fprintf(stderr, "***Unknown option -%s\n", opt.latin1());
			return usage(argv[0]);
		    }
		} else if(Option::qmake_mode == Option::QMAKE_GENERATE_PROJECT) {
		    if(opt == "nopwd") {
			Option::projfile::do_pwd = FALSE;
		    } else if(opt == "r") {
			Option::projfile::do_recursive = TRUE;
		    } else {
			fprintf(stderr, "***Unknown option -%s\n", opt.latin1());
			return usage(argv[0]);
		    }
		}
	    }
	} else {
	    if(x == 1)
		Option::qmake_mode = default_mode(argv[0]);

	    QString arg = argv[x];
	    if(arg.find('=') != -1) {
		if(before)
		    Option::before_user_vars.append(arg);
		else
		    Option::after_user_vars.append(arg);
	    } else {
		QFileInfo fi(arg);
		if(!fi.convertToAbs()) //strange
		    arg = fi.filePath();
		if(Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE ||
		   Option::qmake_mode == Option::QMAKE_GENERATE_PRL)
		    Option::mkfile::project_files.append(arg);
		else
		    Option::projfile::project_dirs.append(arg);
	    }
	}
    }
    if(Option::qmake_mode == Option::QMAKE_GENERATE_NOTHING)
	Option::qmake_mode = default_mode(argv[0]);

    //last chance for defaults
    if(Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE ||
	Option::qmake_mode == Option::QMAKE_GENERATE_PRL) {
	if(Option::mkfile::qmakespec.isNull() || Option::mkfile::qmakespec.isEmpty())
	    Option::mkfile::qmakespec = getenv("QMAKESPEC");

	//try REALLY hard to do it for them, lazy..
	if(Option::mkfile::project_files.isEmpty()) {
	    QString proj = QDir::currentDirPath();
	    proj = proj.right(proj.length() - (proj.findRev('/') + 1)) + ".pro";
	    if(QFile::exists(proj))
		Option::mkfile::project_files.append(proj);
	    else
		return usage(argv[0]);
	}
    }

    //defaults for globals
    Option::moc_mod = "moc_";
    Option::lex_mod = "_lex";
    Option::yacc_mod = "_yacc";
    Option::prl_ext = ".prl";
    Option::prf_ext = ".prf";
    Option::ui_ext = ".ui";
    Option::h_ext << ".h" << ".hpp" << ".hh" << ".H" << ".hxx";
    Option::moc_ext = ".moc";
    Option::cpp_ext << ".cpp" << ".cc" << ".cxx" << ".C";
    Option::lex_ext = ".l";
    Option::yacc_ext = ".y";
    if(Option::target_mode == Option::TARG_WIN_MODE) {
	Option::dir_sep = "\\";
	Option::obj_ext =  ".obj";
    } else {
	if(Option::target_mode == Option::TARG_MAC9_MODE)
	    Option::dir_sep = ":";
	else
	    Option::dir_sep = "/";
	Option::obj_ext = ".o";
    }
    return TRUE;
}

bool Option::postProcessProject(QMakeProject *project)
{
    Option::cpp_ext = project->variables()["QMAKE_EXT_CPP"];
    if(cpp_ext.isEmpty())
	cpp_ext << ".cpp"; //something must be there
    Option::h_ext = project->variables()["QMAKE_EXT_H"];
    if(h_ext.isEmpty())
	h_ext << ".h";

    if(!project->isEmpty("QMAKE_EXT_PRL"))
	Option::prl_ext = project->first("QMAKE_EXT_PRL");
    if(!project->isEmpty("QMAKE_EXT_PRF"))
	Option::prf_ext = project->first("QMAKE_EXT_PRF");
    if(!project->isEmpty("QMAKE_EXT_UI"))
	Option::ui_ext = project->first("QMAKE_EXT_UI");
    if(!project->isEmpty("QMAKE_EXT_MOC"))
	Option::moc_ext = project->first("QMAKE_EXT_MOC");
    if(!project->isEmpty("QMAKE_EXT_LEX"))
	Option::lex_ext = project->first("QMAKE_EXT_LEX");
    if(!project->isEmpty("QMAKE_EXT_YACC"))
	Option::yacc_ext = project->first("QMAKE_EXT_YACC");
    if(!project->isEmpty("QMAKE_EXT_OBJ"))
	Option::obj_ext = project->first("QMAKE_EXT_OBJ");
    if(!project->isEmpty("QMAKE_MOD_MOC"))
	Option::moc_mod = project->first("QMAKE_MOD_MOC");
    if(!project->isEmpty("QMAKE_MOD_LEX"))
	Option::lex_mod = project->first("QMAKE_MOD_LEX");
    if(!project->isEmpty("QMAKE_MOD_YACC"))
	Option::yacc_mod = project->first("QMAKE_MOD_YACC");
    if(!project->isEmpty("QMAKE_DIR_SEP"))
	Option::dir_sep = project->first("QMAKE_DIR_SEP");
    return TRUE;
}

void fixEnvVariables(QString &x)
{
    int rep;
    QRegExp reg_var("\\$\\(.*\\)");
    reg_var.setMinimal( TRUE );
    while((rep = reg_var.search(x)) != -1)
	x.replace(rep, reg_var.matchedLength(), QString(getenv(x.mid(rep + 2, reg_var.matchedLength() - 3).latin1())));
}
static QString fixPath(QString x)
{
#if 0
    QFileInfo fi(x);
    if(fi.isDir()) {
	QDir dir(x);
	x = dir.canonicalPath();
    } else {
	QString dir = fi.dir().canonicalPath();
	if(!dir.isEmpty() && dir.right(1) != Option::dir_sep)
	    dir += Option::dir_sep;
	x = dir + fi.fileName();
    }
#endif
    return QDir::cleanDirPath(x);
}


QString
Option::fixPathToTargetOS(const QString& in, bool fix_env, bool canonical)
{
    QString tmp(in);
    if(fix_env)
	fixEnvVariables(tmp);
    if(canonical)
	tmp = fixPath(tmp);
    QString rep;
    if(Option::target_mode == TARG_MAC9_MODE)
	rep = "[/\\\\]";
    else if(Option::target_mode == TARG_WIN_MODE)
	rep = "[/]";
    else
	rep = "[\\\\]";
    return tmp.replace(QRegExp(rep), Option::dir_sep);
}

QString
Option::fixPathToLocalOS(const QString& in, bool fix_env, bool canonical)
{
    QString tmp(in);
    if(fix_env)
	fixEnvVariables(tmp);
    if(canonical)
	tmp = fixPath(tmp);
#if defined(Q_OS_WIN32)
    return tmp.replace('/', '\\');
#else
    return tmp.replace('\\', '/');
#endif
}

const char *qmake_version()
{
    static char *ret = NULL;
    if(ret)
	return ret;
    ret = (char *)malloc(15);
    sprintf(ret, "%d.%02d%c", QMAKE_VERSION_MAJOR, QMAKE_VERSION_MINOR, 'a' + QMAKE_VERSION_PATCH);
    return ret;
}

void debug_msg(int level, const char *fmt, ...)
{
    if(Option::debug_level < level)
	return;
    fprintf(stderr, "DEBUG %d: ", level);
    {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
    }
    fprintf(stderr, "\n");
}

void warn_msg(QMakeWarn type, const char *fmt, ...)
{
    if(!(Option::warn_level & type))
	return;
    fprintf(stderr, "WARNING: ");
    {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
    }
    fprintf(stderr, "\n");
}

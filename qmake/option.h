/****************************************************************************
** 
**
** Definition of Option class.
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

#ifndef __OPTION_H__
#define __OPTION_H__

#include "project.h"
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>

#define QMAKE_VERSION_MAJOR 1
#define QMAKE_VERSION_MINOR 7
#define QMAKE_VERSION_PATCH 0
const char *qmake_version();

void fixEnvVariables(QString &x);
void debug_msg(int level, const char *fmt, ...);
enum QMakeWarn {
    WarnNone    = 0x00,
    WarnParser  = 0x01,
    WarnLogic   = 0x02,
    WarnAll     = 0xFF
};
void warn_msg(QMakeWarn t, const char *fmt, ...);

struct Option
{
    //simply global convenience
    static QString libtool_ext;
    static QString pkgcfg_ext;
    static QString prf_ext;
    static QString prl_ext;
    static QString ui_ext;
    static QStringList h_ext;
    static QStringList cpp_ext;
    static QString h_moc_ext;
    static QString cpp_moc_ext;
    static QString obj_ext;
    static QString lex_ext;
    static QString yacc_ext;
    static QString h_moc_mod;
    static QString cpp_moc_mod;
    static QString lex_mod;
    static QString yacc_mod;
    static QString dir_sep;
    //both of these must be called..
    static bool parseCommandLine(int argc, char **argv); //parse cmdline
    static bool postProcessProject(QMakeProject *); 

    //and convenience functions    
    static QString fixPathToLocalOS(const QString& in, bool fix_env=TRUE, bool canonical=TRUE);
    static QString fixPathToTargetOS(const QString& in, bool fix_env=TRUE, bool canonical=TRUE);

    //global qmake mode, can only be in one mode per invocation!
    enum QMAKE_MODE { QMAKE_GENERATE_NOTHING, QMAKE_GENERATE_PROJECT, QMAKE_GENERATE_MAKEFILE, 
		      QMAKE_GENERATE_PRL, QMAKE_SET_PROPERTY, QMAKE_QUERY_PROPERTY };
    static QMAKE_MODE qmake_mode;

    //all modes
    static QFile output;
    static QString output_dir;
    static int debug_level;
    static int warn_level;
    static QStringList before_user_vars, after_user_vars;
    enum TARG_MODE { TARG_UNIX_MODE, TARG_WIN_MODE, TARG_MACX_MODE, TARG_MAC9_MODE, TARG_QNX6_MODE };
    static TARG_MODE target_mode;
    static QString user_template, user_template_prefix;

    
    //QMAKE_*_PROPERTY options
    struct prop {
	static QStringList properties;
    };

    //QMAKE_GENERATE_PROJECT options
    struct projfile {
	static bool do_pwd;
	static bool do_recursive;
	static QStringList project_dirs;
    };

    //QMAKE_GENERATE_MAKEFILE options
    struct mkfile {
	static QString qmakespec;
	static bool do_cache;
	static bool do_deps;
	static bool do_mocs;
	static bool do_dep_heuristics;
	static bool do_preprocess;
	static QString cachefile;
	static int cachefile_depth;
	static QStringList project_files;
	static QString qmakespec_commandline;
    };

private:
    static int internalParseCommandLine(int, char **, int=0);
};


#endif /* __OPTION_H__ */

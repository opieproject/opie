/****************************************************************************
** $Id: project.cpp,v 1.1 2002-11-01 00:10:42 kergoth Exp $
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

#include "project.h"
#include "option.h"
#include <qfile.h>
#include <qdir.h>
#include <qregexp.h>
#include <qtextstream.h>
#include <qvaluestack.h>
#ifdef Q_OS_UNIX
# include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#ifdef Q_OS_WIN32
#define QT_POPEN _popen
#else
#define QT_POPEN popen
#endif

struct parser_info {
    QString file;
    int line_no;
} parser;
static void qmake_error_msg(const char *msg)
{
    fprintf(stderr, "%s:%d: %s\n", parser.file.latin1(), parser.line_no, msg);
}

static QString varMap(const QString &x)
{
    QString ret(x);
    ret.replace(QRegExp("^TMAKE"), "QMAKE");
    if(ret == "INTERFACES")
	ret = "FORMS";
    return ret;
}

static QStringList split_arg_list(const QString &params)
{
    QStringList args;
    int last = 0, parens = 0;
    QChar quote = 0;
    for(int x = 0; x < (int)params.length(); x++) {
	if(params[x] == ')') {
	    parens--;
	} else if(params[x] == '(') {
	    parens++;
	} else if(params[x] == quote) {
	    quote = 0;
	} else if(params[x] == '\'' || params[x] == '"') {
	    quote = params[x];
	} else if(!parens && !quote && params[x] == ',') {
	    args << params.mid(last, x - last);
	    last = x+1;
	}
    }
    if(last != (int)params.length())
	args << params.mid(last);
    return args;
}

static QStringList split_value_list(const QString &vals, bool do_semicolon=FALSE)
{
    int last = 0;
    QStringList ret;
    QValueStack<QChar> quote;
    for(int x = 0; x < (int)vals.length(); x++) {
	if(!quote.isEmpty() && vals[x] == quote.top()) {
	    quote.pop();
	} else if(vals[x] == '\'' || vals[x] == '"') {
	    quote.push(vals[x]);
	} else if(quote.isEmpty() && 
		  ((do_semicolon && vals[x] == ';') ||  vals[x] == ' ')) {
	    ret << vals.mid(last, x - last);
	    last = x+1;
	}
    }
    if(last != (int)vals.length())
	ret << vals.mid(last);
    return ret;
}

QMakeProject::QMakeProject()
{
}

bool
QMakeProject::parse(QString t, QMap<QString, QStringList> &place)
{
    QString s = t.simplifyWhiteSpace();
    s.replace(QRegExp("#.*$"), ""); /* bye comments */
    if(s.isEmpty()) /* blank_line */
	return TRUE;

    if(s.stripWhiteSpace().left(1) == "}") {
	debug_msg(1, "Project Parser: %s:%d : Leaving block %d", parser.file.latin1(),
		  parser.line_no, scope_block);
	test_status = ((scope_flag & (0x01 << scope_block)) ? TestFound : TestSeek);
	scope_block--;
	s = s.mid(1).stripWhiteSpace();
	if(s.isEmpty())
	    return TRUE;
    }
    if(!(scope_flag & (0x01 << scope_block))) {
	/* adjust scope for each block which appears on a single line */
	for(int i = (s.contains('{')-s.contains('}')); i; i--)
	    scope_flag &= ~(0x01 << (++scope_block));
	debug_msg(1, "Project Parser: %s:%d : Ignored due to block being false.",
		  parser.file.latin1(), parser.line_no);
	return TRUE;
    }

    QString scope, var, op;
    QStringList val;
#define SKIP_WS(d) while(*d && (*d == ' ' || *d == '\t')) d++
    const char *d = s.latin1();
    SKIP_WS(d);
    bool scope_failed = FALSE, else_line = FALSE, or_op=FALSE;
    int parens = 0, scope_count=0;
    while(*d && *d != '=') {
	if((*d == '+' || *d == '-' || *d == '*' || *d == '~')) {
	    if(*(d+1) == '=') {
		break;
	    } else if(*(d+1) == ' ') {
		const char *k = d + 1;
		SKIP_WS(k);
		if(*k == '=') {
		    QString msg;
		    qmake_error_msg(*d + "must be followed immediatly by =");
		    return FALSE;
		}
	    }
	}

	if ( *d == '(' )
	    ++parens;
	else if ( *d == ')' )
	    --parens;

	if(!parens && (*d == ':' || *d == '{' || *d == ')' || *d == '|')) {
	    scope_count++;
	    scope = var.stripWhiteSpace();
	    if ( *d == ')' )
		scope += *d; /* need this */
	    var = "";

	    bool test = scope_failed;
	    if(scope.lower() == "else") {
		if(scope_count != 1 || test_status == TestNone) {
		    qmake_error_msg("Unexpected " + scope + " ('" + s + "')");
		    return FALSE;
		}
		else_line = TRUE;
		test = (test_status == TestSeek);
		debug_msg(1, "Project Parser: %s:%d : Else%s %s.", parser.file.latin1(), parser.line_no,
			  scope == "else" ? "" : QString(" (" + scope + ")").latin1(),
			  test ? "considered" : "excluded");
	    } else {
		QString comp_scope = scope;
		bool invert_test = (comp_scope.left(1) == "!");
		if(invert_test)
		    comp_scope = comp_scope.right(comp_scope.length()-1);
		int lparen = comp_scope.find('(');
		if(or_op || !scope_failed) {
		    if(lparen != -1) { /* if there is an lparen in the scope, it IS a function */
			int rparen = comp_scope.findRev(')');
			if(rparen == -1) {
			    QCString error;
			    error.sprintf("Function missing right paren: %s ('%s')",
					  comp_scope.latin1(), s.latin1());
			    qmake_error_msg(error);
			    return FALSE;
			}
			QString func = comp_scope.left(lparen);
			test = doProjectTest(func, comp_scope.mid(lparen+1, rparen - lparen - 1), place);
			if ( *d == ')' && !*(d+1) ) {
			    if(invert_test)
				test = !test;
			    test_status = (test ? TestFound : TestSeek);
			    return TRUE;  /* assume we are done */
			}
		    } else {
			test = isActiveConfig(comp_scope.stripWhiteSpace());
		    }
		    if(invert_test)
			test = !test;
		}
	    }
	    if(!test && !scope_failed)
		debug_msg(1, "Project Parser: %s:%d : Test (%s) failed.", parser.file.latin1(),
			  parser.line_no, scope.latin1());
	    if(test == or_op)
		scope_failed = !test;
	    or_op = (*d == '|');
	    if(*d == '{') { /* scoping block */
		if(!scope_failed)
		    scope_flag |= (0x01 << (++scope_block));
		else
		    scope_flag &= ~(0x01 << (++scope_block));
		debug_msg(1, "Project Parser: %s:%d : Entering block %d (%d).", parser.file.latin1(),
			  parser.line_no, scope_block, !scope_failed);
	    }
	} else {
	    var += *d;
	}
	d++;
    }
    if(!scope_count || (scope_count == 1 && else_line))
	test_status = TestNone;
    else if(!else_line || test_status != TestFound)
	test_status = (scope_failed ? TestSeek : TestFound);
    if(scope_failed)
	return TRUE; /* oh well */
    if(!*d) {
	if(!var.isEmpty())
	    qmake_error_msg("Parse Error ('" + s + "')");
	return var.isEmpty(); /* allow just a scope */
    }

    SKIP_WS(d);
    for( ; *d && op.find('=') == -1; op += *(d++));
    op.replace(QRegExp("\\s"), "");

    SKIP_WS(d);
    QString vals(d); /* vals now contains the space separated list of values */
    int rbraces = vals.contains('}'), lbraces = vals.contains('{');
    if(scope_block && rbraces - lbraces == 1) {
	debug_msg(1, "Project Parser: %s:%d : Leaving block %d", parser.file.latin1(),
		  parser.line_no, scope_block);
	test_status = ((scope_flag & (0x01 << scope_block)) ? TestFound : TestSeek);
	scope_block--;
	vals.truncate(vals.length()-1);
    } else if(rbraces != lbraces) {
	warn_msg(WarnParser, "Possible braces mismatch {%s} %s:%d",
		 vals.latin1(), parser.file.latin1(), parser.line_no);
    }
    doVariableReplace(vals, place);

    var = var.stripWhiteSpace();
#undef SKIP_WS

    if(!var.isEmpty() && Option::mkfile::do_preprocess) {
	static QString last_file("*none*");
	if(parser.file != last_file) {
	    fprintf(stderr, "#file %s:%d\n", parser.file.latin1(), parser.line_no);
	    last_file = parser.file;
	}
	fprintf(stderr, "%s %s %s\n", var.latin1(), op.latin1(), vals.latin1());
    }
    var = varMap(var); //backwards compatability

    /* vallist is the broken up list of values */
    QStringList vallist = split_value_list(vals, (var == "DEPENDPATH" || var == "INCLUDEPATH")); 
    if(!vallist.grep("=").isEmpty())
	warn_msg(WarnParser, "Detected possible line continuation: {%s} %s:%d",
		 var.latin1(), parser.file.latin1(), parser.line_no);

    QStringList &varlist = place[var]; /* varlist is the list in the symbol table */
    debug_msg(1, "Project Parser: %s:%d :%s: :%s: (%s)", parser.file.latin1(), parser.line_no,
	      var.latin1(), op.latin1(), vallist.join(" :: ").latin1());

    /* now do the operation */
    if(op == "~=") {
	if(vallist.count() != 1) {
	    qmake_error_msg("~= operator only accepts one right hand paramater ('" +
		s + "')");
	    return FALSE;
	}
	QString val(vallist.first());
	if(val.length() < 4 || val.at(0) != 's') {
	    qmake_error_msg("~= operator only can handle s/// function ('" +
		s + "')");
	    return FALSE;
	}
	QChar sep = val.at(1);
	QStringList func = QStringList::split(sep, val, TRUE);
	if(func.count() < 3 || func.count() > 4) {
	    qmake_error_msg("~= operator only can handle s/// function ('" +
		s + "')");
	    return FALSE;
	}
	bool global = FALSE, case_sense = TRUE;
	if(func.count() == 4) {
	    global = func[3].find('g') != -1;
	    case_sense = func[3].find('i') == -1;
	}
	QRegExp regexp(func[1], case_sense);
	for(QStringList::Iterator varit = varlist.begin();
	    varit != varlist.end(); ++varit) {
	    if((*varit).contains(regexp)) {
		(*varit) = (*varit).replace(regexp, func[2]);
		if(!global)
		    break;
	    }
	}
    } else {
	if(op == "=") {
	    if(!varlist.isEmpty())
		warn_msg(WarnParser, "Operator=(%s) clears variables previously set: %s:%d",
			 var.latin1(), parser.file.latin1(), parser.line_no);
	    varlist.clear();
	}
	for(QStringList::Iterator valit = vallist.begin();
	    valit != vallist.end(); ++valit) {
	    if((*valit).isEmpty())
		continue;
	    if((op == "*=" && !(*varlist.find((*valit)))) ||
	       op == "=" || op == "+=")
		varlist.append((*valit));
	    else if(op == "-=")
		varlist.remove((*valit));
	}
    }
    if(var == "REQUIRES") /* special case to get communicated to backends! */
	doProjectCheckReqs(vallist, place);

    return TRUE;
}

bool
QMakeProject::read(QString file, QMap<QString, QStringList> &place)
{
    parser_info pi = parser;
    /* scope blocks start at true */
    test_status = TestNone;
    scope_flag = 0x01;
    scope_block = 0;

    file = Option::fixPathToLocalOS(file);
    doVariableReplace(file, place);
    bool ret = FALSE, using_stdin = FALSE;
    QFile qfile;
    if(!strcmp(file, "-")) {
	qfile.setName("");
	ret = qfile.open(IO_ReadOnly, stdin);
	using_stdin = TRUE;
    } else {
	qfile.setName(file);
	ret = qfile.open(IO_ReadOnly);
    }
    if ( ret ) {
	QTextStream t( &qfile );
	QString s, line;
	parser.file = file;
	parser.line_no = 0;
	while ( !t.eof() ) {
	    parser.line_no++;
	    line = t.readLine().stripWhiteSpace();
	    int prelen = line.length();
	    line.replace(QRegExp("#.*$"), ""); // bye comments
	    if(!line.isEmpty() && line.right(1) == "\\") {
		line.truncate(line.length() - 1);
		s += line + " ";
	    } else if(!line.isEmpty() || (line.isEmpty() && !prelen)) {
		if(s.isEmpty() && line.isEmpty())
		    continue;
		if(!line.isEmpty())
		    s += line;
		if(!s.isEmpty()) {
		    if(!(ret = parse(s, place)))
			break;
		    s = "";
		}
	    }
	}
	if(!using_stdin)
	    qfile.close();
    }
    parser = pi;
    return ret;
}

bool
QMakeProject::read(QString project, QString)
{
    if(cfile.isEmpty()) {
	// hack to get the Option stuff in there
	base_vars["QMAKE_EXT_CPP"] = Option::cpp_ext;
	base_vars["QMAKE_EXT_H"] = Option::h_ext;

	/* parse the cache */
	if(Option::mkfile::do_cache) {
	    if(Option::mkfile::cachefile.isEmpty())  { //find it as it has not been specified
		QString dir = QDir::convertSeparators(Option::output_dir);
		while(!QFile::exists((Option::mkfile::cachefile = dir +
				      QDir::separator() + ".qmake.cache"))) {
		    dir = dir.left(dir.findRev(QDir::separator()));
		    if(dir.isEmpty() || dir.find(QDir::separator()) == -1) {
			Option::mkfile::cachefile = "";
			break;
		    }
		    if(Option::mkfile::cachefile_depth == -1)
			Option::mkfile::cachefile_depth = 1;
		    else
			Option::mkfile::cachefile_depth++;
		}
	    }
	    if(!Option::mkfile::cachefile.isEmpty()) {
		read(Option::mkfile::cachefile, cache);
		if(Option::mkfile::qmakespec.isEmpty() && !cache["QMAKESPEC"].isEmpty())
		    Option::mkfile::qmakespec = cache["QMAKESPEC"].first();
	    }
	}
	/* parse mkspec */
	QStringList mkspec_roots;
	/* prefer $QTDIR if it is set */
	/* minor hack here, prefer QMAKESPECDIR -cl */
	
	if (getenv("QMAKESPECDIR")){
	    mkspec_roots << getenv("QMAKESPECDIR");
	} else if (getenv("QTDIR")) {
	    mkspec_roots << getenv("QTDIR");
	}
	mkspec_roots << qInstallPathData();
	if(Option::mkfile::qmakespec.isEmpty()) {
	    for(QStringList::Iterator it = mkspec_roots.begin(); it != mkspec_roots.end(); ++it) {
		QString mkspec = (*it) + QDir::separator() + QString("mkspecs") +
				 QDir::separator() + "default";
		if(QFile::exists(mkspec)) {
		    Option::mkfile::qmakespec = mkspec;
		    break;
		}
	    }
	    if(Option::mkfile::qmakespec.isEmpty()) {
		fprintf(stderr, "QMAKESPEC has not been set, so configuration cannot be deduced.\n");
		return FALSE;
	    }
	}

	if(QDir::isRelativePath(Option::mkfile::qmakespec)) {
	    bool found_mkspec = FALSE;
	    for(QStringList::Iterator it = mkspec_roots.begin(); it != mkspec_roots.end(); ++it) {
		QString mkspec = (*it) + QDir::separator() + QString("mkspecs") +
				 QDir::separator() + Option::mkfile::qmakespec;
		if(QFile::exists(mkspec)) {
		    found_mkspec = TRUE;
		    Option::mkfile::qmakespec = mkspec;
		    break;
		}
	    }
	    if(!found_mkspec) {
		fprintf(stderr, "Could not find mkspecs for your QMAKESPEC after trying:\n\t%s\n",
			mkspec_roots.join("\n\t").latin1());
		return FALSE;
	    }
	}

        /* parse qmake configuration */
	QString spec = Option::mkfile::qmakespec + QDir::separator() + "qmake.conf";
	debug_msg(1, "QMAKESPEC conf: reading %s", spec.latin1());
	if(!read(spec, base_vars)) {
	    fprintf(stderr, "Failure to read QMAKESPEC conf file %s.\n", spec.latin1());
	    return FALSE;
	}
	if(Option::mkfile::do_cache && !Option::mkfile::cachefile.isEmpty()) {
	    debug_msg(1, "QMAKECACHE file: reading %s", Option::mkfile::cachefile.latin1());
	    read(Option::mkfile::cachefile, base_vars);
	}

	/* commandline */
	cfile = project;
	parser.line_no = 1; //really arg count now.. duh
	parser.file = "(internal)";
	for(QStringList::Iterator it = Option::before_user_vars.begin();
	    it != Option::before_user_vars.end(); ++it) {
	    if(!parse((*it), base_vars)) {
		fprintf(stderr, "Argument failed to parse: %s\n", (*it).latin1());
		return FALSE;
	    }
	    parser.line_no++;
	}
    }

    /* parse project file */
    debug_msg(1, "Project file: reading %s", project.latin1());
    vars = base_vars; /* start with the base */

    pfile = project;
    if(pfile != "-" && !QFile::exists(pfile) && pfile.right(4) != ".pro")
	pfile += ".pro";

    if(!read(pfile, vars))
	return FALSE;

    parser.line_no = 1; //really arg count now.. duh
    parser.file = "(internal)";
    for(QStringList::Iterator it = Option::after_user_vars.begin();
	it != Option::after_user_vars.end(); ++it) {
	if(!parse((*it), vars)) {
	    fprintf(stderr, "Argument failed to parse: %s\n", (*it).latin1());
	    return FALSE;
	}
	parser.line_no++;
    }

    /* now let the user override the template from an option.. */
    if(!Option::user_template.isEmpty()) {
	debug_msg(1, "Overriding TEMPLATE (%s) with: %s", vars["TEMPLATE"].first().latin1(), Option::user_template.latin1());
	vars["TEMPLATE"].clear();
	vars["TEMPLATE"].append(Option::user_template);
    }

    if(vars["TEMPLATE"].isEmpty())
	vars["TEMPLATE"].append(QString("app"));
    else
	vars["TEMPLATE"].first().replace(QRegExp("\\.t$"), "");
    if(!Option::user_template_prefix.isEmpty())
	vars["TEMPLATE"].first().prepend(Option::user_template_prefix);

    if(vars["TARGET"].isEmpty()) {
	// ### why not simply use:
	// QFileInfo fi(pfile);
	// fi.baseName();
	QString tmp = pfile;
	if(tmp.findRev('/') != -1)
	    tmp = tmp.right( tmp.length() - tmp.findRev('/') - 1 );
	if(tmp.findRev('.') != -1)
	    tmp = tmp.left(tmp.findRev('.'));
	vars["TARGET"].append(tmp);
    }

    QString test_version = getenv("QTESTVERSION");
    if (!test_version.isEmpty()) {
	QString s = vars["TARGET"].first();
	if (s == "qt" || s == "qt-mt" || s == "qte" || s == "qte-mt") {
	    QString &ver = vars["VERSION"].first();
//	    fprintf(stderr,"Current QT version number: " + ver + "\n");
	    if (ver != "" && ver != test_version) {
		ver = test_version;
		fprintf(stderr,"Changed QT version number to " + test_version + "!\n");
	    }
	}
    }
    return TRUE;
}

bool
QMakeProject::isActiveConfig(const QString &x)
{
    if(x.isEmpty())
	return TRUE;

    QRegExp re(x, FALSE, TRUE);
    if((Option::target_mode == Option::TARG_MACX_MODE || Option::target_mode == Option::TARG_QNX6_MODE || Option::target_mode == Option::TARG_UNIX_MODE) &&
       x == "unix")
	return TRUE;
    else if(Option::target_mode == Option::TARG_MACX_MODE && x == "macx")
	return TRUE;
    else if(Option::target_mode == Option::TARG_QNX6_MODE && x == "qnx6")
	return TRUE;
    else if(Option::target_mode == Option::TARG_MAC9_MODE && x == "mac9")
	return TRUE;
    else if((Option::target_mode == Option::TARG_MAC9_MODE || Option::target_mode == Option::TARG_MACX_MODE) &&
	    x == "mac")
	return TRUE;
    else if(Option::target_mode == Option::TARG_WIN_MODE && x == "win32")
	return TRUE;


    QString spec = Option::mkfile::qmakespec.right(Option::mkfile::qmakespec.length() -
						   (Option::mkfile::qmakespec.findRev(QDir::separator())+1));
    if(re.exactMatch(spec))
	return TRUE;
#ifdef Q_OS_UNIX
    else if(spec == "default") {
	static char *buffer = NULL;
	if(!buffer)
	    buffer = (char *)malloc(1024);
	int l = readlink(Option::mkfile::qmakespec, buffer, 1024);
	if(l != -1) {
	    buffer[l] = '\0';
	    QString r = buffer;
	    if(r.findRev('/') != -1)
		r = r.mid(r.findRev('/') + 1);
	    if(re.exactMatch(r))
		return TRUE;
	}
    }
#endif


    QStringList &configs = vars["CONFIG"];
    for(QStringList::Iterator it = configs.begin(); it != configs.end(); ++it) {
	if(re.exactMatch((*it)))
	    return TRUE;
    }
    return FALSE;
}

bool
QMakeProject::doProjectTest(QString func, const QString &params, QMap<QString, QStringList> &place)
{
    QStringList args = split_arg_list(params);
    for(QStringList::Iterator arit = args.begin(); arit != args.end(); ++arit) {
	QString tmp = (*arit).stripWhiteSpace();
	if((tmp[0] == '\'' || tmp[0] == '"') && tmp.right(1) == tmp.left(1))
	    tmp = tmp.mid(1, tmp.length() - 2);
    }
    return doProjectTest(func.stripWhiteSpace(), args, place);
}

bool
QMakeProject::doProjectTest(QString func, QStringList args, QMap<QString, QStringList> &place)
{
    for(QStringList::Iterator arit = args.begin(); arit != args.end(); ++arit) {
	(*arit) = (*arit).stripWhiteSpace(); // blah, get rid of space
	doVariableReplace((*arit), place);
    }
    debug_msg(1, "Running project test: %s( %s )", func.latin1(), args.join("::").latin1());

    if(func == "requires") {
	return doProjectCheckReqs(args, place);
    } else if(func == "exists") {
	if(args.count() != 1) {
	    fprintf(stderr, "%s:%d: exists(file) requires one argument.\n", parser.file.latin1(),
		    parser.line_no);
	    return FALSE;
	}
	QString file = args.first();
	file = Option::fixPathToLocalOS(file);
	doVariableReplace(file, place);

	if(QFile::exists(file))
	    return TRUE;
	//regular expression I guess
	QString dirstr = QDir::currentDirPath();
	int slsh = file.findRev(Option::dir_sep);
	if(slsh != -1) {
	    dirstr = file.left(slsh+1);
	    file = file.right(file.length() - slsh - 1);
	}
	QDir dir(dirstr, file);
	return dir.count() != 0;
    } else if(func == "system") {
	if(args.count() != 1) {
	    fprintf(stderr, "%s:%d: system(exec) requires one argument.\n", parser.file.latin1(),
		    parser.line_no);
	    return FALSE;
	}
	return system(args.first().latin1()) == 0;
    } else if(func == "contains") {
	if(args.count() != 2) {
	    fprintf(stderr, "%s:%d: contains(var, val) requires two arguments.\n", parser.file.latin1(),
		    parser.line_no);
	    return FALSE;
	}
	QRegExp regx(args[1]);
	QStringList &l = place[args[0]];
	for(QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
	    if(regx.exactMatch((*it))) 
		return TRUE;
	}
	return FALSE;
    } else if(func == "infile") {
	if(args.count() < 2 || args.count() > 3) {
	    fprintf(stderr, "%s:%d: infile(file, var, val) requires at least 2 arguments.\n",
		    parser.file.latin1(), parser.line_no);
	    return FALSE;
	}
	QMakeProject proj;
	QString file = args[0];
	doVariableReplace(file, place);
	fixEnvVariables(file);
	int di = file.findRev(Option::dir_sep);
	QDir sunworkshop42workaround = QDir::current();
	QString oldpwd = sunworkshop42workaround.currentDirPath();
	if(di != -1) {
	    if(!QDir::setCurrent(file.left(file.findRev(Option::dir_sep)))) {
		fprintf(stderr, "Cannot find directory: %s\n", file.left(di).latin1());
		return FALSE;
	    }
	    file = file.right(file.length() - di - 1);
	}
	parser_info pi = parser;
	bool ret = !proj.read(file, oldpwd);
	parser = pi;
	if(ret) {
	    fprintf(stderr, "Error processing project file: %s\n", file.latin1());
	    QDir::setCurrent(oldpwd);
	    return FALSE;
	}
	if(args.count() == 2) {
	    ret = !proj.isEmpty(args[1]);
	} else {
	    QRegExp regx(args[2]);
	    QStringList &l = proj.values(args[1]);
	    for(QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
		if(regx.exactMatch((*it))) {
		    ret = TRUE;
		    break;
		}
	    }
	}
	QDir::setCurrent(oldpwd);
	return ret;
    } else if(func == "count") {
	if(args.count() != 2) {
	    fprintf(stderr, "%s:%d: count(var, count) requires two arguments.\n", parser.file.latin1(),
		    parser.line_no);
	    return FALSE;
	}
	return vars[args[0]].count() == args[1].toUInt();
    } else if(func == "isEmpty") {
	if(args.count() != 1) {
	    fprintf(stderr, "%s:%d: isEmpty(var) requires one argument.\n", parser.file.latin1(),
		    parser.line_no);
	    return FALSE;
	}
	return vars[args[0]].isEmpty();
    } else if(func == "include" || func == "load") {
	if(args.count() != 1) {
	    QString func_desc = "include(file)";
	    if(func == "load")
		func_desc = "load(feature)";
	    fprintf(stderr, "%s:%d: %s requires one argument.\n", parser.file.latin1(),
		    parser.line_no, func_desc.latin1());
	    return FALSE;
	}

	QString file = args.first();
	file = Option::fixPathToLocalOS(file);
	file.replace("\"", "");
	doVariableReplace(file, place);
	if(func == "load") {
	    if(!file.endsWith(Option::prf_ext))
		file += Option::prf_ext;
	    if(file.find(Option::dir_sep) == -1 || !QFile::exists(file)) {
		if(QFile::exists(Option::mkfile::qmakespec + QDir::separator() + file)) {
		    file.prepend(Option::mkfile::qmakespec + QDir::separator());
		} else {
		    bool found = FALSE;
		    QStringList feature_roots;
		    if(getenv("QTDIR"))
			feature_roots << getenv("QTDIR");
#ifdef QT_INSTALL_PREFIX
		    feature_roots << QT_INSTALL_PREFIX;
#endif
#ifdef QT_INSTALL_DATA
		    feature_roots << QT_INSTALL_DATA;
#endif
		    for(QStringList::Iterator it = feature_roots.begin(); it != feature_roots.end(); ++it) {
			QString prf = (*it) + QDir::separator() + QString("mkspecs") +
				      QDir::separator() + QString("features") + QDir::separator() + file;
			if(QFile::exists(prf)) {
			    found = TRUE;
			    file = prf;
			    break;
			}
		    }
		    if(!found) {
			printf("Project LOAD(): Feature %s cannot be found.\n", args.first().latin1());
			exit(3);
		    }
		}
	    }
	}

	debug_msg(1, "Project Parser: %s'ing file %s.", func.latin1(), file.latin1());
	parser_info pi = parser;
	int sb = scope_block;
	int sf = scope_flag;
	TestStatus sc = test_status;
	bool r = read(file.latin1(), place);
	if(r) 
	    vars["QMAKE_INTERNAL_INCLUDED_FILES"].append(file);
	parser = pi;
	test_status = sc;
	scope_flag = sf;
	scope_block = sb;
	return r;
    } else if(func == "error" || func == "message") {
	if(args.count() != 1) {
	    fprintf(stderr, "%s:%d: %s(message) requires one argument.\n", parser.file.latin1(),
		    parser.line_no, func.latin1());
	    return FALSE;
	}
	QString msg = args.first();
	doVariableReplace(msg, place);
	fixEnvVariables(msg);
	printf("Project %s: %s\n", func.upper().latin1(), msg.latin1());
	if(func == "message")
	    return TRUE;
	exit(2);
    } else {
	fprintf(stderr, "%s:%d: Unknown test function: %s\n", parser.file.latin1(), parser.line_no,
		func.latin1());
    }
    return FALSE;
}

bool
QMakeProject::doProjectCheckReqs(const QStringList &deps, QMap<QString, QStringList> &place)
{
    bool ret = FALSE;
    for(QStringList::ConstIterator it = deps.begin(); it != deps.end(); ++it) {
	QString chk = (*it);
	if(chk.isEmpty())
	    continue;
	bool invert_test = (chk.left(1) == "!");
	if(invert_test)
	    chk = chk.right(chk.length() - 1);

	bool test;
	int lparen = chk.find('(');
	if(lparen != -1) { /* if there is an lparen in the chk, it IS a function */
	    int rparen = chk.findRev(')');
	    if(rparen == -1) {
		QCString error;
		error.sprintf("Function (in REQUIRES) missing right paren: %s", chk.latin1());
		qmake_error_msg(error);
	    } else {
		QString func = chk.left(lparen);
		test = doProjectTest(func, chk.mid(lparen+1, rparen - lparen - 1), place);
	    }
	} else {
	    test = isActiveConfig(chk);
	}
	if(invert_test) {
	    chk.prepend("!");
	    test = !test;
	}
	if(!test) {
	    debug_msg(1, "Project Parser: %s:%d Failed test: REQUIRES = %s",
		      parser.file.latin1(), parser.line_no, chk.latin1());
	    place["QMAKE_FAILED_REQUIREMENTS"].append(chk);
	    ret = FALSE;
	}
    }
    return ret;
}


QString
QMakeProject::doVariableReplace(QString &str, const QMap<QString, QStringList> &place)
{
    for(int x = 0, rep; x < 5; x++) {
	QRegExp reg_var;
	reg_var.setMinimal(TRUE);
	if( x == 0 ) //function blocked out by {}'s
	    reg_var = QRegExp("\\$\\$\\{([a-zA-Z0-9_]*)\\((\\(.|(.*)\\)*)\\)\\}");
	else if( x == 1 ) //variables blocked out by {}'s
	    reg_var = QRegExp("\\$\\$\\{([a-zA-Z0-9_\\.-]*)\\}");
	else if(x == 2) //environment
	    reg_var = QRegExp("\\$\\$\\(([a-zA-Z0-9_\\.-]*)\\)");
	else if(x == 3) //function
	    reg_var = QRegExp("\\$\\$([a-zA-Z0-9_]*)\\((\\(.|(.*)\\)*)\\)");
	else if(x == 4) //normal variable
	    reg_var = QRegExp("\\$\\$([a-zA-Z0-9_\\.-]*)");
	while((rep = reg_var.search(str)) != -1) {
	    QString replacement;
	    if(x == 2) {//environment
		replacement = getenv(reg_var.cap(1));
	    } else if(x == 0 || x == 3) { //function
		QStringList args = split_arg_list(reg_var.cap(2));
		for(QStringList::Iterator arit = args.begin(); arit != args.end(); ++arit) {
		    (*arit) = (*arit).stripWhiteSpace(); // blah, get rid of space
		    doVariableReplace((*arit), place);
		}
		debug_msg(1, "Running function: %s( %s )", reg_var.cap(1).latin1(), args.join("::").latin1());
		if(reg_var.cap(1).lower() == "member") {
		    if(args.count() < 1 || args.count() > 2) {
			fprintf(stderr, "%s:%d: member(var, place) requires two arguments.\n",
				parser.file.latin1(), parser.line_no);
		    } else {
			uint pos = 0;
			if(args.count() == 2)
			    pos = args[1].toInt();
			const QStringList &var = place[varMap(args.first())];
			if(var.count() >= pos)
			    replacement = var[pos];
		    }
		} else if(reg_var.cap(1).lower() == "list") {
		    if(args.count() != 1) {
			fprintf(stderr, "%s:%d: list(vals) requires one"
				"argument.\n", parser.file.latin1(), parser.line_no);
		    } else {
			static int x = 0;
			replacement.sprintf(".QMAKE_INTERNAL_TMP_VAR_%d", x++);
			(*((QMap<QString, QStringList>*)&place))[replacement] = split_value_list(args.first());
		    }
		} else if(reg_var.cap(1).lower() == "join") {
		    if(args.count() < 1 || args.count() > 4) {
			fprintf(stderr, "%s:%d: join(var, glue, before, after) requires four"
				"arguments.\n", parser.file.latin1(), parser.line_no);
		    } else {
			QString glue, before, after;
			if(args.count() >= 2)
			    glue = args[1].replace("\"", "" );
			if(args.count() >= 3)
			    before = args[2].replace("\"", "" );
			if(args.count() == 4)
			    after = args[3].replace("\"", "" );
			const QStringList &var = place[varMap(args.first())];
			if(!var.isEmpty())
			    replacement = before + var.join(glue) + after;
		    }
		} else if(reg_var.cap(1).lower() == "find") {
		    if(args.count() != 2) {
			fprintf(stderr, "%s:%d find(var, str) requires two arguments\n",
				parser.file.latin1(), parser.line_no);
		    } else {
			QRegExp regx(args[1]);
			const QStringList &var = place[varMap(args.first())];
			for(QStringList::ConstIterator vit = var.begin();
			    vit != var.end(); ++vit) {
			    if(regx.search(*vit) != -1) {
				if(!replacement.isEmpty())
				    replacement += " ";
				replacement += (*vit);
			    }
			}
		    }
		} else if(reg_var.cap(1).lower() == "system") {
		    if(args.count() != 1) {
			fprintf(stderr, "%s:%d system(execut) requires one argument\n",
				parser.file.latin1(), parser.line_no);
		    } else {
			char buff[256];
			FILE *proc = QT_POPEN(args.join(" ").latin1(), "r");
			while(proc && !feof(proc)) {
			    int read_in = fread(buff, 1, 255, proc);
			    if(!read_in)
				break;
			    for(int i = 0; i < read_in; i++) {
				if(buff[i] == '\n' || buff[i] == '\t')
				    buff[i] = ' ';
			    }
			    buff[read_in] = '\0';
			    replacement += buff;
			}
		    }
		} else {
		    fprintf(stderr, "%s:%d: Unknown replace function: %s\n",
			    parser.file.latin1(), parser.line_no, reg_var.cap(1).latin1());
		}
	    } else { //variable
		if(reg_var.cap(1).left(1) == ".")
		    replacement = "";
		else if(reg_var.cap(1) == "LITERAL_WHITESPACE")
		    replacement = "\t";
		else
		    replacement = place[varMap(reg_var.cap(1))].join(" ");
	    }
	    debug_msg(2, "Project parser: %d (%s) :: %s -> %s", x, str.latin1(),
		      reg_var.capturedTexts().join("::").latin1(), replacement.latin1());
	    str.replace(rep, reg_var.matchedLength(), replacement);
	}
    }
    return str;
}

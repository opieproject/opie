/**********************************************************************
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of Qt Palmtop Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include <qobject.h>
#include <qstring.h>

#ifndef TEXTPARSER_H
#define TEXTPARSER_H

enum t_strType { Word, Number};
enum t_lineType {NewLine, LastLine};

const uint MAX_ELEMENTS = 200;		//Should be dynamic
const uint MAX_LINES = 500;			//Should be dynamic

struct t_splitElm
{
	QChar separator;
	int strType;
	QString str;
};

struct t_splitLine
{
	t_lineType lineType;
	QString str;					//a bit redundant...
	t_splitElm elm[MAX_ELEMENTS];
	int elmCount;
};

class TextParser: public QObject
{
	Q_OBJECT
	
public:
	TextParser(QString in, QString lineBreak);
	TextParser(QString in, QString lineBreak, QString sep);
	int find(QString target, QChar sep, int pos, bool upperCase);
	int elmCount();
	QChar separatorAt(int pos);
	QChar nextSeparator();
	bool hasNextSeparator();
	QString wordAt(int pos);
	QString nextWord();
	bool hasNextWord();
    QString getString(int *pos, QChar stop, bool lineEnd = false);
    QString getNextLine();
    bool hasNextLine();
    int endLinePos(int pos);

private:
	void init();
	void createSeparators();
	t_splitLine nextLine();
	void split();
	t_splitLine splitLine(t_splitLine line);
	bool isSeparator(QChar chr);
	t_splitLine splitDone[MAX_LINES];
	int getLineReference(int pos, int *line, int *inLinePos);
	
	int lineCount, linePos, totalElmCount;
	int separatorPos, wordPos;
	QString data, separators, lineSep;
	int sepAtLine, sepAtPosElm;
	int wordAtLine, wordAtPosElm;
	int atLine, atPosElm;
};

#endif

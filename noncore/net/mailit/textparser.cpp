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
#include "textparser.h"

TextParser::TextParser(QString in, QString lineBreak)
{
	data = in;
	lineSep = lineBreak;
	
	init();
	createSeparators();
	split();
}

TextParser::TextParser(QString in, QString lineBreak, QString sep)
{
	data = in;
	lineSep = lineBreak;
	
	init();
	separators = sep;
	split();
}

void TextParser::init()
{
	lineCount = 0;
	linePos = 0;
	totalElmCount = 0;
	separatorPos = -1;				//not initialized
	wordPos = -1;                   //not initialized
	sepAtLine = 0;
	sepAtPosElm = -1;				//such that nextSep equals 0
	wordAtLine = 0;
	wordAtPosElm = -1;				//such that nextWord equals 0
	atLine = 0;
	atPosElm = 0;
}

void TextParser::createSeparators()
{
	separators = " @#,.:;<>*/(){}|'?-+=_";
}

/*	Returns pos of given search criteria, -1 if not found */
int TextParser::find(QString target, QChar sep, int pos, bool upperCase)
{
	int atLine = 0, atPosElm = 0;
	
	for (int x = 0; x < totalElmCount; x++) {
		if (x >= pos) {
			if (upperCase) {
				if ((splitDone[atLine].elm[atPosElm].str.upper() == target) &&
					(splitDone[atLine].elm[atPosElm].separator == sep))
						return x;
			} else {
				if ((splitDone[atLine].elm[atPosElm].str == target) &&
					(splitDone[atLine].elm[atPosElm].separator == sep))
						return x;
			}
		}
		atPosElm++;
		if (atPosElm >= splitDone[atLine].elmCount) {  //new Line
			atLine++;
			atPosElm = 0;
		}
	}
	
	return -1;
}

int TextParser::elmCount()
{
	return totalElmCount;
}

QChar TextParser::separatorAt(int pos)
{
	if (getLineReference(pos, &sepAtLine, &sepAtPosElm) == -1)
		return QChar::null;
	
	separatorPos = pos;
	return splitDone[sepAtLine].elm[sepAtPosElm].separator;
}

QChar TextParser::nextSeparator()
{
	sepAtPosElm++;
	if (splitDone[sepAtLine].elmCount <= sepAtPosElm) {
		sepAtLine++;
		sepAtPosElm = 0;
	}
	
	separatorPos++;
	return splitDone[sepAtLine].elm[sepAtPosElm].separator;
}

bool TextParser::hasNextSeparator()
{
	return ((separatorPos+1) < totalElmCount);
}

QString TextParser::wordAt(int pos)
{
	if (getLineReference(pos, &wordAtLine, &wordAtPosElm) == -1)
		return NULL;
	
	wordPos = pos;
	return splitDone[wordAtLine].elm[wordAtPosElm].str;
}

QString TextParser::nextWord()
{
	wordAtPosElm++;
	if (splitDone[wordAtLine].elmCount <= wordAtPosElm) {
		wordAtLine++;
		wordAtPosElm = 0;
	}
		
	wordPos++;
	return splitDone[wordAtLine].elm[wordAtPosElm].str;
}

bool TextParser::hasNextWord()
{
	return ((wordPos + 1) < totalElmCount);
}

QString TextParser::getString(int *pos, QChar stop, bool lineEnd = false)
{
	QString returnStr = wordAt(*pos);
	QChar chr = separatorAt(*pos);
	QString s;
	
	if (returnStr == "")
		return "";
	if (chr == stop)
		return returnStr;
	
	if (!lineEnd) {
		while ((chr != stop) && hasNextWord()) {
			returnStr.append(chr);
			returnStr += nextWord();
			chr = nextSeparator();
		}
	} else {								//copy from pos to end of line
		getLineReference(*pos, &atLine, &atPosElm);
		returnStr = "";
		while (atPosElm < splitDone[atLine].elmCount) {
			if (splitDone[atLine].elm[atPosElm].str != "") {
				returnStr += splitDone[atLine].elm[atPosElm].str;
			}
			chr = splitDone[atLine].elm[atPosElm].separator;
			if (!chr.isNull() && (int) chr != 0) {
				returnStr.append(splitDone[atLine].elm[atPosElm].separator);
			}
			atPosElm++;
		}
	}
	
	*pos = wordPos;
	return returnStr;
}

QString TextParser::getNextLine()
{
	atLine++;
	atPosElm = 0;
	if (atLine < lineCount)
		return splitDone[atLine].str;
	return "";
}

bool TextParser::hasNextLine()
{
	if (atLine+1 < lineCount)
		return TRUE;;
	return FALSE;
}

int TextParser::endLinePos(int pos)
{
	if ( (getLineReference(pos, &atLine, &atPosElm)) == -1)
		return -1;
	
	return (pos + (splitDone[atLine].elmCount - atPosElm) + 1);
}

int TextParser::getLineReference(int pos, int *line, int *inLinePos)
{
	int currentPos = 0;
	
	for (int x = 0; x < lineCount; x++) {
		if ( currentPos + splitDone[x].elmCount > pos) {
			*line = x;
			*inLinePos = pos - currentPos;
			return 0;						//pos found okay
		}
		currentPos += splitDone[x].elmCount;
	}
	return -1;								//no reference found
}

void TextParser::split()
{
	t_splitLine newLine;

	while ((uint) linePos < data.length()) {
		newLine = nextLine();
		splitDone[lineCount] = splitLine(newLine);
		totalElmCount += splitDone[lineCount].elmCount;
		lineCount++;
	}
}

t_splitLine TextParser::splitLine(t_splitLine line)
{
	uint pos = 0;
	uint elmCount = 0;
	t_splitLine tempLine = line;
	
	tempLine.str = line.str.simplifyWhiteSpace();
	tempLine.elm[0].str = "";
	while ( pos < line.str.length() ) {
		if ( isSeparator(tempLine.str[pos]) ) {
			tempLine.elm[elmCount].separator = tempLine.str[pos];
			elmCount++;
			pos++;
			while (tempLine.str[pos] == ' ')
				pos++;
			if (pos > line.str.length())
				elmCount--;
			tempLine.elm[elmCount].str = "";
		} else {
			if (!tempLine.str[pos].isNull())
				tempLine.elm[elmCount].str += tempLine.str[pos];
			pos++;
		}
	}
	
	tempLine.elmCount = elmCount + 1;
	return tempLine;
}

bool TextParser::isSeparator(QChar chr)
{
	for (uint x = 0; x < separators.length(); x++) {
		if (chr == separators[x])
			return true;
	}
	return false;
}

t_splitLine TextParser::nextLine()
{
	int newLinePos;
	t_splitLine lineType;
	
	newLinePos = data.find(lineSep, linePos);
	
	lineType.lineType = NewLine;
	lineType.str = "";
	
	if (newLinePos == -1) {
		newLinePos = data.length();
		lineType.lineType = LastLine;
	}
	
	for (int x = linePos; x < newLinePos; x++)
		lineType.str += data[x];
		
	linePos = newLinePos;
	if ((uint) linePos < data.length())		//if not EOF, add length of lineSep
		linePos += lineSep.length();
	
	return lineType;
}

/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
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

#ifndef _QIMPENMATCH_H_
#define _QIMPENMATCH_H_

#include "qimpenchar.h"

#include <qpe/qdawg.h>

#include <qlist.h>

class QTimer;
class QIMPenWidget;
class QIMPenSetup;
class QIMPenWordPick;

class QIMPenMatch : public QObject
{
    Q_OBJECT
public:
    QIMPenMatch( QObject *parent=0, const char *name=0 );
    virtual ~QIMPenMatch();

    void resetState();
    void backspace();
    void setMultiStrokeTimeout( int t ) { multiTimeout = t; }

    const QString &word() const { return wordEntered; }

    void setWordMatchingEnabled( bool e ) { doWordMatching = e; }
    bool isWordMatchingEnabled() const { return doWordMatching; }

    struct MatchWord {
	MatchWord( const QString &w, int e ) { word = w; error = e; }
	QString word;
	int error;
    };

    class MatchWordList : public QList<MatchWord>
    {
    public:
	int compareItems( QCollection::Item item1, QCollection::Item item2 ) {
	    MatchWord *m1 = (MatchWord *)item1;
	    MatchWord *m2 = (MatchWord *)item2;
	    return m1->error - m2->error;
	}
    };

public slots:
    void setCharSet( QIMPenCharSet * );
    void beginStroke();
    void strokeEntered( QIMPenStroke *st );

signals:
    void erase();
    void noMatch();
    void removeStroke();
    void keypress( uint ch );
    void matchedCharacters( const QIMPenCharMatchList & );
    void matchedWords( const QIMPenMatch::MatchWordList & );

protected slots:
    void processMatches( QIMPenCharMatchList &ml );
    void endMulti();

protected:
    void updateWordMatch( QIMPenCharMatchList &ml );
    void matchWords();
    void scanDict( const QDawg::Node* n, int ipos, const QString& str, int error );

    QList<QIMPenStroke> strokes;
    QIMPenChar *prevMatchChar;
    int prevMatchError;
    QIMPenCharSet *charSet;
    QIMPenCharSet *multiCharSet;
    QList<QIMPenCharMatchList> wordChars;
    MatchWordList wordMatches;
    QString wordEntered;
    bool doWordMatching;
    bool canErase;
    int errorThreshold;
    int goodMatches;
    int badMatches;
    int maxGuess;
    QTimer *multiTimer;
    int multiTimeout;
};

#endif // _QIMPENINPUT_H_

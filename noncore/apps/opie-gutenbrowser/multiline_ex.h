/**********************************************************************
** $Id: multiline_ex.h,v 1.1 2004-04-07 13:07:42 llornkcor Exp $
**
** Definition of MultiLine_Ex widget class
** Copyright (C) 1992-1999 Troll Tech AS.  All rights reserved.
**
** See the file LICENSE included in the distribution for the usage
** and distribution terms, or http://www.troll.no/free-license.html.
**
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
*****************************************************************************/

#ifndef MULTILINE_EX_H
#define MULTILINE_EX_H

#ifndef QT_H
#include <qlist.h>
#include <qstring.h>
#include <qtableview.h>
//#include <qtable.h>

#include <qmultilineedit.h>
#endif // QT_H

struct MultiLine_ExData;
//class MultiLine_ExData;

class/* Q_EXPORT*/ MultiLine_Ex : public QMultiLineEdit
{
    Q_OBJECT
public:
    MultiLine_Ex( QWidget *parent=0, const char *name=0 );//:
//  QMultiLineEdit(QWidget *parent=0, QString& name=0 );
 //  ~MultiLine_Ex();

//    const char *textLine( int line ) const;
//    QString text() const;
//int row;
//    int numLines() const;
//
//    bool  isReadOnly() const;
//    bool  isOverwriteMode() const;
//
//    void  setFont( const QFont &font );
//    void insertLine( const char *s, int line = -1 );
//    void insertAt( const char *s, int line, int col );
//    void removeLine( int line );
//
//    void  cursorPosition( int *line, int *col ) const;
//    void  setCursorPosition( int line, int col, bool mark = FALSE );
//    void  getCursorPosition( int *line, int *col );
//    bool  atBeginning() const;
//    bool  atEnd() const;
//
//    bool  autoUpdate()    const;
//    void  setAutoUpdate( bool );
//
//    void  setFixedVisibleLines( int lines );
public:
int row;
//setPaper( QBrush& pap);
    bool hasSelectedText();
    void pageUp( bool mark=FALSE );
    void pageDown( bool mark=FALSE );
    void cursorLeft( bool mark=FALSE, bool wrap = TRUE );
    void cursorRight( bool mark=FALSE, bool wrap = TRUE );
    void cursorUp( bool mark=FALSE );
    void cursorDown( bool mark=FALSE );
    int lastRow();
    int topRow();
    int editSize( );
    int pageSize ;
    int maxLineWidth() const;
    void ScrollUp( int lines);
    void ScrollDown( int lines);
    int Rows();
    int Top();
    int lineHeight( int row );
    int PageSize();
public slots:
//    void       clear();
//    void       setText( const char * );
//    void       append( const char * );
//    void       deselect();
//    void       selectAll();
//    void       setReadOnly( bool );
//    void       setOverwriteMode( bool );
//    void       paste();
//    void       copyText();
//    void       cut();
signals:
//    void  textChanged();
//    void  returnPressed();

protected:
protected:
private slots:
private:

private:    // Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    MultiLine_Ex( const MultiLine_Ex & );
    MultiLine_Ex &operator=( const MultiLine_Ex & );
#endif
};
#endif


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
#ifndef PICKBOARDCFG_H
#define PICKBOARDCFG_H

#include <qpe/qdawg.h>

#include <qpushbutton.h>
#include <qhbuttongroup.h>
#include <qdialog.h>
#include <qlist.h>

// Internal stuff...

class PickboardPicks;

class LetterButton : public QPushButton {
    Q_OBJECT
public:
    LetterButton(const QChar& letter, QWidget* parent);
private slots:
    void toggleCase();
private:
    bool skip;
};

class LetterChoice : public QButtonGroup {
    Q_OBJECT
public:
    LetterChoice(QWidget* parent, const QString& set);

    QChar choice() { return ch; }

signals:
    void changed();

private slots:
    void change();

private:
    QChar ch;
};

class PickboardAdd : public QDialog {
    Q_OBJECT
public:
    PickboardAdd(QWidget* owner, const QStringList& setlist);
    ~PickboardAdd();

    QString word() const;
    bool exec();

private slots:
    void checkAllDone();

private:
    QPushButton *yes;
    LetterChoice **lc;
    int nlc;
};

class PickboardConfig : QObject {
public:
    PickboardConfig(PickboardPicks* p) : parent(p), nrows(2), pressx(-1) { }
    virtual ~PickboardConfig();
    virtual void pickPoint(const QPoint& p, bool press);
    virtual void draw(QPainter*)=0;
    virtual void fillMenu(QPopupMenu&);
    virtual void doMenu(int);

protected:
    void updateRows(int from, int to);
    virtual void updateItem(int r, int i);
    virtual void pickInRow(int r, int xpos, bool press)=0;

    void changeMode(int m);
    virtual void generateText(const QString& s);
    void generateKey( int k );

    virtual void pick(bool press, int row, int item)=0;

protected:
    PickboardPicks* parent;
    int nrows;
private:
    int pressrow, pressx;
};

class StringConfig : public PickboardConfig {
public:
    StringConfig(PickboardPicks* p) : PickboardConfig(p) { }

    void draw(QPainter* p);

protected:
    virtual QString text(int r, int i)=0;
    virtual bool spreadRow(int i)=0;
    virtual QColor rowColor(int) { return Qt::black; }
    virtual void pickInRow(int r, int xpos, bool press);
    virtual void updateItem(int r, int i);
    virtual bool highlight(int,int) const;
};

class CharStringConfig : public StringConfig {
    QString input;
    QStringList chars;
public:
    CharStringConfig(PickboardPicks* p) : StringConfig(p) { }

    void addChar(const QString& s);
    virtual void doMenu(int);

protected:
    QString text(int r, int i);
    bool spreadRow(int i);
    void pick(bool press, int row, int item);
};

class DictFilterConfig : public StringConfig {
    QStringList matches;
    QStringList sets;
    QStringList othermodes;
    int lit0;
    int lit1;
    int shift;
    QString capitalize(const QString& s);
    QStringList capitalize(const QStringList& s);

public:
    QStringList input;
    DictFilterConfig(PickboardPicks* p) : StringConfig(p)
    {
	shift = 0;
	lit0 = -1;
	lit1 = -1;
    }

    void addSet(const QString& s);
    void addMode(const QString& s);

    void fillMenu(QPopupMenu& menu);
    void doMenu(int i);
    
    void add(const QString& set);

protected:
    QString text(int r, int i);

    bool spreadRow(int i);

    void pick(bool press, int row, int item);

    bool scanMatch(const QString& set, const QChar& l) const;
    void scan(const QDawg::Node* n, int ipos, const QString& str, int length, bool extend);
    void scanLengths(const QDawg::Node* n, int ipos, int& bitarray);

    bool highlight(int r,int c) const;
};

class CharConfig : public StringConfig {
    QStringList chars1;
    QStringList chars2;
public:
    CharConfig(PickboardPicks* p) : StringConfig(p) { }
    void addChar(int r, const QString& s);

protected:
    QString text(int r, int i);
    bool spreadRow(int);
    void pick(bool press, int row, int item);
};

class KeycodeConfig : public PickboardConfig {
    QValueList<int> keys1;
    QValueList<int> keys2;
    QValueList<QPixmap> keypm1;
    QValueList<QPixmap> keypm2;
    static const int xw = 8;
    static const int xmarg = 8;

public:
    KeycodeConfig(PickboardPicks* p) : PickboardConfig(p) { }
    void addKey(int r, const QPixmap& pm, int code);
    void addGap(int r, int w);

    void draw(QPainter* p);

protected:
    void pickInRow(int r, int xpos, bool press);
    QValueList<QPixmap> row(int i);

    void pick(bool press, int row, int item);
};


#endif

/**********************************************************************
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the Qtopia Environment.
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


/* This file represents shared data structures that will be passed
 * around often.
 */
#ifndef __SHAREDDATA_H__
#define __SHAREDDATA_H__

// TODO rename this to a sensable class name

#include <qvector.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qcstring.h>
#include <qdatastream.h>
#include <qintdict.h>

class DBStore;

/* helper classes to common classes */
class QStringVector : public QVector<QString> 
{ 
public: 
    int compareItems(Item a, Item b);
};

/* in QT 2.3, dates and times not supported int QVariant.  So.....
 * for now use my special Variant type which is basically identical
 * except that does it for my types.  TODO replace with QVariant when
 * qvariant supports all the types we require */

class TVVariantPrivate;

class TVVariant 
{
public:
    enum KeyType {
        Invalid = 0,
        Int,     
        String, 
        Date, 
        Time,
    };

    TVVariant();
    ~TVVariant();

    TVVariant(const TVVariant&);
    TVVariant(QDataStream&);

    TVVariant(const QString &);
    TVVariant(const int);
    TVVariant(const QDate &);
    TVVariant(const QTime &);

    TVVariant& operator=(const TVVariant& );
    bool operator==(const TVVariant&) const;
    bool operator!=(const TVVariant&) const;
    bool operator<(const TVVariant&) const;
    bool operator>(const TVVariant&) const;

    bool closer(TVVariant, TVVariant);
    bool close(TVVariant);

    KeyType type() const;
    const QString typeName() const;
    int numTypes() const;

    const QString typeName(KeyType) const;
    bool canCast(KeyType) const;
    bool isValid() const;
    void clear();

    const QString toString() const;
    const QDate toDate() const;
    const QTime toTime() const;
    int toInt() const;

    QString& asString();
    QDate& asDate();
    QTime& asTime();
    int& asInt();

    void load(QDataStream&);
    void save(QDataStream&) const;

    static const QString typeToName(KeyType typ);
    static KeyType nameToType(const QString &);
private:
    void detach();

    TVVariantPrivate *d;
};

class TVVariantPrivate : public QShared
{
    public: 
        TVVariantPrivate(); 
        TVVariantPrivate(TVVariantPrivate *); 

        ~TVVariantPrivate(); 
        
        void clear(); 
        
        TVVariant::KeyType typ; 
        
        union { 
            int i; 
            void *ptr; 
        } value;
};

inline TVVariant::KeyType TVVariant::type() const
{
    return d->typ;
}

inline bool TVVariant::isValid() const
{
    return (d->typ != Invalid);
}

inline int TVVariant::numTypes() const
{
    return 4;
}

class Key {
public:
    Key();
    Key(QString name, TVVariant example, int flags = 0);
	Key(const Key &);
    Key& operator=(const Key& );

    QString name() const;
    TVVariant example() const;
    TVVariant::KeyType type() const;
    int flags() const;

    void setName(const QString &);
    void setExample(const TVVariant &);
    void setFlags(int);

    bool delFlag() const;
    bool newFlag() const;

    void setDelFlag(bool);
    void setNewFlag(bool);

private:
    QString kname;
    TVVariant kexample;
    int kflags;
};

class KeyList : public QIntDict<Key> {
public:
    KeyList();
    KeyList(const KeyList&);

    ~KeyList();

	bool operator!=(const KeyList &);

    int getNumFields() const;

    int addKey(QString KeyName, TVVariant example);
    int addKey(QString KeyName, TVVariant::KeyType type);

    TVVariant getKeyExample(int ) const;
    void setKeyExample(int, TVVariant e);

    QString getKeyName(int i) const;
    void setKeyName(int i, const QString &n);

    TVVariant::KeyType getKeyType(int i) const;
    void setKeyType(int i, TVVariant::KeyType);

    int getKeyIndex(QString q) const;

    int getKeyFlags(int i) const;
    void setKeyFlags(int i, int flag);

    /* Below should be abstracted a bit more */
    bool checkNewFlag(int i) const;
    void setNewFlag(int i, bool f);
    bool checkDeleteFlag(int i) const;
    void setDeleteFlag(int i, bool f);

    bool validIndex(int) const;
};

class KeyListIterator : public QIntDictIterator<Key>
{
public:
    KeyListIterator(const KeyList &k) : QIntDictIterator<Key>(k) {};
};

/* TODO start using this */
class DataElem {
public:
    DataElem(DBStore *container);
    ~DataElem();

    int getNumFields() const; 
    KeyList getKeys() const;

    bool hasValidValue(int) const; 
    bool hasValidValue(QString) const; 
    TVVariant::KeyType getFieldType(int) const; 
    TVVariant::KeyType getFieldType(QString) const; 
    TVVariant getField(int) const;
    TVVariant getField(QString) const;

    void setField(int, QString);
    void setField(int, TVVariant);
    void setField(QString, QString);
    void setField(QString, TVVariant);
    void unsetField(int);
    void unsetField(QString);

    QString toQString() const;
    QString toQString(int i) const;
    QString toSortableQString(int i) const;

    /* compare functions */
    bool lessThan(int i, TVVariant) const;
    bool moreThan(int i, TVVariant) const;
    bool equalTo(int i, TVVariant) const;
    bool contains(int i, TVVariant) const;
    bool startsWith(int i, TVVariant) const;
    bool endsWith(int i, TVVariant) const;

    /* class functions... Compare is based of the primary key, which
       is determined by the containing DBStores of each element. */
    static int compare(const TVVariant, const TVVariant, int i);

    /* False, second element's primary key is closer to target. 
     * True, first element's primary key is a closer match to target */
    static bool closer(DataElem*, DataElem *, TVVariant, int column);
private:
    QIntDict<TVVariant> values;
    DBStore *contained;
};

typedef struct _TableState {
    int current_column;
    KeyList *kRep;
    DataElem *current_elem;
} TableState;

/* Stream functions */
#ifndef QT_NO_DATASTREAM
Q_EXPORT QDataStream &operator<<( QDataStream &, const KeyList & );
Q_EXPORT QDataStream &operator<<( QDataStream &, const DataElem & );
Q_EXPORT QDataStream &operator>>( QDataStream &, KeyList & );
Q_EXPORT QDataStream &operator>>( QDataStream &, DataElem & );


Q_EXPORT QDataStream &operator>>( QDataStream &, TVVariant & );
Q_EXPORT QDataStream &operator<<( QDataStream &, const TVVariant & );
Q_EXPORT QDataStream &operator>>( QDataStream &, TVVariant::KeyType& );
Q_EXPORT QDataStream &operator<<( QDataStream &, const TVVariant::KeyType& );
#endif

#endif

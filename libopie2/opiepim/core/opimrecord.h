#ifndef OPIE_PIM_RECORD_H
#define OPIE_PIM_RECORD_H

#include <qdatastream.h>
#include <qmap.h>
#include <qstring.h>
#include <qstringlist.h>

/*
 * we need to get customMap which is private...
 */
#define private protected
#include <qpe/palmtoprecord.h>
#undef private

#include <opie/opimxrefmanager.h>

/**
 * This is the base class for
 * all PIM Records
 *
 */
class OPimRecord : public Qtopia::Record {
public:
    /**
     * c'tor
     * uid of 0 isEmpty
     * uid of 1 will be assigned a new one
     */
    OPimRecord(int uid = 0);
    ~OPimRecord();

    /**
     * copy c'tor
     */
    OPimRecord( const OPimRecord& rec );

    /**
     * copy operator
     */
    OPimRecord &operator=( const OPimRecord& );

    /**
     * category names resolved
     */
    QStringList categoryNames( const QString& appname )const;

    /**
     * set category names they will be resolved
     */
    void setCategoryNames( const QStringList& );

    /**
     * addCategoryName adds a name
     * to the internal category list
     */
    void addCategoryName( const QString& );

    /**
     * if a Record isEmpty
     * it's empty if it's 0
     */
    virtual bool isEmpty()const;

    /**
     * toRichText summary
     */
    virtual QString toRichText()const = 0;

    /**
     * a small one line summary
     */
    virtual QString toShortText()const = 0;

    /**
     * the name of the Record
     */
    virtual QString type()const = 0;

    /**
     * matches the Records the regular expression?
     */
    virtual bool match( const QString &regexp ) const
	{setLastHitField( -1 );
            return Qtopia::Record::match(QRegExp(regexp));};

    /**
     * if implemented this function returns which item has been
     * last hit by the match() function.
     * or -1 if not implemented or no hit has occured
     */
    int lastHitField()const;

    /**
     * converts the internal structure to a map
     */
    virtual QMap<int, QString> toMap()const = 0;

    /**
     * key value representation of extra items
     */
    QMap<QString, QString> toExtraMap()const;
    void setExtraMap( const QMap<QString, QString>& );

    /**
     * the name for a recordField
     */
    virtual QString recordField(int)const = 0;

    /**
     * returns a reference of the
     * Cross Reference Manager
     * Partner 'One' is THIS PIM RECORD!
     * 'Two' is the Partner where we link to
     */
    OPimXRefManager& xrefmanager();

    /**
     * set the uid
     */
    virtual void setUid( int uid );

    /*
     * used inside the Templates for casting
     * REIMPLEMENT in your ....
     */
    static int rtti();

    /**
     * some marshalling and de marshalling code
     * saves the OPimRecord
     * to and from a DataStream
     */
    virtual bool loadFromStream(QDataStream& );
    virtual bool saveToStream( QDataStream& stream )const;

protected:
    // need to be const cause it is called from const methods
    mutable int m_lastHit;
    void setLastHitField( int lastHit )const;
    Qtopia::UidGen &uidGen();
//    QString crossToString()const;

private:
    class OPimRecordPrivate;
    OPimRecordPrivate *d;
    OPimXRefManager m_xrefman;
    static Qtopia::UidGen m_uidGen;

private:
    void flush( const OPimXRefPartner&, QDataStream& stream )const;
    OPimXRefPartner partner( QDataStream& );
};



#endif

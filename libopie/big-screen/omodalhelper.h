/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2003 hOlgAr <zecke@handhelds.org>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#ifndef HAZE_OMODAL_HELPER_H
#define HAZE_OMODAL_HELPER_H

#include <qwidget.h>
#include <qvaluelist.h>
#include <qmap.h>

typedef int TransactionID;

class OModalHelperControler;
class OModalHelperSignal;
class QDialog;

struct OModalHelperBase  {
    virtual void done( TransactionID id, QWidget* ) = 0;
    virtual void next( TransactionID, OModalHelperControler * );
    virtual void prev( TransactionID, OModalHelperControler * );
};

/**
 * Modality sucks! ;) But it is easy to work with
 * do exec() on a dialog and you know everything is funky.
 * You only need to have one Dialog loaded and so on.
 * This class helps you to work like with modality and help
 * you to keep things in sync
 * It's a template class but it sends signals once one Item is ready
 * the signals contains the status and id of the item and then you
 * can fetch it.
 * Also if you edit an record external you can tell this class and it'll
 * call the merge() function of your widget to maybe merge in these changes.
 * It also supports multiple modes. Either it can create new dialogs
 * for each item or it can queue them depending on your usage. But it is
 * so smart that if only one item is shown that the queue bar is not shown
 * See the example for simple usage.
 *
 * @short helps to life without modaility
 * @author hOlgAr
 * @version 0.01
 */
template<class Dialog, class Record, typename Id = QString>
class OModalHelper : private OModalHelperBase{
    friend class OModalHelperSignal;
    friend class OModalHelperControler;
public:
    typedef QValueList<Record> RecordList;
    typedef QMap<Id, Record> IdMap;
    typedef QMap<TransactionID, Id> TransactionMap;
    typedef QMap<QDialog*, TransactionID> DialogMap
    enum Mode { Queue, New };
    OModalHelper(enum Mode mode, QObject* parnet );

    bool handles( Id id)const;

    void cancel();
    void cancel( TransactionID );

    void connect( QObject* rec, const char* slot );

    TransactionID handle( const Record& rec = Record() );

    void edited( TransactionID, int what, const QString& data );

    Record record( TransactionID )const;
    RecordList done()const;
private:
};



/* ### FIXME use namespace with Qt3 */

/*
 * A note on flow. The Signal is used for QT Signals when
 * a record is done.
 * There is either one controler and this controler slot will
 * be connected to a dialog signal.
 * In Queue we get the next and prev signals and call the Helper.
 * this then changes the Record of the dialog and sets the transactionId
 * of the controler.
 * For the new mode
 *
 */

class OModalHelperSignal : public QObject {
    Q_OBJECT
public:
    OModalHelperSignal();

signals:
    done( int status,  TransactionID transaction );
    accpeted( TransactionID transaction );
    rejected( TransactionID transaction );
};


class OModalHelperControler : public QObject {
    Q_OBJECT
public:
    OModalHelperControler( TransactionID id );
    virtual TransactionID transactionID()const;
    void setTransactionID( TransactionID id );
    QDialog* dialog()const;

public slots:
    virtual done(int result );
private:
    QDialog *m_dia;
    TransactionID m_id;
}

#endif

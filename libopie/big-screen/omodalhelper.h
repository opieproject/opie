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

#include <qdialog.h>
#include <qwidget.h>
#include <qvaluelist.h>
#include <qmap.h>

typedef int TransactionID;

class OModalHelperControler;
class OModalHelperSignal;
class QDialog;

struct OModalHelperBase  {
    virtual void done( TransactionID ) = 0;
    virtual void next( TransactionID ) = 0;
    virtual void prev( TransactionID ) = 0;
};

/**
 * Modality sucks! ;) But it is easy to work with
 * do exec() on a dialog and you know everything is funky.
 * You only need to have one Dialog loaded and so on.
 * This class helps you to work like with modality and help
 * you to keep things in sync
 * It's a template class but it sends signals once one Item is ready
 * the signals contains the status and id of the item and then you
 * need fetch it.
 * Handled Records will stay available until the first call to retrieve
 * either the record via the TransactionID or via the QValueList<Record>. Note
 * that most functions do not take handled records into account.
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
template<class Dialog, class Record, typename Id = int>
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
    TransactionID transactionID( Id id)const;

    void cancel();
    void cancel( TransactionID );

    void connectDone( QObject* rec, const char* slot );
    void connectAccepted( QObject* rec, const char* slot );
    void connectRejected( QObject* rec, const char* slot );

    TransactionID handle( Id id, const Record& rec = Record() );

    void edited( TransactionID, int what, const QString& data );

    Record record( TransactionID )const;
    RecordList done()const;
private:
    virtual void done( TransactionID );
    virtual void next( TransactionID );
    virtual void prev( TransactionID );

    Record nextRecord( TransactionID, int * )const;
    Record prevRecord( TransactionID, int * )const;

private:
    OModalHelperDialog     *queuedDialog()const; // generate or recycle
    OModalHelperDialog     *m_dialog;
    OModalHelperSignal     *m_signal; // our signal
    OModalHelperControler  *m_controler;
    IdMap                  m_ids; // maps ids (uids) to a record
    IdMap                  m_doneIds;
    TransactionMap         m_transactions; // activate transactions
    TransactionMap         m_done; // done and waiting for getting picked
    DialogMap              m_editing; // only used for New Mode
    enum Mode              m_mode; // the mode we're in
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
    OModalHelperSignal(OModalHelperBase* base,  QObject* parent);
    ~OModalHelperSignal();

signals:
    done( int status,  TransactionID transaction );
    accepted( TransactionID transaction );
    rejected( TransactionID transaction );

private:
    OModalHelperBase* m_base;
};


class OModalHelperControler : public QObject {
    Q_OBJECT
public:
    OModalHelperControler( OModalHelperBase* , QObject* parent);
    virtual TransactionID transactionID()const;
    void setTransactionID( TransactionID id );
    QDialog* dialog()const;

public slots:
    virtual void done(int result );
    virtual void next();
    virtual void prev();
private:
    QDialog *m_dia;
    TransactionID m_id;
    OModalHelperBase *m_base;
}

struct OModalQueueBar;
class OModalQueuedDialog : public QDialog {
    Q_OBJECT
public:
    OModalQueuedDialog(QDialog *mainWidget);
    ~OModalQueuedDialog();

    QDialog* centerDialog()const;

    void setQueueBarEnabled( bool = true );
    void setRecord( int record, int count );

signals:
    void next();
    void prev();

private:
    OModalQueueBar *m_bar;
    QDialog *m_center;
};


/*
 * Tcpp Template Implementation
 */

/**
 * This is the simple Template c'tor. It takes the mode
 * this helper should operate in and the parent object.
 * This helper will be deleted when the parent gets deleted
 * or you delete it yourself.
 *
 * @param mode The mode this dialog should be in
 * @param parent The parent QObject of this helper.
 */
template<class Dialog, class Record, typename Id>
OModalHelper<Dialog, Record, Id>::OModalHelper( enum Mode mode, QObject* parent ) {
    m_mode = mode;
    m_signal = new OModalHelperSignal( this, parent );
    m_controler = new OModalHelperControler( this, m_signal );

}


/**
 * This functions looks for your record and sees if it is
 * handled with this helper. Note that done records
 * will not be returned.
 *
 * @return true if the record is currenlty edited otherwise false
 *
 * @param Id The id which might be handled
 */
template<class Dialog, class Record, typename Id>
bool OModalHelper<Dialog, Record, Id>::handles( Id id )const {
    if ( m_transactions.isEmpty() )
        return false;

    TransactionMap::ConstIterator it = m_transactions.begin();
    for ( ; it != m_transactions.end(); ++it )
        if ( it.data() == id )
            return true;

    return false;
}


/**
 * just like handles( Id ) but returns the TransactionId
 */
template<class Dialog, class Record, typename Id>
TransactionID OModalHelper<Dialog, Record, Id>::transactionID( Id id)const {
    if ( m_transactions.isEmpty() )
        return false;

    TransactionMap::ConstIterator it = m_transactions.begin();
    for ( ; it != m_transactions.end(); ++it )
        if ( it.data() == id )
            return true;

    return false;
}

/**
 * Cancel will cancel all current operations and clear the list
 * of done operations as well.
 * This also clears all done operations you did not popped
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::cancel() {
    m_ids.clear();
    m_doneIds.clear();
    m_done.clear();
    m_transactions.clear();

    /* we also need to remove the QDialogs */
    /* and hide the queue dialog if present */
    if (m_mode == New && !m_editing.isEmpty() ) {
        for (DialogMap::Iterator it = m_editing.begin(); it != m_editing.end(); ++it )
            delete it.key();
        m_editing.clear();
    }else if (m_dialog )
        queuedDialog()->setRecord( 0, 0 );

    m_controler->setTransactionID( -1 );
}


/**
 * This cancels editing of the record behind the Transaction Number
 * Note that if editing is already done it will also be removed from this list
 */
template<class Dialog, class Record, typename Id>
void OModalHelper::cancel( TransactionID tid ) {
    /* wrong tid */
    if (!m_transactions.contains( tid ) && !m_done.contains( tid) )
        return;

    if (m_mode == New )
        /* reverse map eek */
        for (DialogMap::Iterator it = m_editing.begin(); it != m_editing.end(); ++it )
            if ( it.data() == tid ) {
                it.key()->hide();
                delete it.key();
                it = m_editing.remove( it );
                break;
            }
     else if ( m_transactions.contains( tid ) ) {
        /* need to stop editing from the queue block */
        /* if we're currently editing actiavte the previous */
#if 0
        if (tid == m_controler->transactionID() ) {

        }
#endif
        /* now either activate the previous again or hide  */
        if (!  m_transactions.count() -1 )
            queuedDialog()->setRecord( 0, 0 );
        else {
            int pos;
            Record rec = prevRecord( tid, &pos );
            static_cast<Dialog*>( queuedDialog()->centerDialog() )->setRecord( rec );
            queuedDialog()->setRecord( pos, m_transactions.count() );
            m_controler->setTransactionID( tid );
        }
    }
    /* now remove from the various maps  done and currently editing map*/
    if (m_transactions.contains( tid ) )
        m_ids.remove( m_transactions[tid]  );
    if (m_done.contains( tid ) )
        m_doneIds.remove( m_done[tid ]  );


    m_done.remove( tid );
    m_transactions.remove( tid );
}

/**
 * Connect to the done Signal. SIGNAL( done(int, TransactionID ) )
 * This signal gets emitted whenever a Record was accepted or rejected
 *
 * @param rec  The object where the slot belongs to
 * @param slot The slot which should be called. See the needed parameter above
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::connectDone( QObject* rec, const char* slot ) {
    QObject::connect(m_signal, SIGNAL(done(int, TransactionID) ),
                     rec, slot );
}

/**
 * Connect to the accepted Signal. SIGNAL( accepted(TransactionID ) )
 * This signal gets emitted whenever a Record was accepted
 *
 * @param rec  The object where the slot belongs to
 * @param slot The slot which should be called. See the needed parameter above
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::connectAccepted( QObject* rec, const char* slot ) {
    QObject::connect(m_signal, SIGNAL(accepted(TransactionID) ),
                     rec, slot );
}

/**
 * Same as the accepted method but this one gets emitted if the dialog
 * got rejected.
 * SIGNAL( rejected(TransactionID) )
 *
 * @param rec The QObject of the slot
 * @param slot The slot make sure the signature is correct
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::connectRejected( QObject* rec, const char* slot ) {
    QObject::connect(m_signal, SIGNAL(rejected(TransactionID) ),
                     rec, slot );
}

/**
 * Tell the helper to handle a record. If the record is currently handled
 * it will be made active.
 * Already handled record which are waiting getting popped are not taken into account
 * Otherwise this helpers make the record editable.
 * The record supplied needs to have a valid copy operator and constructor.
 * In the case where the record is already present the parameter gets discarded.
 * If you want the new record to be taken you need to cancel the Transaction first
 *
 * @param id  The Identification of the Record. For PIM it would uid()
 * @param rec The record we want to be edited
 *
 * @returns This functions returns the TransactionId assigned to the record
 *
 */
template<class Dialog, class Record, typename Id>
TransactionID OModalHelper<Dialog, Record, Id>::handle( Id id, const Record& rec ) {
    static TransactionID t_id = 0;
    t_id++;
    /*
     *this method consists out of two parts divided each into New and Queued Mode.
     * Either we have the dialog already, in this case we need to highlight the widget
     * Or we need to add it.
     */
    TransactionID tid = 0;
    /* we already have the record lets see if it was done or not */
    if ( m_ids.contains( id ) ) {

    }

    /* fall through if the record is in the done list */
    tid = t_id;


    return;
}

#endif

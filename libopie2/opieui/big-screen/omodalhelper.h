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

#ifndef OMODALHELPER_H
#define OMODALHELPER_H

/* QT*/
#include <qdialog.h>
#include <qwidget.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qvariant.h>

typedef int TransactionID;

class QDialog;

namespace Opie
{

class OModalHelperControler;
class OModalHelperSignal;

struct OModalHelperBase
{
    virtual void done( int status, TransactionID ) = 0;
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
class OModalHelper : private OModalHelperBase
{
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

    void suspend( bool = true );

    void cancel();
    void cancel( TransactionID );

    void connectDone( QObject* rec, const char* slot );
    void connectAccepted( QObject* rec, const char* slot );
    void connectRejected( QObject* rec, const char* slot );

    TransactionID handle( Id id, const Record& rec = Record() );

    void edited( Id, int what, const QVariant& data );

    Record record( TransactionID )const;
    RecordList recordsDone()const;
private:
    virtual void done( int, TransactionID );
    virtual void next( TransactionID );
    virtual void prev( TransactionID );

    Record nextRecord( TransactionID &, int & )const;
    Record prevRecord( TransactionID &, int & )const;
    int pos( TransactionID )const;
    Dialog* newDialogRecord( const Record& );

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
bool                   m_disabled :1;
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

class OModalHelperSignal : public QObject
{
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


class OModalHelperControler : public QObject
{
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
class OModalQueuedDialog : public QDialog
{
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
OModalHelper<Dialog, Record, Id>::OModalHelper( enum Mode mode, QObject* parent )
{
    m_disabled = false;
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
bool OModalHelper<Dialog, Record, Id>::handles( Id id )const
{
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
TransactionID OModalHelper<Dialog, Record, Id>::transactionID( Id id)const
{
    if ( m_transactions.isEmpty() || !m_ids.contains( id ) )
        return 0;

    TransactionMap::ConstIterator it = m_transactions.begin();
    for ( ; it != m_transactions.end(); ++it )
        if ( it.data() == id )
            return it.key();

    return 0;
}

/**
 * If you're requested to flush your data and you do not want
 * to call cancel with this method you can disable and enabled
 * all dialogs.
 * The state gets saved so if you want to handle a new record the dialog
 * will be disabled as well.
 *
 * @param sus If true setDisabled(TRUE) will be called otherwise FALSE
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::suspend(bool sus)
{
    m_disabled = sus;
    if (m_mode == New )
        for (DialogMap::Iterator it = m_editing.begin(); it != m_editing.end(); ++it )
            it.key()->setDisabled( sus );
    else if (m_dialog )
        queuedDialog()->setDisabled( sus );
}

/**
 * Cancel will cancel all current operations and clear the list
 * of done operations as well.
 * This also clears all done operations you did not popped
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::cancel()
{
    m_ids.clear();
    m_doneIds.clear();
    m_done.clear();
    m_transactions.clear();

    /* we also need to remove the QDialogs */
    /* and hide the queue dialog if present */
    if (m_mode == New && !m_editing.isEmpty() )
    {
        for (DialogMap::Iterator it = m_editing.begin(); it != m_editing.end(); ++it )
            delete it.key();

        m_editing.clear();
    }
    else if (m_dialog )
        queuedDialog()->setRecord( 0, 0 );

    m_controler->setTransactionID( 0 );
}


/**
 * This cancels editing of the record behind the Transaction Number
 * Note that if editing is already done it will also be removed from this list
 */
template<class Dialog, class Record, typename Id>
void OModalHelper::cancel( TransactionID tid )
{
    /* wrong tid */
    if (!m_transactions.contains( tid ) && !m_done.contains( tid) )
        return;

    if (m_mode == New )
        /* reverse map eek */
        for (DialogMap::Iterator it = m_editing.begin(); it != m_editing.end(); ++it )
            if ( it.data() == tid )
            {
                it.key()->hide();
                delete it.key();
                it = m_editing.remove( it );
                break;
            }

    /* now remove from the various maps  done and currently editing map*/
    if (m_transactions.contains( tid ) )
        m_ids.remove( m_transactions[tid]  );
    if (m_done.contains( tid ) )
        m_doneIds.remove( m_done[tid ]  );
    m_done.remove( tid );
    m_transactions.remove( tid );

    next( 0 );
}

/**
 * Connect to the done Signal. SIGNAL( done(int, TransactionID ) )
 * This signal gets emitted whenever a Record was accepted or rejected
 *
 * @param rec  The object where the slot belongs to
 * @param slot The slot which should be called. See the needed parameter above
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::connectDone( QObject* rec, const char* slot )
{
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
void OModalHelper<Dialog, Record, Id>::connectAccepted( QObject* rec, const char* slot )
{
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
void OModalHelper<Dialog, Record, Id>::connectRejected( QObject* rec, const char* slot )
{
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
TransactionID OModalHelper<Dialog, Record, Id>::handle( Id id, const Record& rec )
{
    static TransactionID t_id = 0;
    /*
     *this method consists out of two parts divided each into New and Queued Mode.
     * Either we have the dialog already, in this case we need to highlight the widget
     * Or we need to add it.
     */
    TransactionID tid = 0;
    /* we already have the record lets see if it was done or not */
    if ( !(tid = transactionID( id ) ) )
    {
        if (m_mode == New )
        {
            /* lets find the dialog and show it need to reverse map*/
            for (DialogMap::Iterator it = m_editing.begin(); it != m_editing.end(); ++it )
                if ( it.data() == tid )
                    it.key()->show();
        }
        else if (m_controler->transactionID() != tid )
        {
            int po = pos( tid );
            m_controler->setTransactionID( tid );
            static_cast<Dialog*>( queuedDialog()->centerDialog() )->setRecord( m_ids[ m_transactions[tid] ] );
            queuedDialog()->setRecord( po, m_transactions.count() );
        }
    }
    else
    {
        tid = ++t_id;
        m_transactions.insert( tid, id );
        m_ids.insert( id, rec );

        if (m_mode == New )
            m_editing.insert( newDialogRecord( rec ), tid );
        else
        {
            m_controler->setTransactionID( tid );
            static_cast<Dialog*>( queuedDialog()->centerDialog() )->setRecord( rec );
            queuedDialog()->setRecord( m_transactions.count(), m_transactions.count() );
        }
    }
    return tid;
}

/**
 * The goal of this helper is to help you to create non blocking
 * GUIs. In the example of the todolist you can have the edit dialog
 * but still change the priority or completion inline even if you currently
 * edit the record.
 * Your Dialog needs to have a Method setData(int,const QVariant& ) which will be called
 * in these cases.
 * If you edit anything while a record is edited call this function to merge the
 * change in. Note if the record is not handled here we will ignore the request
 *
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::edited( Id id, int what, const QVariant& data )
{
    int tid;
    if (!( tid = transactionID( id ) ) )
        return;

    if (m_mode == New )
    {
        for (DialogMap::Iterator it= m_editing.begin(); it != m_editing.end(); ++it )
            if ( it.data() == tid )
                it.key()->setData( what, data );
    }
    else
    {
        int po = pos( tid );
        Dialog* dia = static_cast<Dialog*>( queuedDialog()->centerDialog() );
        dia->setRecord( m_ids[id] );
        dia->setData( what, data );
        queuedDialog()->setRecord( pos, m_transactions.count() );
    }
}

/**
 * This functions either returns the unedited record the done record
 * or a new empty Record using Record().
 * If a done record is retrieved all traces are removed inside this class. This
 * is what was called popping a  record. This means when you call this function
 * with the same TransactionID an Empty record is retrieved.
 *
 */
template<class Dialog, class Record, typename Id>
Record OModalHelper<Dialog, Record, Id>::record( TransactionID tid)const
{
    if (m_transactions.contains( tid ) )
        return m_ids[ m_transactions[tid] ];
    else if (m_done.contains( tid ) )
    {
        Record rec = m_doneIds[ m_done[ tid] ];
        m_doneIds.remove( m_done[ tid ] );
        m_done.remove( tid );
        return rec;
    }
    else
        return Record();
}

/**
 * Returns all done Records and removes all references to them internally. A 2nd call to this will
 * only contain done record that where edited past the point
 */
template<class Dialog, class Record, typename Id>
OModalHelper<Dialog,Record,Id>::RecordList OModalHelper<Dialog, Record, Id>::recordsDone()const
{
    RecordList list;

    for (IdMap::ConstIterator it = m_doneIds.begin(); it != m_doneIds.end(); ++it )
        list.append( it.data() );

    /* clean up */
    m_done.clear();
    m_doneIds.clear();

    return list;
}


/**
 * @internal
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::done( int status, TransactionID tid)
{
    /* If we're in New mode the transaction Id does not count */
    Record rec;

    if (m_mode == New )
    {
        Dialog *dia = static_cast<Dialog*>( m_controler->dialog() );
        m_controler->setTransactionID( 0 ); // set the internal dialog to 0l again
        tid = m_editing[ dia ];
        m_editing.remove( dia );
        rec = dia->record();
        delete dia;
    }
    else
        rec = queuedDialog()->record();

    Id id = m_transactions[ tid ];
    if (result == QDialog::Accept )
    {
        m_doneIds.insert( is, rec  );
        m_done.insert( tid, id );
    }

    m_transactions.remove( tid );
    m_ids.remove( id );


    if (status == QDialog::Accept )
        emit m_signal->accepted( tid );
    else
        emit m_signal->rejected( tid );

    emit m_signal->done( result, tid );

    next( 0 );
}

/**
 * @internal
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::next( TransactionID tid)
{
    if (m_mode == New )
        return;

    if (! (m_transactions.count() ) )
    {
        m_controler->setTransactionID( 0 );
        queuedDialog()->setRecord( 0, 0 );
        return;
    }

    int next;
    Record rec;

    /* save the maybe edited record before switching */
    Dialog *dia = static_cast<Dialog*>( queuedDialog()->centerDialog() );
    rec = dia->record();
    m_ids.replace( m_transactions[tid], rec );

    rec = nextRecord( tid, next );
    queuedDialog()->setRecord( next, m_transactions.count() );
    dia->setRecord( rec );

    m_controler->setTransactionID( tid ); // was changed during the next call
}

/**
 * @internal
 */
/*
 * code duplication should create a template fcuntion
 * which takes a pointer to a function ( next, prev ) function
 */
template<class Dialog, class Record, typename Id>
void OModalHelper<Dialog, Record, Id>::prev( TransactionID tid )
{
    if (m_mode == New )
        return;

    if (! (m_transactions.count()) )
    {
        m_controler->setTransactionID( 0 );
        queuedDialog()->setRecord( 0, 0 );
        return;
    }

    int prev;
    Record rec;

    /* save the maybe edited record before switching */
    Dialog *dia = static_cast<Dialog*>( queuedDialog()->centerDialog() );
    rec = dia->record();
    m_ids.replace( m_transactions[tid], rec );

    rec = prevRecord( tid, prev );
    queuedDialog()->setRecord( prev, m_transactions.count() );
    dia->setRecord( rec );

    m_controler->setTransactionID( tid ); // was changed during the next call
}

/**
 * @internal
 */
template<class Dialog, class Record, typename Id>
Record  OModalHelper<Dialog, Record, Id>::nextRecord( TransactionID &tid, int &po )
{
    /* if tid is == 0 we will take the first one */
    /* pos starts at 1 here */
    /* we know we're only called if there are records */
    Record rec;
    TransactionMap::Iterator it;
    if (!tid )
    {
        po = 1;
        TransactionMap::Iterator it = m_transactions.begin();
    }
    else
    {
        po = pos( tid );
        /* if it is the last take the first as next */
        if ( po == m_transactions.count() )
        {
            po = 1;
            it = m_transactions.begin();
        }
        else
        {
            /* we know we're not the last and there is one after us */
            it = m_transactions.find( tid );
            ++it; ++po;
        }
    }

    tid = it.key();
    rec = m_ids[ tid ];
    return rec;
}

/**
 * @internal
 */
template<class Dialog, class Record, typename Id>
Record OModalHelper<Dialog, Record, Id>::prevRecord( TransactionID& tid, int& pos )
{
    /* if tid is == 0 we will take the first one */
    /* pos starts at 1 here */
    /* we know we're only called if there are records */
    Record rec;
    TransactionMap::Iterator it;
    if (!tid )
    {
        po = 1;
        TransactionMap::Iterator it = m_transactions.begin();
    }
    else
    {
        po = pos( tid );
        /* if it is the last take the first as next */
        if ( po == 1 )
        {
            po = m_transactions.count();
            it = m_transactions.end();
            --it;
        }
        else
        {
            /* we know we're not the first and there is one before us */
            it = m_transactions.find( tid );
            --it; --po;
        }
    }

    tid = it.key();
    rec = m_ids[ tid ];
    return rec;
}

/**
 * @internal
 */
template<class Dialog, class Record, typename Id>
int OModalHelper<Dialog, Record, Id>::pos( TransactionID id)const
{
    int i = 1;
    for ( TransactionMap::ConstIterator it = m_transactions.begin(); it != m_transactions.end(); ++it, i++ )
        if ( it.key() == id )
            return i;


    return 0;
}

/**
 * @internal
 */
template<class Dialog, class Record, typename Id>
Dialog* OModalHelper<Dialog, Record, Id>::newDialogRecord( const Record& rec )
{
    Dialog* dia = new Dialog;
    dia->setRecord( rec );
    dia->setDisabled( m_disabled );

    QObject::connect(dia, SIGNAL(done(int) ),
                     m_controler, SLOT(done(int) ) );

    /* FIXME big screen QPEApplication needs fixed*/
    dia->show();
}

template<class Record, class Dialog, typename Id>
OModalHelperDialog* OModalHelper<Record, Dialog, Id>::queuedDialog()const
{
    if (!m_dialog )
    {
        m_dialog = new OModalHelperDialog;
        m_dialog->setEnabled( m_disabled );

        QObject::connect(m_dialog, SIGNAL(done(int) ),
                         m_controler, SLOT(done(int) ) );
        QObject::connect(m_dialog, SIGNAL(next() ),
                         m_controler, SLOT(next() ) );
        QObject::connect(m_dialog, SIGNAL(prev() ),
                         m_controler, SLOT(prev() ) );
    }
    return m_dialog;
}

};

#endif

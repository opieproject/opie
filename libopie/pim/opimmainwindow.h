#ifndef OPIE_PIM_MAINWINDOW_H
#define OPIE_PIM_MAINWINDOW_H

#include <qmainwindow.h>

#include <opie/opimrecord.h>

/**
 * This is a common Opie PIM MainWindow
 * it takes care of the QCOP internals
 * and implements some  functions
 * for the URL scripting schema
 */
/*
 * due Qt and Templates with signal and slots
 * do not work that good :(
 * (Ok how to moc a template ;) )
 * We will have the mainwindow which calls a struct which
 * is normally reimplemented as a template ;)
 */

class QCopChannel;
class OPimMainWindow : public QMainWindow {
    Q_OBJECT
public:
    enum TransPort { BlueTooth=0,
                     IrDa };

    OPimMainWindow(  const QString& service, QWidget *parent = 0,  const char* name = 0,
                     WFlags f = WType_TopLevel);
    virtual ~OPimMainWindow();


protected slots:
    /*
     * called when a setDocument
     * couldn't be handled by this window
     */
    virtual void doSetDocument( const QString& );
    /* for syncing */
    virtual void flush()  = 0;
    virtual void reload() = 0;

    /** create a new Records and return the uid */
    virtual int create() = 0;
    /** remove a record with UID == uid */
    virtual bool remove( int uid ) = 0;
    /** beam the record with UID = uid */
    virtual void beam( int uid ,  int transport = IrDa) = 0;

    /** show the record with UID == uid */
    virtual void show( int uid ) = 0;
    /** edit the record */
    virtual void edit( int uid ) = 0;

    /** make a copy of it! */
    virtual void add( const OPimRecord& ) = 0;


    QCopChannel* channel();

private slots:
    void appMessage( const QCString&, const QByteArray& );
    void setDocument( const QString& );


private:
    class Private;
    Private* d;

    int m_rtti;
    QCopChannel* m_channel;
    QString m_service;
    QCString m_str;
    OPimRecord* m_fallBack;

    /* I would love to do this as a template
     * but can't think of a right way
     * because I need signal and slots -zecke
     */
    /*
     * the only pointer in the whole PIM API :(
     */
    virtual OPimRecord* record( int rtti, const QByteArray& ) ;
    int service();
};


#endif

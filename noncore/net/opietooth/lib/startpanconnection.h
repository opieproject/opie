#ifndef startpanconnection_h
#define startpanconnection_h

#include <qobject.h>
#include "connection.h"
#include <opie/oprocess.h>

namespace OpieTooth {

    class StartPanConnection : StartConnection {

	Q_OBJECT

    public:
	StartPanConnection();
	StartPanConnection( QString mac );
	~StartPanConnection();

	QString name();
	void setName( QString name );
	StartConnection::ConnectionType type();
	void setConnectionType( );
	void start();
	void stop();

    private:
	QString m_name;
	QString m_mac;
	ConnectionType m_connectionType;
	OProcess* m_panConnect;
	
    private slots:
	void slotExited( OProcess* proc );
        void slotStdOut( OProcess* proc, char* chars, int len );
    };

 
}

#endif

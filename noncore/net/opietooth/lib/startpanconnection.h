#ifndef startpanconnection_h
#define startpanconnection_h

#include <qobject.h>
#include "connection.h"
#include <opie2/oprocess.h>

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
	Opie::Core::OProcess* m_panConnect;
	
    private slots:
	void slotExited( Opie::Core::OProcess* proc );
        void slotStdOut( Opie::Core::OProcess* proc, char* chars, int len );
    };

 
}

#endif


#ifndef OPIE_TOOTH_BT_CONNECTION_ITEM_H
#define OPIE_TOOTH_BT_CONNECTION_ITEM_H


#include "btlistitem.h"

namespace OpieTooth {

    class ConnectionState;
    class BTConnectionItem : public BTListItem {
    public:
        BTConnectionItem( QListView* parent, const ConnectionState& state );
        ~BTConnectionItem();
        QString type()const;
	QString name();
	QString signalStrength();
        int typeId() const;
        ConnectionState connection()const;
	void setSignalStrength( QString );
	void setName( QString );

    private:
        ConnectionState m_con;
	QString m_name;
	QString m_signalStrength;
    };

};


#endif

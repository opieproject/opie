
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
        int typeId() const;
        ConnectionState connection()const;
    private:
        ConnectionState m_con;

    };

};


#endif

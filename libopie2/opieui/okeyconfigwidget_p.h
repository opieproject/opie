/*
 * Only Internal implemented in the same .cpp file anyway
 */
#include <opie2/olistview.h>


namespace Opie {
namespace Ui {
namespace Internal {
    static QString keyToString( const OKeyPair& );
    static void fixupKeys( int&, int&, QKeyEvent* );
    class OKeyListViewItem : public Opie::Ui::OListViewItem {
    public:
        OKeyListViewItem( const OKeyConfigItem& item, OKeyConfigManager*, Opie::Ui::OListViewItem* parent);
        ~OKeyListViewItem();

        void setDefault();

        OKeyConfigItem& item();
        OKeyConfigItem origItem()const;
        void setItem( const OKeyConfigItem& item );
        void updateText();

        OKeyConfigManager *manager();
    private:
        OKeyConfigItem m_item;
        OKeyConfigItem m_origItem;
        OKeyConfigManager* m_manager;

    };
}
}
}

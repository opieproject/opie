/*
 * Only Internal implemented in the same .cpp file anyway
 */
#include <opie2/olistview.h>

#include <opie2/okeyconfigmanager_p.h>

namespace Opie {
namespace Ui {
namespace Internal {
    static QString keyToString( const Opie::Core::OKeyPair& );
    class OKeyListViewItem : public Opie::Ui::OListViewItem {
    public:
        OKeyListViewItem( const Opie::Core::OKeyConfigItem& item,
                          Opie::Core::OKeyConfigManager*,
                          Opie::Ui::OListViewItem* parent);
        ~OKeyListViewItem();

        void setDefault();

        Opie::Core::OKeyConfigItem& item();
        Opie::Core::OKeyConfigItem origItem()const;
        void setItem( const Opie::Core::OKeyConfigItem& item );
        void updateText();

        Opie::Core::OKeyConfigManager *manager();
    private:
        Opie::Core::OKeyConfigItem m_item;
        Opie::Core::OKeyConfigItem m_origItem;
        Opie::Core::OKeyConfigManager* m_manager;

    };
}
}
}

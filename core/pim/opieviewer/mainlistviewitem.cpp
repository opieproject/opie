
#include "mainlistviewitem.h"

using namespace Viewer;

MainListViewItem::MainListViewItem( QListView* parent,
                                    const QString& category,
                                    int id )
 : QListViewItem(parent) {
    m_category= true;
    m_catName= category;
    m_id = id;
    setText(1,  category );
    setExpandable( true );
    setSelectable( false );
}
MainListViewItem::MainListViewItem(QListViewItem* parent,
                                   const QString& appName,
                                   const QString& text,
                                   const QString& richText,
                                   const QPixmap& pixmap,
                                   int id )
    : QListViewItem( parent ) {
    m_category = false;
    m_id = id;
    m_appName = appName;
    m_text = text;
    m_richText = richText;
    m_pixmap = pixmap;
    setPixmap(0, m_pixmap );
    setText(1,  m_text );
}
MainListViewItem::~MainListViewItem() {

}
bool MainListViewItem::isCategory() const {
    return m_category;
}
QString MainListViewItem::category() const{
    return m_catName;
}
QString MainListViewItem::appName() const{
    return m_appName;
}
QString MainListViewItem::text() const{
    return m_text;
}
QString MainListViewItem::richText() const {
    return m_richText;
}
QPixmap MainListViewItem::pixmap() const {
    return m_pixmap;
}
int MainListViewItem::id() const {
    return m_id;
}

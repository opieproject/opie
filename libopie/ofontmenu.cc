
#include "ofontmenu.h"



OFontMenu::OFontMenu(QWidget *parent, const char *name, const QList<QWidget> &list )
  : QPopupMenu( parent, name )
{
  m_list = list;
  insertItem(tr("Large"), this, SLOT(slotLarge() ),
	     0, 10);
  insertItem(tr("Medium"), this, SLOT(slotMedium() ),
	     0, 11 );
  insertItem(tr("Small"), this, SLOT(slotSmall() ),
	     0, 12 );
  setCheckable( true );
}
void OFontMenu::setWidgets(const QList<QWidget> &list )
{
  m_list = list;
}
void OFontMenu::addWidget( QWidget *wid )
{
  m_list.append(wid );
}
void OFontMenu::removeWidget( QWidget *wid )
{
  m_list.remove( wid );
}
const QList<QWidget> &OFontMenu::widgets()const
{
  return m_list;
}
void OFontMenu::forceSize(QWidget *wid, int size )
{
  WidSize *widz = new WidSize;
  widz->wid = wid;
  widz->size = size;
  m_wids.append( widz );
}
void OFontMenu::slotSmall()
{
  setItemChecked(10, false  );
  setItemChecked(11, false  );
  setItemChecked(12, true   );
  setFontSize( 8 );
}
void OFontMenu::slotMedium()
{
  setItemChecked(10, false  );
  setItemChecked(11, true   );
  setItemChecked(12, false  );
  setFontSize(10 );
}
void OFontMenu::slotLarge()
{
  setItemChecked(10, true   );
  setItemChecked(11, false  );
  setItemChecked(12, false  );
  setFontSize(14 );
}
void OFontMenu::setFontSize(int size )
{
  QWidget *wid;
  for(wid = m_list.first(); wid !=0; wid = m_list.next() ){
    QFont font = wid->font();
    font.setPointSize( size );
    wid->setFont( font );
  }
  if(!m_wids.isEmpty() ){
    WidSize *wids;
    for( wids = m_wids.first(); wids != 0; wids = m_wids.next() ){
      QFont font = wids->wid->font();
      font.setPointSize( wids->size );
      wids->wid->setFont( font );
    }
  }
}



#include <qpe/config.h>
#include "ofontmenu.h"


/**
 * Constructs the FontMenu.
 *
 * @param parent The parent widget
 * @param name A name for this widget
 * @param list The list of widgets to be controlled
 */
OFontMenu::OFontMenu(QWidget *parent, const char *name, const QList<QWidget> &list )
  : QPopupMenu( parent, name )
{
  m_list = list;
  m_wids.setAutoDelete( TRUE );

  insertItem(tr("Large"), this, SLOT(slotLarge() ),
	     0, 10);
  insertItem(tr("Medium"), this, SLOT(slotMedium() ),
	     0, 11 );
  insertItem(tr("Small"), this, SLOT(slotSmall() ),
	     0, 12 );
  setCheckable( true );
  m_size=10;
}

/**
 * This method saves the font size
 * into a Config object
 * OFontMenu will be used as group and size as key
 * @param cfg The Config object to be used
 */
void OFontMenu::save(Config *cfg )
{
  cfg->setGroup("OFontMenu" );
  cfg->writeEntry("size", m_size );
}

/**
 * This method restores the font size from a Config object
 * it'll apply the sizes to the widgets and will also set the
 * menu appropriate
 */
void OFontMenu::restore(Config *cfg )
{
  cfg->setGroup("OFontMeny" );
  m_size = cfg->readNumEntry("size" );
  setItemChecked(10, false  );
  setItemChecked(11, false  );
  setItemChecked(12, false  );
  switch( m_size ){
  case 8:
    setItemChecked(12, true );
    break;
  case 14:
    setItemChecked(10, true );
    break;
  case 10:// fall through
  default:
    setItemChecked(11, true );
    m_size = 10;
    break;
  }
  setFontSize( m_size );
}

/**
 * set the list of widgets
 * @param list the widget list
 */
void OFontMenu::setWidgets(const QList<QWidget> &list )
{
  m_list = list;
}

/**
 * add a widget to the list
 * @param wid The widget to be added
 */
void OFontMenu::addWidget( QWidget *wid )
{
  m_list.append(wid );
}

/**
 * removes the widget from the list of controlled objects
 * @param wid the to be removed widget
 */
void OFontMenu::removeWidget( QWidget *wid )
{
  m_list.remove( wid );
}

/**
 * The list of controlled widgets
 */
const QList<QWidget> &OFontMenu::widgets()const
{
  return m_list;
}

/**
 * Forces a size on a widget
 * @param wid The widget
 * @param size The font size forced onto the widget
 */
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
  m_size = size;
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
  emit fontChanged(size );
}

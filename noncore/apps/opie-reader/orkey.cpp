#include <qstring.h>
#include <qnamespace.h>
#include "orkey.h"

QString orKey::text() const
{
  QString ks;
  if ((m_st & Qt::ControlButton) != 0)
    {
      ks += "ctrl_";
    }
  if ((m_st & Qt::AltButton) != 0)
    {
      ks += "alt_";
    }
  if ((m_st & Qt::ShiftButton) != 0)
    {
      ks += "shift_";
    }
  /*
    if (m_st & meta_flag != 0)
    {
    ks += "meta_";
    }
    if (m_st & capslock_flag != 0)
    {
    ks += "capslock_";
    }
    if (m_st & scrolllock_flag != 0)
    {
    ks += "scrolllock_";
    }
  */
  if (m_ky == 0x20)
    {
      ks += "' '";
    }
  else if (0x21 <= m_ky && m_ky <= 0xff)
    {
      char st[2];
      st[0] = m_ky;
      st[1] = 0;
      ks += st;
    }
  else if (0x1030 <= m_ky && m_ky <= 0x1052)
    {
      ks += QString("f") + QChar(m_ky - 0x1030 + 1);
    }
  else
    {
      switch (m_ky)
	{
	case Qt::Key_Escape:
	  ks += "esc";
	  break;
	case Qt::Key_Tab:
	  ks += "Tab";
	  break;
	case Qt::Key_Backtab:
	  ks += "Backtab";
	  break;
	case Qt::Key_Backspace:
	  ks += "Backspace";
	  break;
	case Qt::Key_Return:
	  ks += "Return";
	  break;
	case Qt::Key_Enter:
	  ks += "Enter";
	  break;
	case Qt::Key_Insert:
	  ks += "Insert";
	  break;
	case Qt::Key_Delete:
	  ks += "Delete";
	  break;
	case Qt::Key_Pause:
	  ks += "Pause";
	  break;
	case Qt::Key_Print:
	  ks += "Print";
	  break;
	case Qt::Key_SysReq:
	  ks += "SysReq";
	  break;
	case Qt::Key_Home:
	  ks += "Home";
	  break;
	case Qt::Key_End:
	  ks += "End";
	  break;
	case Qt::Key_Left:
	  ks += "Left";
	  break;
	case Qt::Key_Up:
	  ks += "Up";
	  break;
	case Qt::Key_Right:
	  ks += "Right";
	  break;
	case Qt::Key_Down:
	  ks += "Down";
	  break;
	case Qt::Key_PageUp:
	  ks += "PageUp";
	  break;
	case Qt::Key_PageDown:
	  ks += "PageDown";
	  break;
	case Qt::Key_Super_L:
	  ks += "Super_L";
	  break;
	case Qt::Key_Super_R:
	  ks += "Super_R";
	  break;
	case Qt::Key_Menu:
	  ks += "Menu";
	  break;
	case Qt::Key_Hyper_L:
	  ks += "Hyper_L";
	  break;
	case Qt::Key_Hyper_R:
	  ks += "Hyper_R";
	  break;
	case Qt::Key_Help:
	  ks += "Help";
	  break;
	default:
	  ks += "???";
	  break;
	}
    }
  return ks;
}

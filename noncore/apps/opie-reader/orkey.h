#ifndef __ORKEY_H
#define __ORKEY_H

const uint KEYMAPVERSION = 0;

enum ActionTypes
{
    cesOpenFile = 0,
    cesAutoScroll,
    cesActionMark,
    cesActionAnno,
    cesFullScreen,
    cesZoomIn,
    cesZoomOut,
    cesBack,
    cesForward,
    cesHome,
    cesPageUp,
    cesPageDown,
    cesLineUp,
    cesLineDown,
    cesStartDoc,
    cesEndDoc,
    cesRotate,
    cesScrollMore,
    cesScrollLess,
    cesInvertColours,
    cesToggleBars,
    cesToggleScrollBar,
    cesToggleStatusBar,
    cesNextLink,
    cesGotoLink
};

class orKey
{
  friend bool operator<(const orKey& lhs, const orKey& rhs);
  Qt::ButtonState m_st;
  int m_ky;
  bool m_scroll;
 public:
  orKey(Qt::ButtonState _s = Qt::NoButton, int _k = 0, bool _sc = false) : m_st(_s), m_ky(_k), m_scroll(_sc) {}
  QString text() const;
  bool isScroll() const { return m_scroll; }
  void toggleScroll() { m_scroll = !m_scroll; }
};

inline bool operator<(const orKey& lhs, const orKey& rhs)
{
  if (lhs.m_ky < rhs.m_ky)
    {
      return true;
    }
  if (lhs.m_ky > rhs.m_ky)
    {
      return false;
    }
  if (lhs.m_st < rhs.m_st)
    {
      return true;
    }
  if (lhs.m_st > rhs.m_st)
    {
      return false;
    }
  return (lhs.m_scroll < rhs.m_scroll);
}
#endif

/////////////////////////////////////////////////////////////////////////////
//
// functions generating layout-aware widgets
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __NEWWIDGET__H__
#define __NEWWIDGET__H__

#include <qwidget.h>
#include <qlineedit.h>

#define L_FIXEDW 1
#define L_FIXEDH 2
#define L_FIXED  (L_FIXEDW | L_FIXEDH)

QLineEdit *newLineEdit(int visiblewidth, QWidget *parent);

#endif

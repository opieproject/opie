//
// VUMeter class
//

// Copyright (c) 2001 Camilo Mesias
// camilo@mesias.co.uk
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef __VUMETER_H
#define __VUMETER_H

#include <qwidget.h>
#include <qstring.h>
#include <qpixmap.h>

class VUMeter : public QWidget {

    Q_OBJECT

 private:
    int vuvalue;
    QString Lannot;
    QString Rannot;

    QPixmap *pix;

 protected:
  void paintEvent(QPaintEvent *);
  void bupdate();
  void vupdate();

 public:
  VUMeter(QWidget *parent=0, const char *name=0);
  ~VUMeter();

  // slots
  public slots:
  void Value(int x){vuvalue = x; vupdate(); };
  void AnnotL(const QString &s){Lannot = s; bupdate(); };
  void AnnotR(const QString &s){Rannot = s; bupdate(); };
};

#endif




/**********************************************************************
** Copyright (C) 2003 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "zcameraio.h"
#include "imageio.h"
#include "avi.h"

#include <qfile.h>
#include <qframe.h>
#include <qimage.h>

class Capturer: public QFrame
{
  Q_OBJECT

  public:
    Capturer();
    virtual ~Capturer();

    int height;
    int width;
    int zoom;
    int quality;
    QString flip;
    QString format;
    QString name;
    QImage image;

  public slots:
    void capture();
    void checkSettings();
};


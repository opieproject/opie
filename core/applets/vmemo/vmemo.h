/**************************************************************************************94x78**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*********************************************************************************************/

/*
 * $Id: vmemo.h,v 1.8 2002-05-19 17:00:41 llornkcor Exp $
 */

#ifndef __VMEMO_H__
#define __VMEMO_H__

#include <qwidget.h>
#include <qpixmap.h>
#include <qpe/applnk.h>
#include <qfile.h>
#include <qpe/qcopenvelope_qws.h>

class VMemo : public QWidget
{
  Q_OBJECT
public:
  VMemo( QWidget *parent, const char *name = NULL);
  ~VMemo();
  QFile track;  
  QString fileName, errorMsg;

public slots:
  void record();
  void mousePressEvent( QMouseEvent * );
  void mouseReleaseEvent( QMouseEvent * );
  void receive( const QCString &msg, const QByteArray &data );
  bool startRecording();
  void stopRecording();
private:
  bool useAlerts;
  void paintEvent( QPaintEvent* );

  int openDSP();
  int openWAV(const char *filename);
  bool fromToggle;
  QPixmap vmemoPixmap;
  QCopChannel *myChannel;
  bool systemZaurus;
  int dsp, wav, rate, speed, channels, format, resolution;
  bool recording;
};

#endif // __VMEMO_H__


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
 * $Id: vmemo.h,v 1.13 2002-07-26 01:14:13 llornkcor Exp $
 */

#ifndef __VMEMO_H__
#define __VMEMO_H__


#include <qwidget.h>
#include <qpixmap.h>
#include <qpe/applnk.h>
#include <qfile.h>
#include <qpe/qcopenvelope_qws.h>
#include <qlabel.h>
#include <qtimer.h>

class VMemo : public QWidget
{
  Q_OBJECT
public:
  VMemo( QWidget *parent, const char *name = NULL);
  ~VMemo();
  QFile track;  
  QString fileName, errorMsg;
  QLabel* msgLabel;
  QTimer *t_timer;
bool usingIcon, useADPCM;
public slots:
  bool record();
  void mousePressEvent( QMouseEvent * );
  void mouseReleaseEvent( QMouseEvent * );
  void receive( const QCString &msg, const QByteArray &data );
  bool startRecording();
  void stopRecording();
  void timerBreak();
private:
  bool useAlerts;
  void paintEvent( QPaintEvent* );
  int setToggleButton(int);
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


//-- -*- c++ -*-
/**********************************************************************
** Copyright (C) 2001 LISA Systems
**
** This file is an additional part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** For further information contact info@lisa.de 
**
**********************************************************************/

/*
 * AUTHOR: Christian Rahn
 * EMAIL: cdr@lisa.de
 *
 * $Id: qdmdialogimpl.h,v 1.1 2002-01-25 22:14:57 kergoth Exp $
 */

#ifndef _QDM_IMPL_H
#define _QDM_IMPL_H

#include "qdm_config.h"

#if defined(QT_QWS_LOGIN)

#include "qdmdialog.h"

class InputMethods;

class QDMDialogImpl : public QDMDialog
{
  Q_OBJECT
    
 public:
  /** Pop up login dialog and do all stuff */
  static bool login( QWidget * parent = 0 );

 protected:
  /** a protected constructor */
  QDMDialogImpl( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags f = 0 );  
  ~QDMDialogImpl();

  /** Timer for clock display */
  void  timerEvent( QTimerEvent * );
  
  /** The Timer for the clock */
  int clockTimer;

  /** Conersation function for PAM */
  //  int PAM_conv (int num_msg, pam_message_type **msg,  struct pam_response **resp,  void *);

  /** Just become (i.e. log in as) user <name> */
  bool changePersona( const char *name );

  /** Inform about an incorrect given password */
  void informBadPassword();

private:
  InputMethods *input;

 protected slots:
 /** These got to be overridden so that the login dialog can't be circumvented */
 virtual void accept ();
  virtual void reject ();
  
public slots:
  /** Display the atual time and date */
  void showTime( void );
  
  /** login button pressed */
  virtual void slot_login();
  
  /** Shutdown button pressed */
  virtual void slot_shutdown();

  /** Sleep button pressed */
  virtual void slot_sleepmode();
};

#endif //-- QT_QWS_LOGIN

#endif //-- _QDM_IMPL_H


/***************************************************************************
                          qlinphone.cpp  -  description
                             -------------------
    begin                : sam mai 24 2003
    copyright            : (C) 2003 by Simon Morlat, 2004 Maximilian Reiss
    email                : simon.morlat@linphone.org, harlekin@handhelds.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qlinphone.h"
#include <qlineedit.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qslider.h>
#include <qtabwidget.h>
#include <qcheckbox.h>
#include <qmessagebox.h>
#include <qpe/config.h>
#include <qpe/qpeapplication.h>
#include <linphonecore.h>

extern "C" {
    static void stub(LinphoneCore*lc, char*msg) {}

    static void qt_show(LinphoneCore *lc) {
        QLinphoneMainWidget *w=(QLinphoneMainWidget*)lc->data;
        w->pushGuiTask(new ShowTask(w));
    }

    static void qt_inv_recv(LinphoneCore *lc, char *from) {
        QLinphoneMainWidget *w=(QLinphoneMainWidget*)lc->data;
        QString tmp(from);
        w->pushGuiTask(new InviteReceivedTask(w,tmp));
    }

    static void qt_display_status(LinphoneCore *lc, char *status) {
        QLinphoneMainWidget *w=(QLinphoneMainWidget*)lc->data;
        QString tmp(status);
        w->pushGuiTask(new UpdateStatusBarTask(w,tmp));
    }
    static void qt_display_message(LinphoneCore *lc, char *message) {
        QString qmsg(message);
        QLinphoneMainWidget *w=(QLinphoneMainWidget*)lc->data;
        w->pushGuiTask(new DisplayMessageTask(w,qmsg,DisplayMessageTask::Info));
    }
    static void qt_display_warning(LinphoneCore *lc, char *message) {
        QLinphoneMainWidget *w=(QLinphoneMainWidget*)lc->data;
        QString qmsg(message);
        w->pushGuiTask(new DisplayMessageTask(w,qmsg,DisplayMessageTask::Warn));
    }
    static void qt_display_url(LinphoneCore *lc, char *message, char *url) {
        QLinphoneMainWidget *w=(QLinphoneMainWidget*)lc->data;
        QString qmsg=QString(message)+QString(url);
        w->pushGuiTask(new DisplayMessageTask(w,qmsg,DisplayMessageTask::Info));
    }
    static void qt_notify_recv(LinphoneCore *lc, const char *url, const char *status) {
    }

    LinphoneCoreVTable lcvtable={
                                show:
                                    qt_show,
                                inv_recv:
                                    qt_inv_recv,
                                bye_recv :
                                    stub,
                                notify_recv :
                                    qt_notify_recv,
                                display_status :
                                    qt_display_status,
                                display_message :
                                    qt_display_message,
                                display_warning :
                                    qt_display_warning,
                                display_url :
                                    qt_display_url,
                                display_question :
                                    stub
                                };


} //extern "C"

void UpdateStatusBarTask::execute() {
    static_cast<QLinphoneMainWidget*>(_w)->displayStatus(_msg);
}

void InviteReceivedTask::execute() {
    static_cast<QLinphoneMainWidget*>(_w)->inviteReceived(_msg);
}

void DisplayMessageTask::execute() {
    switch(_msgtype) {
    case Info:
        QMessageBox::information(0,QObject::tr("Information"),_msg);
        break;
    case Warn:
        QMessageBox::warning(0,QObject::tr("Warning"),_msg);
        break;
    }
}

QLinphoneMainWidget::QLinphoneMainWidget(QWidget* parent , const char* name , WFlags fl ) :
_QLinphoneMainWidget( parent, name, fl ) {

     readConfig();
     createLinphoneCore();
     connect( CheckBox1,  SIGNAL( toggled(bool) ), this,  SLOT( slotHide(bool) ) );
     CheckBox1->setChecked( true );
}

QLinphoneMainWidget::~QLinphoneMainWidget() {
    linphone_core_destroy(_core);
    writeConfig();
}

void QLinphoneMainWidget::slotHide( bool show ) {
    if (  show )  {
         TabWidget2->show();
    } else {
       TabWidget2->hide();
    }
}

void QLinphoneMainWidget::callOrAccept() {
    if (linphone_core_inc_invite_pending(_core)) {
        linphone_core_accept_dialog(_core,NULL);
        return;
    }
    QString url=sip_url->text();
    linphone_core_invite(_core,(char*)url.ascii());
}
void QLinphoneMainWidget::terminateCall() {
    linphone_core_terminate_dialog(_core,NULL);
}

void QLinphoneMainWidget::inviteReceived(QString &tmp) {
    sip_url->setText(tmp);
}

void QLinphoneMainWidget::displayStatus(QString &msg) {
    status_bar->setText(msg);
}

void QLinphoneMainWidget::pushGuiTask(GuiTask* g) {
    _mutex.lock();
    _actionq.enqueue(g);
    _mutex.unlock();
    printf("New action added to task list.\n");
}

void QLinphoneMainWidget::processGuiTasks() {
    GuiTask *g;
    _mutex.lock();
    while(!_actionq.isEmpty()) {
        g=_actionq.dequeue();
        printf("Executing action...\n");
        g->execute();
        delete g;
    }
    _mutex.unlock();
}

void QLinphoneMainWidget::timerEvent(QTimerEvent *t) {
    processGuiTasks();
}

void QLinphoneMainWidget::readConfig() {
   Config cfg( "opie-phone" );
   cfg.setGroup( "audio" );
   SliderInput->setValue(  cfg.readNumEntry( "rec_lev", 50 ) );
   SliderOutput->setValue( cfg.readNumEntry( "play_lev", 50 ) );
}

void QLinphoneMainWidget::writeConfig() {
   Config cfg( "opie-phone" );
   cfg.setGroup( "audio" );
   cfg.writeEntry(  "rec_lev",  SliderInput->value() );
   cfg.writeEntry(  "playlev",  SliderOutput->value() );
}

void QLinphoneMainWidget::helpAbout() {
    QMessageBox::about(this,tr("About Linphone"),tr("QT version of linphone\nJuly 2003 - Made in old Europe."));
}

void QLinphoneMainWidget::createLinphoneCore() {
    if ( _core ) {
         linphone_core_destroy(_core);
    }

    gchar *home=getenv("HOME");
    gchar *suffix="/Settings/opie-phone.conf";
    if (home==0)
        home=strdup("/root");
    gchar *config=new char[strlen(home)+strlen(suffix)+2];
    sprintf(config,"%s/%s",home,suffix);
    /* tracing for osip */
    TRACE_INITIALIZE((trace_level_t)5,stdout);
    _core=linphone_core_new(&lcvtable,config,(gpointer)this);
    delete [] config;
    startTimer(10);
}


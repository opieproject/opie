/* $Id: servicesdialog.cpp,v 1.1 2006-04-20 12:37:33 korovkin Exp $ */
/* Bluetooth services configuration dialog */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "servicesdialog.h"
#include "btconfhandler.h"
#include <opie2/odebug.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
using namespace Opie::Core;

using namespace OpieTooth;

ServicesDialog::ServicesDialog(const QString& conf,
    QWidget* parent, const char* name, bool modal, WFlags fl) :
    ServicesDialogBase(parent, name, modal, fl),
    cfg(conf)
{
    int i; //just an index
    QString role;
    hcidEnableCBox->setChecked(cfg.getHcidEnable());
    hiddEnableCBox->setChecked(cfg.getHiddEnable());
    sdpdEnableCBox->setChecked(cfg.getSdpdEnable());
    rfcommEnableCBox->setChecked(cfg.getRfcommEnable());
    dundEnableCBox->setChecked(cfg.getDundEnable());
    dundDisableSDPCBox->setChecked(cfg.getDunDisableSDP());
    dundPersistCBox->setChecked(cfg.getDunPersist());
    dundPersistEdit->setText(cfg.getDunPersistPeriod());
    dundCacheCBox->setChecked(cfg.getDunCache());
    dundCacheEdit->setText(cfg.getDunCachePeriod());
    for (i = 1; i < 32; i++)
        dundChannelCBox->insertItem(QString::number(i));
    role = cfg.getDunChannel();
    for (i = 0; i < dundChannelCBox->count(); i++) {
        if (role == dundChannelCBox->text(i)) {
            dundChannelCBox->setCurrentItem(i);
            break;
        }
    }
    dundMsdunCBox->setChecked(cfg.getDunMsdun());
    dundMsdunTimeoutEdit->setText(cfg.getDunMsdunTimeout());
    dundPPPdCBox->setChecked(cfg.getDunPPPd());
    dundPPPdPathEdit->setText(cfg.getDunPPPdPath());
    dundPPPdOptionsEdit->setText(cfg.getDunPPPdOptions());

    pandEnableCBox->setChecked(cfg.getPanEnable());
    pandEncryptCBox->setChecked(cfg.getPanEncrypt());
    pandSecureCBox->setChecked(cfg.getPanSecure());
    pandMasterCBox->setChecked(cfg.getPanMaster());
    role = cfg.getPanRole();
    for (i = 0; i < pandRoleComboBox->count(); i++) {
        if (role == pandRoleComboBox->text(i)) {
            pandRoleComboBox->setCurrentItem(i);
            break;
        }
    }
    pandDisableSDPCBox->setChecked(cfg.getPanDisableSDP());
    pandPersistCBox->setChecked(cfg.getPanPersist());
    pandPersistEdit->setText(cfg.getPanPersistPeriod());
    pandCacheCBox->setChecked(cfg.getPanCache());
    pandCacheEdit->setText(cfg.getPanCachePeriod());
}

ServicesDialog::~ServicesDialog()
{
}

void ServicesDialog::accept()
{
    QString tmp;
    odebug << "save configuration" << oendl;
    cfg.setHcidEnable(hcidEnableCBox->isChecked());
    cfg.setHiddEnable(hiddEnableCBox->isChecked());
    cfg.setSdpdEnable(sdpdEnableCBox->isChecked());
    cfg.setRfcommEnable(rfcommEnableCBox->isChecked());
    cfg.setDundEnable(dundEnableCBox->isChecked());
    cfg.setDunDisableSDP(dundDisableSDPCBox->isChecked());
    cfg.setDunPersist(dundPersistCBox->isChecked());
    tmp = dundPersistEdit->text();
    cfg.setDunPersistPeriod(tmp);
    cfg.setDunCache(dundCacheCBox->isChecked());
    tmp = dundCacheEdit->text();
    cfg.setDunCachePeriod(tmp);
    tmp = dundChannelCBox->currentText();
    cfg.setDunChannel(tmp);
    cfg.setDunMsdun(dundMsdunCBox->isChecked());
    tmp = dundMsdunTimeoutEdit->text();
    cfg.setDunMsdunTimeout(tmp);
    cfg.setDunPPPd(dundPPPdCBox->isChecked());
    tmp = dundPPPdPathEdit->text();
    cfg.setDunPPPdPath(tmp);
    tmp = dundPPPdOptionsEdit->text();
    cfg.setDunPPPdOptions(tmp);

    cfg.setPanEnable(pandEnableCBox->isChecked());
    cfg.setPanEncrypt(pandEncryptCBox->isChecked());
    cfg.setPanSecure(pandSecureCBox->isChecked());
    cfg.setPanMaster(pandMasterCBox->isChecked());
    cfg.setPanDisableSDP(pandDisableSDPCBox->isChecked());
    tmp = pandRoleComboBox->currentText();
    cfg.setPanRole(tmp);
    cfg.setPanPersist(pandPersistCBox->isChecked());
    tmp = pandPersistEdit->text();
    cfg.setPanPersistPeriod(tmp);
    cfg.setPanCache(pandCacheCBox->isChecked());
    tmp = pandCacheEdit->text();
    cfg.setPanCachePeriod(tmp);
    cfg.saveConfig();
    ServicesDialogBase::accept();
}

//eof

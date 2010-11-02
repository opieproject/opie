/**
 * \file ownerInfoConfigWidget.cpp
 * \brief Owner info configuration widget
 * \author Paul Eggleton (bluelightning@bluelightning.org)
 */
/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2007 Opie Developer Team <opie-devel@lists.sourceforge.net>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/

#include "ownerInfoConfigWidget.h"

#include <opie2/odebug.h>
#include <opie2/multiauthcommon.h>

#include <qpe/qcopenvelope_qws.h>

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qregexp.h>
#include <qgroupbox.h>
#include <qwhatsthis.h>

using Opie::Security::MultiauthConfigWidget;

/// constructs the widget, filling the mleMessage QMultiLineEdit with the "noticeText" entry
OwnerInfoConfigWidget::OwnerInfoConfigWidget(QWidget* parent = 0, const char* name = "Owner info config widget") : MultiauthConfigWidget(parent, name)
{
    QVBoxLayout *baseLayout = new QVBoxLayout( this);
    baseLayout->setSpacing(4);
    baseLayout->setMargin(8);
    baseLayout->setAlignment( Qt::AlignTop );

    QGroupBox *gbWhen = new QGroupBox(1, Qt::Horizontal, tr("When to show"), this);
    gbWhen->layout()->setMargin(8);
    cbWhen = new QComboBox( FALSE, gbWhen, "whenToShow" );
    QWhatsThis::add( cbWhen, tr( "Specifies when the owner information should be shown." ) );
    // Don't forget to update the ownerInfoWhenToShow enum in multiauthcommon.h if you change these
    cbWhen->insertItem(tr("Never"));
    cbWhen->insertItem(tr("Before authentication"));
    cbWhen->insertItem(tr("Authentication failure"));

    baseLayout->addWidget(gbWhen);

    QGroupBox *gbOptions = new QGroupBox(2, Qt::Horizontal, tr("Details"), this);
    gbOptions->layout()->setSpacing(4);
    ckShowHomePhone   = new QCheckBox(tr("Home phone"), gbOptions);
    ckShowHomeMobile  = new QCheckBox(tr("Home mobile"), gbOptions);
    ckShowWorkPhone   = new QCheckBox(tr("Work phone"), gbOptions);
    ckShowWorkMobile  = new QCheckBox(tr("Work mobile"), gbOptions);
    ckShowHomeAddress = new QCheckBox(tr("Home address"), gbOptions);
    ckShowWorkAddress = new QCheckBox(tr("Work address"), gbOptions);
    ckShowEmail       = new QCheckBox(tr("Email"), gbOptions);

    baseLayout->addWidget(gbOptions);

    QGroupBox *gbMessage = new QGroupBox(0, Qt::Vertical, tr("Additional Message"), this);
    gbMessage->layout()->setMargin(8);
    baseLayout->addWidget(gbMessage);

    // Set the multilineedit box text to getNoticeText()
    mleMessage = new QMultiLineEdit(gbMessage, "notice text");
    mleMessage->setWordWrap(QMultiLineEdit::WidgetWidth);
    QVBoxLayout *msgLayout = new QVBoxLayout( gbMessage->layout() );
    msgLayout->addWidget(mleMessage);

    readConfig();

    pbEditOwnerInfo = new QPushButton( tr("Edit Owner Info"), this );
    connect(pbEditOwnerInfo, SIGNAL( clicked() ), this, SLOT( editOwnerInfo() ));
    baseLayout->addWidget(pbEditOwnerInfo, 0, Qt::AlignHCenter);
}

/// nothing to do
OwnerInfoConfigWidget::~OwnerInfoConfigWidget()
{}

/// read the options from the config file
void OwnerInfoConfigWidget::readConfig() {
    Config config("Security");
    config.setGroup("OwnerInfo");
    mleMessage->setText(config.readEntry("Message", "" ));
    mleMessage->setEdited(false);

    ckShowHomePhone->setChecked(   config.readBoolEntry("ShowHomePhone", TRUE ) );
    ckShowHomeMobile->setChecked(  config.readBoolEntry("ShowHomeMobile", TRUE ) );
    ckShowWorkPhone->setChecked(   config.readBoolEntry("ShowWorkPhone", TRUE ) );
    ckShowWorkMobile->setChecked(  config.readBoolEntry("ShowWorkMobile", TRUE ) );
    ckShowHomeAddress->setChecked( config.readBoolEntry("ShowHomeAddress", FALSE ) );
    ckShowWorkAddress->setChecked( config.readBoolEntry("ShowWorkAddress", FALSE ) );
    ckShowEmail->setChecked(       config.readBoolEntry("ShowEmail", TRUE ) );

    cbWhen->setCurrentItem( config.readNumEntry("WhenToShow", Opie::Security::oimAuthFail ) );
}

/// write the options to the config file
void OwnerInfoConfigWidget::writeConfig()
{
    Config config("Security");
    config.setGroup("OwnerInfo");

    if ( mleMessage->edited() ) {
        odebug << "writing new notice text in Security.conf" << oendl;
        // since Config files do not allow true newlines, we replace them with literal "\n"
        QString textString = mleMessage->text();
        config.writeEntry("Message", textString.replace( QRegExp("\n"), "\\n" ));
    }

    config.writeEntry("ShowHomePhone", ckShowHomePhone->isChecked() );
    config.writeEntry("ShowHomeMobile", ckShowHomeMobile->isChecked() );
    config.writeEntry("ShowWorkPhone", ckShowWorkPhone->isChecked() );
    config.writeEntry("ShowWorkMobile", ckShowWorkMobile->isChecked() );
    config.writeEntry("ShowHomeAddress", ckShowHomeAddress->isChecked() );
    config.writeEntry("ShowWorkAddress", ckShowWorkAddress->isChecked() );
    config.writeEntry("ShowEmail", ckShowEmail->isChecked() );

    config.writeEntry("WhenToShow", cbWhen->currentItem() );
}

void OwnerInfoConfigWidget::editOwnerInfo()
{
    QCopEnvelope env( "QPE/Application/addressbook", "editPersonalAndClose()" );
}

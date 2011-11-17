/*
                             This file is part of the Opie Project

              =.             Copyright (C) 2011 Paul Eggleton <bluelightning@bluelightning.org>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "serviceedit.h"

/* Opie */
#include <opie2/odebug.h>

/* Qt DBUS */
#include <dbus/qdbusdatalist.h>
#include <dbus/qdbusdatamap.h>
#include <dbus/qdbuserror.h>
#include <dbus/qdbusmessage.h>
#include <dbus/qdbusobjectpath.h>
#include <dbus/qdbusvariant.h>

/* Qt */
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qfile.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qprogressbar.h>

static int findComboItem( QComboBox *combo, const QString &str )
{
    int index = -1;
    for( int i=0; i < combo->count(); i++ ) {
        if( combo->text(i) == str ) {
            index = i;
            break;
        }
    }
    return index;
}

ConnManServiceEditor::ConnManServiceEditor( QDBusProxy *proxy, QWidget* parent, const char* name, WFlags fl )
        : ConnManServiceEditorBase( parent, name, TRUE, fl | Qt::WStyle_ContextHelp )
{
    setCaption(tr("Configure Service"));
    m_proxy = proxy;

    QPalette p = palette();
    QColor color = p.color(QPalette::Disabled, QColorGroup::Background);
    p.setColor(QColorGroup::Base, color);
    p.setColor(QColorGroup::Background, color);
    leName->setPalette(p);
    leName->setReadOnly(true);
    leState->setPalette(p);
    leState->setReadOnly(true);
    leV4AddressInfo->setPalette(p);
    leV4AddressInfo->setReadOnly(true);
    leV6AddressInfo->setPalette(p);
    leV6AddressInfo->setReadOnly(true);
    leAdapter->setPalette(p);
    leAdapter->setReadOnly(true);
    leMacAddress->setPalette(p);
    leMacAddress->setReadOnly(true);

    m_immutable = true;
    QDBusMessage reply = proxy->sendWithReply("GetProperties", QValueList<QDBusData>());
    if (reply.type() == QDBusMessage::ReplyMessage && reply.count() == 1) {
        QMap<QString,QDBusData> map = reply[0].toStringKeyMap().toQMap();
        // *** Info tab ***
        leName->setText( map["Name"].toVariant().value.toString() );
        QString state = map["State"].toVariant().value.toString();
        if( state == "failure" )
            state += QString(" (%1)").arg( map["Error"].toVariant().value.toString() );
        leState->setText( state );
        QString type = map["Type"].toVariant().value.toString();
        if( type == "wifi" )
            pbStrength->setProgress( map["Strength"].toVariant().value.toByte() );
        else {
            pbStrength->hide();
            lblStrengthLabel->hide();
        }
        m_ipv4map = map["IPv4"].toVariant().value.toStringKeyMap().toQMap();
        m_ipv6map = map["IPv6"].toVariant().value.toStringKeyMap().toQMap();
        leV4AddressInfo->setText( m_ipv4map["Address"].toVariant().value.toString() );
        leV6AddressInfo->setText( m_ipv6map["Address"].toVariant().value.toString() );
        QMap<QString,QDBusData> ethernetMap = map["Ethernet"].toVariant().value.toStringKeyMap().toQMap();
        leAdapter->setText( ethernetMap["Interface"].toVariant().value.toString() );
        leMacAddress->setText( ethernetMap["Address"].toVariant().value.toString() );

        m_immutable = map["Immutable"].toVariant().value.toBool();
        
        // *** IPv4 tab ***
        m_method4 = m_ipv4map["Method"].toVariant().value.toString();
        if( m_method4 == "" )
            m_method4 = "off";
        if( m_method4 == "fixed" ) {
            cbV4Config->insertItem("fixed");
            cbV4Config->setEnabled(false);
        }
        else {
            cbV4Config->insertItem("dhcp");
            cbV4Config->insertItem("manual");
            cbV4Config->insertItem("off");
            cbV4Config->setCurrentItem( findComboItem( cbV4Config, m_method4 ) );
            cbV4Config->setEnabled(!m_immutable);
        }
        leV4Address->setText( m_ipv4map["Address"].toVariant().value.toString() );
        leV4Netmask->setText( m_ipv4map["Netmask"].toVariant().value.toString() );
        leV4Gateway->setText( m_ipv4map["Gateway"].toVariant().value.toString() );

        // *** IPv6 tab ***
        m_method6 = m_ipv6map["Method"].toVariant().value.toString();
        if( m_method6 == "" )
            m_method6 = "off";
        else if( m_method6 == "6to4" )
            m_method6 = "auto";

        if( m_method6 == "fixed" ) {
            cbV6Config->insertItem("fixed");
            cbV6Config->setEnabled(false);
        }
        else {
            cbV6Config->insertItem("auto");
            cbV6Config->insertItem("manual");
            cbV6Config->insertItem("off");
            cbV6Config->setCurrentItem( findComboItem( cbV6Config, m_method6 ) );
            cbV6Config->setEnabled(!m_immutable);
        }
        leV6Address->setText( m_ipv6map["Address"].toVariant().value.toString() );
        spV6Prefix->setValue( m_ipv6map["PrefixLength"].toVariant().value.toByte() );
        leV6Gateway->setText( m_ipv6map["Gateway"].toVariant().value.toString() );
        QString method = m_ipv6map["Privacy"].toVariant().value.toString();
        if( method == "" )
            method = "disabled";
        cbV6Privacy->insertItem("disabled");
        cbV6Privacy->insertItem("enabled");
        cbV6Privacy->insertItem("prefered"); // FIXME mis-spelled in docs or api?
        cbV6Privacy->setCurrentItem( findComboItem( cbV6Privacy, method ) );
        
        // *** Proxy tab ***
        m_proxymap = map["Proxy"].toVariant().value.toStringKeyMap().toQMap();
        method = m_proxymap["Method"].toVariant().value.toString();
        if( method == "" )
            method = "direct";
        cbProxyMethod->insertItem("direct");
        cbProxyMethod->insertItem("auto");
        //cbProxyMethod->insertItem("manual"); // FIXME implement
        cbProxyMethod->setCurrentItem( findComboItem( cbProxyMethod, method ) );
        leProxyAddress->setText( m_proxymap["URL"].toVariant().value.toString() );
        
        // *** Misc tab ***
        QStringList servers = map["Nameservers"].toVariant().value.toList().toStringList();
        m_nameservers = servers.join(" ");
        leDnsServers->setText( m_nameservers );
        QStringList domains = map["Domains"].toVariant().value.toList().toStringList();
        m_domains = domains.join(" ");
        leDnsDomains->setText( m_domains );
        if( map.contains("Passphrase") ) {
            m_password = map["Passphrase"].toVariant().value.toString();
            lePassword->setText( m_password );
        }
        else {
            m_password = "";
            gbSecurity->hide();
        }
        m_autoconnect = map["AutoConnect"].toVariant().value.toBool();
        ckAutoConnect->setChecked( m_autoconnect );
    }

    connect( cbV4Config, SIGNAL( activated(int) ), this, SLOT( slotV4ConfigChanged(int) ) );
    connect( cbV6Config, SIGNAL( activated(int) ), this, SLOT( slotV6ConfigChanged(int) ) );
    connect( cbProxyMethod, SIGNAL( activated(int) ), this, SLOT( slotProxyMethodChanged(int) ) );

    slotV4ConfigChanged( cbV4Config->currentItem() );
    slotV6ConfigChanged( cbV6Config->currentItem() );
    slotProxyMethodChanged( cbProxyMethod->currentItem() );
}

ConnManServiceEditor::~ConnManServiceEditor()
{

}

void ConnManServiceEditor::slotV4ConfigChanged( int index )
{
    QString value = cbV4Config->text( index );
    if( value == "manual" && !m_immutable ) {
        leV4Address->setEnabled( true );
        leV4Netmask->setEnabled( true );
        leV4Gateway->setEnabled( true );
    }
    else {
        // fixed, auto, off
        leV4Address->setEnabled( false );
        leV4Netmask->setEnabled( false );
        leV4Gateway->setEnabled( false );
    }
}

void ConnManServiceEditor::slotV6ConfigChanged( int index )
{
    QString value = cbV6Config->text( index );
    if( value == "manual" && !m_immutable ) {
        leV6Address->setEnabled( true );
        spV6Prefix->setEnabled( true );
        leV6Gateway->setEnabled( true );
    }
    else {
        // fixed, auto, off; or immutable
        leV6Address->setEnabled( false );
        spV6Prefix->setEnabled( false );
        leV6Gateway->setEnabled( false );
    }
    cbV6Privacy->setEnabled( value == "auto" && !m_immutable );
}

void ConnManServiceEditor::slotProxyMethodChanged( int index )
{
    QString value = cbProxyMethod->text( index );
    leProxyAddress->setEnabled( value == "auto" && !m_immutable );
}

void ConnManServiceEditor::accept()
{
    if( !m_immutable ) {
        if( leDnsServers->text() != m_nameservers ) {
            odebug << "setting nameservers" << oendl;
            QValueList<QDBusData> params;
            params << QDBusData::fromString("Nameservers.Configuration");
            params << QDBusData::fromVariant(QDBusVariant(QDBusData::fromList(QDBusDataList(QStringList::split(" ", leDnsServers->text())))));
            QDBusMessage reply = m_proxy->sendWithReply("SetProperty", params);
        }

        if( leDnsDomains->text() != m_domains ) {
            odebug << "setting domains" << oendl;
            QValueList<QDBusData> params;
            params << QDBusData::fromString("Domains.Configuration");
            params << QDBusData::fromVariant(QDBusVariant(QDBusData::fromList(QDBusDataList(QStringList::split(" ", leDnsDomains->text())))));
            QDBusMessage reply = m_proxy->sendWithReply("SetProperty", params);
        }

        if( gbSecurity->isVisible() ) {
            if( lePassword->text() != m_password ) {
                odebug << "setting password" << oendl;
                QValueList<QDBusData> params;
                params << QDBusData::fromString("Passphrase");
                params << QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(lePassword->text())));
                QDBusMessage reply = m_proxy->sendWithReply("SetProperty", params);
            }
        }
        
        if( ckAutoConnect->isChecked() != m_autoconnect ) {
            odebug << "setting autoconnect" << oendl;
            QValueList<QDBusData> params;
            params << QDBusData::fromString("AutoConnect");
            params << QDBusData::fromVariant(QDBusVariant(QDBusData::fromBool(ckAutoConnect->isChecked())));
            QDBusMessage reply = m_proxy->sendWithReply("SetProperty", params);
        }

        QString method = cbV4Config->currentText();
        if( method != m_method4 || 
            (method == "manual" && (
            leV4Address->text() != m_ipv4map["Address"].toVariant().value.toString() ||
            leV4Netmask->text() != m_ipv4map["Netmask"].toVariant().value.toString() ||
            leV4Gateway->text() != m_ipv4map["Gateway"].toVariant().value.toString() ))
        ) {
            m_ipv4map["Method"] == QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(method)));
            if( method == "manual" ) {
                m_ipv4map["Address"] == QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(leV4Address->text())));
                m_ipv4map["Netmask"] == QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(leV4Netmask->text())));
                m_ipv4map["Gateway"] == QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(leV4Gateway->text())));
            }
            odebug << "setting ipv4 config" << oendl;
            QValueList<QDBusData> params;
            params << QDBusData::fromString("IPv4.Configuration");
            params << QDBusData::fromVariant(QDBusVariant(QDBusData::fromStringKeyMap(QDBusDataMap<QString>(m_ipv4map))));
            QDBusMessage reply = m_proxy->sendWithReply("SetProperty", params);
        }

        method = cbV6Config->currentText();
        if( method != m_method6 || 
            (method == "manual" && (
            leV6Address->text() != m_ipv6map["Address"].toVariant().value.toString() ||
            spV6Prefix->value() != m_ipv6map["PrefixLength"].toVariant().value.toByte() ||
            leV6Gateway->text() != m_ipv6map["Gateway"].toVariant().value.toString() )) ||
            (method == "auto" &&
            cbV6Privacy->currentText() != m_ipv6map["Privacy"].toVariant().value.toString())
        ) {
            m_ipv6map["Method"] == QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(method)));
            if( method == "manual" ) {
                m_ipv6map["Address"] = QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(leV6Address->text())));
                m_ipv6map["PrefixLength"] = QDBusData::fromVariant(QDBusVariant(QDBusData::fromByte(spV6Prefix->value())));
                m_ipv6map["Gateway"] = QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(leV6Gateway->text())));
            }
            else if( method == "auto" ) {
                m_ipv6map["Privacy"] = QDBusData::fromVariant(QDBusVariant(QDBusData::fromString(cbV6Privacy->currentText())));
            }
            odebug << "setting ipv6 config" << oendl;
            QValueList<QDBusData> params;
            params << QDBusData::fromString("IPv6.Configuration");
            params << QDBusData::fromVariant(QDBusVariant(QDBusData::fromStringKeyMap(QDBusDataMap<QString>(m_ipv6map))));
            QDBusMessage reply = m_proxy->sendWithReply("SetProperty", params);
        }
    }
    QDialog::accept();
}

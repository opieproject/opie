/***************************************************************************
                          katesyntaxdocument.cpp  -  description
                             -------------------
    begin                : Sat 31 March 2001
    copyright            : (C) 2001,2002 by Joseph Wenninger
    email                : jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "katesyntaxdocument.h"
#include <qfile.h>
#include <kdebug.h>
#include <kstddirs.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <qstringlist.h>
#include <kconfig.h>
#include <kglobal.h>
#include <qpe/qpeapplication.h>
#include <qdir.h>

SyntaxDocument::SyntaxDocument()
{
  m_root=0;
  currentFile="";
  setupModeList();
}

void SyntaxDocument::setIdentifier(const QString& identifier)
{
#warning FIXME  delete m_root;
  m_root=Opie::XMLElement::load(identifier);
  if (!m_root)    KMessageBox::error( 0L, i18n("Can't open %1").arg(identifier) );
  
}

SyntaxDocument::~SyntaxDocument()
{
}

void SyntaxDocument::setupModeList(bool force)
{

  if (myModeList.count() > 0) return;

  KConfig *config=KGlobal::config();
  KStandardDirs *dirs = KGlobal::dirs();

//  QStringList list=dirs->findAllResources("data","kate/syntax/*.xml",false,true);
  QString path=QPEApplication::qpeDir() +"share/tinykate/syntax/";

  QDir dir(path);
  QStringList list=dir.entryList("*.xml");

  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it )
  {
    QString Group="Highlighting_Cache"+path+*it;

    if ((config->hasGroup(Group)) && (!force))
    {
      config->setGroup(Group);
      syntaxModeListItem *mli=new syntaxModeListItem;
      mli->name = config->readEntry("name","");
      mli->section = config->readEntry("section","");
      mli->mimetype = config->readEntry("mimetype","");
      mli->extension = config->readEntry("extension","");
      mli->identifier = *it;
      myModeList.append(mli);
    }
    else
    {
	qDebug("Found a description file:"+path+(*it));
        setIdentifier(path+(*it));
        Opie::XMLElement *e=m_root;
        if (e)
        {
	  e=e->firstChild();
	  qDebug(e->tagName());
          if (e->tagName()=="language")
          {
            syntaxModeListItem *mli=new syntaxModeListItem;
            mli->name = e->attribute("name");
            mli->section = e->attribute("section");
            mli->mimetype = e->attribute("mimetype");
            mli->extension = e->attribute("extensions");
	    qDebug(QString("valid description for: %1/%2").arg(mli->section).arg(mli->name));
            if (mli->section.isEmpty())
              mli->section=i18n("Other");

            mli->identifier = path+(*it);
#warning fixme
/*
            config->setGroup(Group);
            config->writeEntry("name",mli->name);
            config->writeEntry("section",mli->section);
            config->writeEntry("mimetype",mli->mimetype);
            config->writeEntry("extension",mli->extension);
*/
            myModeList.append(mli);
          }
        }
      }
    }
//  }

//  config->sync();
}

SyntaxModeList SyntaxDocument::modeList()
{
  return myModeList;
}

bool SyntaxDocument::nextGroup( syntaxContextData* data)
{
  if(!data) return false;

  if (!data->currentGroup)
    data->currentGroup=data->parent->firstChild();
  else
    data->currentGroup=data->currentGroup->nextChild();

  data->item=0;

  if (!data->currentGroup)
    return false;
  else
    return true;
}

bool SyntaxDocument::nextItem( syntaxContextData* data)
{
  if(!data) return false;

  if (!data->item)
    data->item=data->currentGroup->firstChild();
  else
    data->item=data->item->nextChild();

  if (!data->item)
    return false;
  else
    return true;
}

QString SyntaxDocument::groupItemData( syntaxContextData* data,QString name)
{
  if(!data)
    return QString::null;

  if ( (data->item) && (name.isEmpty()))
    return data->item->tagName();

  if (data->item)
    return data->item->attribute(name);
  else
    return QString();
}

QString SyntaxDocument::groupData( syntaxContextData* data,QString name)
{
  if(!data)
    return QString::null;

  if (data->currentGroup)
    return data->currentGroup->attribute(name);
  else
    return QString();
}

void SyntaxDocument::freeGroupInfo( syntaxContextData* data)
{
  if (data)
    delete data;
}

syntaxContextData* SyntaxDocument::getSubItems(syntaxContextData* data)
{
  syntaxContextData *retval=new syntaxContextData;
	retval->parent=0;
	retval->currentGroup=0;
	retval->item=0;
  if (data != 0)
  {
    retval->parent=data->currentGroup;
    retval->currentGroup=data->item;
    retval->item=0;
   }

  return retval;
}

syntaxContextData* SyntaxDocument::getConfig(const QString& mainGroupName, const QString &Config)
{
  Opie::XMLElement *e = m_root->firstChild()->firstChild();

  while (e)
  {
    kdDebug(13010)<<"in SyntaxDocument::getGroupInfo (outer loop) " <<endl;

    if (e->tagName().compare(mainGroupName)==0 )
    {
       Opie::XMLElement *e1=e->firstChild();

      while (e1)
      {
        kdDebug(13010)<<"in SyntaxDocument::getGroupInfo (inner loop) " <<endl;

        if (e1->tagName()==Config)
        {
          syntaxContextData *data=new ( syntaxContextData);
	  data->currentGroup=0;
	  data->parent=0;
          data->item=e1;
          return data;
        }

        e1=e1->nextChild();
      }

      kdDebug(13010) << "WARNING :returning null 3"<< endl;
      return 0;
    }

    e=e->nextChild();
  }

  kdDebug(13010) << "WARNING :returning null 4"  << endl;
  return 0;
}



syntaxContextData* SyntaxDocument::getGroupInfo(const QString& mainGroupName, const QString &group)
{

  Opie::XMLElement *e=m_root->firstChild()->firstChild();

  while (e)
  {
    kdDebug(13010)<<"in SyntaxDocument::getGroupInfo (outer loop) " <<endl;

    if (e->tagName().compare(mainGroupName)==0 )
    {
      Opie::XMLElement *e1=e->firstChild();

      while (e1)
      {
        kdDebug(13010)<<"in SyntaxDocument::getGroupInfo (inner loop) " <<endl;
        if (e1->tagName()==group+"s")
        {
          syntaxContextData *data=new ( syntaxContextData);
          data->parent=e1;
	  data->currentGroup=0;
	  data->item=0;
          return data;
        }

        e1=e1->nextChild();
      }

      kdDebug(13010) << "WARNING : getGroupInfo returning null :1 " << endl;
      return 0;
    }

    e=e->nextChild();
  }

  kdDebug(13010) << "WARNING : getGroupInfo returning null :2" << endl;
  return 0;
}


QStringList& SyntaxDocument::finddata(const QString& mainGroup,const QString& type,bool clearList)
{
  Opie::XMLElement *e  = m_root->firstChild();
  if (clearList)
    m_data.clear();

  for(e=e->firstChild(); e; e=e->nextChild())
  {
    if (e->tagName()==mainGroup)
    {
	for (Opie::XMLElement *e1=e->firstChild();e1;e1=e1->nextChild())
	{
	  if (e1->tagName()!="list") continue;

	        if (e1->attribute("name")==type)
        	{
			for (Opie::XMLElement *e2=e1->firstChild();e2;e2=e2->nextChild())
			{
				qDebug("FOUND A LIST ENTRY("+e2->tagName()+"):"+e2->firstChild()->value());
	            		m_data+=e2->firstChild()->value().stripWhiteSpace();
			}
	           break;
		}
        }
      break;
    }
  }

  return m_data;
}

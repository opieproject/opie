/***************************************************************************
 *   Copyright (C) 2004 by ljp                                             *
 *   lpotter@trolltech.com                                                 *
 *                                                                         *
 *   This program may be distributed under the terms of the Q Public       *
 *   License as defined by Trolltech AS of Norway and appearing in the     *
 *   file LICENSE.QPL included in the packaging of this file.              *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                  *
 ***************************************************************************/
#include "rssparser.h"

#include <qstring.h>
#include <qmessagebox.h>
#include <qlist.h>

RssParser::RssParser()
    : tag(NoneTag)
    , Item(0)
    , channel(0)
    , image(0)
    , isItem(false)
    , htmlString()
{
}

RssParser::~RssParser()
{
    if(channel) {
        int size = channel->rssItems.size();
        for (int i = 0; i < size; i++) {
            delete channel->rssItems.at(i);
        }
        delete channel;
    }
}

bool RssParser::startElement( const QString &, const QString & /*localName*/,const QString & qName, const QXmlAttributes &atts)
{
   if( qName == "rss") {
      channel = new rssChannel();
      channel->rssItems.resize( 0);
      return true;
   }
//   qWarning(qName + " %d",atts.length());
   if(qName == "item") {
      isItem = true;
      tag = NewItemTag;
      Item = new rssItem();
   }

   if(qName == "image") {
      tag = NewItemTag;
      image = new rssImage();
   }

   if(qName == "title") {
      tag = TitleTag;
   }

   if (qName == "description") {
      tag = DescriptionTag;
   }

   if( qName == "link") {
      tag = LinkTag;
   }

   if( qName == "pubDate") {
      tag = pubDateTag;
   }

//    if( qName == "enclosure") {
//       tag = EnclosureTag;
//    }

   if(atts.length() > 0/* && tag == EnclosureTag*/) {
//      qWarning(qName +" attributes %d", atts.length());
//       Item->attributes << qName;
//       for(int i=0; i < atts.length(); i++) {
//          Item->attributes << atts.qName(i) << atts.value(atts.qName(i));
//       }

      QStringList *sList;
      sList = new QStringList();
      sList->append(qName);
      for(int i=0; i < atts.length(); i++) {
         sList->append( atts.qName(i));
         sList->append( atts.value(atts.qName(i)));
      }
      if(isItem)
         Item->attributes.append( sList);
      else
         channel->attributes.append( sList);
   }

   return true;
}

bool RssParser::endElement( const QString &, const QString &, const QString & qName )
{
    tag = NoneTag;
    if(qName == "item") {
        isItem = false;
        int size = channel->rssItems.size();
        channel->rssItems.resize( size + 1);
        channel->rssItems.insert( channel->rssItems.size() - 1, Item);
    }
    if(qName == "channel") {
//         isItem = false;
//         int size = channel->rssItems.size();
//         channel->rssItems.resize( size + 1);
//         channel->rssItems.insert( channel->rssItems.size() - 1, Item);
    }
    return true;
}

bool RssParser::characters( const QString & ch )
{
   if(!ch.isEmpty()) {
      if(isItem) {
//       qWarning("ch "+ch);
         switch(tag) {
         case NewItemTag:
            break;
         case TitleTag:
            Item->title = ch;
            break;
         case DescriptionTag:
            Item->description = ch;
            break;
         case LinkTag:
            Item->link = ch;
            break;
         case pubDateTag:
            Item->pubdate = ch;
            break;
         case NoneTag:
            break;
         };
      } else { //channel
         switch(tag) {
         case TitleTag:
            channel->title = ch;
            break;
         case DescriptionTag:
            channel->description = ch;
            break;
         case LinkTag:
            channel->link = ch;
            break;
         case pubDateTag:
            channel->pubdate = ch;
            break;
         case NoneTag:
         case NewItemTag:
            break;
         };
      }
   }
      return true;
}

bool RssParser::warning(const QXmlParseException &e)
{
      errorMessage = e.message();
//      QMessageBox::message("Warning",tr("<p>Sorry, could not find the requested document.</p>"));
      qWarning("Warning " + errorMessage);
      return true;
}

bool RssParser::error(const QXmlParseException &e)
{
      errorMessage = e.message();
//      QMessageBox::message("Error", "<p>" + errorMessage + "</p>");
      qWarning("Error: " + errorMessage);
      return true;
}

bool RssParser::fatalError(const QXmlParseException &e)
{
      errorMessage = e.message();
//       errorMessage += " line: " + e.lineNumber();
//       errorMessage += " col: " + e.columnNumber();
       qWarning("Fatal Error: "+ errorMessage);
       qWarning("line %d, col %d\n", e.lineNumber(),  e.columnNumber());
//      QMessageBox::message("Fatal Error", errorMessage );
      return false;
}

QVector<rssItem> &RssParser::getItems()
{
    return channel->rssItems;
}

int RssParser::getSize()
{
   return channel->rssItems.size();
}

QStringList RssParser::getChannelInfo()
{
   QStringList ch;
   ch << channel->title <<  channel->description << channel->link << channel->pubdate << channel->copyright << channel->language;
   return ch;
}

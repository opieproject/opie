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

/* RSS required tags:
    title,link,description
*/
/* RSS optional tags:
    language,copyright,managingEditor,webMaster,pubDate,lastBuildDate,category,generator,docs,cloud,ttl,
    image,rating,textInput,skipHours,skipDays
*/
/*
  podcast tags
  <rss xmlns:itunes="http://www.itunes.com/DTDs/Podcast-1.0.dtd" version="2.0">
  title,link,copyright,pubDate,enclosure,guid,itunes:author,itunes:block,itunes:category,itunes:duration,itunes:explicit,itunes:keywords,itunes:owner,itunes:subtitle,itunes:summary
*/

#ifndef RSSPARSER_H
#define RSSPARSER_H

#include <qxml.h>
#include <qstringlist.h>
#include <qvector.h>
#include <qstringlist.h>
#include <qlist.h>

class QString;

class rssImage {
public:
   QString url;
   QString title;
   int width;
   int height;
   QString description;
};

class rssItem {
 public:
  QString title;
	QString description;
  QString link;
  QString pubdate;
  QVector <rssImage> rssImageTags;
  QList<QStringList> attributes; //tags with atttributes
};

class rssChannel {
 public:
  QString title;
	QString description;
  QString link;
  QString pubdate;
  QString copyright;
  QString language;
  QVector <rssImage> rssImageTags;
  QVector <rssItem> rssItems;
  QList<QStringList> attributes; //tags with atttributes
};

class RssParser : public QXmlDefaultHandler
{
public:
   RssParser();
   ~RssParser();
	 QString errorMessage;

	 QVector<rssItem> &getItems();
	 QStringList getChannelInfo();
   int getSize();

private:

	 enum Tag {
			 NoneTag = 0,
			 TitleTag = 1,
			 NewItemTag = 2,
			 DescriptionTag = 3,
			 LinkTag = 4,
       pubDateTag = 5,
/*
       ImageTag = 6,
       UrlTag = 7,
       WidthTag = 8,
       HeightTag = 9,
 */
	 };
	 Tag tag;

//   QVector <rssItem> rssItems;
   QStringList channelInfo;
	 rssItem *Item;
	 rssChannel *channel;
	 rssImage *image;

	 bool isItem;
   QStringList tokenNames;
	 QString htmlString;

protected:

   bool startElement( const QString&, const QString&, const QString& ,const QXmlAttributes& );
   bool endElement( const QString&, const QString&, const QString& );
	 bool characters( const QString &);

	 bool warning(const QXmlParseException &);
	 bool error(const QXmlParseException &);
	 bool fatalError(const QXmlParseException &);

	 QString itemTitle, itemLink, itemDescription;
};

#endif

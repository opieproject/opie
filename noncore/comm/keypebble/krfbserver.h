// -*- c++ -*-

#ifndef KRFBOPTIONS_H
#define KRFBOPTIONS_H

class Config;

class KRFBServer
{
public:
  KRFBServer();
  ~KRFBServer();

  int encodings();

	QString name;
	QString hostname;
	QString password;
	int display;

  bool hexTile;
  bool corre;
  bool rre;
  bool copyrect;
    
  bool colors256;
  bool shared;
  bool readOnly;
  bool deIconify;

  int updateRate;
};

#endif // KRFBOPTIONS_H

// -*- c++ -*-

#ifndef KRFBOPTIONS_H
#define KRFBOPTIONS_H

class Config;

class KRFBOptions
{
public:
  KRFBOptions();
  ~KRFBOptions();

  int encodings();
  void readSettings( Config *config );
  void writeSettings( Config *config );

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

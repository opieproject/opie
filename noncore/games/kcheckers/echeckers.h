
#ifndef ECHECKERS_H
#define ECHECKERS_H

#include "checkers.h"


class ECheckers:public Checkers
{
  public:
   	ECheckers(int skill):Checkers(skill){};
    bool go1(int,int);

    bool checkCapture1();
    bool checkCapture2();

  private:
    void kingMove2(int,int &);

    bool manCapture1(int,int,bool &);
    bool kingCapture1(int,int,bool &);

    bool manCapture2(int,int &);
    bool kingCapture2(int,int,int &);

};

#endif

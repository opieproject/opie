#ifndef CHECKERS_H
#define CHECKERS_H

#define NONE   0
#define MAN1   1
#define KING1  2
#define FREE   3
#define KING2  4
#define MAN2   5
#define FULL   6

#define UL -6
#define UR -5
#define DL  5
#define DR  6


class Checkers
{
  public:
	  Checkers(int);
    virtual bool go1(int,int)=0;

    void go2();
    bool checkMove1();
    bool checkMove2();

    virtual bool checkCapture1()=0;
    virtual bool checkCapture2()=0;

    void setLevel(int i) {levelmax=i;};

    int board[54];

  protected:
    int level;        // Current level
    int levelmax;     // Maximum level

    int  turn();
    void turn(int &,bool capture=false);

    int to;
    int bestboard[54];
    int bestcounter;

    virtual void kingMove2(int,int &)=0;

    virtual bool manCapture2(int,int &)=0;
    virtual bool kingCapture2(int,int,int &)=0;

    virtual bool manCapture1(int,int,bool &)=0;
    virtual bool kingCapture1(int,int,bool &)=0;

};

#endif

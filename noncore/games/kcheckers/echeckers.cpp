//
// English Checkers


#include "echeckers.h"


///////////////////////////////////////////////////
//
//  User Functions
//
///////////////////////////////////////////////////


bool ECheckers::go1(int from,int field)
{
  to=field;

  if(checkCapture1())
  {
    bool capture=false;
    switch(board[from])
    {
      case MAN1:
        if(manCapture1(from,UL,capture)) return true;
        if(manCapture1(from,UR,capture)) return true;
        return false;
      case KING1:
        if(kingCapture1(from,UL,capture)) return true;
        if(kingCapture1(from,UR,capture)) return true;
        if(kingCapture1(from,DL,capture)) return true;
        if(kingCapture1(from,DR,capture)) return true;
        return false;
    }
  }
  else
  {
    switch(board[from])
    {
      case MAN1:
        if((to==(from-6))||(to==(from-5)))
        {
          board[from]=FREE;
          if(to<10) board[to]=KING1;
          else board[to]=MAN1;
          return true;
        }
        return false;
      case KING1:
        if((to==(from-6))||(to==(from-5))||
           (to==(from+5))||(to==(from+6)) )
        {
          board[from]=FREE;
          board[to]=KING1;
          return true;
        }
        return false;
    }
  }
  return false;
}


bool ECheckers::checkCapture1()
{
  for(int i=6;i<48;i++)
  {
    switch(board[i])
    {
      case MAN1:
        if(board[i-6]==MAN2 || board[i-6]==KING2)
          if(board[i-12]==FREE) return true;
        if(board[i-5]==MAN2 || board[i-5]==KING2)
          if(board[i-10]==FREE) return true;
        break;
      case KING1:
        if(board[i-6]==MAN2 || board[i-6]==KING2)
          if(board[i-12]==FREE) return true;
        if(board[i-5]==MAN2 || board[i-5]==KING2)
          if(board[i-10]==FREE) return true;
        if(board[i+5]==MAN2 || board[i+5]==KING2)
          if(board[i+10]==FREE) return true;
        if(board[i+6]==MAN2 || board[i+6]==KING2)
          if(board[i+12]==FREE) return true;
    }
  }
  return false;
}


// Return TRUE if a course of the user true
// Return FALSE if a course of the user incorrect

bool ECheckers::manCapture1(int from,int direction,bool &capture)
{
  int i=from+direction;
  if(board[i]==MAN2 || board[i]==KING2)
  {
    int k=i+direction;
    if(board[k]==FREE)
    {
      bool next=false;
      int save=board[i];
      board[from]=FREE;
      board[i]=NONE;

      if(k<10)
      {
        board[k]=KING1;
        if(kingCapture1(k,direction+11,next)) {board[i]=FREE;return true;}
      }
      else
      {
        board[k]=MAN1;
        if(manCapture1(k,UL,next)) {board[i]=FREE;return true;}
        if(manCapture1(k,UR,next)) {board[i]=FREE;return true;}
      }

      if((!next) && k==to) {board[i]=FREE;return true;}

      board[k]=FREE;
      board[i]=save;
      board[from]=MAN1;
      capture=true;
    }
  }
  return false;
}


bool ECheckers::kingCapture1(int from,int direction,bool &capture)
{
  int i=from+direction;
  if(board[i]==MAN2 || board[i]==KING2)
  {
    int k=i+direction;
    if(board[k]==FREE)
    {
      bool next=false;
      int save=board[i];
      board[from]=FREE;
      board[i]=NONE;
      board[k]=KING1;

      if(direction==UL || direction==DR)
      {
        if(kingCapture1(k,UR,next)) {board[i]=FREE;return true;}
        if(kingCapture1(k,DL,next)) {board[i]=FREE;return true;}
      }
      else
      {
        if(kingCapture1(k,UL,next)) {board[i]=FREE;return true;}
        if(kingCapture1(k,DR,next)) {board[i]=FREE;return true;}
      }
      if(kingCapture1(k,direction,next)) {board[i]=FREE;return true;}

      if((!next) && k==to) {board[i]=FREE;return true;}

      board[k]=FREE;
      board[i]=save;
      board[from]=KING1;
      capture=true;
    }
  }
  return false;
}


////////////////////////////////////////////////////
//
// Computer Functions
//
////////////////////////////////////////////////////


void ECheckers::kingMove2(int from,int &resMax)
{
  board[from]=FREE;

  int i=from-6;
  if(board[i]==FREE)
  {
    board[i]=KING2;
    turn(resMax);
    board[i]=FREE;
  }

  i=from-5;
  if(board[i]==FREE)
  {
    board[i]=KING2;
    turn(resMax);
    board[i]=FREE;
  }

  i=from+5;
  if(board[i]==FREE)
  {
    board[i]=KING2;
    turn(resMax);
    board[i]=FREE;
  }

  i=from+6;
  if(board[i]==FREE)
  {
    board[i]=KING2;
    turn(resMax);
    board[i]=FREE;
  }

  board[from]=KING2;
}


bool ECheckers::checkCapture2()
{
  for(int i=6;i<48;i++)
  {
    switch(board[i])
    {
      case MAN2:
        if(board[i+5]==MAN1 || board[i+5]==KING1)
          if(board[i+10]==FREE) return true;
        if(board[i+6]==MAN1 || board[i+6]==KING1)
          if(board[i+12]==FREE) return true;
        break;
      case KING2:
        if(board[i-6]==MAN1 || board[i-6]==KING1)
          if(board[i-12]==FREE) return true;
        if(board[i-5]==MAN1 || board[i-5]==KING1)
          if(board[i-10]==FREE) return true;
        if(board[i+5]==MAN1 || board[i+5]==KING1)
          if(board[i+10]==FREE) return true;
        if(board[i+6]==MAN1 || board[i+6]==KING1)
          if(board[i+12]==FREE) return true;
    }
  }
  return false;
}


// Return TRUE if it is possible to capture
// Return FALSE if it is impossible to capture

bool ECheckers::manCapture2(int from,int &resMax)
{
  bool capture=false;

  int i=from+5;
  if(board[i]==MAN1 || board[i]==KING1)
  {
    int k=from+10;
    if(board[k]==FREE)
    {
      int save=board[i];
      board[from]=FREE;
      board[i]=NONE;
      resMax--;
      if(from>32)
      {
        board[k]=KING2;
        if(!kingCapture2(k,UL,resMax)) turn(resMax,true);
      }
      else
      {
        board[k]=MAN2;
        if(!manCapture2(k,resMax)) turn(resMax,true);
      }
      resMax++;
      board[k]=FREE;
      board[i]=save;
      board[from]=MAN2;
      capture=true;
    }
  }

  i=from+6;
  if(board[i]==MAN1 || board[i]==KING1)
  {
    int k=from+12;
    if(board[k]==FREE)
    {
      int save=board[i];
      board[from]=FREE;
      board[i]=NONE;
      resMax--;
      if(from>32)
      {
        board[k]=KING2;
        if(!kingCapture2(k,UR,resMax)) turn(resMax,true);
      }
      else
      {
        board[k]=MAN2;
        if(!manCapture2(k,resMax)) turn(resMax,true);
      }
      resMax++;
      board[k]=FREE;
      board[i]=save;
      board[from]=MAN2;
      capture=true;
    }
  }

  if(capture) return true;
  return false;
}


bool ECheckers::kingCapture2(int from,int direction,int &resMax)
{
  int i=from+direction;
  if(board[i]==MAN1 || board[i]==KING1)
  {
    int k=i+direction;
    if(board[k]==FREE)
    {
      bool capture=false;
      int save=board[i];
      board[from]=FREE;
      board[i]=NONE;
      resMax--;

      board[k]=KING2;
      if(direction==UL || direction==DR)
      {
        if(kingCapture2(k,UR,resMax)) capture=true;
        if(kingCapture2(k,DL,resMax)) capture=true;
      }
      else
      {
        if(kingCapture2(k,UL,resMax)) capture=true;
        if(kingCapture2(k,DR,resMax)) capture=true;
      }
      if(kingCapture2(k,direction,resMax)) capture=true;

      if(!capture) turn(resMax,true);
      board[k]=FREE;

      resMax++;
      board[i]=save;
      board[from]=KING2;
      return true;
    }
  }
  return false;
}


/* -------------------------------------------------------------------------- */
/*                                                                            */
/* [TEHistory.H]                   History Buffer                             */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>            */
/*                                                                            */
/* This file is part of Konsole - an X terminal for KDE                       */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                        */
/* Ported Konsole to Qt/Embedded                                              */
/*                        */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*                        */
/* -------------------------------------------------------------------------- */

#ifndef TEHISTORY_H
#define TEHISTORY_H

#include "TECommon.h"

class HistoryScroll
{
public:
  HistoryScroll();
 ~HistoryScroll();

public:
 void setSize(int lines, int cells);
 void setScroll(bool on);
 bool hasScroll();

 int  getLines();
 int  getLineLen(int lineno);
 void getCells(int lineno, int colno, int count, ca *res);

  ca   getCell(int lineno, int colno) { ca res; getCells(lineno,colno,1,&res); return res; }

  void addCells(ca *text, int count);
  void addLine();

private:
  int startOfLine(int lineno);

  int m_max_lines;
  int *m_lines;
  int m_max_cells;
  ca *m_cells;
  int m_first_line;
  int m_last_cell;
  int m_num_lines;
  int m_start_line;
};

#endif // TEHISTORY_H

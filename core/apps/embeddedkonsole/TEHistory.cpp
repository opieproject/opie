/* -------------------------------------------------------------------------- */
/*                                                                            */
/* [TEHistory.C]                   History Buffer                             */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/* Copyright (c) 1997,1998 by Lars Doelle <lars.doelle@on-line.de>            */
/*                                                                            */
/* This file is part of Qkonsole - an X terminal for KDE                       */
/*                                                                            */
/* -------------------------------------------------------------------------- */
/*                        */
/* Ported Qkonsole to Qt/Embedded                                              */
/*                        */
/* Copyright (C) 2000 by John Ryland <jryland@trolltech.com>                  */
/*                        */
/* -------------------------------------------------------------------------- */

#include "TEHistory.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include <qpe/config.h>

#define HERE printf("%s(%d): here\n",__FILE__,__LINE__)

/*
   An arbitrary long scroll.

   One can modify the scroll only by adding either cells
   or newlines, but access it randomly.

   The model is that of an arbitrary wide typewriter scroll
   in that the scroll is a serie of lines and each line is
   a serie of cells with no overwriting permitted.

   The implementation provides arbitrary length and numbers
   of cells and line/column indexed read access to the scroll
   at constant costs.

*/


HistoryScroll::HistoryScroll()
{
  m_lines = NULL;
  m_max_lines = 0;
  m_cells = NULL;
  m_max_cells = 0;
  m_num_lines = 0;
  m_first_line = 0;
  m_last_cell = 0;
  m_start_line = 0;
}

HistoryScroll::~HistoryScroll()
{
  setSize(0,0);
}
 
void HistoryScroll::setSize(int lines, int cells) 
{
  // could try to preserve the existing data...
  //  printf("setSize(%d,%d)\n", lines, cells);
  if (m_lines) {
    delete m_lines;
    m_lines = NULL;
  }
  if (m_cells) {
    delete m_cells;
    m_cells = NULL;
  }
  m_max_lines = m_max_cells = 0;
  if (lines > 0 && cells > 0) {
    m_max_lines = lines;
    m_lines = new int[m_max_lines];
    m_lines[0] = 0;
    m_max_cells = cells;
    m_cells = new ca[m_max_cells];
  }
  m_first_line = 0;
  m_num_lines = 0;
  m_last_cell = 0;
  m_start_line = 0;
}
  
void HistoryScroll::setScroll(bool on)
{
  Config cfg( "Konsole" );
  cfg.setGroup("History");
  //  printf("setScroll(%d)\n", on);
  if (on) {
    int lines = cfg.readNumEntry("history_lines",300);
    int avg_line = cfg.readNumEntry("avg_line_length",60);
    int cells = lines * avg_line;
    setSize(lines,cells);
  } else {
    setSize(0,0);
  }
}
 
bool HistoryScroll::hasScroll()
{
  return (m_max_lines > 0);
}

int HistoryScroll::getLines()
{
  return(m_num_lines);
}

int HistoryScroll::getLineLen(int lineno)
{
  if (!hasScroll()) return 0;
  if (lineno >= m_num_lines) {
    //    printf("getLineLen(%d) out of range %d\n", lineno, m_num_lines);
    return(0);
  }
  int len =  startOfLine(lineno+1) - startOfLine(lineno);
  if (len < 0) {
    len += m_max_cells;
  }
  //  printf("getLineLen(%d) = %d\n", lineno, len);
  return(len);
}

int HistoryScroll::startOfLine(int lineno)
{
  //  printf("startOfLine(%d) =", lineno);
  if (!hasScroll()) return 0;
  assert(lineno >= 0 && lineno <= m_num_lines);
  if (lineno < m_num_lines) {
    int index = lineno + m_first_line;
    if (index >= m_max_lines)
      index -= m_max_lines;
    //    printf("%d\n", m_lines[index]);
    return(m_lines[index]);
  } else {
    //    printf("last %d\n", m_last_cell);
    return(m_last_cell);
  }
}

void HistoryScroll::getCells(int lineno, int colno, int count, ca *res)
{
  //  printf("getCells(%d,%d,%d) num_lines=%d\n", lineno, colno, count, m_num_lines);
  assert(hasScroll());
  assert(lineno >= 0 && lineno < m_num_lines);
  int index = lineno + m_first_line;
  if (index >= m_max_lines)
    index -= m_max_lines;
  assert(index >= 0 && index < m_max_lines);
  index = m_lines[index] + colno;
  assert(index >= 0 && index < m_max_cells);
  while(count-- > 0) {
    *res++ = m_cells[index];
    if (++index >= m_max_cells) {
      index = 0;
    }
  }
}

void HistoryScroll::addCells(ca *text, int count)
{
  if (!hasScroll()) return;
  int start_cell = m_last_cell;
  //  printf("addCells count=%d start=%d first_line=%d first_cell=%d lines=%d\n", 
  //  	 count, start_cell, m_first_line, m_lines[m_first_line], m_num_lines);
  if (count <= 0) {
    return;
  }
  while(count-- > 0) {
    assert (m_last_cell >= 0 && m_last_cell < m_max_cells );
    m_cells[m_last_cell] = *text++;
    if (++m_last_cell >= m_max_cells) {
      m_last_cell = 0;
    }
  }
  if (m_num_lines > 1) {
    if (m_last_cell > start_cell) {
      while(m_num_lines > 0
	    && m_lines[m_first_line] >= start_cell 
	    && m_lines[m_first_line] < m_last_cell) {
	//		printf("A remove %d>%d && %d<%d first_line=%d num_lines=%d\n", 
	//		       m_lines[m_first_line], start_cell, m_lines[m_first_line], m_last_cell,
	//		       m_first_line, m_num_lines);
	if (++m_first_line >= m_max_lines) {
	  m_first_line = 0;
	}
	m_num_lines--;
      }
    } else {
      while(m_num_lines > 0
	    && (m_lines[m_first_line] >= start_cell 
		|| m_lines[m_first_line] < m_last_cell)) {
	//		printf("B remove %d>%d || %d<%d first_line=%d num_lines=%d\n", 
	//		       m_lines[m_first_line], start_cell, m_lines[m_first_line], m_last_cell,
	//       m_first_line, m_num_lines);
	if (++m_first_line >= m_max_lines) {
	  m_first_line = 0;
	}
	m_num_lines--;
      }
    }
  }
}

void HistoryScroll::addLine()
{
  if (!hasScroll()) return;
  int index = m_first_line + m_num_lines;
  if (index >= m_max_lines) {
    index -= m_max_lines;
  }
  //  printf("addLine line=%d cell=%d\n", index, m_last_cell);
  assert(index >= 0 && index < m_max_lines);
  m_lines[index] = m_start_line;
  m_start_line = m_last_cell;
  if (m_num_lines >= m_max_lines) {
    if (++m_first_line >= m_num_lines) {
      m_first_line = 0;
    }
  } else {
    m_num_lines++;
  }
}

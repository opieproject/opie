/* 
 * kmolcalc.h
 *
 * Copyright (C) 2000 Tomislav Gountchev <tomi@idiom.com>
 */


#ifndef KMOLCALC_H
#define KMOLCALC_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 


#include "kmolelements.h"
#include <qdict.h>
#include <qlist.h>
#include <qstring.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qtextstream.h>


/**
 * KMOLCALC is the calculation engine. It knows about a hashtable of user defined atomic
 * weights and group definitions ELSTABLE, and the currently processed formula, stored
 * as a list of elements and their coefficients, ELEMENTS.
 */
class KMolCalc {

public:

  KMolCalc();
  ~KMolCalc();
  
  /** 
   * Parse a string S and construct the ElementList ELEMENTS, representing the composition
   * of S. Returns "OK" if successful, or an error message if parsing failed.
   * The elements is S must be valid element or group symbols, as stored in ELSTABLE.
   * See help files for correct formula syntax.
   */
  QString readFormula(const QString& s);
  
  /**
   * Calculate and return the molecular weight of the current chemical formula.
   */
  double getWeight();
  
  /**
   * Return the elemental composition of the current formula, as a string of tab-separated
   * element - percentage pairs, separated by newlines.
   */
  QString getEA();


  /**
   * Return the empirical formula of the current compound as a QString.
   */
  QString getEmpFormula();

  /**
   * Add a new element name - atomic weight record to the ELSTABLE hashtable. 
   * Assumes NAME has valid syntax.
   */
  void defineElement(const QString& name, double weight);

  /**
   * Add a new group definition to the ELSTABLE. Returns "OK" if OK, error message
   * if parsing FORMULA fails. Assumes the syntax of NAME is correct.
   */
  QString defineGroup(const QString& name, const QString& formula);

  /**
   * Remove a group or element definition from ELSTABLE.
   */
  void undefineGroup(const QString& name);

  /**
   * Read the element definitions file and construct ELSTABLE.
   */
  void readElstable();

  /**
   * The element dictionary.
   */
  QDict<SubUnit>* elstable;

  QString mwfile;

 private:
  double weight;

  QString readGroup(const QString& s, ElementList* els);
  void readMwfile(QFile& f);
  ElementList* elements;
};
    
#endif






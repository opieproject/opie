/*
 * kmolelements.h 
 *
 * Copyright (C) 2000 Tomislav Gountchev <tomi@idiom.com>
 */

// classes that store and manipulate chemical formulas represented as
// lists of elements

#ifndef KMOLELEMENTS_H
#define KMOLELEMENTS_H


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <qlist.h>
#include <qdict.h>
#include <qstring.h>
#include <qtextstream.h>

class ElementCoef;
class ElementList;

/**
 * A generic chemical entity. Can be an element or a group. 
 */
class SubUnit {
 public:

  SubUnit();
  
  virtual ~SubUnit();

  /**
   * Construct a subunit and return a pointer to it. The syntax of LINE is 
   * the one used in the element definition file.
   */
  static SubUnit* makeSubUnit(QString line);
  
  /**
   * Get the molecular weight of THIS, based on the data from ELSTABLE.
   */
  virtual double getWeight(QDict<SubUnit>* elstable) const;

  /**
   * Add THIS to ELS.
   */
  virtual void addTo(ElementList& els, double coef) = 0;
  
  virtual QString getName() const;
  
  /**
   * Write THIS to LINE, in the format used in the definition file.
   */
  virtual void writeOut(QString& line) = 0;
};
	       

/**
 * A group of elements. 
 */
class ElementList : public SubUnit {
 public:
  ElementList ();
  ElementList (QString name);
  virtual ~ElementList();
  double getWeight(QDict<SubUnit>* elstable) const;
  
  /**
   * Return a string representing the elemental composition of THIS, as 
   * a tab-separated element - percentage pairs, separated by newlines.
   */
  QString getEA(QDict<SubUnit>* elstable, double mw = 0) const;

  /**
   * Return a string representing THIS as an empirical chemical formula.
   */
  QString getEmpFormula() const;
  
  /**
   * Multiply THIS (i.e. the coefficient of each element) by coef.
   */
  void multiplyBy(double coef);

  /**
   * Add THIS to ELS. THIS is not modified; ELS is.
   */
  void addTo(ElementList& els, double coef);

  /**
   * Add an element to THIS, with a coefficient COEF.
   */
  void addElement(const QString& name, double coef);

  /**
   * True iff THIS contains element named NAME.
   */
  bool contains(const QString& name);


  bool isEmpty();

  /**
   * The name of THIS, as a chemical group.
   */
  QString getName() const;

  /**
   * Write THIS to LINE, in a format suitable for the element definition file.
   */
  void writeOut(QString& line);

 private:
  QString name;
  QList<ElementCoef>* elements;
};

/**
 * A chemical element.
 */
class Element : public SubUnit {
 public:
  Element(const QString& name, double weight);
  virtual ~Element();
  double getWeight(QDict<SubUnit>* elstable) const;

  /**
   * Add THIS to ELS, with a coefficient COEF.  
   */
  void addTo(ElementList& els, double coef);

  QString getName() const;

  void writeOut(QString& line);

 private:
  double weight;
  QString name;
};


/**
 * An element - coefficient pair. Used to represent elements within an 
 * element list.
 */
class ElementCoef {
  friend class ElementList;
 public:
  ElementCoef(const QString& name, double coef = 1.0);
 private:
  QString name;
  double coef;
};


#endif



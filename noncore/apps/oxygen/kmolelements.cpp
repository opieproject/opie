/*
 * kmolelements.cpp
 *
 * Copyright (C) 2000 Tomislav Gountchev <tomi@idiom.com>
 */

// classes that store and manipulate chemical formulas represented as
// lists of elements

#include <stdio.h>
#include "kmolelements.h" 

/**
 * A generic chemical entity. Can be an element or a group. 
 */
SubUnit::SubUnit () {}

SubUnit::~SubUnit () {}

/**
 * Construct a subunit and return a pointer to it. The syntax of LINE is 
 * the one used in the element definition file.
 */
SubUnit* SubUnit::makeSubUnit(QString line) {
  QString name, grpname, weight, coef;
  QTextStream str (line, IO_ReadOnly);
  str >> name;
  if (name != "-group") { // not a group - must be represented as Element
    str >> weight >> ws;
    return new Element(name, weight.toDouble());
  } 
  else {
    str >> grpname;
    ElementList* els = new ElementList(grpname); // group - make an ElementList
    while (!str.eof()) {
      str >> name >> ws;
      str >> coef >> ws;
      els->addElement(name, coef.toDouble());
    }
    return els;
  }
}

QString SubUnit::getName() const {
  return QString("None");
}

/**
 * Get the molecular weight of THIS, based on the data from ELSTABLE.
 */
double SubUnit::getWeight(QDict<SubUnit>* elstable) const {
  return -1;
}

/**
 * A group of elements. 
 */
ElementList::ElementList () {
  elements = new QList<ElementCoef>;
}

ElementList::~ElementList () {
  delete elements;
}


/**
 * A group of elements. 
 */
ElementList::ElementList (QString name) {
  this->name = name;
  elements = new QList<ElementCoef>;
}

/**
 * Write THIS to LINE, in a format suitable for the element definition file.
 */
void ElementList::writeOut(QString& line) {
  QString coef;
  line = "-group " + name;
  ElementCoef* current = elements->first();
  while (current != 0) {
    line += " " + current->name + " " + coef.setNum(current->coef, 'g', 10); 
    // precision set to 10 digits
    current = elements->next();
  }
}

/**
 * Get the molecular weight of THIS, based on the data from ELSTABLE.
 */
double ElementList::getWeight(QDict<SubUnit>* elstable) const {
  double weight = 0;
  ElementCoef* current = elements->first();
  while (current != 0) {
    SubUnit* e = elstable->find(current->name);
    if (e != 0) {
      weight += (current->coef) * (e->getWeight(elstable));
    } else return -1; //ERROR
    current = elements->next();
  }
  return weight;
}

/**
 * Return a string representing the elemental composition of THIS, as 
 * a tab-separated element - percentage pairs, separated by newlines.
 */
QString ElementList::getEA(QDict<SubUnit>* elstable, double mw) const {
  if (mw == 0) mw = getWeight(elstable);
  QString ea;
  QString temp;
  ElementCoef* current = elements->first();
  while (current != 0) {
    SubUnit* e = elstable->find(current->name);
    if (e != 0) {
      double current_percent = 100 * (current->coef) * 
	(e->getWeight(elstable)) 
	/ mw;
      ea += current->name + "\t" + 
	temp.setNum(current_percent) + "\n";
    } else return QString("ERROR!\n"); //ERROR
    current = elements->next();
  }
  return ea;
}
      
/**
 * Return a string representing THIS as an empirical chemical formula.
 */
QString ElementList::getEmpFormula() const {
  QString ef;
  QString temp;
  ElementCoef* current = elements->first();
  while (current != 0) {
    ef += current->name + temp.setNum(current->coef);
    current = elements->next();
  }
  return ef;
}

/**
 * Multiply THIS (i.e. the coefficient of each element) by coef.
 */
void ElementList::multiplyBy(double coef) {
  ElementCoef* current = elements->first();
  while (current != 0) {
    (current->coef) *= coef;
    current = elements->next();
  }
}

/**
 * Add THIS to ELS. THIS is not modified; ELS is.
 */
void ElementList::addTo(ElementList& els, double coef) {
  ElementCoef* current = elements->first();
  while (current != 0) {
    els.addElement(current->name, (current->coef) * coef);
    current = elements->next();
  }
}

/**
 * Add an element to THIS, with a coefficient COEF. If THIS already contains 
 * an element with the same name, adjust its coefficient only; if not, create
 * a new ElementCoef pair and add to THIS.
 */
void ElementList::addElement(const QString& name, double coef) {
  ElementCoef* current = elements->first();
  while (current != 0) {
    if (current->name == name) {
      current->coef += coef;
      return;
    }
    current = elements->next();
  }
  elements->append(new ElementCoef(name, coef));
}

/**
 * True iff THIS contains element named NAME.
 */
bool ElementList::contains(const QString& name) {
  ElementCoef* current = elements->first();
  while (current != 0) {
    if (current->name == name) 
      return true;
    current = elements->next();
  }
  return false;
}

bool ElementList::isEmpty() {
  return elements->isEmpty();
}

QString ElementList::getName() const {
  return name;
}

/**
 * A chemical element.
 */
Element::Element(const QString& n, double w) 
  : weight(w), name(n) { }


Element::~Element() {
}


/**
 * Write THIS to LINE, in a format suitable for the element definition file.
 */
void Element::writeOut(QString& line) {
  line.setNum(weight);
  line = name + " " + line;
}

double Element::getWeight(QDict<SubUnit>* elstable) const {
  return weight;
}

void Element::addTo(ElementList& els, double coef) {
  els.addElement(name, coef);
}

QString Element::getName() const {
  return name;
}

/**
 * An element - coefficient pair. Used to represent elements within an 
 * element list.
 */
ElementCoef::ElementCoef(const QString& n, double c) : name(n), coef(c) {}


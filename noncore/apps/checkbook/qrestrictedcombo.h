/*
 *
 * Definition of QRestrictedCombo
 *
 * Copyright (C) 1997 Michael Wiedmann, <mw@miwie.in-berlin.de>
 *
 * Edited 2001 by Nick Betcher <nbetcher@usinternet.com> to work
 * with Qt-only. Changed class name from LRestrictedLine to
 * QRestrictedCombo in order to accomidate Combo Boxes.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef QRESTRICTEDCOMBO_H
#define QRESTRICTEDCOMBO_H

#include <qcombobox.h>

/**
 * The QRestrictedCombo widget is a variant of @ref QComboBox which
 * accepts only a restricted set of characters as input.
 * All other characters will be discarded and the signal @ref #invalidChar()
 * will be emitted for each of them.
 *
 * Valid characters can be passed as a QString to the constructor
 * or set afterwards via @ref #setValidChars().
 * The default key bindings of @ref QComboBox are still in effect.
 *
 * @short A line editor for restricted character sets.
 * @author Michael Wiedmann <mw@miwie.in-berlin.de>
 * @version 0.0.1
 */
class QRestrictedCombo : public QComboBox
{
  Q_OBJECT
  Q_PROPERTY( QString validChars READ validChars WRITE setValidChars )

public:

  /**
   * Constructor: This contructor takes three - optional - arguments.
   *  The first two parameters are simply passed on to @ref QLineEdit.
   *  @param parent   pointer to the parent widget
   *  @param name     pointer to the name of this widget
   *  @param valid    pointer to set of valid characters
   */
  QRestrictedCombo( QWidget *parent=0, const char *name=0,
		   const QString& valid = QString::null);

  /**
   * Destructs the restricted line editor.
   */
  ~QRestrictedCombo();

  /**
   * All characters in the string valid are treated as
   * acceptable characters.
   */
  void setValidChars(const QString& valid);
  /**
   * @return the string of acceptable characters.
   */
  QString validChars() const;

signals:

  /**
   * Emitted when an invalid character was typed.
   */
  void	invalidChar(int);

protected:
  /**
   * @reimplemented
   */
  void	keyPressEvent( QKeyEvent *e );

private:
  /// QString of valid characters for this line
  QString	qsValidChars;
};

#endif // QRESTRICTEDCOMBO_H

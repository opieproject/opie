/*
 * Letter Chooser Widget.
 *
 * (c) 2002 Mike Crawford
 *
 * This file is FREE SOFTWARE covered under the GUN General Public License.  
 *
 *
*/
#ifndef PICKER_H
#define PICKER_H

#include <qlabel.h>
#include <qframe.h>
#include <qevent.h>

class PickerLabel: public QLabel {
Q_OBJECT
	public:
		PickerLabel( QWidget *parent = 0, const char *name = 0 );
		~PickerLabel();

		void setLetters( char ch1, char ch2, char ch3 );
		void clearLetter();

	signals:

		void selectedLetter( char );
		void clearAll();

	protected:

		void mouseReleaseEvent( QMouseEvent *e );

	private:

		int currentLetter;
		static char lastLetter;
		char letter1, letter2, letter3;

	private slots:

		void emitClearSignal();

};
class LetterPicker: public QFrame {
Q_OBJECT
	public:
		LetterPicker( QWidget *parent = 0, const char *name = 0 );
		~LetterPicker();

	public slots:
		void clear();
	
	signals:
		void letterClicked( char );

	private:
		PickerLabel *lblABC, *lblDEF, *lblGHI, *lblJKL, *lblMNO, *lblPQR, *lblSTU, *lblVWX, *lblYZ;

	private slots:
		void newLetter( char letter );
};

#endif

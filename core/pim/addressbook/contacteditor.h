/*
 * Copyright (c) 2002 Michael R. Crawford <mike@tuxnami.org>
 *
 * This file is an add-on for the OPIE Palmtop Environment
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the pacakaging
 * of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 * This is a rewrite of the abeditor.h file, modified to provide a more
 * intuitive interface to TrollTech's original Address Book editor.  This
 * is made to operate exactly in interface with the exception of name.
 *
 */

#ifndef CONTACTEDITOR_H
#define CONTACTEDITOR_H

#include <opie/ocontact.h>

#include <qpe/datebookmonth.h>

#include <qdialog.h>
#include <qlist.h>
#include <qmap.h>
#include <qstringlist.h>

const int NAME_LF = 0;
const int NAME_LFM = 1;
const int NAME_FL = 2;
const int NAME_FMLS = 3;

const int NAME_F = 4;
const int NAME_M = 5;
const int NAME_L = 6;
const int NAME_S = 7;


class QScrollView;
class QTabWidget;
class QMultiLineEdit;
class QLineEdit;
class QComboBox;
class QPushButton;
class CategorySelect;
class QLabel;

class ContactEditor : public QDialog {
	Q_OBJECT

	public:
		ContactEditor(	const OContact &entry,
				QWidget *parent = 0,
				const char *name = 0,
				WFlags fl = 0 );
		~ContactEditor();
		void loadFields();
		void setNameFocus();
		void setPersonalView( bool personal = true );
		OContact entry() const { return ent; }

	public slots:
		void slotNote();
		void slotName();
		void setEntry(const OContact &entry);

	protected slots:
		void accept();

	private:
		void init();
		void initMap();
		void saveEntry();
		bool isEmpty();
		void cleanupFields();
		QString parseName( QString fullName, int type );
	private slots:
		void slotChooser1Change( const QString &textChanged );
		void slotChooser2Change( const QString &textChanged );
		void slotChooser3Change( const QString &textChanged );
		void slotChooser4Change( const QString &textChanged );
		void slotCmbChooser1Change( int index );
		void slotCmbChooser2Change( int index );
		void slotCmbChooser3Change( int index );
		void slotCmbChooser4Change( int index );
		void slotAddressTypeChange( int index );
		void slotAddressChange( const QString &textChanged );
		void slotAddress2Change( const QString &textChanged );
		void slotPOBoxChange( const QString &textChanged );
		void slotCityChange( const QString &textChanged );
		void slotStateChange( const QString &textChanged );
		void slotZipChange( const QString &textChanged );
		void slotCountryChange( const QString &textChanged );
		void slotFullNameChange( const QString &textChanged );
		void slotAnniversaryDateChanged( int year, int month, int day);
		void slotBirthdayDateChanged( int year, int month, int day);

	private:
		bool useFullName;
		//		bool hasGender, hasTitle, hasCompany, hasNotes, hasStreet, hasStreet2, hasPOBox, hasCity, hasState, hasZip, hasCountry;

		OContact ent;

		QDialog *dlgNote;
		QDialog *dlgName;

		QList<QLineEdit> listValue;
		QList<QLabel> listName;

		QStringList slDynamicEntries;
		QStringList trlDynamicEntries;

		bool m_personalView;

		QStringList slHomeAddress;
		QStringList slBusinessAddress;
		QStringList slChooserNames;
		QStringList slChooserValues;

		QMultiLineEdit *txtNote;
		QLabel *lblNote;

		//QLineEdit *txtTitle;
		QLineEdit *txtFirstName;
		QLineEdit *txtMiddleName;
		QLineEdit *txtLastName;
		QLineEdit *txtSuffix;

		QTabWidget *tabMain;
		QScrollView *svGeneral;
		QPushButton *btnFullName;
		QPushButton *btnNote;
		QLineEdit *txtFullName;
		QLineEdit *txtJobTitle;
		QLineEdit *txtOrganization;
		QLineEdit *txtChooserField1;
		QLineEdit *txtChooserField2;
		QLineEdit *txtChooserField3;
		QLineEdit *txtChooserField4;
		QComboBox *cmbChooserField1;
		QComboBox *cmbChooserField2;
		QComboBox *cmbChooserField3;
		QComboBox *cmbChooserField4;
		QComboBox *cmbFileAs;
		CategorySelect *cmbCat;
		QLabel *labCat;

		QScrollView *svAddress;
		QLineEdit *txtAddress;
		//QLineEdit *txtAddress2;
		//QLineEdit *txtPOBox;
		QLineEdit *txtCity;
		QLineEdit *txtState;
		QLineEdit *txtZip;
		QComboBox *cmbAddress;
		QComboBox *cmbCountry;

		QScrollView *svDetails;
		QComboBox *cmbGender;
		DateBookMonth* birthdayPicker;
		QToolButton* birthdayButton;
		DateBookMonth* anniversaryPicker;
		QToolButton* anniversaryButton;
	};

#endif

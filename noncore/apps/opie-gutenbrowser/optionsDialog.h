/***************************************************************************
                          optionsDialog.h  -  description
                             -------------------
    begin                : Tue Jul 25 2000
    copyright            : (C) 2000 -2004 by llornkcor
    email                : ljp@llornkcor.com
    *   This program is free software; you can redistribute it and/or modify  *
    *   it under the terms of the GNU General Public License as published by  *
    *   the Free Software Foundation; either version 2 of the License, or     *
    *   (at your option) any later version.                                   *
    *                                                                         *
    ***************************************************************************/
#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include "gutenbrowser.h"
#include "fontDialog.h"

#include <qvariant.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qpe/fontdatabase.h>
#include <qfont.h>

class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QCheckBox;
class QComboBox;
class QLabel;
class QListBox;
class QListBoxItem;
class QPushButton;
class QTabWidget;
class QWidget;

class optionsDialog : public QDialog
{ 
  Q_OBJECT

    public:
  optionsDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
  ~optionsDialog();

  QListBox* listBox,*http_ListBox1;
  QPushButton* buttonOk, *fontChangeButton,*PushButton_Browse;
  QTabWidget* tabWidget;
  QWidget *tab, *Widget3, * Widget2, *fontWidget;
  QCheckBox* Ftp_CheckBox,*Http_CheckBox;
  QString s_Wrap;

  QCheckBox* useWordWrap_CheckBox;
  QCheckBox* cb_queryExit;
  bool b_qExit;
  QLabel* TextLabel2,*TextLabel3;
  QListBox* ftp_QListBox_1;
  QPushButton* ftp_DownloadButton;
  QLabel* TextLabel3_3, *TextLabel4,*TextLabel5;
  QComboBox *ComboBox1 ;///* * ComboBoxStyle,*/ ;
  QLineEdit *downloadDirEdit;
  bool changedFonts;
  /////////////////////////////////////////////////////

  void initDialog();
  void initConnections();

  //    CConfigFile *config;
  QString ftp_base_dir, ftp_host, brow, local_library;

  // ftpsite
  bool useIcon;
  bool parseFtpList(QString outputFile);
  QString ftpSite,  ListFile, s_site,  host, base_dir, browserName;
//  int styleInt;
  // font dialog
  FontDialog *fontDlg; 
   
//  bool styleChanged;
 protected:
  void openSiteList();
  void doOptions();
  void ftpSiteDlg();
  void BrowserDlg();
  public slots:
    void getConfig();
  //    void writeConfig();
  //    void iconCheckB();
  void getSite();
  void select_site( const char *index );
  void getSelection(  QListBoxItem *item);
  //    void lineEditChanged(const QString &);
  private slots:
    void slotWordWrap(bool); 
  void slotQueryExit();
  void changeFonts();
  protected slots:
    //    virtual void OnOK();
    void listBoxMainSel( int index);
//  void setStyle( int index);
  void setHttp(int);
  //    virtual void OptionsFtpClicked();
  //    virtual void httpCheckB();
  //    virtual void FtpCheckB();
  //    virtual void slotUseSplit();
  //  virtual void goBrowser(const QString&);
  //    virtual void chooseBrowser();
  void select_title(int index);
  void BrowseSelected();

};

#endif // OPTIONSDIALOG_H

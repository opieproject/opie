// -*- c++ -*-

#ifndef KVNCCONNECTION_H
#define KVNCCONNECTION_H

#include "kvncconndlgbase.h"
#include "krfbserver.h"


class KVNCConnDlg : public KVNCConnDlgBase
{
		Q_OBJECT

		public:
				KVNCConnDlg(  KRFBServer *options,
								QWidget *parent = 0, char *name = 0, bool modal = true );
				~KVNCConnDlg();

		protected:
				void accept();
		private slots:
				void save();
				void showPassword(int);

		private:
				KRFBServer tmpOptions;
				KRFBServer * options;
};

#endif // KVNCCONNECTION_H





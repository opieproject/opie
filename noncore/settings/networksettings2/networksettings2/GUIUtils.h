#ifndef __GUIUTILS_H
#define __GUIUTILS_H

#include <qstring.h>

// if TEXT capable widget has changed
#define TXTM( Data, Wdg, FL ) \
      if( Wdg->text() != Data ) { \
        FL = 1; \
        Data = Wdg->text(); \
      }

// if IsChecked capable widget has changed
#define CBM(Data,Wdg,FL) \
    if( Wdg->isChecked() != Data) { \
      FL = 1; \
      Data = Wdg->isChecked(); \
    }

// if Value capable widget has changed
#define SBM(Data,Wdg,FL) \
    if( Wdg->value() != Data) { \
      FL = 1; \
      Data = Wdg->value(); \
    }

// if currentItem capable widget has changed
#define CIM(Data,Wdg,FL) \
    if( Wdg->currentItem() != Data) { \
      FL = 1; \
      Data = Wdg->currentItem(); \
    }

extern bool validIP( const QString & S );

#endif

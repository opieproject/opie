/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

/*
 * 01/14/2002 Charles-Edouard Ruault <ce@ruault.com>
 * Added support for Temperature conversions.
 */
// Sat 03-09-2002 L.J. Potter added the inlined pixmaps here

#include "calculatorimpl.h"

#include <qpe/resource.h>
#include <qpe/qmath.h>
#include <qpe/qpeapplication.h>

#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qfont.h>
#include <qlayout.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <math.h>
/* XPM */
static char *oneoverx_xpm[] = {
/* width height num_colors chars_per_pixel */
"    13    11        2            1",
/* colors */
". c None",
"# c #000000",
/* pixels */
"......#......",
".....##......",
"......#......"
".....###.....",
".............",
"..#########..",
".............",
"....##.##....",
"......#......",
"......#......",
"....##.##....",
};
/* XPM */
static char *ythrootofx_xpm[] = {
/* width height num_colors chars_per_pixel */
"    13    11        2            1",
/* colors */
". c None",
"# c #000000",
/* pixels */
"#.#..........",
"#.#..........",
"###...#######",
"..#..#.......",
"###..#.......",
".....#.#...#.",
".#..#...#.#..",
"#.#.#....#...",
"..#.#...#.#..",
"...#...#...#.",
"...#........."
};
/* XPM */
static char *xtopowerofy_xpm[] = {
/* width height num_colors chars_per_pixel */
"     9     8        2            1",
/* colors */
". c None",
"# c #000000",
/* pixels */
"......#.#",
"......#.#",
"......###",
"#...#...#",
".#.#..###",
"..#......",
".#.#.....",
"#...#...."
};

CalculatorImpl::CalculatorImpl( QWidget * parent, const char * name,
        WFlags f )
    : Calculator( parent, name, f )
{
//      xtopowerofy = Resource::loadPixmap("xtopowerofy");
//      ythrootofx = Resource::loadPixmap("ythrootofx");
//      oneoverx = Resource::loadPixmap("oneoverx");

    memMark = new QLabel( "m", LCD );
    memMark->setFont( QFont( "helvetica", 12, QFont::Bold, TRUE ) );
    memMark->resize( 12, 12 );
    memMark->move( 4, 2 );
    memMark->hide();
    mem = 0;

    PushButtonMR->setEnabled( FALSE );

    current_mode = max_mode = conversion_mode_count = 0;
    last_conversion = -1;

//bgr_command.insert( PushButtonFunction);
    bgr_command.insert( PushButtonMPlus);
    bgr_command.insert( PushButtonMR);
    bgr_command.insert( PushButtonMC);
    bgr_command.insert( PushButtonCE);
    connect( &bgr_command, SIGNAL(clicked(int) ), this, SLOT(command_buttons(int)));

    bgr_digits.insert(PushButton0);
    bgr_digits.insert(PushButton1);
    bgr_digits.insert(PushButton2);
    bgr_digits.insert(PushButton3);
    bgr_digits.insert(PushButton4);
    bgr_digits.insert(PushButton5);
    bgr_digits.insert(PushButton6);
    bgr_digits.insert(PushButton7);
    bgr_digits.insert(PushButton8);
    bgr_digits.insert(PushButton9);
    connect( &bgr_digits, SIGNAL(clicked(int) ), this, SLOT(enterNumber(int)));


    bgr_std.insert(PushButtonEquals);
    bgr_std.insert(PushButtonDecimal);
    bgr_std.insert(PushButtonAdd);
    bgr_std.insert(PushButtonMinus);
    bgr_std.insert(PushButtonDivide);
    bgr_std.insert(PushButtonTimes);
    connect( &bgr_std, SIGNAL(clicked(int) ), this, SLOT(std_buttons(int)));

// change the / to a proper division signal
    PushButtonDivide->setText(QChar(0xF7));

    func_buttons[0] = PushButtonF1;
    func_buttons[1] = PushButtonF2;
    func_buttons[2] = PushButtonF3;
    func_buttons[3] = PushButtonF4;
    func_buttons[4] = PushButtonF5;
    func_buttons[5] = PushButtonF6;
    func_buttons[6] = PushButtonF7;
    func_buttons[7] = PushButtonF8;
    func_buttons[8] = PushButtonF9;
    func_buttons[9] = PushButtonF10;
    func_buttons[10] = PushButtonF11;
    func_buttons[11] = PushButtonF12;

    for ( int x = 0 ; x < func_button_count ; x++ ) {
        QPushButton* tmpbutton = func_buttons[x];
        faces << tmpbutton->text();
        bgr_function.insert(tmpbutton);
    }
    connect( &bgr_function, SIGNAL(clicked(int) ) , this, SLOT(do_convert(int) ) );
    connect( &bgr_function, SIGNAL(clicked(int) ) , this, SLOT(std_funcs (int) ) );

    connect(ComboBoxFunction, SIGNAL(activated(int) ), this, SLOT(function_button(int) ) );

    captions.append(tr("Standard"));
    ComboBoxFunction->insertItem(captions.last());

    // now add in the conversion modes
    // when the menu gets done, these should be in a submenu
    QString tmp = QPEApplication::qpeDir();
    tmp += "etc/unit_conversion.dat";
    QFile myfile(tmp);
    if ( !myfile.open( IO_Translate | IO_ReadOnly ) ) {
        qDebug("Data file unit_conversion.dat not found\nNo conversion features will be available\n"+tmp);
        // disable the f button if no conv file available
        ComboBoxFunction->setEnabled(FALSE);
    }
    else {
        QString line, line2;
        QTextStream ts(&myfile);

        // first pass, see how many conversion types there are in order to allocate for them
        while ( ! ts.eof() ) {
            line = ts.readLine();
            if ( line.contains ("STARTTYPE" ) )
                conversion_mode_count++;
        }

        entry_list = new double[conversion_mode_count*func_button_count];
    preoffset_list = new double[conversion_mode_count*func_button_count];
    postoffset_list = new double[conversion_mode_count*func_button_count];
        myfile.close();
        myfile.open( IO_Translate | IO_ReadOnly );
        QTextStream ts2(&myfile);

        // second pass, read in values
        int x = 0;
        while ( ! ts2.eof() ) {
                line = ts2.readLine();
                if ( line.contains("STARTTYPE") ) {
                        captions << line.remove(0,10);
      ComboBoxFunction->insertItem(captions.last());
                        while ( !line.contains("ENDTYPE") ) {
                            line = ts2.readLine();
                            if ( line.contains("NAME") ) {
                                faces << line.remove(0,5);
                                line2 = ts2.readLine();
                                line2.remove(0,6);
                                entry_list[x] = line2.toDouble();
                line2 = ts2.readLine();
                                line2.remove(0,7);
                                preoffset_list[x] = line2.toDouble();
                line2 = ts2.readLine();
                                line2.remove(0,8);
                                postoffset_list[x] = line2.toDouble();
                                x++;
                            }
                        }
                }
        }
    }
    myfile.close();
    clear();
    max_mode = pre_conv_modes_count + conversion_mode_count + post_conv_modes_count - 1;
    display_pixmap_faces();

    qApp->installEventFilter( this );
}

bool CalculatorImpl::eventFilter( QObject *o, QEvent *e )
{
    if ( e->type() == QEvent::KeyPress && state != sError ) {
  QKeyEvent *k = (QKeyEvent*)e;
  if ( k->key() >= Key_0 && k->key() <= Key_9 ) {
      enterNumber( k->key() - Key_0 );
      return true;
  } else {
      switch ( k->key() ) {
    case Key_Equal:
        std_buttons(0);
        return true;
    case Key_Period:
        std_buttons(1);
        return true;
    case Key_Plus:
        std_buttons(2);
        return true;
    case Key_Minus:
        std_buttons(3);
        return true;
    case Key_Slash:
        std_buttons(4);
        return true;
    case Key_Asterisk:
        std_buttons(5);
        return true;
    case Key_Percent:
        execOp( oPercent );
        return true;
    case Key_ParenLeft:
        if ( current_mode <  pre_conv_modes_count )
      execOp( oOpenBrace );
        return true;
    case Key_ParenRight:
        if ( current_mode <  pre_conv_modes_count )
      execOp( oCloseBrace );
        return true;
    default:
        break;
      }
  }
    }
    return Calculator::eventFilter( o, e );
}

void CalculatorImpl::do_convert(int button) {
    if ( state == sError )
  return;
    if ( current_mode >= pre_conv_modes_count && current_mode <= (max_mode - post_conv_modes_count) &&
  button < changeable_func_button_count ) {
        if ( last_conversion > -1 ) {
            if( state == sNewNumber ){
        acc = (num+ preoffset_list[(current_mode - pre_conv_modes_count) * func_button_count + last_conversion])
                    / (entry_list[(current_mode - pre_conv_modes_count) * func_button_count + last_conversion])
                    * (entry_list[(current_mode - pre_conv_modes_count) * func_button_count + button])
        +postoffset_list[(current_mode - pre_conv_modes_count) * func_button_count + button];
    num = acc;
                LCD->display( acc );
            } else {
                state = sNewNumber;
                num = (num+ preoffset_list[(current_mode - pre_conv_modes_count) * func_button_count + last_conversion])
                    / (entry_list[(current_mode - pre_conv_modes_count) * func_button_count + last_conversion])
                    * (entry_list[(current_mode - pre_conv_modes_count) * func_button_count + button])
          + postoffset_list[(current_mode - pre_conv_modes_count) * func_button_count + button];;
                LCD->display( num );
                acc = num;
            }
        }
        last_conversion = button;
    }
}


void CalculatorImpl::function_button(int mode){
    if ( state == sError )
  clear();
    // dont need the next line when using a popup menu
    current_mode = mode;

    // reset the last conv
    last_conversion = -1;

    // set the caption
    this->setCaption( captions[current_mode] );

    reset_conv();

    for ( int x = 0 ; x < changeable_func_button_count ; x++ ) {
        QPushButton* tmpbutton = func_buttons[x];

         // if its a conversion , make it a toggle button
        if ( current_mode >= pre_conv_modes_count && current_mode <= (max_mode - post_conv_modes_count) ) {
            tmpbutton->setToggleButton(TRUE);
        } else {
            tmpbutton->setToggleButton(FALSE);
        }
        tmpbutton->setText( faces[current_mode * func_button_count + x] );
    }

    if ( current_mode == 0 ) display_pixmap_faces();

    if ( current_mode >= pre_conv_modes_count && current_mode <= (max_mode - post_conv_modes_count) ) {
            bgr_function.setExclusive(TRUE);
    } else {
            bgr_function.setExclusive(FALSE);
    }
}

void CalculatorImpl::display_pixmap_faces() {
    QPixmap image0( ( const char** ) xtopowerofy_xpm);
    QPushButton* tmpbutton = func_buttons[5];
    tmpbutton->setPixmap(image0);

    QPixmap image1( ( const char** ) ythrootofx_xpm);
    tmpbutton = func_buttons[6];
    tmpbutton->setPixmap(image1);

    QPixmap image2( ( const char** ) oneoverx_xpm);
    tmpbutton = func_buttons[3];
    tmpbutton->setPixmap(image2);
}

void CalculatorImpl::clear() {
    acc = num = 0;
    operationStack.clear();
    state = sStart;
    numDecimals = 0;
    numOpenBraces = 0;
    flPoint = FALSE;
    LCD->display( 0 );
    fake = QString::null;

    reset_conv();
}

void CalculatorImpl::reset_conv() {
    for ( int x = 0 ; x < changeable_func_button_count ; x++ ) {
        QPushButton* tmpbutton = func_buttons[x];

        // dont carry any selections into the next mode
        if ( tmpbutton->state() == QPushButton::On ) {
           tmpbutton->toggle();
        }
    }

    last_conversion = -1;
}

void CalculatorImpl::std_buttons(int button)
{
    if ( state == sError )
  return;
    execOp( (Operation)(button + oSum) );
}

void CalculatorImpl::std_funcs(int button) {
    if ( state == sError )
  return;
    if ( current_mode <  pre_conv_modes_count ||
  button > changeable_func_button_count-1 ) {
  Operation op;
  if ( button < 10 )
      op = (Operation)(button + oSin);
  else if ( button == 10 )
      op = oOpenBrace;
  else
      op = oCloseBrace;
        execOp( op );
    }
}

void CalculatorImpl::execOp( Operation i )
{
    switch (i) {
      // these operators only affect the current number.
  case oDivX:
  case oLog:
  case oLn:
  case oSin:
  case oCos:
  case oTan:
      num = evalExpr(i);
      break;

  case oAdd:
  case oSub: {
      processStack( oAdd );
      Op op( num, i );
      operationStack.push( op );
      break;
  }
  case oDiv:
  case oMult:
  case oRoot:
  case oXsquared: {
      processStack( oDiv );
      Op op( num, i );
      operationStack.push( op );
      break;
  }
  case oChSign:
      num = -num;
      LCD->display(num);
      return;

  case oOpenBrace: {
      Op op( 0, oOpenBrace );
      operationStack.push( op );
      numOpenBraces++;
      state = sNewNumber;
      return;
  }
  case oCloseBrace: {
      if ( numOpenBraces == 0 )
    return;
      processStack( oAdd );
      if ( operationStack.top().operation != oOpenBrace )
    qDebug( "Calculator: internal Error" );
      operationStack.pop();
      state = sNewNumber;
      numOpenBraces--;
      break;
  }

  case oPoint:
      flPoint = TRUE;
      return;

  case oPercent:
      processStack( oPercent );
      break;


  case oSum:
      processStack( oSum );
      break;

  default:
      return;
    };

    if ( state == sError ) {
  LCD->display( "Error" );
  return;
    } else {
  LCD->display(num);
    }
    state  = sNewNumber;
    numDecimals = 0;
    flPoint = FALSE;
}


void CalculatorImpl::processStack( int op )
{
    //dubious percent hack, since the changeable operator precedences are
    //pretty much hardwired to be less than the non-changeable
    bool percent = FALSE;
    if ( op == oPercent ) {
  percent = TRUE;
  op = oSum;
    }
    while( !operationStack.isEmpty() && operationStack.top().operation >= op ) {
  Op operation = operationStack.pop();
  acc = operation.number;
  if ( percent ) {
      if ( operation.operation == oAdd || operation.operation == oSub )
    num = acc*num/100;
      else
    num = num / 100;
  }
  num = evalExpr( operation.operation );
  percent = FALSE;
    }
}


double CalculatorImpl::evalExpr( int op ) {
    double sum = 0;

    switch( op ){
        case oPercent: sum = num / 100.; break;
        case oDivX:
      if (num == 0)
    state = sError;
      else
    sum = 1 / num;
      break;
        case oXsquared:
      sum = pow(acc,num);
      break;
        case oChSign: (state == sStart) ? sum = -num : sum = -acc; break;
      case oSub:  sum = acc - num; break;
      case oMult: sum = acc * num; break;
      case oAdd:  sum = acc + num; break;
        case oDiv:  {
      if (num == 0) {
    state = sError;
      } else {
    sum = acc / num;
      }
      break;
  }
        case oRoot:
      /* the linux library is dumb, and can't to -x to 1/n
         when n is odd.  (even and error of course is acceptable */
            if((acc < 0) && (int(num) == num) && (int(num) % 2 == 1 )) {
    sum = pow(-acc, 1 / num);
    sum = -sum;
      } else {
    sum = pow(acc, 1 / num);
      }
      break;
        case oLog:
      sum = log10(num);
      break;
        case oLn:
      sum = log(num);
      break;
        case oTan: sum = qTan(num);break;
        case oSin: sum = qSin(num);break;
        case oCos: sum = qCos(num);break;
      default: sum = num; break;
    }

    if ( isinf( sum ) || isnan( sum ) )
  state = sError;
    return sum;
}


void CalculatorImpl::enterNumber( int n )
{
    if ( state == sError )
  return;
    if( state == sStart ){
  if( LCD->value() > 0 ){
      QString s = QString::number( LCD->value(), 'g', LCD->numDigits());
      if( s.length() > (uint)(LCD->numDigits() - 2)) return;

  } else if( (int)fake.length() >= LCD->numDigits() || numDecimals >=12 ){
      return;
  }
    }

    if( state == sNewNumber ){
      state = sStart;
      acc = 0;
  if( flPoint ){
      numDecimals = 1;
      num = n / pow(10, numDecimals);
  } else
      num = n;
    } else if( flPoint ){
  numDecimals++;
  if( num < 0 ){
      num -= n / pow(10, numDecimals);
  } else {
      num += n / pow(10, numDecimals);
  }
    } else {
  num *= 10;
  if( num  < 0 )
      num -= n;
  else
      num += n;
    }

    // We need feedback in the calc display while entering fl.point zeros.
    // This is a small hack to display sequences like: 0.000 and 1.100
    double integer, fraction;
    fraction = modf( num, &integer );
    if( flPoint ){
  QString is, fs, zeros;

  is = QString::number( integer, 'g', 13 );
  fs = QString::number( fraction, 'g', numDecimals );
  if( fs.contains('e') ){
      fs = QString::number( fraction, 'f', LCD->numDigits() );
  }
  fs = fs.mid( 2, numDecimals );

  if( (integer == 0) && (fraction == 0) )
      fake = "0.";
  else if( (integer != 0) && (fraction == 0) )
      fake = is + ".";
  else
      fake = is + "." + fs;

  zeros.fill( '0', (numDecimals - fs.length()) );
  fake += zeros;
  // ### This code sets LCD->value() to zero since it sets a text
  // ### Avoid getting the current value from LCD->value() for
  // ### calculations.
  LCD->display( fake );
    } else
  LCD->display( num );
}

void CalculatorImpl::command_buttons(int i) {
    if ( state == sError && i != 3 )
  return;
    switch (i) {
    case 0:  // M+
      mem += num;
        if( mem != 0 ){
          memMark->show();
          PushButtonMR->setEnabled( TRUE ); };
  state = sNewNumber;
         break;
    case 1: // MR
  acc = num = mem;
  state = sNewNumber;
        LCD->display( mem );
        break;
    case 2: // MC
        mem = 0;
        memMark->hide();
        PushButtonMR->setEnabled( FALSE );
        break;
    case 3: // CE
  if ( state == sStart ) {
      // clear the entered number on the first press
      state = sNewNumber;
      num = acc = 0;
      flPoint = FALSE;
      LCD->display( 0 );
      fake = QString::null;
      numDecimals = 0;
  } else {
      clear();
  }
        break;
    };

}

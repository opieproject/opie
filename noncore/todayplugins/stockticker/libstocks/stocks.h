/* libstocks - Library to get current stock quotes from Yahoo Finance
 *
 * Copyright (C) 2000 Eric Laeuffer
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
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __STOCKS_H__
#define __STOCKS_H__

/* Defines for prototypes */
#ifndef __LISTS_C__
#define _LISTS_C_EXT extern
#else
#define _LISTS_C_EXT
#endif /* __LISTS_C__ */

#ifndef __STOCKS_C__
#define _STOCKS_C_EXT extern
#else
#define _STOCKS_C_EXT
#endif /* __STOCKS_C__ */

#ifndef __HTTP_C__
#define _HTTP_C_EXT extern
#else
#define _HTTP_C_EXT
#endif /* __HTTP_C__ */

#ifndef __CURRENCY_C__
#define _CURRENCY_C_EXT extern
#else
#define _CURRENCY_C_EXT
#endif /* __CURRENCY_C__ */

#ifndef __HISTORY_C__
#define _HISTORY_C_EXT extern
#else
#define _HISTORY_C_EXT
#endif /* __HISTORY_C__ */

typedef struct stockstruct stock;

struct stockstruct {
  char  *Symbol;
  char  *Name;
  char  *Time;
  char  *Date;
  float CurrentPrice;
  float LastPrice;
  float OpenPrice;
  float MinPrice;
  float MaxPrice;
  float Variation;
  float Pourcentage;
  int   Volume;
  stock *PreviousStock;
  stock *NextStock;
};

/* Errors generates by the library */
typedef enum {
  NOERR   = 0,  /* No error */
  ERRHOST = 1,  /* No such host */
  ERRSOCK = 2,  /* Can't create socket */
  ERRCONN = 3,  /* Can't connect to host */
  ERRWHEA = 4,  /* Write error on socket while writing */
  ERRRHEA = 5,  /* Cannot find header in http response */
  ERRPAHD = 7,  /* Invalid answer from data server */
  ERRPCSV = 8,  /* Error in parsing csv file */
  ERRPROX = 20, /* Bad proxy url */
  ERRDATE = 30, /* Bad date format */
  ERRDATA = 40, /* No data available */
  ERRRANG = 50  /* No prices for this date range */

} libstocks_return_code;

_LISTS_C_EXT  stock *next_stock(stock *);
_LISTS_C_EXT  stock *previous_stock(stock *);
_LISTS_C_EXT  void free_stocks(stock *);
_LISTS_C_EXT  stock *find_stock(stock *stocks, char *);

_STOCKS_C_EXT libstocks_return_code get_stocks(const char *, stock **);

_HTTP_C_EXT   libstocks_return_code set_proxy(char *);


_CURRENCY_C_EXT libstocks_return_code get_currency_exchange(char *, 
                  char *, 
                  float *);

_HISTORY_C_EXT libstocks_return_code get_history_csv(char *, 
                 char *, 
                 char *, 
                 char **);

_HISTORY_C_EXT libstocks_return_code get_stock_history(char *, 
                   char *, 
                   char *, 
                   stock **);


#endif /* __STOCKS_H__ */

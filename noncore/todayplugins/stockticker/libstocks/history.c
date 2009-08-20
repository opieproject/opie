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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#define __HISTORY_C__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __WINDOWS__
#include <malloc.h>
#endif

#include "stocks.h"
#include "http.h"
#include "csv.h"

const char history_stocks_server[]="chart.yahoo.com";
const char url_history_beg[]="/table.csv?s=";

/*****************************************************************************/
/* Gets the stock history csv file and return in csv_file the result.        */
/* The stock is defined by its symbol.                                       */
/* The history file starts at date1 and stops at date2.                      */
/*****************************************************************************/
libstocks_return_code get_history_csv(char *symbol,
				      char *date1,
				      char *date2,
				      char **csv_file)
{
  char *data;
  char url[80];

  char adate[12];

  char *y1, *y2;
  char *m1, *m2;
  char *d1, *d2;

  libstocks_return_code error;

  char *test;

  int ch_printed = snprintf(url, 80, "%s%s", url_history_beg, symbol);
  if (ch_printed < 0 || ch_printed == 80)
      return ERRURL;

  if (strlen(date1) > 11) return ERRDATE;
  strncpy(adate, date1, 11);
  if (strlen(date1) > 10)
      adate[11] = '\0';

  y1=strtok(adate, "/");
  if(!y1) return ERRDATE;
  m1 = strtok(NULL, "/");
  if(!m1) return ERRDATE;
  d1 = strtok(NULL, "/");
  if(!d1) return ERRDATE;

  ch_printed += snprintf(&(url[ch_printed]), 80 - ch_printed, "%s%s%s%s%s%s",
			 "&a=", m1, "&b=", d1, "&c=", y1);
  if (ch_printed < 0 || ch_printed == 80)
      return ERRURL;

  if (strlen(date2) > 11) return ERRDATE;
  strncpy(adate, date2, 11);
  if (strlen(date2) > 10)
      adate[11] = '\0';

  y2=strtok(adate, "/");
  if(!y2) return ERRDATE;
  m2 = strtok(NULL, "/");
  if(!m2) return ERRDATE;
  d2 = strtok(NULL, "/");
  if(!d2) return ERRDATE;

  ch_printed += snprintf(&(url[ch_printed]), 80 - ch_printed, "%s%s%s%s%s%s%s",
			 "&d=", m2, "&e=", d2, "&f=", y2, "&g=d&q=q&y=0&x=.csv");

  if (ch_printed < 0)
      return ERRURL;

  error=http_get(url, (char *)history_stocks_server, &data);
  if (error) return error;

  /* tests if data is valid */

  /* The server returns "No data available for symbol" */
  /* when the stock is not available */
  test = strstr(data, "No data available");
  if (test)
    {
      free(data);
      return ERRDATA;
    }

  /* The server return "No Prices in this date range" */
  /* when dates have not the good range */
  test = strstr(data, "No Prices in this date range");
  if (test)
    {
      free(data);
      return ERRRANG;
    }

  *csv_file = data;

  return(0);
}

/*****************************************************************************/
/* Gets the stock history and return a stock structure list.                 */
/* The stock is defined by its symbol.                                       */
/* The history file starts at date1 and stops at date2.                      */
/*****************************************************************************/
libstocks_return_code get_stock_history(char *symbol,
				        char *date1,
				        char *date2,
				        stock **stock_history)
{
  libstocks_return_code error;
  char *csv_file;
  stock *stock_parsed;

  error = get_history_csv(symbol, date1, date2, &csv_file);
  if (error) return error;

  stock_parsed = parse_csv_history_file(csv_file);

  free(csv_file);

  if (!stock_parsed) return ERRPCSV;
  *stock_history = stock_parsed;

  return(0);
}

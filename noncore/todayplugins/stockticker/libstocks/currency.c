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

#define __CURRENCY_C__


#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#include "stocks.h"

/*****************************************************************************/
/* returns the currency exchange rate of "from" currency into                */
/* "into" currency.                                                          */
/*****************************************************************************/
libstocks_return_code get_currency_exchange(char *from, 
					    char *into, 
					    float *exchange)
{
  char *symbol;
  stock *data;
  libstocks_return_code error;

  if((symbol = (char *)malloc(strlen(from)+strlen(into)+3))==NULL)
    {
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
	      ,__FILE__, __LINE__);
      exit(1);
    }

  strcpy(symbol, from);
  strcat(symbol, into);
  strcat(symbol, "=X");
  
  error = get_stocks(symbol, &data);
  if (error) 
    {
      *exchange = 0;
      return(error);
    }

  free_stocks(data);

  *exchange = data->CurrentPrice;
  return(error);
 
}

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

#define __LISTS_C__

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#ifdef __WINDOWS__
#include <stdlib.h>
#endif

#include "stocks.h"
#include "lists.h"

/******************************************************************************/
/* Finds a stock in the stocks list and return the stock pointer.             */
/* *stocks points to the stocks list                                          */
/* *stock_to_find is the stock to find :))                                    */
/******************************************************************************/
stock *find_stock(stock *stocks, char *stock_to_find)
{
  char *test=NULL;

  while(stocks != NULL)
    {
      test = strstr(stocks->Symbol, stock_to_find);
      if(test) return(stocks);
      stocks = stocks->NextStock;
    }

  return(0);
}

/******************************************************************************/
/* Allocate memory for a stock structure and return the pointer of this       */
/* structure.                                                                 */
/******************************************************************************/
stock *malloc_stock(void)
{
  stock *stockptr;

  if((stockptr = (stock *)malloc(sizeof(stock)))==NULL)
    {
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
	      ,__FILE__, __LINE__);
      exit(1);
    }

  stockptr->Symbol=NULL;
  stockptr->Name=NULL;
  stockptr->Time=NULL;
  stockptr->Date=NULL;  
  stockptr->CurrentPrice=0;
  stockptr->LastPrice=0;
  stockptr->OpenPrice=0;
  stockptr->MinPrice=0;
  stockptr->MaxPrice=0;
  stockptr->Variation=0;
  stockptr->Pourcentage=0;
  stockptr->Volume=0;
  stockptr->PreviousStock=0;
  stockptr->NextStock=0;
  
  return(stockptr);
}

/******************************************************************************/
/* Deallocates a stock structure.                                             */
/* *stocks_to_free is the pointer of the structure to free                    */
/******************************************************************************/
void free_stocks(stock *stock_to_free)
{
  stock *next_stock;
  
  while(stock_to_free)
    {    
      next_stock = stock_to_free->NextStock;
      free(stock_to_free->Symbol);
      free(stock_to_free->Name);
      free(stock_to_free->Time);
      free(stock_to_free->Date);
      free(stock_to_free);
      stock_to_free = next_stock;
    }
}

/******************************************************************************/
/* Returns the next stock structure from the list.                            */
/* *the_stocks points to the list.                                            */
/******************************************************************************/
stock *next_stock(stock *the_stock)
{
  return((stock *)(the_stock->NextStock));
}

/******************************************************************************/
/* Returns the previous stock structure from the list.                        */
/* *the_stocks points to the list.                                            */
/******************************************************************************/
stock *previous_stock(stock *the_stock)
{
  return((stock *)(the_stock->PreviousStock));
}


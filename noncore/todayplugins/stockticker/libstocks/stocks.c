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

#define __STOCKS_C__

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#ifdef __WINDOWS__
#include <mbstring.h>
#endif

#include "http.h"
#include "csv.h"

#include "stocks.h"

/*
s  = symbol
n  = name
l1 = last trade
d1 = date
t1 = time
c1 = variation
o  = open
h  = higher price
g  = lower price
v  = volume
*/

const char yahoo_us_stocks_server[]="finance.yahoo.com";
const char yahoo_eu_stocks_server[]="finance.yahoo.com";
//const char yahoo_eu_stocks_server[]="fr.finance.yahoo.com";

const char yahoo_url_beg[]="/d/quotes.csv?s=";
const char yahoo_url_end[]="&f=snl1d1t1c1ohgv&e=.csv";

typedef enum {
  YAHOO_EUROPE,
  YAHOO_US
} yahoo_source;

#define YAHOO_US_EXT_NB  11
const char *yahoo_us_ext[YAHOO_US_EXT_NB] =
{
  ".US",   /* United States */
  ".TO",   /* Canada */
  ".M",    /* Canada */
  ".V",    /* Canada */
  ".AL",   /* Canada */
  ".MX",   /* Mexico */
  ".SA",   /* Brasil */
  ".BA",   /* Argentina */
  ".CR",   /* Venezuela */
  ".SN",   /* Chili */
  ".AX"    /* Australia */
};

/*****************************************************************************/
/* Finds, according to the symbol extension, the http source of the quotes.  */
/* Actually just finance.yahoo.com and fr.finance.yahoo.com are supported.   */
/* The function returns the country source (US or EUROPE).                   */
/*****************************************************************************/
yahoo_source find_yahoo_source(char *symbol)
{
  char *ptr;
  int i;
  int test;

  ptr = strrchr(symbol, '.');

  /* If no extension we suppose it is a US stock */
  if (!ptr) return YAHOO_US;

  /* extension is found */

  /* Test if it is canadian stock */
  for (i=0; i<YAHOO_US_EXT_NB; i++)
    {

#ifdef __UNIX__
      test = strcasecmp(yahoo_us_ext[i], ptr);
#elif __WINDOWS__
      test = _mbsnbicmp(yahoo_us_ext[i], ptr, strlen(yahoo_us_ext[i]));
#endif

      if (!test) return YAHOO_US;
    }
  
  /* We suppose now it is a European stock */
  return YAHOO_EUROPE;
}

/*****************************************************************************/
/* Gets quotes csv file, parses the file and create the quotes list          */
/* *stocks points to the stocks to fetch                                     */
/* *stock_datas points to the beginning of the list                          */
/* count allows to connect to all country servers                            */
/*****************************************************************************/
libstocks_return_code download_stocks(char *stocks, 
              stock **stock_datas, 
              yahoo_source source)
{
  char *stocks_server=NULL;
  char *url_beg=NULL;
  char *url_end=NULL;

  char *url;
  char *data;

  libstocks_return_code error;

#ifdef DEBUG
  printf("*download_stocks\n");
#endif

  switch (source)
    {
    case YAHOO_US:
      stocks_server = (char *)yahoo_us_stocks_server;
      break;
      
    case YAHOO_EUROPE:
      stocks_server = (char *)yahoo_eu_stocks_server;
      break;
      default:
      stocks_server = (char *)yahoo_us_stocks_server;
    break;
          
    }

  url_beg = (char *)yahoo_url_beg;
  url_end = (char *)yahoo_url_end;

  url = (char *)malloc(strlen(url_beg)
           +strlen(url_end)
           +strlen(stocks)+1);
  if (url==NULL)
    {
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
        ,__FILE__, __LINE__);
      exit(1);
    }

  strcpy(url, url_beg);
  strcat(url, stocks);
  strcat(url, url_end);
  
  error=http_get(url, stocks_server, &data);
 
  free(url);

  if (error) return error;
  
  *stock_datas = parse_csv_file(data);

  free(data);

  if (!(*stock_datas)) return ERRPCSV;

  return 0;

}

/*****************************************************************************/
/* Gets quotes from yahoo                                                    */
/* Choses to fetch European or US depending on the symbol extension          */
/* and merges the two lists to one                                           */
/* *stocks points to the stocks to fetch                                     */
/* *stock_datas points to the beginning of the list                          */
/*****************************************************************************/
libstocks_return_code get_stocks(const char *stocks, stock **stock_datas)
{
  char *tok_ptr;
  char *eu_quotes=NULL;
  char *eu_quotes_temp=NULL;
  int lgr_eu_quotes=0;

  char *us_quotes=NULL;
  char *us_quotes_temp=NULL;
  int lgr_us_quotes=0;

  char *symbol;

  yahoo_source source;

  int lgr_symbol=0;

  libstocks_return_code error;

  stock *stocks_tmp=NULL;
  stock *stocks_tmp2=NULL;
  stock *stocks_getted=NULL;
  stock *last_stock=NULL;

#ifdef DEBUG
  printf("*get_stocks\n");
#endif

  /* to preserve stocks */
  tok_ptr = malloc(strlen(stocks)+1);
  if(tok_ptr==NULL)
    {
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
        ,__FILE__, __LINE__);
      exit(1);  
    }
  strcpy(tok_ptr, stocks);

  while( (symbol = strtok(tok_ptr, "+"))!=0)
    {
      /* clear tok_ptr for next strtok */
      tok_ptr = NULL;

      /* look for "." in the symbol */
      source = find_yahoo_source(symbol);

      switch (source)
  {
  case YAHOO_US:

    if (us_quotes) 
      {
        lgr_us_quotes = strlen(us_quotes);
        lgr_symbol = strlen(symbol);

        us_quotes_temp = malloc(lgr_us_quotes + lgr_symbol +2);
        if(us_quotes_temp==NULL)
    {
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
        ,__FILE__, __LINE__);
      exit(1);  
    }
        strcpy(us_quotes_temp, us_quotes);
        strcat(us_quotes_temp,"+");
        strcat(us_quotes_temp,symbol);

        free(us_quotes);
        us_quotes = us_quotes_temp; 
      }       
    else 
      {
        us_quotes = malloc(strlen(symbol)+1);

        if(us_quotes==NULL)
    {
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
        ,__FILE__, __LINE__);
      exit(1);  
    }
        strcpy(us_quotes, symbol);
      }

    break;

  case YAHOO_EUROPE:

    if (eu_quotes) 
      {
        lgr_eu_quotes = strlen(eu_quotes);
        lgr_symbol = strlen(symbol);
        
        eu_quotes_temp = malloc(lgr_eu_quotes + lgr_symbol +2);
        if(eu_quotes_temp==NULL)
    {
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
        ,__FILE__, __LINE__);
      exit(1);  
    }
        strcpy(eu_quotes_temp, eu_quotes);
        strcat(eu_quotes_temp, "+");
        strcat(eu_quotes_temp, symbol);

        free(eu_quotes);
        eu_quotes = eu_quotes_temp; 
      }       
    else 
      {
        eu_quotes = malloc(strlen(symbol)+1);
        if(eu_quotes==NULL)
    {
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
        ,__FILE__, __LINE__);
      exit(1);  
    }
        strcpy(eu_quotes, symbol);
      }
    break;
  }
    }

  free(tok_ptr);

  if (us_quotes)
    {
      /* Gets us quotes */
      error = download_stocks(us_quotes, &stocks_tmp, YAHOO_US);
      if (error) return error;
    }
 
  if (eu_quotes)
    {
      /* Gets european quotes */
      error = download_stocks(eu_quotes, &stocks_getted, YAHOO_EUROPE);
      if (error) return error;

      /* concats lists if needed */
      if (stocks_tmp)
  {
    stocks_tmp2 = stocks_tmp;

    while(stocks_tmp2 != NULL) 
      {
        last_stock = stocks_tmp2;
        stocks_tmp2 = next_stock(stocks_tmp2);
      }

    last_stock->NextStock = stocks_getted;
    stocks_getted->PreviousStock = last_stock;

  }
      else (stocks_tmp = stocks_getted);      
    }

  *stock_datas = stocks_tmp; 
  
  return(0);
}

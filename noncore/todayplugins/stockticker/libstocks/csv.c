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

#define __CSV_C__
#ifdef __UNIX__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __WINDOWS__
#include <mbstring.h>
#endif

#include "csv.h"
#include "stocks.h"
#include "lists.h"

#define DATE_LENGTH 7    /*YYMMDD*/

const char *months[12]=
{
  "Jan",
  "Feb",
  "Mar",
  "Apr",
  "May",
  "Jun",
  "Jul",
  "Aug",
  "Sep",
  "Oct",
  "Nov",
  "Dec"
};

/*****************************************************************************/
/* Replacement of the strtok function. This one forgets "delim" when it is   */
/* between two commas.                                                       */
/* Thanks to Julio Lucas who has told me the bug and has proposed me a patch */
/*****************************************************************************/
char *csv_strtok(char *s, char *delim) 
{
   static char *next=NULL;
   char *temp, *first;
   int comma=0;

   if (s!=NULL) first=s;
   else first=next;
   
   temp=first;
   if (*temp=='\0') return NULL;

   while (*temp!='\0' && ((*temp!=*delim) || comma))
     {
       if (*temp=='"') comma ^= 1;
       temp++;
     }

   if (*temp=='\0') next=temp;
   else 
     {
       *temp='\0';
       next=temp+1;
     }

   return first;
}

/*****************************************************************************/
/* Parses the csv file and return a list of stocks structure.                */
/* *csv points on the csv file in memory                                     */
/* count defines the country, because csv depends on the country             */
/*****************************************************************************/
stock *parse_csv_file(char *csv)
{
  char *line;
  char *end_line;

  char *ptr;

  char *date;
  char *time;
  char *name;
  char *symbol;

  stock *StockPtr=NULL;
  stock *LastStockPtr=NULL;

  /* Used to return the pointer to the list */
  stock *FirstStockPtr=NULL;

  /* used to see if symbol is valid */
  int valid;
  char *test;

  line = csv;
  end_line = csv;

  while ((end_line = strstr(line, "\n")))
    {
      *end_line = 0;

      /* Check if symbol valid */
      /* if 1 "N/A" then ok because Indices have N/A for volume */
      /* if 4 "N/A" then ok because Mutual funds have */
      /* if 5 "N/A" then ok because currencies have */
      /* So if >5 then stock not valid */

      test = line;
      valid = 0;
      while ( (test = strstr(test, "N/A")) )
  {
    valid ++;
    test = test +3;
  }
      
      if (valid < 6)
  {
    /* This Symbol is valid */

    StockPtr = malloc_stock();
    
    ptr = csv_strtok(line, ",");
    if (!ptr) return 0;

    symbol = (char *)malloc(strlen(ptr)+1);
    if (symbol==NULL)
      {
        fprintf(stderr,"Memory allocating error (%s line %d)\n"
          ,__FILE__, __LINE__);
        exit(1);
      }
    strcpy((char *)(symbol), ptr);
    StockPtr->Symbol = symbol;

    ptr = csv_strtok(NULL, ",");
    if (!ptr) return 0;

    name = (char *)malloc(strlen(ptr)+1);
    if (name==NULL)
      {
        fprintf(stderr,"Memory allocating error (%s line %d)\n"
          ,__FILE__, __LINE__);
        exit(1);
      }
    strcpy((char *)(name), ptr);
    StockPtr->Name = name;

    ptr = csv_strtok(NULL, ",");
    if (!ptr) return 0;
    sscanf(ptr,"%f",&(StockPtr->CurrentPrice));

    ptr = csv_strtok(NULL, ",");
    if (!ptr) return 0;

    date = (char *)malloc(strlen(ptr)+1);
    if (date==NULL)
      {
        fprintf(stderr,"Memory allocating error (%s line %d)\n"
          ,__FILE__, __LINE__);
        exit(1);
      }
    strcpy((char *)(date), ptr);
    StockPtr->Date = date;

    ptr = csv_strtok(NULL, ",");
    if (!ptr) return 0;

    time = (char *)malloc(strlen(ptr)+1);
    if (time==NULL)
      {
        fprintf(stderr,"Memory allocating error (%s line %d)\n"
          ,__FILE__, __LINE__);
        exit(1);
      }
    strcpy((char *)(time), ptr);
    StockPtr->Time = time;
    
    ptr = csv_strtok(NULL, ",");
    if (!ptr) return 0;
    sscanf(ptr,"%f",&(StockPtr->Variation));      
    
    StockPtr->Pourcentage = 100 * StockPtr->Variation /
      (StockPtr->CurrentPrice - StockPtr->Variation);

    StockPtr->LastPrice = StockPtr->CurrentPrice - StockPtr->Variation;

    ptr = csv_strtok(NULL, ",");
    if (!ptr) return 0;
    sscanf(ptr,"%f",&(StockPtr->OpenPrice));      

    ptr = csv_strtok(NULL, ",");
    if (!ptr) return 0;
    sscanf(ptr,"%f",&(StockPtr->MaxPrice));      

    ptr = csv_strtok(NULL, ",");
    if (!ptr) return 0;
    sscanf(ptr,"%f",&(StockPtr->MinPrice));      
  
    ptr = csv_strtok(NULL, ",");
    if (!ptr) return 0;
    StockPtr->Volume = atoi(ptr);

    if( !FirstStockPtr ) 
      {
        FirstStockPtr = StockPtr;
        StockPtr->PreviousStock = 0;
      }
    
    StockPtr->NextStock = 0;
  
    if (LastStockPtr) 
      {
        LastStockPtr->NextStock = StockPtr;
        StockPtr->PreviousStock = LastStockPtr;
      }
    
    LastStockPtr = StockPtr;
    
  }
      else
  {
    /* this symbol is not valid */
    /* Set the stock struct just with Symbol, all other are NULL */
    /* This can be used to see if the symbol has been reached are not */

    StockPtr = malloc_stock();
    
    ptr = csv_strtok(line, ",");
    if (!ptr) return 0;

    symbol = (char *)malloc(strlen(ptr)+1);
    if (symbol==NULL)
      {
        fprintf(stderr,"Memory allocating error (%s line %d)\n"
          ,__FILE__, __LINE__);
        exit(1);
      }
    strcpy((char *)(symbol), ptr);
    StockPtr->Symbol = symbol;

    if( !FirstStockPtr ) 
      {
        FirstStockPtr = StockPtr;
        StockPtr->PreviousStock = 0;
      }
    
    StockPtr->NextStock = 0;
  
    if (LastStockPtr) 
      {
        LastStockPtr->NextStock = StockPtr;
        StockPtr->PreviousStock = LastStockPtr;
      }
    
    LastStockPtr = StockPtr;
  }

      end_line++;
      line = end_line; 
      
    }
    
  return (FirstStockPtr);
}

/*****************************************************************************/
/* Parses the history quotes file and return a stock structure list.         */
/*****************************************************************************/
stock *parse_csv_history_file(char *csv_file)
{

  char *line;
  char *end_line;
  char *ptr;

  int day;
  char smonth[10];
  int month;
  int year;

  char *date;

  int i;
  int test;

  stock *StockPtr=NULL;
  stock *LastStockPtr=NULL;

  /* Used to return the pointer to the list */
  stock *FirstStockPtr=NULL;

  line = csv_file;
  end_line = csv_file;

  /* do not use the first line */
  end_line = strstr(line, "\n");
  *end_line = 0;
  end_line++;
  line = end_line;

  while ((end_line = strstr(line, "\n")))
    {
      *end_line = 0;

      StockPtr = malloc_stock();
    
      /* Date */
      ptr = strtok(line, ",");
      if (!ptr) return 0;

      sscanf(ptr,"%d-%3s-%d",&day,smonth,&year);

      i=0;

#ifdef __UNIX__
      while((test=strcasecmp(months[i], smonth))) i++;
#elif __WINDOWS__
      while(test=_mbsnbicmp(months[i], smonth, strlen(months[i]))) i++;
#endif

      month = i+1;

      date = (char *)malloc(DATE_LENGTH);
      if (date==NULL)
  {
    fprintf(stderr,"Memory allocating error (%s line %d)\n"
      ,__FILE__, __LINE__);
    exit(1);
  }
      sprintf(date,"%.2d%.2d%.2d", year, month, day);
      StockPtr->Date = date;    
      
      /* Open */
      ptr = strtok(NULL, ",");
      if (!ptr) return 0;
      sscanf(ptr,"%f",&(StockPtr->OpenPrice));      

      /* High */
      ptr = strtok(NULL, ",");
      if (!ptr) return 0;
      sscanf(ptr,"%f",&(StockPtr->MaxPrice));

      /* Low */
      ptr = strtok(NULL, ",");
      if (!ptr) return 0;
      sscanf(ptr,"%f",&(StockPtr->MinPrice));

      /* Close */
      ptr = strtok(NULL, ",");
      if (!ptr) return 0;
      sscanf(ptr,"%f",&(StockPtr->LastPrice));

      /* Volume */

      ptr = strtok(NULL, ",");
      if (!ptr) 
  /* It seems to be an indice */
  /* No volume for indices */
  StockPtr->Volume = 0;
      else
  StockPtr->Volume = atoi(ptr);

      if( !FirstStockPtr ) 
  {
    FirstStockPtr = StockPtr;
    StockPtr->PreviousStock = 0;
  }
      
      StockPtr->NextStock = 0;
      
      if (LastStockPtr) 
  {
    LastStockPtr->NextStock = StockPtr;
    StockPtr->PreviousStock = LastStockPtr;
  }
      
      LastStockPtr = StockPtr;

      end_line++;
      line = end_line;       
    }
    
  return (FirstStockPtr);
}

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

#define __HTTP_C__
#ifndef __UNIX__
#define __UNIX__
#endif

#ifdef __UNIX__
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#elif __WINDOWS__
#include <winsock.h>
#include <mbstring.h>
#endif

#include <sys/types.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "http.h"
#include "stocks.h"

#define BUF_SIZE 1024
#define HEADER_MAXBUF 512

/* pointer to proxy server name or NULL */
char *http_proxy_server=NULL;
/* proxy server port number or 0 */
int http_proxy_port=0;

/*****************************************************************************/
/* Gets the file from http://http_server/http_file                           */
/* This function is used to get quotes csv file from yahoo.                  */
/* It allocates memory for the file and defines *pdata (the pointer of datas)*/
/*****************************************************************************/
libstocks_return_code http_get(char *http_file, char *http_server, char **pdata)
{
  struct hostent *host;          /* structs needed by socket */
  struct sockaddr_in server;
  int s;                         /* socket descriptor */
  char header[HEADER_MAXBUF];    /* request header */
  int hlg;                       /* header length */
  char buf[BUF_SIZE+1];          /* tempory buffer from socket read */
  int r;                         /* number of bytes read by read function */
  char *data=NULL;               /* http server response */
  int data_lgr;                  /* http server response length */
  char *temp;                    /* pointer used to split header and csv */
  int error_code;                /* error code returned by http server */
  char *csv_ptr;                 /* pointer to the csv content */
  int header_founded;            /* test if header is founded */

#ifdef DEBUG
  printf("*http_get\n");
#endif

  /* get host info by name :*/
  if ((host = gethostbyname( http_proxy_server ? http_proxy_server : http_server)))
    {
      memset((char *) &server,0, sizeof(server));
      memmove((char *) &server.sin_addr, host->h_addr, host->h_length);
      server.sin_family = host->h_addrtype;
      server.sin_port = (unsigned short) htons( http_proxy_server ? http_proxy_port : 80 );

    } else 
      {

#ifdef DEBUG
  printf(" gethostbyname : NOK\n");
#endif
  return ERRHOST;
      }

  /* create socket */
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {

#ifdef DEBUG
      printf(" create socket : NOK\n");
#endif
      return ERRSOCK;
    }

  setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, 0, 0);

  /* connect to server */
  if (connect(s, &server, sizeof(server)) < 0)
    {
#ifdef DEBUG
      printf(" connect to server : NOK\n");
#endif

#ifdef __UNIX__
      close(s);
#elif __WINDOWS__
      closesocket(s);
#endif
      return ERRCONN;
    }

  /* create header */
  if (http_proxy_server) 
    {
      sprintf(header,"GET http://%.128s:80%.256s HTTP/1.0\015\012\015\012",
         http_server, http_file);
    }
  else 
    {
      sprintf(header,"GET %s HTTP/1.0\015\012\015\012",http_file);
    }

  hlg=strlen(header);
  
  /* send header */
#ifdef __UNIX__
  if (write(s,header,hlg)!=hlg) 
#elif __WINDOWS__
  if (send(s,header,hlg, 0)!=hlg)
#endif
    {
#ifdef DEBUG
  printf(" send header : NOK\n");
#endif
      return ERRWHEA;         
    }

  data_lgr = 0;
  r=1;
  while(r)
    {
      /* Clear Buffer */
      memset(buf,0,BUF_SIZE+1);

#ifdef __UNIX__
      r=read(s,buf,BUF_SIZE);
#elif __WINDOWS__
      r=recv(s,buf,BUF_SIZE,0);
#endif      
      
      if (r)
  {
    if(!data_lgr)
      {
        if((data = malloc(r+1))==NULL)
    {
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
        ,__FILE__, __LINE__);
      exit(1);
    }
        
        memcpy(data,buf,r);
        data_lgr = r;
        data[r]=0;
      }
    else
      {
        if((temp = malloc(r+data_lgr+1))==NULL)
    {
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
        ,__FILE__, __LINE__);
      exit(1);
    }
        memcpy(temp, data, data_lgr);
        memcpy(temp+data_lgr, buf, r);
        temp[r+data_lgr]=0;
        data_lgr += r;
        free(data);
        data = temp;
      }
  }
    }
  
  /* close socket */
#ifdef __UNIX__
  close(s);
#elif __WINDOWS__
  closesocket(s);
#endif

#ifdef DEBUG
  printf("%s\n", data);
#endif

  /* get headers to test status line */
  /* and to split headers and content */
  
  temp = data;
  header_founded = 0;
  while( !header_founded )
    {
      if (*temp==0) return ERRRHEA;

      if( *temp==0x0A )
  {
    /* test if it is the header end */
    temp ++;
    if (*temp == 0x0D) temp++;
    if (*temp == 0x0A) header_founded = 1;
  }
      else
  temp++;
    }

  *temp = 0;
  temp++;

  sscanf(data,"HTTP/1.%*d %03d",&error_code);
  
  if (error_code != 200)
    {
#ifdef DEBUG
      printf(" HTTP error code : %d\n", error_code);
#endif
      free(data);
      return ERRPAHD;
    }

  if ((csv_ptr = malloc(strlen(temp)+1))==NULL)
    {
      free(data);
      fprintf(stderr,"Memory allocating error (%s line %d)\n"
        ,__FILE__, __LINE__);
      exit(1);
    }
      
  memcpy(csv_ptr, temp, strlen(temp)+1);
  free(data);

#ifdef DEBUG
  printf(" CSV\n");
  printf("%s,\n", csv_ptr);
#endif

  *pdata = csv_ptr;
  
  return 0;
}

/******************************************************************************/
/* Set the proxy server to use                                                */
/******************************************************************************/
libstocks_return_code set_proxy(char *proxy)
{
  char *ptr;
  char c;

#ifdef DEBUG
  printf("*set_proxy\n");
#endif

  /* Parse the proxy URL - It must start with http://  */
#ifdef __UNIX__
  if (strncasecmp("http://",proxy,7)) return ERRPROX;
#elif __WINDOWS__
  if (_mbsnbicmp("http://",proxy,7)) return ERRPROX;
#endif

  proxy+=7;

  /* find ":" in the proxy url */
  ptr = proxy;
  for (c=*ptr; (c && c!=':');) c=*ptr++;

  /* ptr points just after the ":" or at the end of proxy if : not founded */
  *(ptr-1)=0;  /* clear the ":"  */

  http_proxy_server=strdup(proxy);
  
#ifdef DEBUG
  printf("http_proxy_server : %s\n", http_proxy_server);
#endif

  /* get the port number of the url */
  if (sscanf(ptr,"%d",&http_proxy_port)!=1) return ERRPROX;

#ifdef DEBUG
  printf("http_proxy_port : %d\n", http_proxy_port);
#endif

  return 0;
}

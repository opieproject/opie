/* 
 * Configfile operations for wellenreiter
 * 
 * $Id: wl_conf.cc,v 1.1 2003-01-05 11:18:27 mjm Exp $
 */

#include "wl_conf.hh"
#include "wl_log.hh"
#include "wl_types.hh"

/* Check whether configfile exists and is readable */
int wl_checkcfg(void)
{
  FILE *wl_config;

  if((wl_config = fopen(WL_CONFFILE, "r")) == NULL)
  {
    wl_logerr("Cannot open configfile: %s", strerror(errno));
    return 0;
  }
  else
  {
    fclose(wl_config);
    return 1;
  }

}

/* Get value for given token from config file */
int wl_cfgvalue(const char *token, char *out, int maxlen)
{
  FILE *wl_config;
  char *ptr, *ptr2;
  char confbuf[WL_CONFBUFF];

  if(token == NULL)
    return -1;

  if((wl_config = fopen(WL_CONFFILE, "r")) == NULL)
  {
    wl_logerr("Cannot open configfile: %s", strerror(errno));
    return -1;
  }
   
  /* Clear buffers */
  memset(out, 0, maxlen);
  memset(confbuf, 0, sizeof(confbuf));

  while((fgets(confbuf, sizeof(confbuf) - 1, wl_config)) != NULL)
  {
    
    /* Ignore comments */
    if(confbuf[0] == '#') continue;
 
    /* Search for token, if found check whether next character
     *  is a '=' or a ' ' 
     */
    if(strstr(confbuf, token) != NULL &&
       (confbuf[strlen(token)] == '=' || confbuf[strlen(token)] == ' '))
    {

      /* Get value between quotes */
      if((ptr = strstr(confbuf, "\"")) == NULL)
	break;
      ++ptr;
      if((ptr2 = strstr(ptr, "\"")) == NULL)
	break;
      ptr2[0] = '\0';

      memcpy(out, ptr, maxlen - 1);
      break;

    }
    memset(confbuf, 0, sizeof(confbuf));
  }

  fclose(wl_config);

  return (out[0] == '\0' ? 0 : 1);
}


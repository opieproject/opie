/*
 *
 * Global configuration for wellenreiter
 *
 * $Id: config.hh,v 1.4 2002-12-16 17:59:21 mjm Exp $
 *
 * Written by Martin J. Muench <mjm@codito.de>
 *
 */

#ifndef CONFIG_HH
#define CONFIG_HH

#define PROGNAME "wellenreiter"  /* Name of program (for syslog et.al.) */
#define VERSION  "0.2"           /* Version of wellenreiter             */

#define DAEMONADDR    "127.0.0.1"
#define DAEMONPORT    37772            /* Port of Daemon                           */

#define GUIADDR  "127.0.0.1"     /* Adress of GUI, later specified in configfile */
#define GUIPORT  37773          /* Port of GUI, "   " */

/* Temporary cardmode stuff, will hopefully removed soon */
#define   CARD_TYPE_CISCO       1
#define   CARD_TYPE_NG          2
#define   CARD_TYPE_HOSTAP      3

#endif /* CONFIG_HH */

require 'mkmf'
$CFLAGS = '-I..'
$LDFLAGS = '-ldl'
$objs = ["kconfig_wrap.o"]
create_makefile 'kconfig'

TEMPLATE        = subdirs
SUBDIRS         = pluckerbaselib \
                  codeclib \
                  pdblib \
                  PluckerCodec \
                  AportisCodec \
                  CHM \
                  HTMLFilter \
                  WeaselCodec \
                  iSiloCodec \
                  ppmsCodec \
                  FliteCmd \
                  pluckerdecompress \
                  ArriereCodec \
                  RebCodec \
                  reader
LIBS            += -lopiecore2

# these are added only built if enabled in "make menuconfig"
#                  FliteDyn
#                  FliteDyn16

include( $(OPIEDIR)/include.pro )



TEMPLATE	=   lib
CONFIG		+=  qt warn_on release
HEADERS		=   it_defs.h modplugin.h modpluginimpl.h sndfile.h stdafx.h memfile.h
SOURCES	        =   fastmix.cpp load_669.cpp load_amf.cpp load_ams.cpp \
                    load_dbm.cpp load_dmf.cpp load_dsm.cpp load_far.cpp \
		    load_it.cpp load_j2b.cpp load_mdl.cpp load_med.cpp \
		    load_mod.cpp load_mt2.cpp load_mtm.cpp load_okt.cpp \ 
		    load_psm.cpp load_ptm.cpp load_s3m.cpp load_stm.cpp \
		    load_ult.cpp load_umx.cpp load_xm.cpp \
		    mmcmp.cpp modplugin.cpp modpluginimpl.cpp snd_dsp.cpp \
		    snd_flt.cpp snd_fx.cpp sndfile.cpp sndmix.cpp tables.cpp \
		    memfile.cpp
TARGET		=   modplugin
DESTDIR		=   $(OPIEDIR)/plugins/codecs
INCLUDEPATH 	+=  $(OPIEDIR)/include
DEPENDPATH      +=  $(OPIEDIR)/include
LIBS            +=  -lqpe -lm 
VERSION		=   1.0.0


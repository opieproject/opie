# Microsoft Developer Studio Project File - Name="$$MSVCDSP_PROJECT" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version $$MSVCDSP_VER
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=$$MSVCDSP_PROJECT - Win32 $$MSVCDSP_CONFIGMODE
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "$$MSVCDSP_PROJECT.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "$$MSVCDSP_PROJECT.mak" CFG="$$MSVCDSP_PROJECT - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "$$MSVCDSP_PROJECT - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "$$MSVCDSP_PROJECT - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "$$MSVCDSP_PROJECT - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "$$MSVCDSP_TARGETDIRREL"
# PROP BASE Intermediate_Dir "$$MSVCDSP_OBJECTSDIRREL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "$$MSVCDSP_TARGETDIRREL"
# PROP Intermediate_Dir "$$MSVCDSP_OBJECTSDIRREL"
# PROP Target_Dir ""
# ADD CPP $$MSVCDSP_MTDEF /W3 $$MSVCDSP_INCPATH /D "WIN32" /D "NDEBUG" /D "_LIB" $$MSVCDSP_DEFINES $$MSVCDSP_RELDEFS /FD /c $$MSVCDSP_CXXFLAGS $$QMAKE_CXXFLAGS_RELEASE $$PRECOMPILED_FLAGS_REL
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo $$MSVCDSP_TARGET $$PRECOMPILED_OBJECT
$$MSVCDSP_POST_LINK_REL

!ELSEIF  "$(CFG)" == "$$MSVCDSP_PROJECT - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "$$MSVCDSP_TARGETDIRDEB"
# PROP BASE Intermediate_Dir "$$MSVCDSP_OBJECTSDIRDEB"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "$$MSVCDSP_TARGETDIRDEB"
# PROP Intermediate_Dir "$$MSVCDSP_OBJECTSDIRDEB"
# PROP Target_Dir ""
# ADD CPP $$MSVCDSP_MTDEFD /W3 $$MSVCDSP_DEBUG_OPT /Od $$MSVCDSP_INCPATH /D "WIN32" /D "_DEBUG" /D "_LIB" $$MSVCDSP_DEFINES /FD /c $$MSVCDSP_CXXFLAGS  $$QMAKE_CXXFLAGS_DEBUG  $$PRECOMPILED_FLAGS_DEB
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD LIB32 /nologo $$MSVCDSP_TARGET $$PRECOMPILED_OBJECT
$$MSVCDSP_POST_LINK_DBG

!ENDIF 

# Begin Target

# Name "$$MSVCDSP_PROJECT - Win32 Release"
# Name "$$MSVCDSP_PROJECT - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
$$MSVCDSP_SOURCES
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
$$MSVCDSP_HEADERS
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group

$$MSVCDSP_FORMS
$$MSVCDSP_TRANSLATIONS
$$MSVCDSP_LEXSOURCES
$$MSVCDSP_YACCSOURCES
$$MSVCDSP_PICTURES

# Begin Group "Generated"
$$MSVCDSP_MOCSOURCES
$$MSVCDSP_FORMSOURCES
$$MSVCDSP_FORMHEADERS
$$MSVCDSP_IMAGES
$$MSVCDSP_STRIPPEDTRANSLATIONS
# Prop Default_Filter "moc"
# End Group
# End Target
# End Project

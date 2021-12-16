# Microsoft Developer Studio Project File - Name="lundftpd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=lundftpd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lundftpd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lundftpd.mak" CFG="lundftpd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lundftpd - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "lundftpd - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lundftpd - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../lion" /I "../../../lion/contrib/libdirlist" /I "$(SSL)/inc32" /I "$(GDBM)" /D "NDEBUG" /D "RACESTATS" /D "DUPECHECK" /D "USE_GDBM" /D "IRCBOT" /D WIN32=1 /D "_CONSOLE" /D "_MBCS" /D "INTERNAL_LS" /D strcasecmp=stricmp /D strncasecmp=strnicmp /D "WITH_SSL" /D strtoull=_atoi64 /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 gdbm.lib wsock32.lib lion.lib dirlist.lib libeay32s.lib SSLeay32s.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"../../../lion/Win32/Release" /libpath:"../../../lion/contrib/libdirlist/win32/Release" /libpath:"$(SSL)/out32dll/Release" /libpath:"$(GDBM)/gdbm/Release"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy
PostBuild_Cmds=Copy Release\lundftpd.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "lundftpd - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../lion" /I "../../../lion/contrib/libdirlist" /I "$(SSL)/inc32" /I "$(GDBM)" /D "_DEBUG" /D "RACESTATS" /D "DUPECHECK" /D "USE_GDBM" /D "IRCBOT" /D WIN32=1 /D "_CONSOLE" /D "_MBCS" /D "INTERNAL_LS" /D strcasecmp=stricmp /D strncasecmp=strnicmp /D "WITH_SSL" /D strtoull=_atoi64 /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gdbm.lib wsock32.lib lion.lib dirlist.lib libeay32s.lib SSLeay32s.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"../../../lion/Win32/Debug" /libpath:"../../../lion/contrib/libdirlist/win32/Debug" /libpath:"$(SSL)/out32dll/debug" /libpath:"$(GDBM)/gdbm/Debug"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copy
PostBuild_Cmds=Copy Debug\lundftpd.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "lundftpd - Win32 Release"
# Name "lundftpd - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\check.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\checkchild.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\conf.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\data.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\db.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\dupe.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\export.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\file.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\fnmatch.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\getopt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\global.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\global_var.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\groups.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\help.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ident.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\io.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\irc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\local.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\log.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\login.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\main.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\quota.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\race.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\remote.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\remote.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\section.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sfv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\site.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sitecust.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\strmode.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\tops.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\user.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\w32service.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\win32.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\src\check.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\checkchild.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\conf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\data.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\db.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\dirent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\dupe.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\export.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\file.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\fnmatch.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\getput.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\global.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\groups.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\help.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\ident.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\io.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\irc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\local.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\login.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\mp3genre.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\oldlogin.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\quota.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\race.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\section.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sfv.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\site.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\sitecust.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\socket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\tops.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\user.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\userflag.h
# End Source File
# Begin Source File

SOURCE=..\..\..\src\win32.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\lundftpd.conf
# End Source File
# End Target
# End Project

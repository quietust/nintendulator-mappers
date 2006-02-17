# Microsoft Developer Studio Project File - Name="NSF" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NSF - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NSF.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NSF.MAK" CFG="NSF - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NSF - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NSF - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Mappers/NSF", PDAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NSF - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj_nsf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NSF_EXPORTS" /YX /FD /c
# ADD CPP /nologo /Gr /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NSF_EXPORTS" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386

!ELSEIF  "$(CFG)" == "NSF - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj_nsf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NSF_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /Gr /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NSF_EXPORTS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# SUBTRACT MTL /mktyplib203
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "NSF - Win32 Release"
# Name "NSF - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "c;cpp;cxx;tli"
# Begin Group "Hardware Source Files"

# PROP Default_Filter "c;cpp;cxx;tli"
# End Group
# Begin Group "Sound Source Files"

# PROP Default_Filter "c;cpp;cxx;tli"
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_FDS.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_FME07.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_FMOPL.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_MMC5.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_N106.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_VRC6.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_VRC7.c
# End Source File
# End Group
# Begin Group "Mapper Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\NSF\NSF_Mapper.c
# End Source File
# End Group
# Begin Group "DLL Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\DLL\d_NSF.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Hardware Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Sound Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_FDS.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_FME07.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_FMOPL.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_MMC5.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_N106.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_VRC6.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_VRC7.h
# End Source File
# End Group
# Begin Group "DLL Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\Dll\d_NSF.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\interface.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\NSF\NSF.rc
# End Source File
# Begin Source File

SOURCE=..\src\NSF\resource.h
# End Source File
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="iNES" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=iNES - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "INES.MAK".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "INES.MAK" CFG="iNES - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iNES - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "iNES - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/Mappers/INES", NDAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iNES - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj_ines"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INES_EXPORTS" /YX /FD /c
# ADD CPP /nologo /Gr /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INES_EXPORTS" /FR /FD /c
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

!ELSEIF  "$(CFG)" == "iNES - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\bin"
# PROP Intermediate_Dir "..\obj_ines"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INES_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /Gr /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INES_EXPORTS" /FR /FD /GZ /c
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

# Name "iNES - Win32 Release"
# Name "iNES - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "c;cpp;cxx;tli"
# Begin Group "Hardware Source Files"

# PROP Default_Filter "c;cpp;cxx;tli"
# Begin Source File

SOURCE=..\src\Hardware\h_FME7.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_Latch.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_MMC1.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_MMC2.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_MMC3.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_MMC4.c
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_MMC5.c
# End Source File
# End Group
# Begin Group "Sound Source Files"

# PROP Default_Filter "c;cpp;cxx;tli"
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_FME7.c
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

# PROP Default_Filter "c;cpp;cxx;tli"
# Begin Source File

SOURCE=..\src\iNES\mapper000.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper001.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper002.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper003.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper004.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper005.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper007.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper009.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper010.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper011.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper013.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper015.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper016.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper018.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper019.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper021.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper022.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper023.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper024.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper025.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper026.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper032.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper033.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper034.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper040.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper041.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper042.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper043.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper044.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper045.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper046.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper047.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper049.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper050.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper051.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper052.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper056.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper058.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper059.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper060.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper061.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper062.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper063.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper064.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper065.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper066.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper067.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper068.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper069.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper070.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper071.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper073.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper075.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper076.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper078.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper079.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper080.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper083.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper085.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper086.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper087.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper090.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper091.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper093.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper094.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper095.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper097.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper099.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper100.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper105.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper112.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper113.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper114.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper115.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper118.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper119.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper126.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper140.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper141.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper142.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper143.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper151.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper164.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper180.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper182.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper184.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper185.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper189.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper225.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper226.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper227.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper228.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper229.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper231.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper232.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper234.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper235.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper243.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper246.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper250.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\mapper254.c
# End Source File
# Begin Source File

SOURCE=..\src\iNES\unimplemented.c
# End Source File
# End Group
# Begin Group "DLL Source Files"

# PROP Default_Filter "c;cpp;cxx;tli"
# Begin Source File

SOURCE=..\src\DLL\d_iNES.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Hardware Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\Hardware\h_FME7.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_Latch.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_MMC1.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_MMC2.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_MMC3.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_MMC4.h
# End Source File
# Begin Source File

SOURCE=..\src\Hardware\h_MMC5.h
# End Source File
# End Group
# Begin Group "Sound Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\Hardware\Sound\s_FME7.h
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

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\src\Dll\d_iNES.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\src\interface.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "rc"
# Begin Source File

SOURCE=..\src\iNES\iNES.rc
# End Source File
# Begin Source File

SOURCE=..\src\iNES\resource.h
# End Source File
# End Group
# End Target
# End Project

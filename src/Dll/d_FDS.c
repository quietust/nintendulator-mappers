/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"d_FDS.h"

HWND			hWnd;
HINSTANCE		hInstance;
CPEmulatorInterface	EMU;
CPROMInfo		ROM;

extern	CTMapperInfo	MapperInfo_FDS;

static	void	MAPINT	UnloadMapper (void)
{
	ROM = NULL;
}

static	CPMapperInfo	MAPINT	LoadMapper (CPROMInfo _ROM)
{
	ROM = _ROM;
	if (ROM->ROMType == ROM_UNDEFINED)	/* Allow enumerating mappers */
	{
		unsigned int i = (unsigned int)ROM->Filename;
		if (i >= 1)
		{
			UnloadMapper();
			return NULL;
		}
		((PROMInfo)ROM)->ROMType = ROM_FDS;
		return &MapperInfo_FDS;
	}
	if (ROM->ROMType != ROM_FDS)
	{
		UnloadMapper();
		return NULL;
	}
	return &MapperInfo_FDS;
}


static	TDLLInfo	DLL_Info =
{
	_T("FDS.DLL by Quietust"),
	0x20060111,
	0x00040001,
	LoadMapper,
	UnloadMapper
};

__declspec(dllexport)	void	MAPINT	UnloadMapperDLL (void)
{
	EMU = NULL;
	hWnd = NULL;
}

__declspec(dllexport)	PDLLInfo	MAPINT	LoadMapperDLL (HWND hWndEmu, CPEmulatorInterface _EMU, int VersionRequired)
{
	hWnd = hWndEmu;
	EMU = _EMU;
	if (VersionRequired != CurrentMapperInterface)
	{
		MessageBox(hWnd,_T("Mapper interface incompatible!"),_T("FDS.DLL"),MSGBOX_FLAGS);
		UnloadMapperDLL();
		return NULL;
	}
	return &DLL_Info;
}

BOOL	WINAPI	DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	hInstance = hinstDLL;
	return TRUE;
}

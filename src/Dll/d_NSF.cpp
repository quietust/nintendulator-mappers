/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"d_NSF.h"

HWND			hWnd;
HINSTANCE		hInstance;
CPEmulatorInterface	EMU;
CPROMInfo		ROM;

namespace
{
void	MAPINT	UnloadMapper (void)
{
	ROM = NULL;
}

CPMapperInfo	MAPINT	LoadMapper (CPROMInfo _ROM)
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
		((PROMInfo)ROM)->ROMType = ROM_NSF;
		return &MapperInfo_NSF;
	}
	if (ROM->ROMType != ROM_NSF)
	{
		UnloadMapper();
		return NULL;
	}
	return &MapperInfo_NSF;
}

TDLLInfo	DLL_Info =
{
	_T("NSF.DLL by Quietust"),
	0x20100102,
	0x00040002,
	LoadMapper,
	UnloadMapper
};
} // namespace

extern "C" __declspec(dllexport)	void	MAPINT	UnloadMapperDLL (void)
{
	EMU = NULL;
	hWnd = NULL;
}

extern "C" __declspec(dllexport)	PDLLInfo	MAPINT	LoadMapperDLL (HWND hWndEmu, CPEmulatorInterface _EMU, int VersionRequired)
{
	hWnd = hWndEmu;
	EMU = _EMU;
	if (VersionRequired != CurrentMapperInterface)
	{
		MessageBox(hWnd,_T("Mapper interface incompatible!"),_T("NSF.DLL"),MSGBOX_FLAGS);
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
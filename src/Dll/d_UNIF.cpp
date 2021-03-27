/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"d_UNIF.h"

HWND			hWnd;
HINSTANCE		hInstance;
const EmulatorInterface	*EMU;
const ROMInfo		*ROM;

void	UNIF_SetMirroring (FSync MapMirror)
{
	switch (ROM->UNIF_Mirroring)
	{
	case 0:	EMU->Mirror_H();	break;
	case 1:	EMU->Mirror_V();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	case 4:	EMU->Mirror_4();	break;
	case 5:	if (MapMirror)		break;
			MapMirror();	break;
	}
}

void	UNIF_SetSRAM (int Amt)
{
	if (ROM->UNIF_Battery)
		EMU->Set_SRAMSize(Amt);
}

namespace
{
void	MAPINT	UnloadMapper (void)
{
	ROM = NULL;
}

const MapperInfo	*MAPINT	LoadMapper (const ROMInfo *_ROM)
{
	const MapperInfo *found = NULL;
	if (_ROM->ROMType == ROM_UNDEFINED)
	{	/* Allow enumerating mappers */
		found = findByIndex((unsigned int)(size_t)_ROM->Filename);
		if (found)
		{
			/* Yes, this is overwriting a Const field. No, I don't care. */
			((ROMInfo *)_ROM)->ROMType = ROM_UNIF;
		}
	}
	else if (_ROM->ROMType == ROM_UNIF)
	{
		/* Only permit UNIF ROM images */
		found = findByName(_ROM->UNIF_BoardName);
	}
	if (found)
		ROM = _ROM;
	return found;
}

DLLInfo	DLL_Info =
{
	_T("UNIF.DLL by Quietust"),
	0x20190101,
	0x00040003,
	LoadMapper,
	UnloadMapper
};
} // namespace

extern "C" __declspec(dllexport)	void	MAPINT	UnloadMapperDLL (void)
{
	EMU = NULL;
	hWnd = NULL;
}

extern "C" __declspec(dllexport)	DLLInfo	*MAPINT	LoadMapperDLL (HWND hWndEmu, const EmulatorInterface *_EMU, int VersionRequired)
{
	if (VersionRequired != CurrentMapperInterface)
		return NULL;
	hWnd = hWndEmu;
	EMU = _EMU;
	return &DLL_Info;
}

BOOL	WINAPI	DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	hInstance = hinstDLL;
	return TRUE;
}

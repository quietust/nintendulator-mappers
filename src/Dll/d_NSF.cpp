/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"d_NSF.h"

HWND			hWnd;
HINSTANCE		hInstance;
const EmulatorInterface	*EMU;
const ROMInfo		*ROM;

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
			((ROMInfo *)_ROM)->ROMType = ROM_NSF;
		}
	}
	else if (_ROM->ROMType == ROM_NSF)
		found = findByName(NSF_MAPPERNAME);
	if (found)
		ROM = _ROM;
	return found;
}

DLLInfo	DLL_Info =
{
	_T("NSF.DLL by Quietust"),
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

/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"d_iNES.h"
#include	<stdio.h>

void	iNES_SetMirroring (void)
{
	if (ROM->INES_Flags & 0x01)
		EMU->Mirror_V();
	else	EMU->Mirror_H();
	if (ROM->INES_Flags & 0x08)
		EMU->Mirror_4();
}

void	iNES_SetSRAM (void)
{
	if (ROM->INES_Flags & 0x02)
	{
		if (ROM->INES_Version == 2)
		{
			int size = (ROM->INES2_PRGRAM & 0xF0) >> 4;
			if (size == 0)
				EMU->DbgOut(_T("ERROR - NES 2.0 ROM claims to have battery-backed SRAM but specified size of 0!"));
			else if (size < 0xF)
				EMU->Set_SRAMSize(64 << size);
			else	EMU->DbgOut(_T("ERROR - NES 2.0 ROM specified reserved value for battery-backed SRAM size!"));
		}
		else	EMU->Set_SRAMSize(8192);
	}
}

// Shortcuts for automatically mapping CHR RAM if CHR ROM is unavailable
void	iNES_SetCHR_Auto1 (int Bank, int Val)
{
	if (ROM->INES_CHRSize)
		EMU->SetCHR_ROM1(Bank, Val);
	else
	{
		// only NES 2.0 will allow more than 8KB of CHR RAM
		if (ROM->INES_Version == 1)
			Val &= 0x7;
		EMU->SetCHR_RAM1(Bank, Val);
	}
}

void	iNES_SetCHR_Auto2 (int Bank, int Val)
{
	Val <<= 1;
	iNES_SetCHR_Auto1(Bank+0, Val+0);
	iNES_SetCHR_Auto1(Bank+1, Val+1);
}

void	iNES_SetCHR_Auto4 (int Bank, int Val)
{
	Val <<= 2;
	iNES_SetCHR_Auto1(Bank+0, Val+0);
	iNES_SetCHR_Auto1(Bank+1, Val+1);
	iNES_SetCHR_Auto1(Bank+2, Val+2);
	iNES_SetCHR_Auto1(Bank+3, Val+3);
}

void	iNES_SetCHR_Auto8 (int Bank, int Val)
{
	Val <<= 3;
	iNES_SetCHR_Auto1(Bank+0, Val+0);
	iNES_SetCHR_Auto1(Bank+1, Val+1);
	iNES_SetCHR_Auto1(Bank+2, Val+2);
	iNES_SetCHR_Auto1(Bank+3, Val+3);
	iNES_SetCHR_Auto1(Bank+4, Val+4);
	iNES_SetCHR_Auto1(Bank+5, Val+5);
	iNES_SetCHR_Auto1(Bank+6, Val+6);
	iNES_SetCHR_Auto1(Bank+7, Val+7);
}

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

const MapperInfo *MAPINT	LoadMapper (const ROMInfo *_ROM)
{
	const MapperInfo *found = NULL;
	if (_ROM->ROMType == ROM_UNDEFINED)
	{	/* Allow enumerating mappers */
		found = findByIndex((unsigned int)(size_t)_ROM->Filename);
		if (found)
		{
			/* Yes, this is overwriting a Const field. No, I don't care. */
			((ROMInfo *)_ROM)->ROMType = ROM_INES;
		}
	}
	else if ((_ROM->ROMType == ROM_INES) && !(_ROM->INES_Flags & 0x10))
	{
		/* Only permit INES ROM images with the Vs flag clear */
		found = findByNumber(_ROM->INES_MapperNum);
	}
	if (found)
		ROM = _ROM;
	return found;
}

DLLInfo	DLL_Info =
{
	_T("INES.DLL by Quietust"),
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

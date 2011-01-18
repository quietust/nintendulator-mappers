/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	<stdio.h>
#include	<string.h>
#include	<malloc.h>
#include	"d_UNIF.h"

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

HWND			hWnd;
HINSTANCE		hInstance;
const EmulatorInterface	*EMU;
const ROMInfo		*ROM;

namespace
{
const MapperInfo MapperInfo_0 = {NULL,NULL,COMPAT_NONE,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

const MapperInfo *BoardTable[] =
{
	/* NES/HVC */
	&MapperInfo_NES_AMROM,
	&MapperInfo_NES_ANROM,
	&MapperInfo_NES_AN1ROM,
	&MapperInfo_NES_AOROM,

	&MapperInfo_NES_BNROM,

	&MapperInfo_NES_CNROM,
	&MapperInfo_NES_CPROM,

	&MapperInfo_NES_DEROM,
	&MapperInfo_NES_DEIROM,
	&MapperInfo_NES_DRROM,

	&MapperInfo_NES_EKROM,
	&MapperInfo_NES_ELROM,
	&MapperInfo_NES_ETROM,
	&MapperInfo_NES_EWROM,

	&MapperInfo_NES_FJROM,
	&MapperInfo_NES_FKROM,

	&MapperInfo_NES_GNROM,

	&MapperInfo_NES_HKROM,

	&MapperInfo_NES_JLROM,
	&MapperInfo_NES_JSROM,
	&MapperInfo_NES_BTR,

	&MapperInfo_NES_MHROM,

	&MapperInfo_NES_NROM_128,
	&MapperInfo_NES_RROM_128,
	&MapperInfo_NES_NROM_256,

	&MapperInfo_NES_PEEOROM,
	&MapperInfo_NES_PNROM,

	&MapperInfo_NES_SAROM,
	&MapperInfo_NES_SBROM,
	&MapperInfo_NES_SCROM,
	&MapperInfo_NES_SC1ROM,
	&MapperInfo_NES_SEROM,
	&MapperInfo_NES_SFROM,
	&MapperInfo_NES_SGROM,
	&MapperInfo_NES_SHROM,
	&MapperInfo_NES_SH1ROM,
	&MapperInfo_NES_SIROM,
	&MapperInfo_NES_SJROM,
	&MapperInfo_NES_SKROM,
	&MapperInfo_NES_SLROM,
	&MapperInfo_NES_SL1ROM,
	&MapperInfo_NES_SL2ROM,
	&MapperInfo_NES_SL3ROM,
	&MapperInfo_NES_SLRROM,
	&MapperInfo_NES_SMROM,
	&MapperInfo_NES_SNROM,
	&MapperInfo_NES_SOROM,
	&MapperInfo_NES_SUROM,
	&MapperInfo_NES_SXROM,

	&MapperInfo_NES_TBROM,
	&MapperInfo_NES_TEROM,
	&MapperInfo_NES_TFROM,
	&MapperInfo_NES_TGROM,
	&MapperInfo_NES_TKROM,
	&MapperInfo_NES_TKSROM,
	&MapperInfo_NES_TLROM,
	&MapperInfo_NES_TL1ROM,
	&MapperInfo_NES_TL2ROM,
	&MapperInfo_NES_TLSROM,
	&MapperInfo_NES_TNROM,
	&MapperInfo_NES_TQROM,
	&MapperInfo_NES_TR1ROM,
	&MapperInfo_NES_TSROM,
	&MapperInfo_NES_TVROM,

	&MapperInfo_NES_UNROM,
	&MapperInfo_NES_UOROM,
	
	/* Unlicensed */
	&MapperInfo_UNL_SA_0161M,
	&MapperInfo_UNL_SA_72007,
	&MapperInfo_UNL_SA_72008,
	&MapperInfo_UNL_TC_U0115M,
	&MapperInfo_UNL_Sachen_74LS374N,
	&MapperInfo_UNL_SA_0036,	
	&MapperInfo_UNL_SA_0037,
	&MapperInfo_UNL_Sachen_8259A,
	&MapperInfo_UNL_Sachen_8259B,
	&MapperInfo_UNL_H2288,
	&MapperInfo_UNL_A9712,
	&MapperInfo_UNL_8237,
	&MapperInfo_UNL_DRIPGAME,

	/* Bootleg Singles */
	&MapperInfo_BTL_MARIO1_MALEE2,

	/* Bootleg Multicarts */
	&MapperInfo_BMC_1991SuperHiK7in1,
	&MapperInfo_BMC_1992Ballgames11in1,
	&MapperInfo_BMC_Generic115in1,
	&MapperInfo_BMC_Generic15in1,
	&MapperInfo_BMC_Generic20in1,
	&MapperInfo_BMC_Generic20in1A,
	&MapperInfo_BMC_Generic42in1,
	&MapperInfo_BMC_Generic52in1,
	&MapperInfo_BMC_Generic76in1,
	&MapperInfo_BMC_GoldenGame150in1,
	&MapperInfo_BMC_Mario7in1,
	&MapperInfo_BMC_NovelDiamond9999999in1,
	&MapperInfo_BMC_Reset4in1,
	&MapperInfo_BMC_StudyGame32in1,
	&MapperInfo_BMC_Super1Min1,
	&MapperInfo_BMC_Super700in1,
	&MapperInfo_BMC_Supervision16in1,
	&MapperInfo_BMC_T3H53,
	&MapperInfo_BMC_Super24in1SC03,
	&MapperInfo_BMC_SV01,
	&MapperInfo_BMC_WS,

	/* Null-terminator */
	&MapperInfo_0
};

void	MAPINT	UnloadMapper (void)
{
	ROM = NULL;
}

const MapperInfo	*MAPINT	LoadMapper (const ROMInfo *_ROM)
{
	char *BoardName;
	ROM = _ROM;
	
	if (ROM->ROMType == ROM_UNDEFINED)	/* Allow enumerating mappers */
	{
		unsigned int i = (unsigned int)ROM->Filename;
		if (i >= sizeof(BoardTable)/sizeof(MapperInfo *)-1)
		{
			UnloadMapper();
			return NULL;
		}
		((ROMInfo *)ROM)->ROMType = ROM_UNIF;
		return BoardTable[i];
	}

	if (ROM->ROMType != ROM_UNIF)
	{
		UnloadMapper();
		return NULL;
	}
	BoardName = _strdup(ROM->UNIF_BoardName);
	if ((!strncmp(BoardName,"BTL-",4)) || (!strncmp(BoardName,"HVC-",4)) || (!strncmp(BoardName,"UNL-",4)))
		strncpy(BoardName,"NES-",4);
	for (int i = 0; BoardTable[i] != &MapperInfo_0; i++)
	{
		if ((!strcmp((char *)BoardTable[i]->MapperId,BoardName)) || (!strcmp((char *)BoardTable[i]->MapperId,ROM->UNIF_BoardName)))
		{
			free(BoardName);
			return BoardTable[i];
		}
	}
	free(BoardName);
	UnloadMapper();
	return NULL;
}

DLLInfo	DLL_Info =
{
	_T("UNIF.DLL by Quietust"),
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

extern "C" __declspec(dllexport)	DLLInfo	*MAPINT	LoadMapperDLL (HWND hWndEmu, const EmulatorInterface *_EMU, int VersionRequired)
{
	hWnd = hWndEmu;
	EMU = _EMU;
	if (VersionRequired != CurrentMapperInterface)
	{
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

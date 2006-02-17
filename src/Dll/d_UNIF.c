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
	case 5:	if (!MapMirror)		break;
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
CPEmulatorInterface	EMU;
CPROMInfo		ROM;

extern	CTMapperInfo
/* NES/HVC: Official Nintendo boards */
	MapperInfo_NES_AMROM,	MapperInfo_NES_ANROM,	MapperInfo_NES_AOROM,
	MapperInfo_NES_BNROM,
	MapperInfo_NES_CNROM,	MapperInfo_NES_CPROM,
	MapperInfo_NES_DEROM,	MapperInfo_NES_DEIROM,	MapperInfo_NES_DRROM,
	MapperInfo_NES_EKROM,	MapperInfo_NES_ELROM,	MapperInfo_NES_ETROM,	MapperInfo_NES_EWROM,
	MapperInfo_NES_FJROM,
	MapperInfo_NES_GNROM,
	MapperInfo_NES_HKROM,
	MapperInfo_NES_JLROM,	MapperInfo_NES_JSROM,	MapperInfo_NES_BTR,
	MapperInfo_NES_MHROM,
	MapperInfo_NES_NROM_128,MapperInfo_NES_RROM_128,MapperInfo_NES_NROM_256,
	MapperInfo_NES_PNROM,
	MapperInfo_NES_SAROM,	MapperInfo_NES_SBROM,	MapperInfo_NES_SCROM,	MapperInfo_NES_SEROM,
	MapperInfo_NES_SGROM,	MapperInfo_NES_SKROM,	MapperInfo_NES_SLROM,	MapperInfo_NES_SL1ROM,
	MapperInfo_NES_SNROM,	MapperInfo_NES_SOROM,	MapperInfo_NES_SUROM,
	MapperInfo_NES_TFROM,	MapperInfo_NES_TGROM,	MapperInfo_NES_TKROM,	MapperInfo_NES_TLROM,
	MapperInfo_NES_TR1ROM,	MapperInfo_NES_TSROM,
	MapperInfo_NES_UNROM,	MapperInfo_NES_UOROM,

/* UNL: Unlicensed (legal) boards */
	MapperInfo_UNL_SA_0161M,MapperInfo_UNL_SA_72007,MapperInfo_UNL_SA_72008,MapperInfo_UNL_TC_U0115M,
	MapperInfo_UNL_Sachen_74LS374N,		MapperInfo_UNL_SA_0036,		MapperInfo_UNL_SA_0037,
	MapperInfo_UNL_Sachen_8259A,	MapperInfo_UNL_Sachen_8259B,		MapperInfo_UNL_H2288,
	MapperInfo_UNL_8237,

	MapperInfo_UNL_DRIPGAME,

/* BTL: Pirate simgle-game carts */
	MapperInfo_BTL_MARIO1_MALEE2,
/* BMC: Pirate multicarts */
	MapperInfo_BMC_1991SuperHiK7in1,	MapperInfo_BMC_1992Ballgames11in1,
	MapperInfo_BMC_1995SuperHiK4in1,	MapperInfo_BMC_Generic115in1,
	MapperInfo_BMC_Generic15in1,		MapperInfo_BMC_Generic20in1,
	MapperInfo_BMC_Generic20in1A,		MapperInfo_BMC_Generic42in1,
	MapperInfo_BMC_Generic52in1,		MapperInfo_BMC_Generic76in1,
	MapperInfo_BMC_GoldenGame150in1,	MapperInfo_BMC_Mario7in1,
	MapperInfo_BMC_NovelDiamond9999999in1,	MapperInfo_BMC_Reset4in1,
	MapperInfo_BMC_StudyGame32in1,		MapperInfo_BMC_Super1Min1,
	MapperInfo_BMC_Super700in1,		MapperInfo_BMC_Supervision16in1,
	MapperInfo_BMC_T3H53,			MapperInfo_BMC_Super24in1SC03,
	MapperInfo_BMC_SV01,			MapperInfo_BMC_WS
	;

CTMapperInfo	MapperInfo_0 = {NULL,NULL,COMPAT_NONE,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

static	CPMapperInfo	BoardTable[] =
{
	/* NES/HVC */
	&MapperInfo_NES_AMROM	,&MapperInfo_NES_ANROM	,&MapperInfo_NES_AOROM	,

	&MapperInfo_NES_BNROM	,

	&MapperInfo_NES_CNROM	,&MapperInfo_NES_CPROM	,

	&MapperInfo_NES_DEROM	,&MapperInfo_NES_DEIROM	,&MapperInfo_NES_DRROM	,

	&MapperInfo_NES_EKROM	,&MapperInfo_NES_ELROM	,&MapperInfo_NES_ETROM	,&MapperInfo_NES_EWROM	,

	&MapperInfo_NES_FJROM	,

	&MapperInfo_NES_GNROM	,

	&MapperInfo_NES_HKROM	,

	&MapperInfo_NES_JLROM	,&MapperInfo_NES_JSROM	,&MapperInfo_NES_BTR	,

	&MapperInfo_NES_MHROM	,

	&MapperInfo_NES_NROM_128	,&MapperInfo_NES_RROM_128		,&MapperInfo_NES_NROM_256,

	&MapperInfo_NES_PNROM	,

	&MapperInfo_NES_SAROM	,&MapperInfo_NES_SBROM	,&MapperInfo_NES_SCROM	,&MapperInfo_NES_SEROM	,
	&MapperInfo_NES_SGROM	,&MapperInfo_NES_SKROM	,&MapperInfo_NES_SLROM	,&MapperInfo_NES_SL1ROM	,
	&MapperInfo_NES_SNROM	,&MapperInfo_NES_SOROM	,&MapperInfo_NES_SUROM	,

	&MapperInfo_NES_TFROM	,&MapperInfo_NES_TGROM	,&MapperInfo_NES_TKROM	,&MapperInfo_NES_TLROM	,
	&MapperInfo_NES_TR1ROM	,&MapperInfo_NES_TSROM	,

	&MapperInfo_NES_UNROM	,&MapperInfo_NES_UOROM	,
	/* Unlicensed */
	&MapperInfo_UNL_SA_0161M	,&MapperInfo_UNL_SA_72007	,&MapperInfo_UNL_SA_72008	,
	&MapperInfo_UNL_TC_U0115M	,&MapperInfo_UNL_Sachen_74LS374N,&MapperInfo_UNL_SA_0036	,
	&MapperInfo_UNL_SA_0037		,&MapperInfo_UNL_Sachen_8259A	,&MapperInfo_UNL_Sachen_8259B	,
	&MapperInfo_UNL_H2288		,&MapperInfo_UNL_8237		,&MapperInfo_UNL_DRIPGAME	,
	/* Bootleg Singles */
	&MapperInfo_BTL_MARIO1_MALEE2	,
	/* Bootleg Multicarts */
	&MapperInfo_BMC_1991SuperHiK7in1	,&MapperInfo_BMC_1992Ballgames11in1	,
	&MapperInfo_BMC_1995SuperHiK4in1	,&MapperInfo_BMC_Generic115in1		,
	&MapperInfo_BMC_Generic15in1		,&MapperInfo_BMC_Generic20in1		,
	&MapperInfo_BMC_Generic20in1A		,&MapperInfo_BMC_Generic42in1		,
	&MapperInfo_BMC_Generic52in1		,&MapperInfo_BMC_Generic76in1		,
	&MapperInfo_BMC_GoldenGame150in1	,&MapperInfo_BMC_Mario7in1		,
	&MapperInfo_BMC_NovelDiamond9999999in1	,&MapperInfo_BMC_Reset4in1		,
	&MapperInfo_BMC_StudyGame32in1		,&MapperInfo_BMC_Super1Min1		,
	&MapperInfo_BMC_Super700in1		,&MapperInfo_BMC_Supervision16in1	,
	&MapperInfo_BMC_T3H53			,&MapperInfo_BMC_Super24in1SC03		,
	&MapperInfo_BMC_SV01			,&MapperInfo_BMC_WS			,
	/* Null-terminator */
	&MapperInfo_0
};

static	void	_MAPINT	UnloadMapper (void)
{
	ROM = NULL;
}

static	CPMapperInfo	_MAPINT	LoadMapper (CPROMInfo _ROM)
{
	int x = 0;
	char *BoardName;
	ROM = _ROM;
	if (ROM->ROMType != ROM_UNIF)
	{
		UnloadMapper();
		return NULL;
	}
	BoardName = _strdup(ROM->UNIF_BoardName);
	if ((!strncmp(BoardName,"BTL-",4)) || (!strncmp(BoardName,"HVC-",4)) || (!strncmp(BoardName,"UNL-",4)))
		strncpy(BoardName,"NES-",4);
	for (x = 0; BoardTable[x]->MapperId; x++)
	{
		if ((!strcmp(BoardTable[x]->MapperId,BoardName)) || (!strcmp(BoardTable[x]->MapperId,ROM->UNIF_BoardName)))
		{
			free(BoardName);
			return BoardTable[x];
		}
	}
	free(BoardName);
	UnloadMapper();
	return NULL;
}

static	TDLLInfo	DLL_Info =
{
	"The Quietust <quietust@ircN.org>",
	0x20020224,		/* Date */
	0x00010002,		/* Version 1.2 */
	LoadMapper,
	UnloadMapper
};

__declspec(dllexport)	void	_MAPINT	UnloadMapperDLL (void)
{
	EMU = NULL;
	hWnd = NULL;
}

__declspec(dllexport)	PDLLInfo	_MAPINT	LoadMapperDLL (HWND hWndEmu, CPEmulatorInterface _EMU, int VersionRequired)
{
	hWnd = hWndEmu;
	EMU = _EMU;
	if (VersionRequired != CurrentMapperInterface)
	{
		MessageBox(hWnd,"Mapper interface incompatible!","UNIF.DLL",MSGBOX_FLAGS);
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

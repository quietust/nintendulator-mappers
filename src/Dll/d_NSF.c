#include	"d_NSF.h"

HWND			hWnd;
HINSTANCE		hInstance;
CPEmulatorInterface	EMU;
CPROMInfo		ROM;

extern	CTMapperInfo	MapperInfo_NSF;

static	void	_MAPINT	UnloadMapper (void)
{
	ROM = NULL;
}

static	CPMapperInfo	_MAPINT	LoadMapper (CPROMInfo _ROM)
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

static	TDLLInfo	DLL_Info =
{
	"Quietust <quietust@ircN.org>",
	0x20041201,
	0x00030005,
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
		MessageBox(hWnd,"Mapper interface incompatible!","NSF.DLL",MSGBOX_FLAGS);
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
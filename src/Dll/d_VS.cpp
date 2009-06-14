/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"d_VS.h"
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
		EMU->Set_SRAMSize(8192);
}

HWND			hWnd;
HINSTANCE		hInstance;
CPEmulatorInterface	EMU;
CPROMInfo		ROM;

namespace
{
CPMapperInfo	MapperTable[256] =
{
	&MapperInfo_000,&MapperInfo_001,&MapperInfo_002,&MapperInfo_003,&MapperInfo_004,&MapperInfo_005,&MapperInfo_006,&MapperInfo_007,
	&MapperInfo_008,&MapperInfo_009,&MapperInfo_010,&MapperInfo_011,&MapperInfo_012,&MapperInfo_013,&MapperInfo_014,&MapperInfo_015,
	&MapperInfo_016,&MapperInfo_017,&MapperInfo_018,&MapperInfo_019,&MapperInfo_020,&MapperInfo_021,&MapperInfo_022,&MapperInfo_023,
	&MapperInfo_024,&MapperInfo_025,&MapperInfo_026,&MapperInfo_027,&MapperInfo_028,&MapperInfo_029,&MapperInfo_030,&MapperInfo_031,
	&MapperInfo_032,&MapperInfo_033,&MapperInfo_034,&MapperInfo_035,&MapperInfo_036,&MapperInfo_037,&MapperInfo_038,&MapperInfo_039,
	&MapperInfo_040,&MapperInfo_041,&MapperInfo_042,&MapperInfo_043,&MapperInfo_044,&MapperInfo_045,&MapperInfo_046,&MapperInfo_047,
	&MapperInfo_048,&MapperInfo_049,&MapperInfo_050,&MapperInfo_051,&MapperInfo_052,&MapperInfo_053,&MapperInfo_054,&MapperInfo_055,
	&MapperInfo_056,&MapperInfo_057,&MapperInfo_058,&MapperInfo_059,&MapperInfo_060,&MapperInfo_061,&MapperInfo_062,&MapperInfo_063,
	&MapperInfo_064,&MapperInfo_065,&MapperInfo_066,&MapperInfo_067,&MapperInfo_068,&MapperInfo_069,&MapperInfo_070,&MapperInfo_071,
	&MapperInfo_072,&MapperInfo_073,&MapperInfo_074,&MapperInfo_075,&MapperInfo_076,&MapperInfo_077,&MapperInfo_078,&MapperInfo_079,
	&MapperInfo_080,&MapperInfo_081,&MapperInfo_082,&MapperInfo_083,&MapperInfo_084,&MapperInfo_085,&MapperInfo_086,&MapperInfo_087,
	&MapperInfo_088,&MapperInfo_089,&MapperInfo_090,&MapperInfo_091,&MapperInfo_092,&MapperInfo_093,&MapperInfo_094,&MapperInfo_095,
	&MapperInfo_096,&MapperInfo_097,&MapperInfo_098,&MapperInfo_099,&MapperInfo_100,&MapperInfo_101,&MapperInfo_102,&MapperInfo_103,
	&MapperInfo_104,&MapperInfo_105,&MapperInfo_106,&MapperInfo_107,&MapperInfo_108,&MapperInfo_109,&MapperInfo_110,&MapperInfo_111,
	&MapperInfo_112,&MapperInfo_113,&MapperInfo_114,&MapperInfo_115,&MapperInfo_116,&MapperInfo_117,&MapperInfo_118,&MapperInfo_119,
	&MapperInfo_120,&MapperInfo_121,&MapperInfo_122,&MapperInfo_123,&MapperInfo_124,&MapperInfo_125,&MapperInfo_126,&MapperInfo_127,
	&MapperInfo_128,&MapperInfo_129,&MapperInfo_130,&MapperInfo_131,&MapperInfo_132,&MapperInfo_133,&MapperInfo_134,&MapperInfo_135,
	&MapperInfo_136,&MapperInfo_137,&MapperInfo_138,&MapperInfo_139,&MapperInfo_140,&MapperInfo_141,&MapperInfo_142,&MapperInfo_143,
	&MapperInfo_144,&MapperInfo_145,&MapperInfo_146,&MapperInfo_147,&MapperInfo_148,&MapperInfo_149,&MapperInfo_150,&MapperInfo_151,
	&MapperInfo_152,&MapperInfo_153,&MapperInfo_154,&MapperInfo_155,&MapperInfo_156,&MapperInfo_157,&MapperInfo_158,&MapperInfo_159,
	&MapperInfo_160,&MapperInfo_161,&MapperInfo_162,&MapperInfo_163,&MapperInfo_164,&MapperInfo_165,&MapperInfo_166,&MapperInfo_167,
	&MapperInfo_168,&MapperInfo_169,&MapperInfo_170,&MapperInfo_171,&MapperInfo_172,&MapperInfo_173,&MapperInfo_174,&MapperInfo_175,
	&MapperInfo_176,&MapperInfo_177,&MapperInfo_178,&MapperInfo_179,&MapperInfo_180,&MapperInfo_181,&MapperInfo_182,&MapperInfo_183,
	&MapperInfo_184,&MapperInfo_185,&MapperInfo_186,&MapperInfo_187,&MapperInfo_188,&MapperInfo_189,&MapperInfo_190,&MapperInfo_191,
	&MapperInfo_192,&MapperInfo_193,&MapperInfo_194,&MapperInfo_195,&MapperInfo_196,&MapperInfo_197,&MapperInfo_198,&MapperInfo_199,
	&MapperInfo_200,&MapperInfo_201,&MapperInfo_202,&MapperInfo_203,&MapperInfo_204,&MapperInfo_205,&MapperInfo_206,&MapperInfo_207,
	&MapperInfo_208,&MapperInfo_209,&MapperInfo_210,&MapperInfo_211,&MapperInfo_212,&MapperInfo_213,&MapperInfo_214,&MapperInfo_215,
	&MapperInfo_216,&MapperInfo_217,&MapperInfo_218,&MapperInfo_219,&MapperInfo_220,&MapperInfo_221,&MapperInfo_222,&MapperInfo_223,
	&MapperInfo_224,&MapperInfo_225,&MapperInfo_226,&MapperInfo_227,&MapperInfo_228,&MapperInfo_229,&MapperInfo_230,&MapperInfo_231,
	&MapperInfo_232,&MapperInfo_233,&MapperInfo_234,&MapperInfo_235,&MapperInfo_236,&MapperInfo_237,&MapperInfo_238,&MapperInfo_239,
	&MapperInfo_240,&MapperInfo_241,&MapperInfo_242,&MapperInfo_243,&MapperInfo_244,&MapperInfo_245,&MapperInfo_246,&MapperInfo_247,
	&MapperInfo_248,&MapperInfo_249,&MapperInfo_250,&MapperInfo_251,&MapperInfo_252,&MapperInfo_253,&MapperInfo_254,&MapperInfo_255
};

void	MAPINT	UnloadMapper (void)
{
	ROM = NULL;
}

CPMapperInfo	MAPINT	LoadMapper (CPROMInfo _ROM)
{
	ROM = _ROM;
	if (ROM->ROMType == ROM_UNDEFINED)
	{	/* Allow enumerating mappers */
		unsigned int i = (int)ROM->Filename;
		if (i >= 256)
		{
			UnloadMapper();
			return NULL;
		}
		((PROMInfo)ROM)->ROMType = ROM_INES;
		return MapperTable[i];
	}
	if (ROM->ROMType != ROM_INES)
	{	/* Only support iNES ROM images*/
		UnloadMapper();
		return NULL;
	}
	if (!(ROM->INES_Flags & 0x10))
	{	/* Don't support non-VS Unisystem ROMs */
		UnloadMapper();
		return NULL;
	}
	if (MapperTable[ROM->INES_MapperNum]->Compatibility == COMPAT_NONE)
	{	/* Don't accept mappers listed with zero compatibility */
		UnloadMapper();
		return NULL;
	}
	return MapperTable[ROM->INES_MapperNum];
}

TDLLInfo	DLL_Info =
{
	_T("VS.DLL by Quietust"),
	0x20090613,
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
		MessageBox(hWnd,_T("Mapper interface incompatible!"),_T("INES.DLL"),MSGBOX_FLAGS);
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

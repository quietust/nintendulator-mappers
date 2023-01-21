/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include	"..\DLL\d_iNES.h"

namespace
{
BOOL	MAPINT	Load (void)
{
	return TRUE;
}
void	MAPINT	Reset (RESET_TYPE ResetType)
{
	EMU->SetPRG_ROM32(0x8, 0);

	int va10 = -1;
	switch (ROM->INES_Flags & 0x9)
	{
	case 0:	va10 = 1;	break;
	case 1:	va10 = 0;	break;
	case 8:	va10 = 2;	break;
	case 9:	va10 = 3;	break;
	}
	for (int i = 0; i < 16; i++)
		EMU->SetCHR_NT1(i, (i >> va10) & 1);
}

uint16_t MapperNum = 218;
} // namespace

const MapperInfo MapperInfo_218
(
	&MapperNum,
	_T("Magic Floor"),
	COMPAT_FULL,
	Load,
	Reset,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
);

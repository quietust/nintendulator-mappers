/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#pragma once

#include	"..\interface.h"

namespace MMC1
{
void		Load		(FSync,BOOL,BOOL);
void		Reset		(RESET_TYPE);
void		Unload		(void);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
void	MAPINT	Write		(int,int,int);

void		SyncMirror	(void);
int		GetPRGBankLo	(void);
int		GetPRGBankHi	(void);
int		GetCHRBankLo	(void);
int		GetCHRBankHi	(void);
BOOL		GetWRAMEnabled	(void);

void		SyncPRG		(int,int);
void		SyncCHR_ROM	(int,int);
void		SyncCHR_RAM	(int,int);
void		SyncWRAM	(void);
} // namespace MMC1

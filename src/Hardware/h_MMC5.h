/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#pragma once

#include	"..\interface.h"

namespace MMC5
{
enum WRAM
{
	WRAM_0KB_0KB,
	WRAM_8KB_0KB,
	WRAM_32KB_0KB,
	WRAM_0KB_8KB,
	WRAM_8KB_8KB,
	WRAM_32KB_8KB,
	WRAM_0KB_32KB,
	WRAM_8KB_32KB,
	WRAM_32KB_32KB,
	WRAM_MAXOPTS
};

void		Load		(int,BOOL);
void		Reset		(RESET_TYPE);
void		Unload		(void);
void		SetRAMSize	(int);
int		GetRAMSize	(void);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
void		SetPRG		(int,int,int);
void		SyncPRG		(void);
void		SyncCHR		(int);
void		SyncCHRA	(void);
void		SyncCHRB	(void);
void		SyncMirror	(void);
void	MAPINT	WritePPU	(int,int,int);
int	MAPINT	CPURead5	(int,int);
int	MAPINT	CPURead5Safe	(int,int);
void	MAPINT	CPUWrite5	(int,int,int);
void	MAPINT	CPUWrite6F	(int,int,int);
void	MAPINT	PPUCycle	(int,int,int,int);
int	MAPINT	MapperSnd	(int);
} // namespace MMC5

/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2010 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_MMC5_H
#define	H_MMC5_H

#include	"..\interface.h"

#define	MMC5WRAM_0KB_0KB	0
#define	MMC5WRAM_8KB_0KB	1
#define	MMC5WRAM_32KB_0KB	2
#define	MMC5WRAM_0KB_8KB	3
#define	MMC5WRAM_8KB_8KB	4
#define	MMC5WRAM_32KB_8KB	5
#define	MMC5WRAM_0KB_32KB	6
#define	MMC5WRAM_8KB_32KB	7
#define	MMC5WRAM_32KB_32KB	8
#define	MMC5WRAM_MAXOPTS	9

namespace MMC5
{
void		Load		(int);
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
void	MAPINT	CPUWrite5	(int,int,int);
void	MAPINT	CPUWrite6F	(int,int,int);
void	MAPINT	PPUCycle	(int,int,int,int);
int	MAPINT	MapperSnd	(int);
} // namespace MMC5

#endif	// H_MMC5_H
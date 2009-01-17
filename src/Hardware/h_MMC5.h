/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
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

void		MMC5_Load	(int);
void		MMC5_Reset	(RESET_TYPE);
void		MMC5_Unload	(void);
void		MMC5_SetRAMSize	(int);
int		MMC5_GetRAMSize	(void);
int	MAPINT	MMC5_SaveLoad	(STATE_TYPE,int,unsigned char *);
void		MMC5_SetPRG	(int,int,int);
void		MMC5_SyncPRG	(void);
void		MMC5_SyncCHR	(int);
void		MMC5_SyncCHRA	(void);
void		MMC5_SyncCHRB	(void);
void		MMC5_SyncMirror	(void);
void	MAPINT	MMC5_WritePPU	(int,int,int);
int	MAPINT	MMC5_CPURead5	(int,int);
void	MAPINT	MMC5_CPUWrite5	(int,int,int);
void	MAPINT	MMC5_CPUWrite6F	(int,int,int);
void	MAPINT	MMC5_PPUCycle	(int,int,int,int);
int	MAPINT	MMC5_MapperSnd	(int);

#endif	/* H_MMC5_H */
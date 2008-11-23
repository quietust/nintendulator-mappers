/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2008 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_MMC1_H
#define	H_MMC1_H

#include	"..\interface.h"

void		MMC1_Load	(FSync);
void		MMC1_Reset	(RESET_TYPE);
void		MMC1_Unload	(void);
int	MAPINT	MMC1_SaveLoad		(STATE_TYPE,int,unsigned char *);
void	MAPINT	MMC1_Write		(int,int,int);

void		MMC1_SyncMirror		(void);
int		MMC1_GetPRGBankLo	(void);
int		MMC1_GetPRGBankHi	(void);
int		MMC1_GetCHRBankLo	(void);
int		MMC1_GetCHRBankHi	(void);
BOOL		MMC1_GetWRAMEnabled	(void);

void		MMC1_SyncPRG		(int,int);
void		MMC1_SyncCHR_ROM	(int,int);
void		MMC1_SyncCHR_RAM	(int,int);
void		MMC1_SyncWRAM		(void);

#endif	/* H_MMC1_H */
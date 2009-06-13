/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2009 QMT Productions
 *
 * $URL$
 * $Id$
 */

#ifndef	H_MMC1_H
#define	H_MMC1_H

#include	"..\interface.h"

namespace MMC1
{
void		Load		(FSync);
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

#endif	// H_MMC1_H
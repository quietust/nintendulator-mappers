/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#pragma once

namespace MMC5sound
{
void		Load		(void);
void		Reset		(RESET_TYPE);
void		Unload		(void);
void		Write		(int,int);
int		Read		(int);
int	MAPINT	Get		(int);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
BOOL		HaveIRQ		(void);
} // namespace MMC5sound

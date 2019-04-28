/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#pragma once

namespace FDSsound
{
void		Load		(void);
void		Reset		(RESET_TYPE);
void		Unload		(void);
int		Read		(int);
void		Write		(int,int);
int	MAPINT	Get		(int);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
} // namespace FDSsound

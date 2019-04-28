/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#pragma once

namespace VRC6sound
{
void		Load		(void);
void		Reset		(RESET_TYPE);
void		Unload		(void);
void		Write		(int,int);
int	MAPINT	Get		(int);
int	MAPINT	SaveLoad	(STATE_TYPE,int,unsigned char *);
} // namespace VRC6sound

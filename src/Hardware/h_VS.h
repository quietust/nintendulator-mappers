#ifndef	H_VS_H
#define	H_VS_H

#include	"..\interface.h"

typedef	struct	VS
{
	FCPURead Read;
	u8 DipSwitch, Coin;
	u32 CoinDelay;

	HWND ConfigWindow;
	u8 ConfigCmd;
}	TVS, *PVS;

extern	TVS	VS;

void			VS_Load		(void);
void			VS_Reset	(RESET_TYPE);
void			VS_Unload	(void);
int		_MAPINT	VS_SaveLoad	(STATE_TYPE,int,unsigned char *);
int		_MAPINT	VS_Read		(int,int);
void		_MAPINT	VS_CPUCycle	(void);
unsigned char	_MAPINT VS_Config	(CFG_TYPE,unsigned char);

#endif	/* H_VS_H */
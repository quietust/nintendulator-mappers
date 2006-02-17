#ifndef	H_FDS_H
#define	H_FDS_H

#include	"..\interface.h"

typedef	struct	FDS
{
	FCPURead Read;
	FCPUWrite Write;
	u8 DiskNum;
	u16 IRQcounter;
	u16_n IRQlatch;
	u8 IRQenabled;
	u8 IOenable;
	u8 IOcontrol;
	u8 IOstatus;
	int BytePtr;
	u8 WriteSkip;
	u8 DiskIRQ;
	u8 Mirror;
	HWND ConfigWindow;
	u8 ConfigCmd;
}	TFDS, *PFDS;

extern	TFDS	FDS;

void			FDS_Load	(void);
void			FDS_Reset	(RESET_TYPE);
void			FDS_Unload	(void);
int		_MAPINT	FDS_SaveLoad	(STATE_TYPE,int,unsigned char *);
void		_MAPINT	FDS_CPUCycle	(void);
unsigned char	_MAPINT	FDS_Config	(int,unsigned char);
int		_MAPINT	FDS_MapperSnd	(int);
int		_MAPINT	FDS_Read	(int,int);
void		_MAPINT	FDS_Write	(int,int,int);

#endif	/* H_FDS_H */
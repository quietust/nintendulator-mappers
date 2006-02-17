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
	HWND ConfigWindow;
}	TFDS, *PFDS;

extern	TFDS	FDS;

int		_MAPINT	FDS_SaveLoad	(int,int,char *);
void			FDS_Init	(int);
void		_MAPINT	FDS_CPUCycle	(void);
unsigned char	_MAPINT	FDS_Config	(int,unsigned char);
int		_MAPINT	FDS_MapperSnd	(int);
int		_MAPINT	FDS_Read	(int,int);
void		_MAPINT	FDS_Write	(int,int,int);
void			FDS_Destroy	(void);

#endif	/* H_FDS_H */
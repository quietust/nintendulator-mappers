#ifndef	__FDS_H__
#define	__FDS_H__

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

int	_MAPINT	FDS_SaveLoad	(int,int,char *);
void		FDS_Init	(int);
void	_MAPINT	FDS_CPUCycle	(void);
void	_MAPINT	FDS_Config	(void);
int	_MAPINT	FDS_MapperSnd	(int);
int	_MAPINT	FDS_Read	(int,int);
void	_MAPINT	FDS_Write	(int,int,int);
void		FDS_Destroy	(void);

#endif	/* __FDS_H__ */
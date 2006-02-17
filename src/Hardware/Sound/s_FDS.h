#ifndef	S_FDS_H
#define	S_FDS_H

void		FDSsound_Init		(void);
int		FDSsound_Read		(int);
void		FDSsound_Write		(int,int);
int	_MAPINT	FDSsound_Get		(int);
int	_MAPINT	FDSsound_SaveLoad	(SAVELOAD_TYPE,int,unsigned char *);
void		FDSsound_Destroy	(void);

#endif	/* S_FDS_H */
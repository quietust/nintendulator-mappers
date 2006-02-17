#ifndef	__SND_FDS_H__
#define	__SND_FDS_H__

void	FDSsound_Init (void);
int	FDSsound_Read (int);
void	FDSsound_Write (int,int);
int	_MAPINT	FDSsound_Get (int);
int	_MAPINT	FDSsound_SaveLoad (int,int,char *);
void	FDSsound_Destroy (void);

#endif	/* __SND_FDS_H__ */
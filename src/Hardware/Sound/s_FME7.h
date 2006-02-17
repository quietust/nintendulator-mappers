#ifndef	__SND_FME7_H__
#define	__SND_FME7_H__

void	FME7sound_Init (void);
void	FME7sound_Write (int,int);
int	_MAPINT	FME7sound_Get (int);
int	_MAPINT	FME7sound_SaveLoad (int,int,char *);
void	FME7sound_Destroy (void);

#endif	/* __SND_FME7_H__ */
#ifndef	__SND_FME07_H__
#define	__SND_FME07_H__

void	FME07sound_Init (void);
void	FME07sound_Write (int,int);
int	_MAPINT	FME07sound_Get (int);
int	_MAPINT	FME07sound_SaveLoad (int,int,char *);
void	FME07sound_Destroy (void);

#endif	/* __SND_FME07_H__ */
#ifndef	__SND_N106_H__
#define	__SND_N106_H__

void	N106sound_Init (void);
int	N106sound_Read (int);
void	N106sound_Write (int,int);
int	_MAPINT	N106sound_Get (int);
int	_MAPINT	N106sound_SaveLoad (int,int,char *);
void	N106sound_Destroy (void);

#endif	/* __SND_N106_H__ */
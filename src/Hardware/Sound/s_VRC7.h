#ifndef	__SND_VRC7_H__
#define	__SND_VRC7_H__

void	VRC7sound_Init (void);
void	VRC7sound_Write (int,int);
int	_MAPINT	VRC7sound_Get (int);
int	_MAPINT	VRC7sound_SaveLoad (int,int,char *);
void	VRC7sound_Destroy (void);

#endif	/* __SND_VRC7_H__ */
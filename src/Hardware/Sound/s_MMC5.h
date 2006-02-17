#ifndef	__SND_MMC5_H__
#define	__SND_MMC5_H__

void	MMC5sound_Init (void);
void	MMC5sound_Write (int,int);
int	_MAPINT	MMC5sound_Get (int);
int	_MAPINT	MMC5sound_SaveLoad (int,int,char *);
void	MMC5sound_Destroy (void);

#endif	/* __SND_MMC5_H__ */
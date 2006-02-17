#ifndef	S_MMC5_H
#define	S_MMC5_H

void	MMC5sound_Init (void);
void	MMC5sound_Write (int,int);
int	_MAPINT	MMC5sound_Get (int);
int	_MAPINT	MMC5sound_SaveLoad (int,int,char *);
void	MMC5sound_Destroy (void);

#endif	/* S_MMC5_H */
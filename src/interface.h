/*
 * Nintendulator Mapper Interface v3.0
 */

#ifndef	WIN32_LEAN_AND_MEAN
#define	WIN32_LEAN_AND_MEAN
#endif

/* So this file only gets included once */

#ifndef	__INTERFACE_H__
#define	__INTERFACE_H__

/* Standard header files, used by all mappers */

#include	<windows.h>

#define	MSGBOX_FLAGS	(MB_OK | MB_ICONERROR | MB_DEFBUTTON1 | MB_APPLMODAL)

/* Mapper Interface version (3.1) */

#define	CurrentMapperInterface 0x00030001

/* Integer types */

typedef	signed __int8		s8;
typedef	signed __int16		s16;
typedef	signed __int32		s32;
typedef	signed __int64		s64;

typedef	unsigned __int8		u8;
typedef	unsigned __int16	u16;
typedef	unsigned __int32	u32;
typedef	unsigned __int64	u64;

typedef	union
{
	struct
	{
		unsigned n0 : 4;
		unsigned n1 : 4;
	};
	struct
	{
		u8 b[1];
	};
}	u8_n;

typedef	union
{
	struct
	{
		unsigned n0 : 4;
		unsigned n1 : 4;
		unsigned n2 : 4;
		unsigned n3 : 4;
	};
	struct
	{
		u8 b[2];
	};
	struct
	{
		u16 s[1];
	};
}	u16_n;

typedef	union
{
	struct
	{
		unsigned n0 : 4;
		unsigned n1 : 4;
		unsigned n2 : 4;
		unsigned n3 : 4;
		unsigned n4 : 4;
		unsigned n5 : 4;
		unsigned n6 : 4;
		unsigned n7 : 4;
	};
	struct
	{
		u8 b[4];
	};
	struct
	{
		u16 s[2];
	};
	struct
	{
		u32 l[1];
	};
}	u32_n;

#define b0 b[0]
#define b1 b[1]
#define b2 b[2]
#define b3 b[3]

#define s0 s[0]
#define s1 s[1]

#define l0 l[0]

/* Function types */

#define	_MAPINT	__cdecl

typedef	void	(_MAPINT *FCPUWrite)	(int Bank,int Addr,int Val);
typedef	int	(_MAPINT *FCPURead)	(int Bank,int Addr);

typedef	void	(_MAPINT *FPPUWrite)	(int Bank,int Addr,int Val);
typedef	int	(_MAPINT *FPPURead)	(int Bank,int Addr);

/* Mapper Interface Structure - Pointers to data and functions within emulator */

typedef	struct	EmulatorInterface
{
	/* Functions for managing read/write handlers */
		void		(_MAPINT *SetCPUReadHandler)	(int,FCPURead);
		void		(_MAPINT *SetCPUWriteHandler)	(int,FCPUWrite);
		FCPURead	(_MAPINT *GetCPUReadHandler)	(int);
		FCPUWrite	(_MAPINT *GetCPUWriteHandler)	(int);

		void		(_MAPINT *SetPPUReadHandler)	(int,FPPURead);
		void		(_MAPINT *SetPPUWriteHandler)	(int,FPPUWrite);
		FPPURead	(_MAPINT *GetPPUReadHandler)	(int);
		FPPUWrite	(_MAPINT *GetPPUWriteHandler)	(int);

	/* Functions for mapping PRG */
		void		(_MAPINT *SetPRG_ROM4)		(int,int);
		void		(_MAPINT *SetPRG_ROM8)		(int,int);
		void		(_MAPINT *SetPRG_ROM16)		(int,int);
		void		(_MAPINT *SetPRG_ROM32)		(int,int);
		int		(_MAPINT *GetPRG_ROM4)		(int);		/* -1 if no ROM mapped */
#define		GetPRG_ROM8(x)	GetPRG_ROM4(x) >> 1
#define		GetPRG_ROM16(x)	GetPRG_ROM4(x) >> 2
		void		(_MAPINT *SetPRG_RAM4)		(int,int);
		void		(_MAPINT *SetPRG_RAM8)		(int,int);
		void		(_MAPINT *SetPRG_RAM16)		(int,int);
		void		(_MAPINT *SetPRG_RAM32)		(int,int);
		int		(_MAPINT *GetPRG_RAM4)		(int);		/* -1 if no RAM mapped */
#define		GetPRG_RAM8(x)	GetPRG_RAM4(x) >> 1

		char *		(_MAPINT *GetPRG_Ptr4)		(int);
		void		(_MAPINT *SetPRG_OB4)		(int);		/* Open bus */

	/* Functions for mapping CHR */
		void		(_MAPINT *SetCHR_ROM1)		(int,int);
		void		(_MAPINT *SetCHR_ROM2)		(int,int);
		void		(_MAPINT *SetCHR_ROM4)		(int,int);
		void		(_MAPINT *SetCHR_ROM8)		(int,int);
		int		(_MAPINT *GetCHR_ROM1)		(int);		/* -1 if no ROM mapped */

		void		(_MAPINT *SetCHR_RAM1)		(int,int);
		void		(_MAPINT *SetCHR_RAM2)		(int,int);
		void		(_MAPINT *SetCHR_RAM4)		(int,int);
		void		(_MAPINT *SetCHR_RAM8)		(int,int);
		int		(_MAPINT *GetCHR_RAM1)		(int);		/* -1 if no RAM mapped */

		char *		(_MAPINT *GetCHR_Ptr1)		(int);
		void		(_MAPINT *SetCHR_OB1)		(int);		/* Open bus */

	/* Functions for controlling mirroring */
		void		(_MAPINT *Mirror_H)		(void);
		void		(_MAPINT *Mirror_V)		(void);
		void		(_MAPINT *Mirror_4)		(void);
		void		(_MAPINT *Mirror_S0)		(void);
		void		(_MAPINT *Mirror_S1)		(void);
		void		(_MAPINT *Mirror_Custom)	(int,int,int,int);

	/* IRQ */
		void		(_MAPINT *SetIRQ)		(int);		/* Sets the state of the /IRQ line */

	/* Save RAM Handling */
		void		(_MAPINT *Set_SRAMSize)		(int);		/* Sets the size of the SRAM (in bytes) */
		void		(_MAPINT *Save_SRAM)		(void);		/* Saves SRAM to disk */
		void		(_MAPINT *Load_SRAM)		(void);		/* Loads SRAM from disk */

	/* Misc Callbacks */
		void		(_MAPINT *DbgOut)		(char *);	/* Echo text to debug window */
		void		(_MAPINT *StatusOut)		(char *);	/* Echo text on status bar */
}	TEmulatorInterface, *PEmulatorInterface;
typedef	const	TEmulatorInterface	CTEmulatorInterface, *CPEmulatorInterface;

#define	COMPAT_NONE	0
#define	COMPAT_PARTIAL	1
#define	COMPAT_NEARLY	2
#define	COMPAT_FULL	3

/* Mapper Information structure - Contains pointers to mapper functions, sent to emulator on load mapper  */

typedef	struct	MapperInfo
{
	/* Mapper Information */
		void *		MapperId;
		char *		Description;
		int		Compatibility;

	/* Mapper Functions */
		void		(_MAPINT *Reset)	(int);			/* IsHardReset */
		void		(_MAPINT *Shutdown)	(void);
		void		(_MAPINT *CPUCycle)	(void);
		void		(_MAPINT *PPUCycle)	(int,int,int,int);	/* Address, Scanline, Cycle, IsRendering */
		int		(_MAPINT *SaveLoad)	(int,int,char *);	/* Mode, Offset, Data */
		int		(_MAPINT *GenSound)	(int);			/* Cycles */
		void		(_MAPINT *Config)	(void);
}	TMapperInfo, *PMapperInfo;
typedef	const	TMapperInfo	CTMapperInfo, *CPMapperInfo;

#define	STATE_SAVE	0
#define	STATE_LOAD	1
#define	STATE_SIZE	2

#define	SAVELOAD_BYTE(mode,x,data,value) \
{ \
	if (mode == STATE_SAVE) data[x++] = value; \
	else if (mode == STATE_LOAD) value = data[x++]; \
	else if (mode == STATE_SIZE) x++; \
	else MessageBox(hWnd,"Invalid save/load type!",__FILE__,MB_OK); \
}
#define	SAVELOAD_WORD(mode,x,data,value) \
{ \
	u16_n sl_tmp; \
	if (mode == STATE_SAVE) { sl_tmp.s0 = value; data[x++] = sl_tmp.b0; data[x++] = sl_tmp.b1; } \
	else if (mode == STATE_LOAD) { sl_tmp.b0 = data[x++]; sl_tmp.b1 = data[x++]; value = sl_tmp.s0; } \
	else if (mode == STATE_SIZE) x += 2; \
	else MessageBox(hWnd,"Invalid save/load type!",__FILE__,MB_OK); \
}
#define	SAVELOAD_LONG(mode,x,data,value) \
{ \
	u32_n sl_tmp; \
	if (mode == STATE_SAVE) { sl_tmp.l0 = value; data[x++] = sl_tmp.b0; data[x++] = sl_tmp.b1; data[x++] = sl_tmp.b2; data[x++] = sl_tmp.b3; } \
	else if (mode == STATE_LOAD) { sl_tmp.b0 = data[x++]; sl_tmp.b1 = data[x++]; sl_tmp.b2 = data[x++]; sl_tmp.b3 = data[x++]; value = sl_tmp.l0; } \
	else if (mode == STATE_SIZE) x += 4; \
	else MessageBox(hWnd,"Invalid save/load type!",__FILE__,MB_OK); \
}

/* ROM Information Structure:- Contains information about the ROM currently loaded */

#define	ROM_UNDEFINED	0
#define	ROM_INES	1
#define	ROM_UNIF	2
#define	ROM_FDS		3
#define	ROM_NSF		4

typedef	struct	ROMInfo
{
	char *	Filename;
	int	ROMType;
	union
	{
		struct
		{
			BYTE	INES_MapperNum;
			BYTE	INES_Flags;	// byte 6 flags in lower 4 bits, byte 7 flags in upper 4 bits
			BYTE	INES_PRGSize;	// number of 16KB banks
			BYTE	INES_CHRSize;	// number of 8KB banks
		};	// INES
		struct
		{
			char *	UNIF_BoardName;
			BYTE	UNIF_Mirroring;
			BYTE	UNIF_NTSCPAL;	// 0 for NTSC, 1 for PAL, 2 for either
			BOOL	UNIF_Battery;
			BYTE	UNIF_NumPRG;	// number of PRG# blocks
			BYTE	UNIF_NumCHR;	// number of CHR# blocks
			DWORD	UNIF_PRGSize[16];	// size of each PRG block, in bytes
			DWORD	UNIF_CHRSize[16];	// size of each CHR block, in bytes
		};	// UNIF
		struct
		{
			BYTE	FDS_NumSides;
		};	// FDS
		struct
		{
			BYTE	NSF_NumSongs;
			BYTE	NSF_InitSong;
			WORD	NSF_InitAddr;
			WORD	NSF_PlayAddr;
			char *	NSF_Title;
			char *	NSF_Artist;
			char *	NSF_Copyright;
			WORD	NSF_NTSCSpeed;	// Number of microseconds between PLAY calls for NTSC
			WORD	NSF_PALSpeed;	// Number of microseconds between PLAY calls for PAL
			BYTE	NSF_NTSCPAL;	// 0 for NTSC, 1 for PAL, 2 for either
			BYTE	NSF_SoundChips;
			BYTE	NSF_InitBanks[8];
			DWORD	NSF_DataSize;	// total amount of (PRG) data present
		};	// NSF
		struct
		{
			BYTE	reserved[256];
		};	// reserved for additional file types
	};
}	TROMInfo, *PROMInfo;
typedef	const	TROMInfo	CTROMInfo, *CPROMInfo;

/* DLL Information Structure:- Contains general information about the mapper DLL */

typedef	struct	DLLInfo
{
		char *		Author;
		int		Date;
		int		Version;
		CPMapperInfo	(_MAPINT *LoadMapper)	(CPROMInfo);
		void		(_MAPINT *UnloadMapper)	(void);
}	TDLLInfo, *PDLLInfo;

extern	HWND			hWnd;
extern	HINSTANCE		hInstance;
extern	CPEmulatorInterface	EMU;
extern	CPROMInfo		ROM;

#endif	/* __INTERFACE_H__ */
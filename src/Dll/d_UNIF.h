/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#pragma once

#include	"..\interface.h"

void	UNIF_SetMirroring (FSync);
void	UNIF_SetSRAM (int);

extern	const MapperInfo
/* NES/HVC: Official Nintendo boards */
	MapperInfo_NES_AMROM,
	MapperInfo_NES_ANROM,
	MapperInfo_NES_AN1ROM,
	MapperInfo_NES_AOROM,

	MapperInfo_NES_BNROM,

	MapperInfo_NES_CNROM,
	MapperInfo_NES_CPROM,

	MapperInfo_NES_DEROM,
	MapperInfo_NES_DEIROM,
	MapperInfo_NES_DRROM,

	MapperInfo_NES_EKROM,
	MapperInfo_NES_ELROM,
	MapperInfo_NES_ETROM,
	MapperInfo_NES_EWROM,

	MapperInfo_NES_FJROM,
	MapperInfo_NES_FKROM,

	MapperInfo_NES_GNROM,

	MapperInfo_NES_HKROM,

	MapperInfo_NES_JLROM,
	MapperInfo_NES_JSROM,
	MapperInfo_NES_BTR,

	MapperInfo_NES_MHROM,

	MapperInfo_NES_NROM_128,
	MapperInfo_NES_RROM_128,
	MapperInfo_NES_NROM_256,

	MapperInfo_NES_PEEOROM,
	MapperInfo_NES_PNROM,

	MapperInfo_NES_SAROM,
	MapperInfo_NES_SBROM,
	MapperInfo_NES_SCROM,
	MapperInfo_NES_SC1ROM,
	MapperInfo_NES_SEROM,
	MapperInfo_NES_SFROM,
	MapperInfo_NES_SGROM,
	MapperInfo_NES_SHROM,
	MapperInfo_NES_SH1ROM,
	MapperInfo_NES_SIROM,
	MapperInfo_NES_SJROM,
	MapperInfo_NES_SKROM,
	MapperInfo_NES_SLROM,
	MapperInfo_NES_SL1ROM,
	MapperInfo_NES_SL2ROM,
	MapperInfo_NES_SL3ROM,
	MapperInfo_NES_SLRROM,
	MapperInfo_NES_SMROM,
	MapperInfo_NES_SNROM,
	MapperInfo_NES_SOROM,
	MapperInfo_NES_SUROM,
	MapperInfo_NES_SXROM,

	MapperInfo_NES_TBROM,
	MapperInfo_NES_TEROM,
	MapperInfo_NES_TFROM,
	MapperInfo_NES_TGROM,
	MapperInfo_NES_TKROM,
	MapperInfo_NES_TKSROM,
	MapperInfo_NES_TLROM,
	MapperInfo_NES_TL1ROM,
	MapperInfo_NES_TL2ROM,
	MapperInfo_NES_TLSROM,
	MapperInfo_NES_TNROM,
	MapperInfo_NES_TQROM,
	MapperInfo_NES_TR1ROM,
	MapperInfo_NES_TSROM,
	MapperInfo_NES_TVROM,

	MapperInfo_NES_UNROM,
	MapperInfo_NES_UOROM,

/* UNL: Unlicensed (legal) boards */
	MapperInfo_UNL_SA_0161M,
	MapperInfo_UNL_SA_72007,
	MapperInfo_UNL_SA_72008,
	MapperInfo_UNL_TC_U0115M,
	MapperInfo_UNL_Sachen_74LS374N,
	MapperInfo_UNL_SA_0036,
	MapperInfo_UNL_SA_0037,
	MapperInfo_UNL_Sachen_8259A,
	MapperInfo_UNL_Sachen_8259B,
	MapperInfo_UNL_H2288,
	MapperInfo_UNL_A9712,
	MapperInfo_UNL_8237,
	MapperInfo_UNL_DRIPGAME,

/* BTL: Pirate simgle-game carts */
	MapperInfo_BTL_MARIO1_MALEE2,

/* BMC: Pirate multicarts */
	MapperInfo_BMC_1991SuperHiK7in1,
	MapperInfo_BMC_1992Ballgames11in1,
	MapperInfo_BMC_Generic115in1,
	MapperInfo_BMC_Generic15in1,	
	MapperInfo_BMC_Generic20in1,
	MapperInfo_BMC_Generic20in1A,
	MapperInfo_BMC_Generic42in1,
	MapperInfo_BMC_Generic52in1,
	MapperInfo_BMC_Generic76in1,
	MapperInfo_BMC_GoldenGame150in1,
	MapperInfo_BMC_Mario7in1,
	MapperInfo_BMC_NovelDiamond9999999in1,
	MapperInfo_BMC_Reset4in1,
	MapperInfo_BMC_StudyGame32in1,
	MapperInfo_BMC_Super1Min1,
	MapperInfo_BMC_Super700in1,
	MapperInfo_BMC_Supervision16in1,
	MapperInfo_BMC_T3H53,
	MapperInfo_BMC_Super24in1SC03,
	MapperInfo_BMC_SV01,
	MapperInfo_BMC_WS
	;

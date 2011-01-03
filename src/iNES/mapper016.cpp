/* Nintendulator Mapper DLLs
 * Copyright (C) 2002-2011 QMT Productions
 *
 * $URL$
 * $Id$
 */

#include	"..\DLL\d_iNES.h"

#define EEP_DIR	0x80
#define EEP_DAT	0x40
#define EEP_CLK	0x20


class EEPROM
{
public:
	virtual	int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data) = 0;
	virtual	void	Write (int Val) = 0;
	virtual	int	Read (void) = 0;
};

class EEPROM_24C02 : public EEPROM
{
protected:
	uint8 LastBits;
	uint8 State;
	uint8 *EEP;
	uint8 Addr, Data;
	uint8 BitPtr;
public:
	EEPROM_24C02 (void)
	{
		EMU->Set_SRAMSize(256);
		EMU->SetPRG_RAM4(0x6, 0);
		EEP = EMU->GetPRG_Ptr4(0x6);
		EMU->SetPRG_OB4(0x6);
		State = 0;
		LastBits = 0;
	}
	virtual	int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
	{
		SAVELOAD_BYTE(mode, offset, data, LastBits);
		SAVELOAD_BYTE(mode, offset, data, State);
		SAVELOAD_BYTE(mode, offset, data, Addr);
		SAVELOAD_BYTE(mode, offset, data, Data);
		SAVELOAD_BYTE(mode, offset, data, BitPtr);
		return offset;
	}
	virtual	void	Write (int Val)
	{
		// rising edge on data line while clock is high == stop data transfer
		if ((LastBits & Val & EEP_CLK) && !(LastBits & EEP_DAT) && (Val & EEP_DAT))
			State = 0;
		// falling edge on data line while clock is high == start data transfer
		if ((LastBits & Val & EEP_CLK) && (LastBits & EEP_DAT) && !(Val & EEP_DAT))
			State = 1;
		// from this point forward, we only care about falling edges on CLK
		if (!((LastBits & EEP_CLK) && !(Val & EEP_CLK)))
		{
			LastBits = Val;
			return;
		}
		switch (State)
		{
		case 1:	// Catch CLK falling edge
			BitPtr = 0;
			State++;
			break;

		case 2:	// Get slave address: 1010xxxM
			switch (BitPtr++)
			{
			case 0:	case 2:
				if (!(Val & EEP_DAT))
					State = 0;
				break;
			case 1:	case 3:
				if (Val & EEP_DAT)
					State = 0;
				break;
			case 4:	case 5:	case 6:
				break;
			case 7:
				State++;
				// use BitPtr to hold R/W state until we get the following Ack
				if (Val & EEP_DAT)
					BitPtr = 1;
				else	BitPtr = 0;
				break;
			}
			break;
		case 3:	// Ack
			if (BitPtr)
			{
				// perform Read
				State = 8;
				Data = EEP[Addr];
			}
			else
			{
				// set Address
				State = 4;
				Addr = 0;
			}
			BitPtr = 0;
			break;

		case 4:	// Set Address
			if (Val & EEP_DAT)
				Addr |= 0x80 >> BitPtr;
			BitPtr++;
			if (BitPtr == 8)
				State++;
			break;
		case 5:	// Ack
			BitPtr = 0;
			State++;
			break;

		case 6:	// Write Data
			if (Val & EEP_DAT)
				Data |= 0x80 >> BitPtr;
			BitPtr++;
			if (BitPtr == 8)
				State++;
			break;
		case 7:	// Ack
			EEP[Addr] = Data;
			Addr = (Addr & 0xF8) | ((Addr + 1) & 0x7);
			Data = 0;
			State = 6;
			break;

		case 8:	// Read Data
			if (Val & EEP_DAT)
				BitPtr++;
			if (BitPtr == 8)
				State++;
			break;
		case 9:	// Ack
			State = 8;
			Addr++;
			Data = EEP[Addr];
			break;
		}
		LastBits = Val;
	};
	virtual	int	Read (void)
	{
		// Read Data
		if ((State == 8) && (LastBits & EEP_DIR) && (LastBits & EEP_CLK) && (LastBits & EEP_DAT))
			return (Data & (0x80 >> BitPtr)) ? 0x10 : 0x00;
		return 0;
	}
};

class EEPROM_24C01 : public EEPROM
{
protected:
	uint8 LastBits;
	uint8 State;
	uint8 *EEP;
	uint8 Addr, Data;
	uint8 BitPtr;
public:
	EEPROM_24C01 (void)
	{
		EMU->Set_SRAMSize(128);
		EMU->SetPRG_RAM4(0x6, 0);
		EEP = EMU->GetPRG_Ptr4(0x6);
		EMU->SetPRG_OB4(0x6);
		State = 0;
		LastBits = 0;
	}
	virtual	int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
	{
		SAVELOAD_BYTE(mode, offset, data, LastBits);
		SAVELOAD_BYTE(mode, offset, data, State);
		SAVELOAD_BYTE(mode, offset, data, Addr);
		SAVELOAD_BYTE(mode, offset, data, Data);
		SAVELOAD_BYTE(mode, offset, data, BitPtr);
		return offset;
	}
	virtual	void	Write (int Val)
	{
		// rising edge on data line while clock is high == stop data transfer
		if ((LastBits & Val & EEP_CLK) && !(LastBits & EEP_DAT) && (Val & EEP_DAT))
			State = 0;
		// falling edge on data line while clock is high == start data transfer
		if ((LastBits & Val & EEP_CLK) && (LastBits & EEP_DAT) && !(Val & EEP_DAT))
			State = 1;
		// from this point forward, we only care about falling edges on CLK
		if (!((LastBits & EEP_CLK) && !(Val & EEP_CLK)))
		{
			LastBits = Val;
			return;
		}
		switch (State)
		{
		case 1:	// Catch CLK falling edge
			BitPtr = 0;
			Addr = 0;
			State++;
			break;

		case 2:	// Set Address
			if (Val & EEP_DAT)
				Addr |= 1 << BitPtr;
			BitPtr++;
			if (BitPtr == 7)
				State++;
			break;
		case 3:	// Set R/W
			State++;
			// use BitPtr to hold R/W state until we get the following Ack
			if (Val & EEP_DAT)
				BitPtr = 1;
			else	BitPtr = 0;
			break;
		case 4:	// Ack
			if (BitPtr)
			{
				// perform Read
				State = 7;
				Data = EEP[Addr];
			}
			else
			{
				// perform Write
				State = 5;
				Data = 0;
			}
			BitPtr = 0;
			break;
		case 5:	// Write Data
			if (Val & EEP_DAT)
				Data |= 1 << BitPtr;
			BitPtr++;
			if (BitPtr == 8)
				State++;
			break;
		case 6:	// Ack
			EEP[Addr] = Data;
			Addr = (Addr & 0x7C) | ((Addr + 1) & 0x3);
			Data = 0;
			State = 5;
			break;

		case 7:	// Read Data
			if (Val & EEP_DAT)
				BitPtr++;
			if (BitPtr == 8)
				State++;
			break;
		case 8:	// Ack
			State = 7;
			Addr = (Addr + 1) & 0x7F;
			Data = EEP[Addr];
			break;
		}
		LastBits = Val;
	};
	virtual	int	Read (void)
	{
		// Read Data
		if ((State == 7) && (LastBits & EEP_DIR) && (LastBits & EEP_CLK) && (LastBits & EEP_DAT))
			return (Data & (1 << BitPtr)) ? 0x10 : 0x00;
		return 0;
	}
};
namespace
{
uint8 PRG, CHR[8], Mirror;
uint8 IRQenabled;
uint16_n IRQcounter;
EEPROM *SaveEEPROM;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);
	for (int i = 0; i < 8; i++)
	{
		if (ROM->INES_CHRSize)
			EMU->SetCHR_ROM1(i, CHR[i]);
		else	EMU->SetCHR_RAM1(i, CHR[i] & 7);
	}
	switch (Mirror)
	{
	case 0:	EMU->Mirror_V();	break;
	case 1:	EMU->Mirror_H();	break;
	case 2:	EMU->Mirror_S0();	break;
	case 3:	EMU->Mirror_S1();	break;
	}
}

int	MAPINT	SaveLoad (STATE_TYPE mode, int offset, unsigned char *data)
{
	SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	offset = SaveEEPROM->SaveLoad(mode, offset, data);
	if (mode == STATE_LOAD)
		Sync();
	return offset;
}

void	MAPINT	CPUCycle (void)
{
	if ((IRQenabled) && (!--IRQcounter.s0))
		EMU->SetIRQ(0);
}

int	MAPINT	Read (int Bank, int Addr)
{
	if ((Addr & 0xF) == 0)
		return SaveEEPROM->Read();
	else	return -1;
}

void	MAPINT	Write (int Bank, int Addr, int Val)
{
	switch (Addr & 0xF)
	{
	case 0x0:	CHR[0] = Val;		break;
	case 0x1:	CHR[1] = Val;		break;
	case 0x2:	CHR[2] = Val;		break;
	case 0x3:	CHR[3] = Val;		break;
	case 0x4:	CHR[4] = Val;		break;
	case 0x5:	CHR[5] = Val;		break;
	case 0x6:	CHR[6] = Val;		break;
	case 0x7:	CHR[7] = Val;		break;
	case 0x8:	PRG = Val;		break;
	case 0x9:	Mirror = Val & 0x3;	break;
	case 0xA:	IRQenabled = Val & 1;
			EMU->SetIRQ(1);		break;
	case 0xB:	IRQcounter.b0 = Val;	break;
	case 0xC:	IRQcounter.b1 = Val;	break;
	case 0xD:	SaveEEPROM->Write(Val);	break;
	}
	Sync();
}

void	MAPINT	Load_016 (void)
{
	SaveEEPROM = new EEPROM_24C02();
}

void	MAPINT	Load_159 (void)
{
	SaveEEPROM = new EEPROM_24C01();
}

void	MAPINT	Reset (RESET_TYPE ResetType)
{
	for (int i = 0x6; i < 0x8; i++)
		EMU->SetCPUReadHandler(i, Read);
	for (int i = 0x6; i < 0x10; i++)
		EMU->SetCPUWriteHandler(i, Write);

	if (ResetType == RESET_HARD)
	{
		PRG = 0;
		for (int i = 0; i < 8; i++)
			CHR[i] = i;
		IRQenabled = 0;
		IRQcounter.s0 = 0;
	}
	Sync();
}
void	MAPINT	Unload (void)
{
	delete SaveEEPROM;
	SaveEEPROM = NULL;
}

uint8 MapperNum = 16;
uint8 MapperNum2 = 159;
} // namespace

const MapperInfo MapperInfo_016 =
{
	&MapperNum,
	_T("Bandai + 24C02"),
	COMPAT_FULL,
	Load_016,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};

const MapperInfo MapperInfo_159 =
{
	&MapperNum2,
	_T("Bandai + 24C01"),
	COMPAT_FULL,
	Load_159,
	Reset,
	Unload,
	CPUCycle,
	NULL,
	SaveLoad,
	NULL,
	NULL
};
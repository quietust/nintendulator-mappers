/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
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
	uint8_t LastBits;
	uint8_t State;
	uint8_t *EEP;
	uint8_t Addr, Data;
	uint8_t BitPtr;
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
		uint8_t ver = 0;
		CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

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
			Data = 0;
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
			// Some 24C02 chips (e.g. from Microchip) use 8 or 16-byte pages, but
			// Xicor chips (which Bandai used consistently) only use 4-byte pages
			Addr = (Addr & 0xFC) | ((Addr + 1) & 0x3);
			BitPtr = 0;
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
		// Ack states
		else if ((State == 3) || (State == 5) || (State == 7) || (State == 9))
			return 0x00;
		// Data pin is open drain
		else	return 0x10;
	}
};

class EEPROM_24C01 : public EEPROM
{
protected:
	uint8_t LastBits;
	uint8_t State;
	uint8_t *EEP;
	uint8_t Addr, Data;
	uint8_t BitPtr;

	static const unsigned char AddrFlip[128]; // see below

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
		uint8_t ver = 0;
		CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

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
			// NOTE: Bandai operates this chip as if the address and data are
			// little-endian, but it turns out they're actually big-endian.
			// We will still treat them as little endian, though, both for
			// compatibility with previous savestates and so that the resulting
			// .SAV file will be intelligible if opened in a hex-editor.
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
			// NOTE: due to the above "little endian" treatment, we need to increment
			// the address "backwards" so that multi-byte I/O works correctly.
			// Fortunately, no released games ever actually did this.
			Addr = AddrFlip[Addr];
			Addr = (Addr & 0x7C) | ((Addr + 1) & 0x3);
			Addr = AddrFlip[Addr];
			BitPtr = 0;
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
			BitPtr = 0;
			// NOTE: See above
			Addr = AddrFlip[Addr];
			Addr = (Addr + 1) & 0x7F;
			Addr = AddrFlip[Addr];
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

const unsigned char EEPROM_24C01::AddrFlip[128] = {
	0x00,0x40,0x20,0x60,0x10,0x50,0x30,0x70,0x08,0x48,0x28,0x68,0x18,0x58,0x38,0x78,
	0x04,0x44,0x24,0x64,0x14,0x54,0x34,0x74,0x0C,0x4C,0x2C,0x6C,0x1C,0x5C,0x3C,0x7C,
	0x02,0x42,0x22,0x62,0x12,0x52,0x32,0x72,0x0A,0x4A,0x2A,0x6A,0x1A,0x5A,0x3A,0x7A,
	0x06,0x46,0x26,0x66,0x16,0x56,0x36,0x76,0x0E,0x4E,0x2E,0x6E,0x1E,0x5E,0x3E,0x7E,
	0x01,0x41,0x21,0x61,0x11,0x51,0x31,0x71,0x09,0x49,0x29,0x69,0x19,0x59,0x39,0x79,
	0x05,0x45,0x25,0x65,0x15,0x55,0x35,0x75,0x0D,0x4D,0x2D,0x6D,0x1D,0x5D,0x3D,0x7D,
	0x03,0x43,0x23,0x63,0x13,0x53,0x33,0x73,0x0B,0x4B,0x2B,0x6B,0x1B,0x5B,0x3B,0x7B,
	0x07,0x47,0x27,0x67,0x17,0x57,0x37,0x77,0x0F,0x4F,0x2F,0x6F,0x1F,0x5F,0x3F,0x7F
};

namespace
{
uint8_t PRG, CHR[8], Mirror;
uint8_t IRQenabled;
uint16_n IRQcounter;
EEPROM *SaveEEPROM;

void	Sync (void)
{
	EMU->SetPRG_ROM16(0x8, PRG);
	EMU->SetPRG_ROM16(0xC, -1);
	for (int i = 0; i < 8; i++)
		iNES_SetCHR_Auto1(i, CHR[i]);
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
	uint8_t ver = 0;
	CheckSave(SAVELOAD_VERSION(mode, offset, data, ver));

	SAVELOAD_WORD(mode, offset, data, IRQcounter.s0);
	SAVELOAD_BYTE(mode, offset, data, IRQenabled);
	SAVELOAD_BYTE(mode, offset, data, PRG);
	for (int i = 0; i < 8; i++)
		SAVELOAD_BYTE(mode, offset, data, CHR[i]);
	SAVELOAD_BYTE(mode, offset, data, Mirror);
	CheckSave(offset = SaveEEPROM->SaveLoad(mode, offset, data));

	if (IsLoad(mode))
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
	return SaveEEPROM->Read();
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

BOOL	MAPINT	Load_016 (void)
{
	SaveEEPROM = new EEPROM_24C02();
	return (SaveEEPROM != NULL);
}
BOOL	MAPINT	Load_159 (void)
{
	SaveEEPROM = new EEPROM_24C01();
	return (SaveEEPROM != NULL);
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

uint16_t MapperNum = 16;
uint16_t MapperNum2 = 159;
} // namespace

const MapperInfo MapperInfo_016
(
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
);
const MapperInfo MapperInfo_159
(
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
);

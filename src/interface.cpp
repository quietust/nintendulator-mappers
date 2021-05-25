/* Nintendulator Mapper DLLs
 * Copyright (C) QMT Productions
 */

#include "interface.h"
#include <string.h>
#include <vector>

std::vector<MapperInfo *> *mapperList = NULL;

MapperInfo::MapperInfo (
	void *_MapperId,
	TCHAR *_Description,
	COMPAT_TYPE _Compatibility,
	BOOL (MAPINT *_Load) (void),
	void (MAPINT *_Reset) (RESET_TYPE),
	void (MAPINT *_Unload) (void),
	void (MAPINT *_CPUCycle) (void),
	void (MAPINT *_PPUCycle) (int,int,int,int),
	int (MAPINT *_SaveLoad) (STATE_TYPE,int,unsigned char *),
	int (MAPINT *_GenSound) (int),
	unsigned char (MAPINT *_Config) (CFG_TYPE,unsigned char)
) :
	MapperId(_MapperId),
	Description(_Description),
	Compatibility(_Compatibility),
	Load(_Load),
	Reset(_Reset),
	Unload(_Unload),
	CPUCycle(_CPUCycle),
	PPUCycle(_PPUCycle),
	SaveLoad(_SaveLoad),
	GenSound(_GenSound),
	Config(_Config)
{
	if (mapperList == NULL)
		mapperList = new std::vector<MapperInfo *>();
	mapperList->push_back(this);
}

const MapperInfo *findByIndex (unsigned int idx)
{
	if (mapperList == NULL)
		return NULL;
	if ((idx >= 0) && (idx < mapperList->size()))
		return mapperList->at(idx);
	return NULL;
}

const MapperInfo *findByNumber (uint16_t MapperNum)
{
	if (mapperList == NULL)
		return NULL;
	for (unsigned int i = 0; i < mapperList->size(); i++)
	{
		const MapperInfo *cur = mapperList->at(i);
		if (!cur->MapperId)
			continue;
		if (*(uint16_t *)(cur->MapperId) != MapperNum)
			continue;
		return cur;
	}
	return NULL;
}

const MapperInfo *findByName (const char *BoardName)
{
	if (mapperList == NULL)
		return NULL;

	bool MatchAlt = false;
	// If the board name is long enough and has a recognized prefix, allow alternate matching against "NES-"
	if ((strlen(BoardName) >= 5) && (!strncmp(BoardName, "BTL-", 4) || !strncmp(BoardName, "HVC-", 4) || !strncmp(BoardName, "UNL-", 4)))
		MatchAlt = true;
	
	for (unsigned int i = 0; i < mapperList->size(); i++)
	{
		const MapperInfo *cur = mapperList->at(i);
		if (!cur->MapperId)
			continue;
		// First try a direct match
		if (!strcmp((char *)(cur->MapperId), BoardName))
			return cur;
		// If the board name has a recognized prefix, translate that prefix to "NES-" and attempt to rematch
		if (MatchAlt)
		{
			if (strlen((char *)cur->MapperId) < 5)
				continue;
			if (strncmp((char *)(cur->MapperId), "NES-", 4))
				continue;
			if (!strcmp((char *)(cur->MapperId) + 4, BoardName + 4))
				return cur;
		}
		
	}
	return NULL;
}

int SAVELOAD_VERSION(STATE_TYPE mode, int &offset, unsigned char *data, uint8_t &cur_ver)
{
	switch (mode)
	{
	case STATE_SAVE:
		data[offset++] = cur_ver;
		break;
	case STATE_LOAD_VER:
		if (data[offset] > cur_ver)
			return -1;
		cur_ver = data[offset++];
		break;
	case STATE_LOAD:
		cur_ver = 0;
		break;
	case STATE_SIZE:
		offset++;
		break;
	default:
		MessageBox(hWnd, _T("Invalid save/load type!"), _T("Mapper DLL"), MB_OK);
	}
	return 0;
}

void SAVELOAD_BYTE(STATE_TYPE mode, int &offset, unsigned char *data, uint8_t &value)
{
	switch (mode)
	{
	case STATE_SAVE:
		data[offset++] = value;
		break;
	case STATE_LOAD_VER:
	case STATE_LOAD:
		value = data[offset++];
		break;
	case STATE_SIZE:
		offset++;
		break;
	default:
		MessageBox(hWnd, _T("Invalid save/load type!"), _T("Mapper DLL"), MB_OK);
	}
}
void SAVELOAD_BYTE(STATE_TYPE mode, int &offset, unsigned char *data, int8_t &value)
{
	uint8_t _value = value;
	SAVELOAD_BYTE(mode, offset, data, _value);
	value = _value;
}
void SAVELOAD_BYTE(STATE_TYPE mode, int &offset, unsigned char *data, int &value)
{
	uint8_t _value = value;
	SAVELOAD_BYTE(mode, offset, data, _value);
	value = _value;
}

void SAVELOAD_WORD(STATE_TYPE mode, int &offset, unsigned char *data, uint16_t &value)
{
	uint16_n sl_tmp;
	switch (mode)
	{
	case STATE_SAVE:
		sl_tmp.s0 = value;
		data[offset++] = sl_tmp.b0;
		data[offset++] = sl_tmp.b1;
		break;
	case STATE_LOAD_VER:
	case STATE_LOAD:
		sl_tmp.b0 = data[offset++];
		sl_tmp.b1 = data[offset++];
		value = sl_tmp.s0;
		break;
	case STATE_SIZE:
		offset += 2;
		break;
	default:
		MessageBox(hWnd, _T("Invalid save/load type!"), _T("Mapper DLL"), MB_OK);
	}
}
void SAVELOAD_WORD(STATE_TYPE mode, int &offset, unsigned char *data, int16_t &value)
{
	uint16_t _value = value;
	SAVELOAD_WORD(mode, offset, data, _value);
	value = _value;
}
void SAVELOAD_WORD(STATE_TYPE mode, int &offset, unsigned char *data, int &value)
{
	uint16_t _value = value;
	SAVELOAD_WORD(mode, offset, data, _value);
	value = _value;
}

void SAVELOAD_LONG(STATE_TYPE mode, int &offset, unsigned char *data, uint32_t &value)
{
	uint32_n sl_tmp;
	switch (mode)
	{
	case STATE_SAVE:
		sl_tmp.l0 = value;
		data[offset++] = sl_tmp.b0;
		data[offset++] = sl_tmp.b1;
		data[offset++] = sl_tmp.b2;
		data[offset++] = sl_tmp.b3;
		break;
	case STATE_LOAD_VER:
	case STATE_LOAD:
		sl_tmp.b0 = data[offset++];
		sl_tmp.b1 = data[offset++];
		sl_tmp.b2 = data[offset++];
		sl_tmp.b3 = data[offset++];
		value = sl_tmp.l0;
		break;
	case STATE_SIZE:
		offset += 4;
		break;
	default:
		MessageBox(hWnd, _T("Invalid save/load type!"), _T("Mapper DLL"), MB_OK);
	}
}
void SAVELOAD_LONG(STATE_TYPE mode, int &offset, unsigned char *data, int32_t &value)
{
	uint32_t _value = value;
	SAVELOAD_LONG(mode, offset, data, _value);
	value = _value;
}
void SAVELOAD_LONG(STATE_TYPE mode, int &offset, unsigned char *data, long &value)
{
	uint32_t _value = value;
	SAVELOAD_LONG(mode, offset, data, _value);
	value = _value;
}
void SAVELOAD_LONG(STATE_TYPE mode, int &offset, unsigned char *data, unsigned long &value)
{
	uint32_t _value = value;
	SAVELOAD_LONG(mode, offset, data, _value);
	value = _value;
}

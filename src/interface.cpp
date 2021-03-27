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
			if (strcmp((char *)(cur->MapperId) + 4, BoardName + 4))
			return cur;
		}
		
	}
	return NULL;
}

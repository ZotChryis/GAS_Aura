#include "Game/AuraLoadScreenSaveGame.h"

FSavedMap UAuraLoadScreenSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	for (const FSavedMap& SavedMap : SavedMaps)
	{
		if (SavedMap.MapName == InMapName)
		{
			return SavedMap;
		}
	}
	
	return FSavedMap();
}

void UAuraLoadScreenSaveGame::SetSavedMapWithMapName(const FString& InMapName, const FSavedMap& InSavedMap)
{
	if (!HasMap(InMapName))
	{
		return;
	}
	
	for (FSavedMap& SavedMap : SavedMaps)
	{
		if (SavedMap.MapName == InMapName)
		{
			SavedMap = InSavedMap;
			return;
		}
	}
}

bool UAuraLoadScreenSaveGame::HasMap(const FString& InMapName)
{
	for (const FSavedMap& SavedMap : SavedMaps)
	{
		if (SavedMap.MapName == InMapName)
		{
			return true;
		}
	}
	
	return false;
}

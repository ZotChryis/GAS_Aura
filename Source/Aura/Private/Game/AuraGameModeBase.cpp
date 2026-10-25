#include "Game/AuraGameModeBase.h"

#include "Game/AuraLoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	Maps.Add(StartingMapName, StartingMap);
}

void AAuraGameModeBase::SaveSlotData(const UMVVM_LoadSlot* LoadSlotViewModel, int32 SlotIndex) const
{
	DeleteSlotData(LoadSlotViewModel, SlotIndex);
	
	if (USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass))
	{
		if (UAuraLoadScreenSaveGame* LoadScreenSaveGame = Cast<UAuraLoadScreenSaveGame>(SaveGameObject))
		{
			LoadScreenSaveGame->Status = Taken;
			LoadScreenSaveGame->PlayerName = LoadSlotViewModel->GetPlayerName();
			LoadScreenSaveGame->MapName = LoadSlotViewModel->GetMapName();
			UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlotViewModel->LoadSlotName, SlotIndex);
		}
	}
}

void AAuraGameModeBase::TravelToMap(const UMVVM_LoadSlot* LoadSlotViewModel)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(LoadSlotViewModel, Maps.FindChecked(LoadSlotViewModel->GetMapName()));
}

void AAuraGameModeBase::DeleteSlotData(const UMVVM_LoadSlot* LoadSlotViewModel, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(LoadSlotViewModel->LoadSlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlotViewModel->LoadSlotName, SlotIndex);
	}
}

UAuraLoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		return Cast<UAuraLoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	}
	
	return Cast<UAuraLoadScreenSaveGame>(UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass));
}

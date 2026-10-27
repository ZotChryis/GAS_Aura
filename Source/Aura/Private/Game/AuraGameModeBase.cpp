#include "Game/AuraGameModeBase.h"

#include "Game/AuraGameInstance.h"
#include "Game/AuraLoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
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
			LoadScreenSaveGame->PlayerStartTag = LoadSlotViewModel->PlayerStartTag;
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

UAuraLoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		return Cast<UAuraLoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex));
	}
	
	return Cast<UAuraLoadScreenSaveGame>(UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass));
}

UAuraLoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData() const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	const FString InGameSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameSlotIndex = AuraGameInstance->LoadSlotIndex;
	return GetSaveSlotData(InGameSlotName, InGameSlotIndex);
}

void AAuraGameModeBase::SaveInGameProgress(UAuraLoadScreenSaveGame* SaveObject) const
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	const FString InGameSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameSlotIndex = AuraGameInstance->LoadSlotIndex;
	
	AuraGameInstance->PlayerStartTag = SaveObject->PlayerStartTag;
	
	UGameplayStatics::SaveGameToSlot(SaveObject, InGameSlotName, InGameSlotIndex);
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	
	AActor* StartActor = nullptr;
	for (AActor* Actor : Actors)
	{
		if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
		{
			if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
			{
				StartActor = Actor;
				break;
			}
		}
		
	}
	
	return StartActor;
}

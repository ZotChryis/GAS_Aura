#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Chaos/ChaosPerfTest.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraLoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
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

FString AAuraGameModeBase::GetMapNameFromAssetName(const FString& InMapAssetName) const
{
	for (TTuple<FString, TSoftObjectPtr<UWorld>> Pair : Maps)
	{
		if (Pair.Value.ToSoftObjectPath().GetAssetName() == InMapAssetName)
		{
			return Pair.Key;
		}
	}
	
	return FString();
}

void AAuraGameModeBase::SaveWorldState(UWorld* World,  const FString& DestinationMapName, const FName& DestinationStartTag) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	
	if (UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance()))
	{
		if (UAuraLoadScreenSaveGame* SaveData = GetSaveSlotData(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex))
		{
			if (!DestinationMapName.IsEmpty())
			{
				SaveData->MapName = GetMapNameFromAssetName(DestinationMapName);
				SaveData->MapAssetName = DestinationMapName;
			}
			
			if (!DestinationStartTag.IsNone())
			{
				SaveData->PlayerStartTag = DestinationStartTag;
			}
			
			if (!SaveData->HasMap(WorldName))
			{
				FSavedMap NewSavedMap;
				NewSavedMap.MapName = WorldName;
				SaveData->SavedMaps.Add(NewSavedMap);
			}
			
			FSavedMap SavedMap = SaveData->GetSavedMapWithMapName(WorldName);
			SavedMap.SavedActors.Empty();
			
			for (FActorIterator It(World); It; ++It)
			{
				if (AActor* Actor = *It)
				{
					if (!IsValid(Actor) || !Actor->Implements<USaveInterface>())
					{
						continue;
					}
					
					FSavedActor SavedActor;
					SavedActor.ActorName = Actor->GetFName();
					SavedActor.Transform = Actor->GetTransform();
					
					FMemoryWriter MemoryWriter(SavedActor.Bytes);
					FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
					Archive.ArIsSaveGame = true;
					Actor->Serialize(Archive);
					
					SavedMap.SavedActors.AddUnique(SavedActor);
				}
			}
			
			SaveData->SetSavedMapWithMapName(WorldName, SavedMap);
			UGameplayStatics::SaveGameToSlot(SaveData, AuraGI->LoadSlotName, AuraGI->LoadSlotIndex);
		}
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);
	
	if (UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance()))
	{
		if (UAuraLoadScreenSaveGame* SaveData = GetSaveSlotData(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex))
		{
			for (FActorIterator It(World); It; ++It)
			{
				if (AActor* Actor = *It)
				{
					if (!IsValid(Actor) || !Actor->Implements<USaveInterface>())
					{
						continue;
					}
					
					for (FSavedActor SavedActor : SaveData->GetSavedMapWithMapName(WorldName).SavedActors)
					{
						if (SavedActor.ActorName == Actor->GetFName())
						{
							if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
							{
								Actor->SetActorTransform(SavedActor.Transform);
							}
							
							FMemoryReader MemoryReader(SavedActor.Bytes);
							FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
							Archive.ArIsSaveGame = true;
							Actor->Serialize(Archive);
							
							ISaveInterface::Execute_LoadActor(Actor);
						}
					}
				}
			}
		}
	}
}

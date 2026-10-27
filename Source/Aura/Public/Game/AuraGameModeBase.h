#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class UAuraLoadScreenSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;

UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void SaveSlotData(const UMVVM_LoadSlot* LoadSlotViewModel, int32 SlotIndex) const;
	void TravelToMap(const UMVVM_LoadSlot* LoadSlotViewModel);
	static void DeleteSlotData(const UMVVM_LoadSlot* LoadSlotViewModel, int32 SlotIndex);
	UAuraLoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	UAuraLoadScreenSaveGame* RetrieveInGameSaveData() const;
	void SaveInGameProgress(UAuraLoadScreenSaveGame* SaveObject) const;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FString StartingMapName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TSoftObjectPtr<UWorld> StartingMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;
	
	UPROPERTY(EditDefaultsOnly, Category = "Maps")
	FName DefaultPlayerStartTag;
	
protected:
	virtual void BeginPlay() override;
};

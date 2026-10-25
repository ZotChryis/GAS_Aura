#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AuraLoadScreenSaveGame.generated.h"

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

UCLASS()
class AURA_API UAuraLoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY()
	FString SlotName = FString();
	
	UPROPERTY()
	int32 SlotIndex = 0;
	
	UPROPERTY()
	FString PlayerName = FString("Default Name");
	
	UPROPERTY()
	FString MapName = FString("No Map");
	
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> Status = Vacant;
};

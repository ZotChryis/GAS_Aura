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
	
	/** Necessary for System */
	UPROPERTY()
	FString SlotName = FString();
	
	UPROPERTY()
	int32 SlotIndex = 0;
	
	/** Slot Information */
	UPROPERTY()
	FString PlayerName = FString("Default Name");
	
	UPROPERTY()
	FString MapName = FString("No Map");
	
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> Status = Vacant;
	
	/** World */
	UPROPERTY()
	FName PlayerStartTag = FName();
	
	/** Player */
	UPROPERTY()
	int32 PlayerLevel = 0;
	
	UPROPERTY()
	int32 XP = 0;
	
	UPROPERTY()
	int32 SpellPoints = 0;
	
	UPROPERTY()
	int32 AttributePoints = 0;
	
	UPROPERTY()
	float Strength = 0;
	
	UPROPERTY()
	float Intelligence = 0;
	
	UPROPERTY()
	float Resilience = 0;
	
	UPROPERTY()
	float Vigor = 0;
	
};

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "AuraLoadScreenSaveGame.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityType = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityStatus = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel = 0;
};

inline bool operator==(const FSavedAbility& Left, const FSavedAbility& Right)
{
	return Left.AbilityTag.MatchesTagExact(Right.AbilityTag);
}

USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()
	
	UPROPERTY()
	FName ActorName = FName();
	
	UPROPERTY()
	FTransform Transform = FTransform();
	
	// Serialized member variables in Actor marked with SaveGame decorator
	UPROPERTY()
	TArray<uint8> Bytes;
};

inline bool operator==(const FSavedActor& Left, const FSavedActor& Right)
{
	return Left.ActorName == Right.ActorName;
}

USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString MapName = FString();
	
	UPROPERTY()
	TArray<FSavedActor> SavedActors;
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
	FString MapAssetName = FString("Default");
	
	UPROPERTY()
	FName PlayerStartTag = FName();
	
	UPROPERTY()
	bool FirstTimeLoadIn = true;
	
	/** Player */
	UPROPERTY()
	int32 PlayerLevel = 1;
	
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
	
	/** Abilities */
	UPROPERTY()
	TArray<FSavedAbility> Abilities;
	
	/** World Data */
	UPROPERTY()
	TArray<FSavedMap> SavedMaps;
	
	FSavedMap GetSavedMapWithMapName(const FString& InMapName);
	void SetSavedMapWithMapName(const FString& InMapName, const FSavedMap& InSavedMap);
	bool HasMap(const FString& InMapName);
};

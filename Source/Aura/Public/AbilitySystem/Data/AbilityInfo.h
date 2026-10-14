#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityInfo.generated.h"

class UGameplayAbility;
class UTexture2D;
class UMaterialInterface;

USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CooldownTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StatusTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UMaterialInterface> BackgroundMaterial;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelRequirement = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;
};

UCLASS()
class AURA_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	/** We assume these are authored to be sorted from 0->MAX level. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Information")
	TArray<FAuraAbilityInfo> AbilityInformation;

	UFUNCTION(BlueprintCallable)
	FAuraAbilityInfo GetAbilityInfoByTag(const FGameplayTag& Tag, const bool bLogNotFound = false);
};

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

class AURA_API IPlayerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForExperience(int32 InExperience) const;
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetExperience() const;

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(int32 InLevel) const;
    	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(int32 InLevel) const;
	
	UFUNCTION(BlueprintNativeEvent)
	void AddExperience(int32 InExperience);

	UFUNCTION(BlueprintNativeEvent)
	void AddAttributePoints(int32 InAttributePoints);

	UFUNCTION(BlueprintNativeEvent)
	void AddSpellPoints(int32 InSpellPoints);

	UFUNCTION(BlueprintNativeEvent)
	void AddPlayerLevel(int32 InLevels);
	
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;
};

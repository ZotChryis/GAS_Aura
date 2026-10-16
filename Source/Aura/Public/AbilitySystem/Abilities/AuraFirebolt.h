#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFirebolt.generated.h"

UCLASS()
class AURA_API UAuraFirebolt : public UAuraProjectileSpell
{
	GENERATED_BODY()
	
public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
	
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SpawnProjectiles(AActor* TargetActor, const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, const bool bOverridePitch = false, float PitchOverride = 0.f);

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float ProjectileSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	int32 MaxNumProjectile = 5;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bLauchHomingProjectiles = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float HomingAccelerationMin = 1600.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	float HomingAccelerationMax = 3200.f;
};

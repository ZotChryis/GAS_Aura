#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_FireResist.generated.h"

UCLASS()
class AURA_API UMMC_FireResist : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public :
	UMMC_FireResist();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
	FGameplayEffectAttributeCaptureDefinition ResilienceDef;
};

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_LightningResist.generated.h"

UCLASS()
class AURA_API UMMC_LightningResist : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public :
	UMMC_LightningResist();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
	FGameplayEffectAttributeCaptureDefinition VigorDef;
};

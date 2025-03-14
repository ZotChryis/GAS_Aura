#include "AbilitySystem/MMC/MMC_PhysicalResist.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_PhysicalResist::UMMC_PhysicalResist()
{
	StrengthDef.AttributeToCapture = UAuraAttributeSet::GetStrengthAttribute();
	StrengthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	StrengthDef.bSnapshot = false;

	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(StrengthDef);
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_PhysicalResist::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Physical Resist = % of Intelligence + % of Resilience + Player Level Scalar
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = SourceTags;
	EvaluateParams.TargetTags = TargetTags;
	
	float Strength;
	GetCapturedAttributeMagnitude(StrengthDef, Spec, EvaluateParams, Strength);
	Strength = FMath::Max<float>(Strength, 0.0f);

	float Vigor;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParams, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.0f);

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();
	
	return Strength * 0.15f + Vigor * 0.15f + 1.5 * PlayerLevel;
}

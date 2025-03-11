#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	check(SourceASC);
	check(TargetASC);
	
	AActor* SourceAvatar = SourceASC->GetAvatarActor();
	AActor* TargetAvatar = TargetASC->GetAvatarActor();

	check(SourceAvatar);
	check(TargetAvatar);

	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	check(SourceCombatInterface);
	check(TargetCombatInterface);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = SourceTags;
	EvalParams.TargetTags = TargetTags;

	// Get the damage set by caller
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Attributes_Meta_Damage);

	// Capture block chance on target, and determine if there was a successful block
	float BlockChance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvalParams, BlockChance);
	BlockChance = FMath::Max<float>(0, BlockChance);

	// If blocked, halve the damage
	const bool bBlocked = FMath::RandRange(1, 100) <= BlockChance;
	Damage = bBlocked ? Damage / 2.f : Damage;

	// Armor penetration ignores a percentage of the target's armor
	float TargetArmor;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvalParams, TargetArmor);
	TargetArmor = FMath::Max<float>(0, TargetArmor);

	float SourceArmorPenetration;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvalParams, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0, SourceArmorPenetration);
	
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const TObjectPtr<UCurveTable> Coefficients = CharacterClassInfo->DamageCalculationCoefficients;
	const FRealCurve* ArmorCurve = Coefficients->FindCurve(FName("Armor"), FString());
	const FRealCurve* ArmorPenetrationCurve = Coefficients->FindCurve(FName("ArmorPenetration"), FString());

	check(ArmorPenetrationCurve);
	check(ArmorCurve);
	
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());
	const float ArmorCoefficient = ArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	
	// Armor penetration ignores a percentage of the Target's Armor
	const float EffectiveArmor = FMath::Max<float>(TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f, 0.f);

	// Armor ignores a percentage of incoming damage (after block reductions)
	Damage *= (100.f - EffectiveArmor * ArmorCoefficient) / 100.f;

	// Determine if this hit is a critical hit from source's critical hit chance
	// Target can have resistance to critical hit that will skew towards their favor. This is scaled by level
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvalParams, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(0, SourceCriticalHitChance);

	float TargetCriticalHitResistance;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvalParams, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0, TargetCriticalHitResistance);
	
	const FRealCurve* CritialHitResistanceCurve = Coefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float EffectiveCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance * CritialHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel()));

	const float EffectCriticalHitChance = FMath::Max<float>(0, SourceCriticalHitChance - EffectiveCriticalHitResistance);
	const bool bCriticalHit = FMath::RandRange(1, 100) <= EffectCriticalHitChance;
	if (bCriticalHit)
	{
		float SourceCriticalHitDamage = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvalParams, SourceCriticalHitDamage);
		SourceCriticalHitDamage = FMath::Max<float>(0, SourceCriticalHitDamage);
		
		// Critical hits are x2 + CritHitDamage
		Damage = Damage * 2.f + SourceCriticalHitDamage;
	}
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

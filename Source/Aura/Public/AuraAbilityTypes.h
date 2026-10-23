#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	bool IsBlocked() const { return bBlocked; }
	bool IsCriticalHit() const { return bCriticalHit; }
	bool IsSuccessfulDebuff() const { return bSuccessfulDebuff; }
	bool IsSuccessfulKnockback() const { return bSuccessfulKnockback; }
	bool GetCausesHitReact() const { return bCausesHitReact; }
	bool GetDebuffCausesHitReact() const { return bDebuffCausesHitReact; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffDuration() const { return DebuffDuration; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	FVector GetDeathImpulse() const { return DeathImpulse; }
	FVector GetKnockbackForce() const { return KnockbackForce; }
	TSharedPtr<FGameplayTag> GetDamageType () const { return DamageType; }
	bool IsRadialDamage() const { return bIsRadialDamage; }
	float GetRadialDamageInnerRadius() const { return RadialDamageInnerRadius; }
	float GetRadialDamageOuterRadius() const { return RadialDamageOuterRadius; }
	FVector GetRadialDamageOrigin() const { return RadialDamageOrigin; }

	void SetIsBlocked(bool Blocked) { bBlocked = Blocked; }
	void SetIsCriticalHit(bool CriticalHit) { bCriticalHit = CriticalHit; }
	void SetIsSuccessfulDebuff(bool Success) { bSuccessfulDebuff = Success; }
	void SetIsSuccessfulKnockback(bool Success) { bSuccessfulKnockback = Success; }
	void SetCausesHitReact(bool CausesHitReact) { bCausesHitReact = CausesHitReact; }
	void SetDebuffCausesHitReact(bool DebuffCausesHitReact) { bDebuffCausesHitReact = DebuffCausesHitReact; }
	void SetDebuffDamage(float Damage) { DebuffDamage = Damage; }
	void SetDebuffDuration(float Duration) { DebuffDuration = Duration; }
	void SetDebuffFrequency(float Frequency) { DebuffFrequency = Frequency; }
	void SetDeathImpulse(const FVector& Impulse) { DeathImpulse = Impulse; }
	void SetKnockbackForce(const FVector& Impulse) { KnockbackForce = Impulse; }
	void SetDamageType (const TSharedPtr<FGameplayTag>& InDamageType) { DamageType = InDamageType; }
	void SetIsRadialDamage(const bool IsRadialDamage) { bIsRadialDamage = IsRadialDamage; }
	void SetRadialDamageInnerRadius(float Radius) { RadialDamageInnerRadius = Radius; }
	void SetRadialDamageOuterRaidus(float Radius) { RadialDamageOuterRadius = Radius; }
	void SetRadialDamageOrigin(const FVector& Origin) { RadialDamageOrigin = Origin; }
	
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual FAuraGameplayEffectContext* Duplicate() const override;
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:
	UPROPERTY()
	bool bBlocked = false;
	
	UPROPERTY()
	bool bCriticalHit = false;
	
	UPROPERTY()
	bool bSuccessfulDebuff = false;
	
	UPROPERTY()
	bool bSuccessfulKnockback = false;
	
	UPROPERTY()
	bool bCausesHitReact = false;
	
	UPROPERTY()
	bool bDebuffCausesHitReact = false;
	
	UPROPERTY()
	float DebuffDamage = 0.f;
	
	UPROPERTY()
	float DebuffDuration = 0.f;
	
	UPROPERTY()
	float DebuffFrequency = 0.f;
	
	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector;
	
	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector;
	
	TSharedPtr<FGameplayTag> DamageType;
	
	UPROPERTY()
	bool bIsRadialDamage = false;
	
	UPROPERTY()
	float RadialDamageInnerRadius = 0.f;
	
	UPROPERTY()
	float RadialDamageOuterRadius = 0.f;
	
	UPROPERTY()
	FVector RadialDamageOrigin = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()
	
	FDamageEffectParams(){}
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffectClass = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;
	
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f;
	
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType = FGameplayTag();
	
	UPROPERTY(BlueprintReadWrite)
	bool CausesHitReact = true;
	
	UPROPERTY(BlueprintReadWrite)
	bool DebuffCausesHitReact = true;
	
	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 0.;
	
	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsRadialDamage = false;
	
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageInnerRadius = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float RadialDamageOuterRadius = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	FVector RadialDamageOrigin = FVector::ZeroVector;
	
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WIthCopy = true
	};	
};
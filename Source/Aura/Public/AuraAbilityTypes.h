#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	bool IsBlocked() const { return bBlocked; }
	bool IsCriticalHit() const { return bCriticalHit; }

	void SetIsBlocked(bool Blocked) { bBlocked = Blocked; }
	void SetIsCriticalHit(bool CriticalHit) { bCriticalHit = CriticalHit; }
	
	virtual UScriptStruct* GetScriptStruct() const override;
	virtual FAuraGameplayEffectContext* Duplicate() const override;
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:
	UPROPERTY()
	bool bBlocked = false;
	
	UPROPERTY()
	bool bCriticalHit = false;
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
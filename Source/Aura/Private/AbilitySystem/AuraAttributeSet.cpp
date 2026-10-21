#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	// Primary Attributes
	TagToAttributeMap.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	// Secondary Attributes
	TagToAttributeMap.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);

	// Resistances
	TagToAttributeMap.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
	
	// Vital Attributes
	TagToAttributeMap.Add(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
	TagToAttributeMap.Add(GameplayTags.Attributes_Vital_Mana, GetManaAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// Primary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	
	// Secondary Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	// Resistances (Secondary)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
	
	// Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxMana());
	}
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetMaxHealthAttribute() && bTopOffHealth)
	{
		SetHealth(GetMaxHealth());
		bTopOffHealth = false;
	}
	if (Attribute == GetMaxManaAttribute() && bTopOffMana)
	{
		SetMana(GetMaxMana());
		bTopOffMana = false;
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Properties;
	SetEffectProperties(Data, Properties);
	
	if (Properties.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Properties.TargetCharacter))
	{
		return;
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(Properties);
	}

	if (Data.EvaluatedData.Attribute == GetIncomingExperienceAttribute())
	{
		HandleIncomingExperience(Properties);
	}
}

void UAuraAttributeSet::HandleIncomingDamage(FEffectProperties Properties)
{
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);

	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = GetHealth() - LocalIncomingDamage;
		SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

		ICombatInterface* CombatInterface = Cast<ICombatInterface>(Properties.TargetAvatarActor);
		if (const bool bFatal = NewHealth <= 0.f)
		{
			if (CombatInterface)
			{
				CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Properties.EffectContextHandle));
				SendExperienceEvent(Properties);
			}
		}
		else
		{
			bool bIsShocked = false;
			if (Properties.TargetAvatarActor->Implements<UCombatInterface>())
			{
				bIsShocked = ICombatInterface::Execute_GetInShockLoop(Properties.TargetAvatarActor);
			}
			
			if (!bIsShocked && UAuraAbilitySystemLibrary::GetCausesHitReact(Properties.EffectContextHandle))
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FAuraGameplayTags::Get().Event_Montage_HitReact);
				Properties.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}
			if (UAuraAbilitySystemLibrary::IsSuccessfulKnockback(Properties.EffectContextHandle) && CombatInterface)
			{
				CombatInterface->Knockback(UAuraAbilitySystemLibrary::GetKnockbackForce(Properties.EffectContextHandle));
			}
		}

		const bool bBlockedHit = UAuraAbilitySystemLibrary::IsBlockedHit(Properties.EffectContextHandle);
		const bool bCriticalHit = UAuraAbilitySystemLibrary::IsCriticalHit(Properties.EffectContextHandle);
		if (Properties.SourceCharacter != Properties.TargetCharacter)
		{
			ShowFloatingDamageText(Properties, LocalIncomingDamage, bBlockedHit, bCriticalHit);	
		}
			
		if (UAuraAbilitySystemLibrary::IsSuccessfulDebuff(Properties.EffectContextHandle))
		{
			Debuff(Properties);
		}
	}
}

void UAuraAttributeSet::HandleIncomingExperience(FEffectProperties Properties)
{
	const float LocalIncomingExperience = GetIncomingExperience();
	SetIncomingExperience(0.f);
		
	// Source Character should be the owner, since GA_ListenForEvents applies GE_EventBasedEffect to itself in order to add Experience
	if (Properties.SourceCharacter->Implements<UPlayerInterface>() && Properties.SourceCharacter->Implements<UCombatInterface>())
	{
		const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Properties.SourceCharacter);
		const int32 CurrentExperience = IPlayerInterface::Execute_GetExperience(Properties.SourceCharacter);
		const int32 NewLevel = IPlayerInterface::Execute_FindLevelForExperience(Properties.SourceCharacter, CurrentExperience + LocalIncomingExperience);
		
		for (int i = CurrentLevel; i < NewLevel; ++i)
		{
			int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributePointsReward(Properties.SourceCharacter, i);
			int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointsReward(Properties.SourceCharacter, i);
			IPlayerInterface::Execute_AddAttributePoints(Properties.SourceCharacter, AttributePointsReward);
			IPlayerInterface::Execute_AddSpellPoints(Properties.SourceCharacter, SpellPointsReward);
		}
		
		// We should only ever call Execute_AddPlayerLevel once per Effect execution, because we add Abilities through
		// that function, and while we're executing an Effect, there is an Ability Lock. We cannot assume ability state will
		// refresh until post gameplay effect.
		const int32 NumLevelUps = NewLevel - CurrentLevel;
		if (NumLevelUps > 0)
		{
			bTopOffHealth = true;
			bTopOffMana = true;
			
			IPlayerInterface::Execute_LevelUp(Properties.SourceCharacter);
			IPlayerInterface::Execute_AddPlayerLevel(Properties.SourceCharacter, NumLevelUps);
		}
		
		IPlayerInterface::Execute_AddExperience(Properties.SourceCharacter, LocalIncomingExperience);
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties) const
{
	EffectProperties.EffectContextHandle = Data.EffectSpec.GetContext();

	EffectProperties.SourceASC = EffectProperties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	
	if (IsValid(EffectProperties.SourceASC) && EffectProperties.SourceASC->AbilityActorInfo.IsValid() && EffectProperties.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		EffectProperties.SourceAvatarActor = EffectProperties.SourceASC->AbilityActorInfo->AvatarActor.Get();
		EffectProperties.SourceController = EffectProperties.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (!EffectProperties.SourceController && EffectProperties.SourceAvatarActor)
		{
			if (const APawn* SourcePawn = Cast<APawn>(EffectProperties.SourceAvatarActor))
			{
				EffectProperties.SourceController = Cast<APlayerController>(SourcePawn->GetController());
			}
		}

		if (EffectProperties.SourceController)
		{
			EffectProperties.SourceCharacter = EffectProperties.SourceController->GetCharacter();
		}
	}
	
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		EffectProperties.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		EffectProperties.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		EffectProperties.TargetCharacter = Cast<ACharacter>(EffectProperties.TargetAvatarActor);
		EffectProperties.TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(EffectProperties.TargetAvatarActor);
	}
}

void UAuraAttributeSet::ShowFloatingDamageText(const FEffectProperties& Properties, float Damage, const bool bBlockedHit, const bool bCriticalHit) const
{
	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(Properties.SourceController))
	{
		AuraPC->ShowDamageText(Damage, Properties.TargetCharacter, bBlockedHit, bCriticalHit);
		return;
	}

	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(Properties.TargetController))
	{
		AuraPC->ShowDamageText(Damage, Properties.TargetCharacter, bBlockedHit, bCriticalHit);	
	}
}

void UAuraAttributeSet::SendExperienceEvent(const FEffectProperties& Properties) const
{
	if (Properties.TargetCharacter->Implements<UCombatInterface>())
	{
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Properties.TargetCharacter);
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Properties.TargetCharacter);
		const int32 ExperienceReward  = UAuraAbilitySystemLibrary::GetExperienceRewardForClassAndLevel(Properties.TargetCharacter, TargetClass, TargetLevel);
		
		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		FGameplayEventData EventPayload = FGameplayEventData();
		EventPayload.EventTag = Tags.Attributes_Meta_IncomingExperience;
		EventPayload.EventMagnitude = ExperienceReward;
	
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Properties.SourceCharacter, Tags.Attributes_Meta_IncomingExperience, EventPayload);
	}
}

void UAuraAttributeSet::Debuff(const FEffectProperties& Properties)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	
	FGameplayEffectContextHandle EffectContext = Properties.SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(Properties.SourceAvatarActor);
	
	const FGameplayTag& DamageType = UAuraAbilitySystemLibrary::GetDamageType(Properties.EffectContextHandle);
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Properties.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Properties.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Properties.EffectContextHandle);
	
	FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));
	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
	Effect->Period = DebuffFrequency;
	Effect->StackLimitCount = 1;
	Effect->StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::ExtendDuration;
	
	FGameplayModifierInfo ModifierInfo;
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FScalableFloat(DebuffDamage);
	ModifierInfo.Attribute = GetIncomingDamageAttribute();
	Effect->Modifiers.Add(ModifierInfo);

	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
	{
		FAuraGameplayEffectContext* DebuffContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().Get());
		TSharedPtr<FGameplayTag> DebuffDamageType = MakeShareable(new FGameplayTag(DamageType));
		DebuffContext->SetDamageType(DebuffDamageType);
		DebuffContext->SetCausesHitReact(UAuraAbilitySystemLibrary::GetDebuffCausesHitReact(Properties.EffectContextHandle));
		
		const FGameplayTag& DebuffTag = GameplayTags.DamageTypesToDebuffs[DamageType];
		MutableSpec->DynamicGrantedTags.AddTag(DebuffTag);
		
		Properties.SourceASC->ApplyGameplayEffectSpecToTarget(*MutableSpec, Properties.TargetASC);
	}
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldValue);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldValue);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldValue);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldValue);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldValue);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldValue);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldValue);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldValue);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldValue);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldValue);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldValue);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldValue);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldValue);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldValue);
}

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldValue);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldValue);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldValue);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldValue);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldValue);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldValue) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldValue);
}

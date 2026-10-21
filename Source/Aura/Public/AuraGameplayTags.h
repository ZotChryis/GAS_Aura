#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton containing native Gameplay Tags.
 */
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get() { return GameplayTags; };
	static void InitializeNativeGameplayTags();

	// Vital Attributes
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_Mana;
	
	// Primary Attributes
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Vigor;
	
	// Secondary Attributes
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;

	// Resistances
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;

	// Meta Attributes
	FGameplayTag Attributes_Meta_IncomingExperience;
	
	// Combat Damage Types
	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	// Debuff Types
	FGameplayTag Debuff_Burn;
	FGameplayTag Debuff_Stun;
	FGameplayTag Debuff_Arcane;
	FGameplayTag Debuff_Physical;
	
	// Debuff Meta
	FGameplayTag Debuff_Chance;
	FGameplayTag Debuff_Damage;
	FGameplayTag Debuff_Frequency;
	FGameplayTag Debuff_Duration;
	
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;

	// Abilities
	FGameplayTag Ability_Firebolt;
	FGameplayTag Ability_Electrocute;
	
	FGameplayTag Ability_Passive_HaloOfProtection;
	FGameplayTag Ability_Passive_LifeSiphon;
	FGameplayTag Ability_Passive_ManaSiphon;
	
	FGameplayTag Cooldown_Firebolt;
	FGameplayTag Cooldown_Electrocute;
	
	FGameplayTag Ability_None;
	FGameplayTag Ability_Attack;
	FGameplayTag Ability_Summon;
	FGameplayTag Ability_HitReact;
	
	// Ability Types
	FGameplayTag AbilityType_Offensive;
	FGameplayTag AbilityType_Passive;
	FGameplayTag AbilityType_None;
	
	// Ability Status
	FGameplayTag AbilityStatus_Locked;
	FGameplayTag AbilityStatus_Eligible;
	FGameplayTag AbilityStatus_Unlocked;
	FGameplayTag AbilityStatus_Equipped;
	
	// Input
	FGameplayTag Input_LMB;
	FGameplayTag Input_RMB;
	FGameplayTag Input_1;
	FGameplayTag Input_2;
	FGameplayTag Input_3;
	FGameplayTag Input_4;
	FGameplayTag Input_Passive_1;
	FGameplayTag Input_Passive_2;

	// Montage Events
	FGameplayTag Event_Montage_Firebolt;
	FGameplayTag Event_Montage_Electrocute;
	FGameplayTag Event_Montage_HitReact;

	FGameplayTag Event_Montage_Attack_1;
	FGameplayTag Event_Montage_Attack_2;
	FGameplayTag Event_Montage_Attack_3;
	FGameplayTag Event_Montage_Attack_4;

	// Player 
	FGameplayTag Player_Block_InputPressed;
	FGameplayTag Player_Block_InputHeld;
	FGameplayTag Player_Block_InputReleased;
	FGameplayTag Player_Block_CursorTrace;
	
	// Combat Sockets
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_Tail;
	
	// Cues
	FGameplayTag GameplayCue_MeleeImpact;
	FGameplayTag GameplayCue_ShockCast;
	FGameplayTag GameplayCue_ShockLoop;

private:
	static FAuraGameplayTags GameplayTags;
};
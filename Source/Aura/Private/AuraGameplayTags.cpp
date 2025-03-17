#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	// Vital Attributes
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.Health"), FString("Amount of damage before death."));
	GameplayTags.Attributes_Vital_Mana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Vital.Mana"), FString("Amount of mana."));
	
	// Primary Attributes
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Strength"), FString("Increases physical damage."));
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Intelligence"), FString("Increases magical damage."));
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Resilience"), FString("Increases armor and armor penetration."));
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Primary.Vigor"), FString("Increases maximum health."));
	
	// Secondary Attributes
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("Reduces damage taken, improves Block Chance."));
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ArmorPenetration"), FString("Improves damage against armor and critical hit chance."));
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.BlockChance"), FString("Chance to block an attack."));
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitChance"), FString("Chance to achieve a critical hit."));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitDamage"), FString("Additional damage when issuing a critical hit."));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitResistance"), FString("Reduces damage taken when critically hit."));
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.HealthRegeneration"), FString("Health recovered every 1s."));
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.ManaRegeneration"), FString("Mana recovered every 1s."));
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"), FString("Maximum amount of health."));
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.MaxMana"), FString("Maximum amount of mana."));

	// Resistances Attributes
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Fire"), FString("Resistance to fire damage."));
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Lightning"), FString("Resistance to lightning damage."));
	GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Arcane"), FString("Resistance to arcane damage."));
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Resistance.Physical"), FString("Resistance to physical damage."));
	
	// Damage
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"), FString("Temporary attribute used to calculate damage."));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Fire"), FString("Damage that is fire based."));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Lightning"), FString("Damage that is lightning based."));
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Arcane"), FString("Damage that is arcane based."));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Physical"), FString("Damage that is physical based."));

	// Damage Types -> Resistances map
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);

	// Abilities
	GameplayTags.Ability_Melee = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Melee"), FString("Basic melee attack ability."));
	
	// Input
	GameplayTags.Input_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.LMB"), FString("Left mouse button input."));
	GameplayTags.Input_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.RMB"), FString("Right mouse button input."));
	GameplayTags.Input_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.1"), FString("1 button input."));
	GameplayTags.Input_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.2"), FString("2 button input."));
	GameplayTags.Input_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.3"), FString("3 button input."));
	GameplayTags.Input_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.4"), FString("4 button input."));

	// Gameplay Events
	// TODO: Rename Shoot ?
	GameplayTags.Event_Montage_Firebolt = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.Firebolt"), FString("Event fired during the Firebolt montage on the point to shoot the projectile."));
	GameplayTags.Event_Montage_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.HitReact"), FString("Used to trigger hit react montage."));
	GameplayTags.Event_Montage_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.Weapon"), FString("Event fired during the Weapon attack animations on the point to do damage."));
	GameplayTags.Event_Montage_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.LeftHand"), FString("Event fired during the Left Hand attack animations on the point to do damage."));
	GameplayTags.Event_Montage_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.RightHand"), FString("Event fired during the Right Hand attack animations on the point to do damage."));
}

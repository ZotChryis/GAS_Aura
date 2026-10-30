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

	// Meta
	GameplayTags.Attributes_Meta_IncomingExperience =  UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Meta.IncomingExperience"), FString("Incoming experience meta attribute."));
	
	// Damage
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage"), FString("Temporary attribute used to calculate damage."));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Fire"), FString("Damage that is fire based."));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Lightning"), FString("Damage that is lightning based."));
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Arcane"), FString("Damage that is arcane based."));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Damage.Physical"), FString("Damage that is physical based."));

	// Debuffs
	GameplayTags.Debuff_Burn = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Burn"), FString("Debuff for Fire Damage"));
	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Stun"), FString("Debuff for Lightning Damage"));
	GameplayTags.Debuff_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Arcane"), FString("Debuff for Arcane Damage"));
	GameplayTags.Debuff_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Physical"), FString("Debuff for Physical Damage"));
	
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Chance"), FString("Debuff chance to apply."));
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Damage"), FString("Debuff damage per tick."));
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Frequency"), FString("Debuff tick rate."));
	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Debuff.Duration"), FString("Debuff duration."));
	
	// Damage Types -> Resistances map
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);
	
	// Damage Types -> Debuffs
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Fire, GameplayTags.Debuff_Burn);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Lightning, GameplayTags.Debuff_Stun);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Arcane, GameplayTags.Debuff_Arcane);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Physical, GameplayTags.Debuff_Physical);

	// Abilities
	GameplayTags.Ability_None = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.None"), FString("No gameplay ability, used in some instances for 'null'."));
	
	GameplayTags.Ability_Firebolt = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Fire.Firebolt"), FString("Firebolt ability."));
	GameplayTags.Ability_Fireblast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Fire.Fireblast"), FString("Fireblast ability."));
	GameplayTags.Ability_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Lightning.Electrocute"), FString("Electrocute ability."));
	GameplayTags.Ability_ArcaneShards = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Arcane.ArcaneShards"), FString("Arcane Shards ability."));
	
	GameplayTags.Ability_Passive_HaloOfProtection = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Passive.HaloOfProtection"), FString("Passive Halo of Protection."));
	GameplayTags.Ability_Passive_LifeSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Passive.LifeSiphon"), FString("Passive Life Siphon."));
	GameplayTags.Ability_Passive_ManaSiphon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Passive.ManaSiphon"), FString("Passive Mana Siphon."));
	GameplayTags.Ability_Passive_ListenForEvent = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Passive.ListenForEvent"), FString("Passive to listen for events.."));
	
	GameplayTags.Ability_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Attack"), FString("Basic attack ability."));
	GameplayTags.Ability_Summon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Summon"), FString("Basic summon ability."));
	GameplayTags.Ability_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.HitReact"), FString("Basic hit react."));
	
	// Cooldowns
	GameplayTags.Cooldown_Firebolt = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.Fire.Firebolt"), FString("Firebolt ability cooldown."));
	GameplayTags.Cooldown_Fireblast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.Fire.Fireblast"), FString("Fireblast ability cooldown."));
	GameplayTags.Cooldown_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.Lightning.Electrocute"), FString("Electrocute ability cooldown."));
	GameplayTags.Cooldown_ArcaneShards = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Cooldown.Arcane.ArcaneShards"), FString("Arcane Shards ability cooldown."));
	
	// Ability Types
	GameplayTags.AbilityType_Offensive = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Offensive"), FString("An offensive ability."));
	GameplayTags.AbilityType_Passive = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.Passive"), FString("A passive ability."));
	GameplayTags.AbilityType_None = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Type.None"), FString("Empty ability tag, for special cases."));
	
	// Ability Status
	GameplayTags.AbilityStatus_Locked = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Status.Locked"), FString("A locked ability."));
	GameplayTags.AbilityStatus_Eligible = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Status.Eligible"), FString("An eligible ability."));
	GameplayTags.AbilityStatus_Unlocked = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Status.Unlocked"), FString("An unlocked ability."));
	GameplayTags.AbilityStatus_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Ability.Status.Equipped"), FString("An equipped ability."));
	
	// Input
	GameplayTags.Input_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.LMB"), FString("Left mouse button input."));
	GameplayTags.Input_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.RMB"), FString("Right mouse button input."));
	GameplayTags.Input_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.1"), FString("1 button input."));
	GameplayTags.Input_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.2"), FString("2 button input."));
	GameplayTags.Input_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.3"), FString("3 button input."));
	GameplayTags.Input_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.4"), FString("4 button input."));
	GameplayTags.Input_Passive_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.Passive.1"), FString("Input Tag for Passive 1. Needed for UI."));
	GameplayTags.Input_Passive_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Input.Passive.2"), FString("Input Tag for Passive 2. Needed for UI."));

	// Gameplay Events
	GameplayTags.Event_Montage_Firebolt = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.Firebolt"), FString("Event fired during the Firebolt montage on the point to shoot the projectile."));
	GameplayTags.Event_Montage_Fireblast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.Fireblast"), FString("Event fired during the Fireblast montage on the point to shoot the projectile."));
	GameplayTags.Event_Montage_Electrocute = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.Electrocute"), FString("Event fired during the Electrocute montage on the point to shoot the projectile."));
	GameplayTags.Event_Montage_ArcaneShards = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.ArcaneShards"), FString("Event fired during the Arcane Shards montage on the point to shoot the projectile."));
	GameplayTags.Event_Montage_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.HitReact"), FString("Used to trigger hit react montage."));

	GameplayTags.Event_Montage_Attack_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.Attack.1"), FString("Generic attack montage event tag."));
	GameplayTags.Event_Montage_Attack_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.Attack.2"), FString("Generic attack montage event tag."));
	GameplayTags.Event_Montage_Attack_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.Attack.3"), FString("Generic attack montage event tag."));
	GameplayTags.Event_Montage_Attack_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Event.Montage.Attack.4"), FString("Generic attack montage event tag."));

	// Player
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputPressed"), FString("Block Input Pressed callbacks."));
	GameplayTags.Player_Block_InputHeld = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputHeld"), FString("Block Input Held callbacks."));
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.InputReleased"), FString("Block Input Released callbacks."));
	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Player.Block.CursorTrace"), FString("Block Cursor Trace."));

	// Combat Sockets
	GameplayTags.CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.Weapon"), FString("Event fired during the Weapon attack animations on the point to do damage."));
	GameplayTags.CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.LeftHand"), FString("Event fired during the Left Hand attack animations on the point to do damage."));
	GameplayTags.CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.RightHand"), FString("Event fired during the Right Hand attack animations on the point to do damage."));
	GameplayTags.CombatSocket_Tail = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("CombatSocket.Tail"), FString("Event fired during the Tail attack animations on the point to do damage."));

	// Cues
	GameplayTags.GameplayCue_MeleeImpact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.MeleeImpact"), FString("Cue used to replicate melee impacts."));
	GameplayTags.GameplayCue_ShockCast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Shock.Cast"), FString("Cue used to replicate shock casts."));
	GameplayTags.GameplayCue_ShockLoop = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Shock.Loop"), FString("Cue used to replicate shock loops."));
	GameplayTags.GameplayCue_ArcaneShards = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.ArcaneShards"), FString("Cue used to replicate arcane shards."));
	GameplayTags.GameplayCue_Fireblast = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayCue.Fireblast"), FString("Cue used for fire blast projectile hit."));
}

#include "AbilitySystem/Abilities/AuraFirebolt.h"

#include "AuraGameplayTags.h"

FString UAuraFirebolt::GetDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const int32 Damage = GetDamageByType(Level, FAuraGameplayTags::Get().Damage_Fire);
	return FString::Printf(TEXT(
		"<Title>FIRE BOLT</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><Mana>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Launches %d a bolt of fire, exploding on impact and dealing </>"
		"<Damage>%d</><Default> fire damage with a chance to burn.</>"), 
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxProjectiles), 
		Damage
	);
}

FString UAuraFirebolt::GetNextLevelDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const int32 Damage = GetDamageByType(Level, FAuraGameplayTags::Get().Damage_Fire);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><Mana>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Launches %d a bolt of fire, exploding on impact and dealing </>"
		"<Damage>%d</><Default> fire damage with a chance to burn.</>"), 
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxProjectiles), 
		Damage
	);
}

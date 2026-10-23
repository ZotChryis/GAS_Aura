#include "AbilitySystem/Abilities/AuraArcaneShards.h"

FString UAuraArcaneShards::GetDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const float ScaledDamage = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>ARCANE SHARDS</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><Mana>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Spawn %d arcane shards at target location, exploding and dealing </>"
		"<Damage>%.1f - %.1f</><Default> arcane damage, depending on distance from the shard.</>"), 
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxNumShards),
		0.f,
		ScaledDamage
	);
}

FString UAuraArcaneShards::GetNextLevelDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const float ScaledDamage = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><Mana>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Spawn %d arcane shards at target location, exploding and dealing </>"
		"<Damage>%.1f - %.1f</><Default> arcane damage, depending on distance from the shard.</>"), 
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxNumShards),
		0.f,
		ScaledDamage
	);
}

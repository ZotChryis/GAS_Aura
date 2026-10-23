#include "AbilitySystem/Abilities/AuraElectrocute.h"

FString UAuraElectrocute::GetDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const float ScaledDamage = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>ELECTROCUTE</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><Mana>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Channel a beam that connects up to %d targets, "
		"dealing <Damage>%.1f</> lightning damage per tick with a chance to stun when ending the beam.</>"), 
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxNumShockTargets), 
		ScaledDamage
	);
}

FString UAuraElectrocute::GetNextLevelDescription(int32 Level)
{
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	const float ScaledDamage = Damage.GetValueAtLevel(Level);
	return FString::Printf(TEXT(
		"<Title>NEXT LEVEL</>\n\n"
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><Mana>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		"<Default>Channel a beam that connects up to %d targets,</>"
		"<Default> dealing <Damage>%.1f</> lightning damage per tick with a chance to stun when ending the beam.</>"), 
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxNumShockTargets), 
		ScaledDamage
	);
}

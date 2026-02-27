#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForExperience(int32 Experience) const
{
	// We assume the 0th element is the 'starting level', which is level "1"
	for (int Level = 0; Level < LevelUpInformation.Num(); ++Level)
	{
		const FAuraLevelUpInfo& LevelUpInfo = LevelUpInformation[Level];
		if (Experience >= LevelUpInfo.LevelUpRequirement)
		{
			continue;
		}

		// We return a "player facing"
		return Level;
	}

	// If we got here, then the experience is past all level ups
	return LevelUpInformation.Num();
}

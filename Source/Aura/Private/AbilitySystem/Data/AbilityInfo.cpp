#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::GetAbilityInfoByTag(const FGameplayTag& Tag, const bool bLogNotFound)
{
	for (FAuraAbilityInfo Information : AbilityInformation)
	{
		if (Information.AbilityTag == Tag)
		{
			return Information;;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find info for AbilitTag %s on AbilityInfo %s"), *Tag.ToString(), *GetNameSafe(this));
	}
	
	return FAuraAbilityInfo();
}

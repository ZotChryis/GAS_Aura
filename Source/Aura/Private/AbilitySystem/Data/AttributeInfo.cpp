#include "AbilitySystem/Data/AttributeInfo.h"
#include "Aura/AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& Tag, bool bLogNotFound)
{
	for (FAuraAttributeInfo Information : AttributeInformation)
	{
		if (Information.AttributeTag.MatchesTagExact(Tag))
		{
			return Information;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."), *Tag.ToString(), *GetNameSafe(this));
	}

	return FAuraAttributeInfo();
}

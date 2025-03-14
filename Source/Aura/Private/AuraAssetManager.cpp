#include "AuraAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "Engine/Engine.h"

const UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);

	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
	FAuraGameplayTags::InitializeNativeGameplayTags();
	InitializeStaticTagsToCaptureDefsMap();
	
	// This is required to use TargetData. (Although it worked fine, unlike the tutorial videos)
	UAbilitySystemGlobals::Get().InitGlobalData();
}

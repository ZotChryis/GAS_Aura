#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraLoadScreenHud.generated.h"

class UMVVM_LoadScreen;
class ULoadScreenWidget;

UCLASS()
class AURA_API AAuraLoadScreenHud : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULoadScreenWidget> LoadScreenWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadScreen> LoadScreenViewModelClass;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULoadScreenWidget> LoadScreenWidget;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMVVM_LoadScreen> LoadScreenViewModel;
	
protected:
	virtual void BeginPlay() override;
};

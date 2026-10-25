#include "UI/HUD/AuraLoadScreenHud.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void AAuraLoadScreenHud::BeginPlay()
{
	Super::BeginPlay();
	
	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	LoadScreenViewModel->InitializeLoadSlots();
	
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();
	
	LoadScreenWidget->BlueprintInitializeWidget();
	LoadScreenViewModel->LoadData();
}

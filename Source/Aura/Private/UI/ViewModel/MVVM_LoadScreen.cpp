#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	
	LoadSlot0->LoadSlotName = FString("LoadSlot0");
	LoadSlot1->LoadSlotName = FString("LoadSlot1");
	LoadSlot2->LoadSlotName = FString("LoadSlot2");
	
	LoadSlot0->LoadSlotIndex = 0;
	LoadSlot1->LoadSlotIndex = 1;
	LoadSlot2->LoadSlotIndex = 2;
	
	LoadSlots.Add(0, LoadSlot0);
	LoadSlots.Add(1, LoadSlot1);
	LoadSlots.Add(2, LoadSlot2);
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (TTuple<int32, UMVVM_LoadSlot*> Pair : LoadSlots)
	{
		UMVVM_LoadSlot* LoadSlot = Pair.Value;
		UAuraLoadScreenSaveGame* SaveObject = AuraGameMode->GetSaveSlotData(LoadSlot->LoadSlotName, Pair.Key);
		LoadSlot->SetPlayerName(SaveObject->PlayerName);
		LoadSlot->SetMapName(SaveObject->MapName);
		LoadSlot->Status = SaveObject->Status;
		LoadSlot->InitializeSlot();
	}
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 SlotIndex)
{
	if (UMVVM_LoadSlot* LoadSlot = LoadSlots.FindChecked(SlotIndex))
	{
		LoadSlot->SetWidgetSwitcherIndex.Broadcast(1);
	}
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 SlotIndex, const FString& Name)
{
	if (UMVVM_LoadSlot* LoadSlot = LoadSlots.FindChecked(SlotIndex))
	{
		AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
		
		LoadSlot->SetPlayerName(Name);
		LoadSlot->SetMapName(AuraGameModeBase->StartingMapName);
		LoadSlot->Status = Taken;
		
		AuraGameModeBase->SaveSlotData(LoadSlot, SlotIndex);
		
		LoadSlot->SetWidgetSwitcherIndex.Broadcast(2);
	}
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 SlotIndex)
{
	if (UMVVM_LoadSlot* LoadSlot = LoadSlots.FindChecked(SlotIndex))
	{
		SelectedSlot = LoadSlot;
		
		for (TTuple<int32, UMVVM_LoadSlot*> Pair : LoadSlots)
		{
			const UMVVM_LoadSlot* Slot = Pair.Value;
			Slot->EnableSelectSlotButton.Broadcast(SlotIndex != Pair.Key);
			
			SlotSelected.Broadcast();
		}
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	if (SelectedSlot)
	{
		AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
		AuraGameModeBase->TravelToMap(SelectedSlot);
	}
}

void UMVVM_LoadScreen::DeleteSelectedSlot()
{
	if (SelectedSlot)
	{
		if (UMVVM_LoadSlot* LoadSlot = LoadSlots.FindChecked(SelectedSlot->LoadSlotIndex))
		{
			AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
			AuraGameModeBase->DeleteSlotData(LoadSlot, SelectedSlot->LoadSlotIndex);
		
			LoadSlot->EnableSelectSlotButton.Broadcast(true);
			LoadSlot->Status = Vacant;
			LoadSlot->SetWidgetSwitcherIndex.Broadcast(0);
		}
	}
	
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModel(int32 SlotIndex) const
{
	return LoadSlots.FindChecked(SlotIndex);
}

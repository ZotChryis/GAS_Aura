#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	
	LoadSlots.Add(0, LoadSlot0);
	LoadSlots.Add(1, LoadSlot1);
	LoadSlots.Add(2, LoadSlot2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModel(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

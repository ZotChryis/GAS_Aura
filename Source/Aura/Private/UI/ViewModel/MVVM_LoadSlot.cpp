#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot() const
{
	SetWidgetSwitcherIndex.Broadcast(Status.GetIntValue());
}

void UMVVM_LoadSlot::SetPlayerName(const FString& InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadSlot::SetMapName(const FString& InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

void UMVVM_LoadSlot::SetPlayerLevel(const int32 InPlayerLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InPlayerLevel);
}

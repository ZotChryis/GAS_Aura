#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/AuraLoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnabled);

UCLASS()
class AURA_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	
	void InitializeSlot() const;
	
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;
	
	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButton;
	
	UPROPERTY()
	FName PlayerStartTag;
	
	UPROPERTY()
	FString LoadSlotName;
	
	UPROPERTY()
	int32 LoadSlotIndex;
	
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> Status;
	
	void SetPlayerName(const FString& InPlayerName);
	FString GetPlayerName() const { return PlayerName; };
	
	void SetMapName(const FString& InMapName);
	FString GetMapName() const { return MapName; };
	
	void SetPlayerLevel(const int32 InPlayerLevel);
	int32 GetPlayerLevel() const { return PlayerLevel; };
	
private:
	
	/* Field Notifies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta = (AllowPrivateAccess = true))
	FString PlayerName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta = (AllowPrivateAccess = true))
	FString MapName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Getter, Setter, meta = (AllowPrivateAccess = true))
	int32 PlayerLevel;
};

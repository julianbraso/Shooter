// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickupBaseClass.h"
#include "MyUserWidget.generated.h"


/**
 * 
 */
UCLASS()
class MYFPS_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMyUserWidget(const class FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void SetActionText(EPickupType type, bool isPickup, FName name);
	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void SetCurrentWeaponText(FName name);
	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void ActionTextVisibility(bool OnOff);

	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void SetArmorIconTorsoVisibility(bool On);

	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void SetFiremodeText(FName text);

	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
		void BuildInventory();

	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void SetCurrentMagAmmoText(int32 ammo);
	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void SetCurrentPlayerAmmoText(int32 ammo);

	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void SetCurrentHealthBar(float health);
	
	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void ShowDamagedImage(float val);

	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void SetAlivePlayersText(int32 n);

	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void SetScoreText(int32 n);

	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void ShowTimer(bool b);

	UFUNCTION(BlueprintImplementableEvent, Category = MyUUserWidgetClass)
		void ShowCrosshair(bool b);

protected:
	
};

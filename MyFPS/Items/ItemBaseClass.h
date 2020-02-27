// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
//#include "WeaponBaseClass.h"
#include "WeaponIncludes.h"
#include "ItemIncludes.h"
#include "ItemBaseClass.generated.h"

class APickupBaseClass;
class AWeaponBaseClass;
class AWeaponAttachmentBaseClass;
class AArmorBaseClass;
class AClothingBaseClass;
class AAmmoItemClass;

enum class EAmmoType : uint8;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	IT_Weapon UMETA(DisplayName = "Weapon"),
	IT_Medicine UMETA(DisplayName = "Medicine"),
	IT_Armor UMETA(DisplayName = "Armor"),
	IT_Clothing UMETA(DisplayName = "Clothing"),
	IT_Ammo UMETA(DisplayName = "Ammo"),
	IT_Attachment UMETA(DisplayName = "Weapon Attachment"),
};


USTRUCT(BlueprintType)
struct FItemData {
	GENERATED_BODY();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CurrentIndex;							//current index in inventory
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Amount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EAmmoType AmmoType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isStackable;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Image;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<APickupBaseClass> PickupClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AWeaponBaseClass> WeaponClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AArmorBaseClass> ArmorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AClothingBaseClass> ClothingClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AAmmoItemClass> AmmoClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AWeaponAttachmentBaseClass> AttachmentClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CurrentSlot;
	FItemData() {
		ItemName = "";
		ItemID = 0;
		Description = "";
		Amount = 0;
		CurrentSlot = -1;					 //// if -1, its not on any slot
	}
};

UCLASS()
class MYFPS_API AItemBaseClass : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AItemBaseClass();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FItemData ItemData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

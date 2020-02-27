// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBaseClass.h"
#include "ArmorBaseClass.h"
#include "ItemBaseClass.h"
#include "PickupBaseClass.generated.h"


class AMyFPSCharacter;

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	PT_Weapon UMETA(DisplayName="Weapon"),
	PT_Medicine UMETA(DisplayName = "Medicine"),
	PT_Armor UMETA(DisplayName = "Armor"),
	PT_Clothing UMETA(DisplayName = "Clothing"),
	PT_Ammo UMETA(DisplayName = "Ammo"),
	PT_Attachments UMETA(DisplayName = "Attachments"),
};

USTRUCT(BlueprintType)
struct FPickupData {
	GENERATED_BODY();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EPickupType PickupType;
	UPROPERTY()
		FName ItemName;
	UPROPERTY()
		int32 ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Amount;
	UPROPERTY()
		TSubclassOf<AArmorBaseClass> ArmorClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AItemBaseClass> ItemClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundBase* SoundAtPickup;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 OutlineColor;
	FPickupData() {
		PickupType = EPickupType::PT_Weapon;
		ItemName = "";
		ItemID = 0;
		Amount = 1;
		OutlineColor = 0;
	}

};

UCLASS()
class MYFPS_API APickupBaseClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBaseClass();
	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* SM_Pickup;
	UPROPERTY(EditAnywhere)
		class UBoxComponent* GlowCollider;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FPickupData PickupData;

	UFUNCTION()
		void OnPickup();

	UFUNCTION(Client,Unreliable)
		void SetOutline(bool b);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		class USceneComponent* Root;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
		bool outlineStatus = false; 

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};

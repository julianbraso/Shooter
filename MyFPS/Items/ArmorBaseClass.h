// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBaseClass.h"
#include "ArmorBaseClass.generated.h"

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	AT_Tier1 UMETA(DisplayName = "Armor Tier 1"),
	AT_Tier2 UMETA(DisplayName = "Armor Tier 2"),
	AT_Tier3 UMETA(DisplayName = "Armor Tier 3"),
	AT_Tier4 UMETA(DisplayName = "Armor Tier 4")
};

UENUM(BlueprintType)
enum class EArmorPart : uint8
{
	AP_Head UMETA(DisplayName = "Armor Part Head"),
	AP_Torso UMETA(DisplayName = "Armor Part Torso")
};

USTRUCT(BlueprintType)
struct FArmorData {
	GENERATED_BODY();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EArmorType ArmorType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EArmorPart ArmorPart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ItemID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BlockDmg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AItemBaseClass> ItemClass;
	FArmorData(){
		ArmorType = EArmorType::AT_Tier1;
		Name = "";
		ItemID = 0;
		BlockDmg = 0.f;
	}
};

UCLASS()
class MYFPS_API AArmorBaseClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArmorBaseClass();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USkeletalMeshComponent* SK_Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FArmorData ArmorData;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};

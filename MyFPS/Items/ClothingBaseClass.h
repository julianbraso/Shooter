// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBaseClass.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "ClothingBaseClass.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EClothingType : uint8 {
	CT_Pants UMETA(DisplayName = "Pants"),
	CT_Torso UMETA(DisplayName = "Torso"),
	CT_Jacket UMETA(DisplayName = "Jacket"),
	CT_Boots UMETA(DisplayName = "Boots")
};
USTRUCT(BlueprintType)
struct FClothingData {
	GENERATED_BODY();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EClothingType ClothingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh * Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMesh * Mesh2;
};
UCLASS()
class MYFPS_API AClothingBaseClass : public AItemBaseClass
{
	GENERATED_BODY()
	AClothingBaseClass();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FClothingData ClothingData;
};

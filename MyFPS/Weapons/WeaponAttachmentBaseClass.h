// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBaseClass.h"
#include "WeaponBaseClass.h"
#include "WeaponAttachmentBaseClass.generated.h"

UENUM(BlueprintType)
enum class EAttachmentType : uint8 {
	AT_Sight UMETA(DisplayName = "Sight"),
	AT_Scope UMETA(DisplayName = "Scope"),
	AT_Grip UMETA(DisplayName = "Grip"),
	AT_Supressor UMETA(DisplayName = "Supressor")
};

UCLASS()
class MYFPS_API AWeaponAttachmentBaseClass : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponAttachmentBaseClass();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttachmentType AttachmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<EWeaponType> WeaponTypesAccepted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AItemBaseClass> ItemClass;

	UFUNCTION()
		UStaticMeshComponent * GetMesh_FP();

	UFUNCTION()
		UStaticMeshComponent * GetMesh_TP();

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* Mesh_FP;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* Mesh_TP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		class USceneComponent* root;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};

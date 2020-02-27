// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuPawnClass.h"
#include "MyPlayerState.h"
#include "ClothingBaseClass.h"
#include "Runtime/Engine/Classes/Components/SkeletalMeshComponent.h"


// Sets default values
AMainMenuPawnClass::AMainMenuPawnClass()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	SetRootComponent(root);

	Mesh3P_Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Legs"));
	Mesh3P_Legs->SetupAttachment(root);
	Mesh3P_Arms_lower = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Arms_lower"));
	Mesh3P_Arms_lower->SetupAttachment(Mesh3P_Legs);
	Mesh3P_Arms_upper = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Arms_upper"));
	Mesh3P_Arms_upper->SetupAttachment(Mesh3P_Legs);

	Mesh3P_Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Head"));
	Mesh3P_Head->SetupAttachment(Mesh3P_Legs);
	Mesh3P_Head->SetMasterPoseComponent(Mesh3P_Legs);

	Mesh3P_Torso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Torso"));
	Mesh3P_Torso->SetupAttachment(Mesh3P_Legs);
	Mesh3P_Torso->SetMasterPoseComponent(Mesh3P_Legs);

	Mesh3P_Jacket = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Jacket"));
	Mesh3P_Jacket->SetupAttachment(Mesh3P_Legs);
	Mesh3P_Jacket->SetMasterPoseComponent(Mesh3P_Legs);

	Mesh3P_Feet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Feet"));
	Mesh3P_Feet->SetupAttachment(Mesh3P_Legs);
	Mesh3P_Feet->SetMasterPoseComponent(Mesh3P_Legs);

	// Create a gun mesh component
	TP_BodyArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_BodyArmor"));
	TP_BodyArmor->bCastDynamicShadow = true;
	TP_BodyArmor->CastShadow = true;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	TP_BodyArmor->SetupAttachment(Mesh3P_Legs);
}

// Called when the game starts or when spawned
void AMainMenuPawnClass::BeginPlay()
{
	Super::BeginPlay();

	NakedArms = Mesh3P_Arms_lower->SkeletalMesh;
	NakedHead = Mesh3P_Head->SkeletalMesh;
	NakedTorso = Mesh3P_Torso->SkeletalMesh;
	NakedLegs = Mesh3P_Legs->SkeletalMesh;
	NakedFeet = Mesh3P_Feet->SkeletalMesh;

	Mesh3P_Arms_lower->SetMasterPoseComponent(Mesh3P_Legs);
	Mesh3P_Arms_upper->SetMasterPoseComponent(Mesh3P_Legs);
	Mesh3P_Head->SetMasterPoseComponent(Mesh3P_Legs);
	Mesh3P_Torso->SetMasterPoseComponent(Mesh3P_Legs);
	Mesh3P_Jacket->SetMasterPoseComponent(Mesh3P_Legs);
	Mesh3P_Feet->SetMasterPoseComponent(Mesh3P_Legs);

	PState = Cast<AMyPlayerState>(PlayerState);

}

// Called every frame
void AMainMenuPawnClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainMenuPawnClass::ChangeClothing(FClothingData c)
{
	switch (c.ClothingType)
	{
	case EClothingType::CT_Boots:
		if (Mesh3P_Feet->SkeletalMesh == c.Mesh)
		{
			Mesh3P_Feet->SetSkeletalMesh(NakedFeet);
			break;
		}
		Mesh3P_Feet->SetSkeletalMesh(c.Mesh);
		break;
	case EClothingType::CT_Jacket:
		if (Mesh3P_Jacket->SkeletalMesh == c.Mesh)
		{
			Mesh3P_Jacket->SetSkeletalMesh(NULL);
			if (Mesh3P_Torso->SkeletalMesh)
			{
				if (PState)
				{
					TArray<FItemData> a = PState->GetClothing();
					for (int i = 0; i < a.Num();i++ )
					{
						if (a[i].ClothingClass->GetDefaultObject<AClothingBaseClass>()->ClothingData.ClothingType == EClothingType::CT_Torso)
						{
							Mesh3P_Arms_upper->SetSkeletalMesh(a[i].ClothingClass->GetDefaultObject<AClothingBaseClass>()->ClothingData.Mesh2);
						}
					}
				}
				
			}
			break;
		}
		Mesh3P_Jacket->SetSkeletalMesh(c.Mesh);
		if (Mesh3P_Torso->SkeletalMesh)
		{
			Mesh3P_Arms_upper->SetSkeletalMesh(NULL);
		}
		break;
	case EClothingType::CT_Pants:
		if (Mesh3P_Legs->SkeletalMesh == c.Mesh)
		{
			Mesh3P_Legs->SetSkeletalMesh(NakedLegs);
			break;
		}
		Mesh3P_Legs->SetSkeletalMesh(c.Mesh);
		break;
	case EClothingType::CT_Torso:
		if (Mesh3P_Torso->SkeletalMesh == c.Mesh)
		{
			Mesh3P_Torso->SetSkeletalMesh(NakedTorso);
			Mesh3P_Arms_upper->SetSkeletalMesh(NULL);
			break;
		}
		Mesh3P_Torso->SetSkeletalMesh(c.Mesh);
		if (c.Mesh2) {
			if (!Mesh3P_Jacket->SkeletalMesh)
			{
				Mesh3P_Arms_upper->SetSkeletalMesh(c.Mesh2);
			}
		}
		break;
	default:
		break;
	}
}

// Called to bind functionality to input
void AMainMenuPawnClass::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


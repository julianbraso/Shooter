// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupBaseClass.h"
#include "Runtime/Engine/Classes/Components/BoxComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "WeaponBaseClass.h"
#include "ItemBaseClass.h"
#include "MyFPSCharacter.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"

// Sets default values
APickupBaseClass::APickupBaseClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SM_Pickup = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup_SMesh"));
	SM_Pickup->SetupAttachment(Root);
	SM_Pickup->SetRelativeLocation(FVector(-10,-10,1.5));
	SM_Pickup->SetCustomDepthStencilValue(PickupData.OutlineColor);
	SM_Pickup->SetCollisionProfileName("PickupObject");
	GlowCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Glow Collider"));
	GlowCollider->SetupAttachment(Root);
	GlowCollider->SetBoxExtent(FVector(128,128,64));
	GlowCollider->SetRelativeLocation(FVector(-10,-10,63));
	GlowCollider->BodyInstance.SetCollisionProfileName("Trigger");
	GlowCollider->OnComponentBeginOverlap.AddDynamic(this, &APickupBaseClass::OnOverlapBegin);
	GlowCollider->OnComponentEndOverlap.AddDynamic(this, &APickupBaseClass::OnOverlapEnd);
	Tags.Add("Pickup");

	bReplicates = true;

}

void APickupBaseClass::OnPickup()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "OnPickup function");
	if (!IsValidLowLevel()) return;
	this->Destroy();
	return;
}

// Called when the game starts or when spawned
void APickupBaseClass::BeginPlay()
{
	Super::BeginPlay();
	SM_Pickup->SetCustomDepthStencilValue(PickupData.OutlineColor);
	if (PickupData.ItemClass)
	{
		FItemData temp = PickupData.ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData;
		PickupData.ItemName = temp.ItemName;
		PickupData.ItemID = temp.ItemID;
		PickupData.ArmorClass = temp.ArmorClass;
	}
}

void APickupBaseClass::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ACharacter>())
	{
		APlayerController* c = Cast<APlayerController>(OtherActor->GetInstigatorController());
		if (c)
		{
			if (c != GetWorld()->GetFirstPlayerController())
			{
				GLog->Log("Not local player FUCK OFFFFFFFF");
				return;
			}
			SetOutline(true);
		}
	}
}

void APickupBaseClass::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<ACharacter>())
	{
		APlayerController* c = Cast<APlayerController>(OtherActor->GetInstigatorController());
		if (c)
		{
			if (c != GetWorld()->GetFirstPlayerController())
			{
				GLog->Log("Not local player FUCK OFFFFFFFF");
				return;
			}
			SetOutline(false);
		}
	}
}

void APickupBaseClass::SetOutline_Implementation(bool b)
{
	this->SM_Pickup->SetRenderCustomDepth(b);
}
// Called every frame
void APickupBaseClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


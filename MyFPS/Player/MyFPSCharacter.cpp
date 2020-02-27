// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MyFPSCharacter.h"
//#include "MyFPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "DrawDebugHelpers.h"
#include "UnrealNetwork.h"
#include "Runtime/Core/Public/Math/TransformNonVectorized.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Slate.h"
#include "Blueprint/UserWidget.h"
#include "PickupBaseClass.h"
#include "Pickup/WeaponPickup.h"
#include "Items/AmmoItemClass.h"
#include "MyUserWidget.h"
#include "GameInstanceSteam.h"
#include "WeaponBaseClass.h"
#include "WeaponAttachmentBaseClass.h"
#include "ArmorBaseClass.h"
#include "ItemBaseClass.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "ClothingBaseClass.h"
#include "Pickup/ClothingPickupClass.h"
#include "MyFPSHUD.h"
#include "MyFPSGameMode.h"
#include "Runtime/Engine/Public/CollisionDebugDrawingPublic.h"
#include "Runtime/Engine/Classes/Components/TimelineComponent.h"



DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMyFPSCharacter

AMyFPSCharacter::AMyFPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	traceDistance = 300.f;
	FpsTraceDistance = 420.f;
	bReplicates = true;
	bReplicateMovement = true;

	SetbUseHandIK(false);	//bUseHandIk = false;

	// Create a CameraComponent	

	Mesh1P_Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P_Legs"));
	Mesh1P_Legs->SetOnlyOwnerSee(true);
	Mesh1P_Legs->CastShadow = false;
	Mesh1P_Legs->bCastDynamicShadow = false;
	Mesh1P_Legs->SetupAttachment(RootComponent);

	Mesh1P_Arms_lower = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P_Arms_lower"));
	Mesh1P_Arms_lower->SetOnlyOwnerSee(true);
	Mesh1P_Arms_lower->SetupAttachment(Mesh1P_Legs);					/*FirstPersonCameraComponent*/
	Mesh1P_Arms_lower->bCastDynamicShadow = false;
	Mesh1P_Arms_lower->CastShadow = false;
	/*Mesh1P_Arms_lower->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P_Arms_lower->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);*/

	Mesh1P_Arms_upper = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P_Arms_upper"));
	Mesh1P_Arms_upper->SetOnlyOwnerSee(true);
	Mesh1P_Arms_upper->SetupAttachment(Mesh1P_Legs);
	Mesh1P_Arms_upper->bCastDynamicShadow = false;
	Mesh1P_Arms_upper->CastShadow = false;

	Mesh1P_Torso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P_Torso"));
	Mesh1P_Torso->SetOnlyOwnerSee(true);
	Mesh1P_Torso->CastShadow = false;
	Mesh1P_Torso->bCastDynamicShadow = false;
	Mesh1P_Torso->SetupAttachment(Mesh1P_Legs);
	Mesh1P_Torso->SetMasterPoseComponent(Mesh1P_Legs);

	Mesh1P_Jacket = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P_Jacket"));
	Mesh1P_Jacket->SetOnlyOwnerSee(true);
	Mesh1P_Jacket->CastShadow = false;
	Mesh1P_Jacket->bCastDynamicShadow = false;
	Mesh1P_Jacket->SetupAttachment(Mesh1P_Legs);
	Mesh1P_Jacket->SetMasterPoseComponent(Mesh1P_Legs);

	Mesh1P_Feet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P_Feet"));
	Mesh1P_Feet->SetOnlyOwnerSee(true);
	Mesh1P_Feet->CastShadow = false;
	Mesh1P_Feet->bCastDynamicShadow = false;
	Mesh1P_Feet->SetupAttachment(Mesh1P_Legs);
	_b = true;

	FPCameraSceneComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("FPCameraSceneComp"));
	FPCameraSceneComp->SetupAttachment(RootComponent);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(FPCameraSceneComp); //GetCapsuleComponent()
	//FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent()); //GetCapsuleComponent()
	//FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	ActiveCamera = FirstPersonCameraComponent;       //////for future tpp

	TransitionCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TransitionCamera"));
	TransitionCamera->bAutoActivate = false;
	TransitionCamera->SetupAttachment(FirstPersonCameraComponent);

	ThirdPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));

	ADSCamera = CreateDefaultSubobject<USpringArmComponent>(TEXT("ADSCameraSpring"));
	//ADSCamera->TargetArmLength = 50.f;
	//ADSCamera->bUsePawnControlRotation = true;
	

	ADSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ADSCameraComp"));
	ADSCameraComponent->bUsePawnControlRotation = true;
	ADSCameraComponent->FieldOfView = 70.f;
	ADSCameraComponent->SetupAttachment(ADSCamera);

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraSpringArm->TargetArmLength = 200.f;
	CameraSpringArm->RelativeLocation=FVector(0,30,65);
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->SetupAttachment(RootComponent);
	ThirdPersonCameraComponent->SetupAttachment(CameraSpringArm);
	ADSCamera->SetupAttachment(RootComponent);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)


	Mesh3P_Legs = Super::GetMesh();

	Mesh3P_Legs->OnComponentHit.AddDynamic(this, &AMyFPSCharacter::OnHit);

	Mesh3P_Arms_lower = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Arms_lower"));
	Mesh3P_Arms_lower->SetOwnerNoSee(true);
	Mesh3P_Arms_lower->SetupAttachment(Mesh3P_Legs);
	Mesh3P_Arms_upper = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Arms_upper"));
	Mesh3P_Arms_upper->SetOwnerNoSee(true);
	Mesh3P_Arms_upper->SetupAttachment(Mesh3P_Legs);

	Mesh3P_Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Head"));
	Mesh3P_Head->SetOwnerNoSee(true);
	Mesh3P_Head->SetupAttachment(Mesh3P_Legs);
	Mesh3P_Head->SetMasterPoseComponent(Mesh3P_Legs);

	Mesh3P_Torso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Torso"));
	Mesh3P_Torso->SetOwnerNoSee(true);
	Mesh3P_Torso->SetupAttachment(Mesh3P_Legs);
	Mesh3P_Torso->SetMasterPoseComponent(Mesh3P_Legs);

	Mesh3P_Jacket = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Jacket"));
	Mesh3P_Jacket->SetOwnerNoSee(true);
	Mesh3P_Jacket->SetupAttachment(Mesh3P_Legs);
	Mesh3P_Jacket->SetMasterPoseComponent(Mesh3P_Legs);

	Mesh3P_Feet = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P_Feet"));
	Mesh3P_Feet->SetOwnerNoSee(true);
	Mesh3P_Feet->SetupAttachment(Mesh3P_Legs);
	Mesh3P_Feet->SetMasterPoseComponent(Mesh3P_Legs);

	FP_BodyArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_BodyArmor"));
	FP_BodyArmor->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_BodyArmor->bCastDynamicShadow = false;
	FP_BodyArmor->CastShadow = false;
	FP_BodyArmor->SetupAttachment(Mesh1P_Legs);
	

	// Create a gun mesh component
	TP_BodyArmor = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_BodyArmor"));
	TP_BodyArmor->SetOwnerNoSee(true);			// only the owning player will see this mesh
	TP_BodyArmor->bCastDynamicShadow = true;
	TP_BodyArmor->CastShadow = true;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	TP_BodyArmor->SetupAttachment(Mesh3P_Legs);

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/Effects/FloatCurve"));
	check(Curve.Succeeded());
	FloatCurve = Curve.Object;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(80.0f, 0.0f, 0.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	WalkSpeed = 200.f;
	RunSpeed = 400.f;
	SprintSpeed = 600.f;
	CrouchSpeed = 150.f;

	b_CanJump = true;

	bIsCrouching = false;

	bIsUnarmed = true;
	
	fHealth = 100.f;
	//pickupRef = NewObject<APickupBaseClass>();

	WeaponInventory.SetNum(3);

	SetbIsFPS(false);

	bPistolEquipped = false;

	bRifleEquipped = false;

	bCanAim = true;

	//TransitionCamera->bAbsoluteLocation = true;
	ADSCamera->bAbsoluteLocation = false;
	ADSCamera->bUsePawnControlRotation = true;

	CanChangeCamera = true;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio component"));

	_c = false;

	bAlwaysRelevant = true;

}

void AMyFPSCharacter::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	if (CamTimeline != NULL)
	{
		//CamTimeline->TickComponent(deltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}

}

bool AMyFPSCharacter::GetIsADS()
{
	return bIsADS;
}

bool AMyFPSCharacter::EjectWeaponMag_Validate() {
	return true;
}

void AMyFPSCharacter::EjectWeaponMag_Implementation()
{
	EjectWeaponMag_All();
}

void AMyFPSCharacter::EjectWeaponMag_All_Implementation() {
	if (CurrentWeapon)
	{
		CurrentWeapon->EjectMag();
	}
}

bool AMyFPSCharacter::SpawnWeaponMag_Validate() {
	return true;
}

void AMyFPSCharacter::SpawnWeaponMag_Implementation()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->SpawnMag(bIsFPS);
	}
}

void AMyFPSCharacter::SetUseHandIKTrue()
{
	SetbUseHandIK(true);
}

void AMyFPSCharacter::SetUseHandIKFalse()
{
	SetbUseHandIK(false);
}

void AMyFPSCharacter::RefreshADSCamPos_Implementation()
{
	if (CurrentWeapon)
	{
		//ADSCamera->AttachToComponent(CurrentWeapon->SK_FP_Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		ADSCamera->SetRelativeLocation(CurrentWeapon->ADSCameraComponent->RelativeLocation);
		//ADSCamera->SetRelativeRotation(CurrentWeapon->ADSCameraComponent->RelativeRotation);
	}
}

void AMyFPSCharacter::CallChangeWeapVisib_Implementation()
{
	ChangeWeaponsVisibility(bIsFPS);
}

UCameraComponent * AMyFPSCharacter::GetADSCam()
{
	return ADSCameraComponent;
}

void AMyFPSCharacter::FakeTick() {
	
	if (ActiveCamera != NULL)
	{
		SetPitch(ActiveCamera->GetComponentRotation().Pitch);
//		fPitch = ActiveCamera->GetComponentRotation().Pitch;
	}

	if (fHealth <= 0.f)
	{
		if (!bIsDead)
		{
			if (!_c)
			{
				Death(UGameplayStatics::GetPlayerController(GetWorld(),0));
				_c = true;
			}
			else {
				PrintToScreen("c = true;");
			}
			
		}
		else {
			PrintToScreen("IM DEAD");
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::SanitizeFloat(fPitch));
}

bool AMyFPSCharacter::EquipAttachment_Validate(TSubclassOf<AWeaponAttachmentBaseClass> attachment) {
	return true;
}

void AMyFPSCharacter::EquipAttachment_Implementation(TSubclassOf<AWeaponAttachmentBaseClass> attachment)
{
	AWeaponAttachmentBaseClass * a = attachment->GetDefaultObject<AWeaponAttachmentBaseClass>();
	if (!a->ItemClass)
	{
		return;
	}
	int32 i = FindItemInInventory(a->ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData);
	if (i != -1)
	{
		RemoveItemFromInventoryAtIndex(i);
		
	}
	else {
		PrintToScreen("NO ITEM FOUND IN INVENTORY!");
	}
	BuildClientInventory();
	CallChangeWeapVisib();
}

void AMyFPSCharacter::BeginPlay()
{	

	// Call the base class
	Super::BeginPlay();


	//Default_Feet_Mesh = Mesh3P_Feet->SkeletalMesh;
	//Default_Legs_Mesh = Mesh3P_Legs->SkeletalMesh;
	//Default_Torso_Mesh = Mesh3P_Torso->SkeletalMesh;

	

	Mesh1P_Arms_lower->SetHiddenInGame(false, true);
	Mesh1P_Arms_upper->SetHiddenInGame(false, true);
	Mesh1P_Arms_lower->SetMasterPoseComponent(Mesh1P_Legs);
	Mesh1P_Arms_upper->SetMasterPoseComponent(Mesh1P_Legs); //Mesh1P_Arms_lower
	/*Initialize bodyparts first person*/
	Mesh1P_Torso->SetMasterPoseComponent(Mesh1P_Legs);
	Mesh1P_Jacket->SetMasterPoseComponent(Mesh1P_Legs);
	Mesh1P_Feet->SetMasterPoseComponent(Mesh1P_Legs);
	FP_BodyArmor->SetMasterPoseComponent(Mesh1P_Legs);
	/*Initialize third person bodyparts*/
	Mesh3P_Arms_lower->SetMasterPoseComponent(Mesh3P_Legs);
	Mesh3P_Arms_upper->SetMasterPoseComponent(Mesh3P_Legs);
	Mesh3P_Head->SetMasterPoseComponent(Mesh3P_Legs);
	Mesh3P_Torso->SetMasterPoseComponent(Mesh3P_Legs);
	Mesh3P_Jacket->SetMasterPoseComponent(Mesh3P_Legs);
	Mesh3P_Feet->SetMasterPoseComponent(Mesh3P_Legs);
	TP_BodyArmor->SetMasterPoseComponent(Mesh3P_Legs);
	//FirstPersonCameraComponent->AttachToComponent(Mesh1P_Legs, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("camSocket")));			///comento esto para recordar que falta posicionar la camara de fps segun el socket y a la hora de hacer crouch
	FPCameraSceneComp->AttachToComponent(Mesh1P_Legs, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("camSocket")));	
	FPCameraSceneComp->SetWorldTransform(Mesh1P_Legs->GetSocketTransform(FName(TEXT("camSocket"))));
	//FirstPersonCameraComponent->SetWorldTransform(Mesh1P_Legs->GetSocketTransform(FName(TEXT("camSocket"))));
	TransitionCamera->SetWorldTransform(FirstPersonCameraComponent->GetComponentTransform());
	/*		Timer for HUD trace	*/

	
		
		FOnTimelineFloat onTimelineCallback;
		FOnTimelineEventStatic onTimelineFinishedCallback;
		if (FloatCurve != NULL) {
			CamTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));
			/*CamTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
			this->BlueprintCreatedComponents.Add(CamTimeline);
			CamTimeline->SetNetAddressable();

			CamTimeline->SetPropertySetObject(this);
			CamTimeline->SetDirectionPropertyName(FName("TimelineFinishedCallback"));*/

			CamTimeline->SetLooping(false);
			/*CamTimeline->SetTimelineLength(2.f);
			CamTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);*/

			CamTimeline->SetPlaybackPosition(0.0f, false);
			onTimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
			onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
			CamTimeline->AddInterpFloat(FloatCurve, onTimelineCallback, FName("Alpha"));
			CamTimeline->SetTimelineFinishedFunc(onTimelineFinishedCallback);
			CamTimeline->SetIgnoreTimeDilation(true);

			CamTimeline->RegisterComponent();

		}
		//Mesh1P_Arms->AttachToComponent(Mesh1P_Legs, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("camSocket")));

		

		CamStartPos = ThirdPersonCameraComponent->GetRelativeTransform().GetLocation();
		Mesh3pStartPos = Mesh3P_Legs->GetRelativeTransform().GetLocation();
		CapsuleDefaultSize = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		ChangeCamera(false);
		

	if (IsLocallyControlled())
	{
		
		GetWorld()->FlushLevelStreaming();
		GEngine->BlockTillLevelStreamingCompleted(GetWorld());
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AMyFPSCharacter::HUDTrace, 0.06f, true);
		GetWorldTimerManager().SetTimer(TimerHandle2, this, &AMyFPSCharacter::FakeTick, 0.01f, true);

		/*		Initialize HUD	*/
		if (HUDWidgetRef) {
			HUDWidget = CreateWidget<UMyUserWidget>(GetWorld()->GetGameInstance(), HUDWidgetRef);
			if (HUDWidget) {
				HUDWidget->AddToViewport();
			}
		}

		GS = GetWorld()->GetGameState<AMyGameStateBase>();
		
		PS = Cast<AMyPlayerState>(this->PlayerState);

		PrintToScreen("Character Init: IS locally controlled!");
	}
	else {
		PrintToScreen("Character Init: not locally controlled...");
	}
	ClientHandleClothing();
	InitClothing();

	if (!HasAuthority())
	{
		PrintToScreen("error no authority");
		return;
	}
	else {

		AMyFPSGameMode* GM;
		GM = Cast<AMyFPSGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			bFPPOnly = GM->bFPPOnly;
			PrintToScreen(bFPPOnly ? "true" : "false");
		}
		else {
			PrintToScreen("error in gamemode cast");
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

FHitResult AMyFPSCharacter::TraceFromCamera(UCameraComponent * currentCamera, float distance)
{
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);

	FHitResult RV_Hit(ForceInit);
	FVector startTrace = currentCamera->GetComponentLocation();
	FVector endTrace = startTrace + (GetControlRotation().Vector() * distance);
	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, startTrace, endTrace, ECC_Camera, RV_TraceParams)) {
		
	}
	return RV_Hit;
}

FHitResult AMyFPSCharacter::TraceFromOrigin(FVector origin, float distance)
{
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);

	FHitResult RV_Hit(ForceInit);
	FVector startTrace = origin;
	FVector endTrace = startTrace + (GetControlRotation().Vector() * distance);
	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, startTrace, endTrace, ECC_Camera, RV_TraceParams)) {

	}
	return RV_Hit;
}

void AMyFPSCharacter::HUDTrace()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (HUDWidget) {
		HUDWidget->SetCurrentHealthBar(fHealth);
		if (GS)
		{
			HUDWidget->SetAlivePlayersText(GS->AlivePlayers);
		} else {
			PrintToScreen("No gamestate...");
		}
		if (PS)
		{
			HUDWidget->SetScoreText(PS->Score);
		}
		else {
			PrintToScreen("No PlayerState...");
			if (this->PlayerState)
			{
				PS = Cast<AMyPlayerState>(this->PlayerState);
			}
		}
		
	}

	if (CurrentWeapon) {																										/* Takes care of HUD info updates */
		if (HUDWidget) {
			FName name = CurrentWeapon->WeaponData.WeaponName;
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, CurrentWeapon->WeaponData.WeaponName.ToString());
			HUDWidget->SetCurrentWeaponText(name);
			HUDWidget->SetCurrentMagAmmoText(CurrentWeapon->WeaponData.CurrentAmmo);
			int32 totalAmmo = GetCurrentAmmoByType(CurrentWeapon->WeaponData.AmmoType);
			if (totalAmmo != -1)
			{
				HUDWidget->SetCurrentPlayerAmmoText(totalAmmo);
			}
			else {
				HUDWidget->SetCurrentPlayerAmmoText(00);
			}
			if (CurrentWeapon->WeaponData.FireMode == EWeaponFireMode::FM_Auto)
			{
				HUDWidget->SetFiremodeText("Auto");
			}
			else if (CurrentWeapon->WeaponData.FireMode == EWeaponFireMode::FM_Single)
			{
				HUDWidget->SetFiremodeText("Single");
			}
		}
	}
	else if (!CurrentWeapon) {
		if (HUDWidget) {
			//FName name = CurrentWeapon->WeaponData.WeaponName;
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, CurrentWeapon->WeaponData.WeaponName.ToString());
			HUDWidget->SetCurrentWeaponText("None");
			HUDWidget->SetFiremodeText("None");
			HUDWidget->SetCurrentPlayerAmmoText(00);
			HUDWidget->SetCurrentMagAmmoText(00);		}
	}

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);
	if (ActiveCamera)
	{

		
		FHitResult HUDTraceHit(ForceInit);
		FVector startTrace = ActiveCamera->GetComponentLocation();                    //////////trazo funciona en fps pero no en tpp
		float distance;
		if (bIsFPS)
		{
			distance = traceDistance;
		}
		else {
			distance = 420.f;		///tpp distancetrace
		}
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, ActiveCamera->GetName());
		FVector endTrace = startTrace + (GetControlRotation().Vector() * distance);
		if (GetWorld()->LineTraceSingleByChannel(HUDTraceHit, startTrace, endTrace, ECC_Camera, RV_TraceParams)) {
			//DrawDebugLine(GetWorld(), HUDTraceHit.TraceStart, HUDTraceHit.TraceEnd, FColor::Red, false, 5.0f, 0, 2.0f);
			FName HitActorTag = "";
			if (GEngine)
				if (HUDTraceHit.GetActor()->Tags.GetData() != nullptr) {
					HitActorTag = *HUDTraceHit.GetActor()->Tags.GetData();
				}
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, HitActorTag.ToString());
		}
		//DrawDebugLine(GetWorld(), HUDTraceHit.TraceStart, HUDTraceHit.TraceEnd, FColor::Red, false, 5.0f, 0, 2.0f);

		if (HUDTraceHit.bBlockingHit) {
			if (HUDTraceHit.GetActor()->ActorHasTag("Pickup")) {                              /*	Using tags to organize objects in front of you (usables,pickups)	 */
				if (pickupRef != NULL) {
					pickupRef = NULL;
				}
				if (!pickupRef) {
					pickupRef = Cast<APickupBaseClass>(HUDTraceHit.GetActor());
					if (HUDWidget) {
						HUDWidget->ActionTextVisibility(false);
					}
				}
				if (pickupRef) {
					if (HUDWidget) {
						HUDWidget->SetActionText(pickupRef->PickupData.PickupType, true, pickupRef->PickupData.ItemName);
						//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, pickupRef->PickupData.ItemName.ToString());
						HUDWidget->ActionTextVisibility(true);
					}
				}
			}
			else {
				if (HUDWidget) {
					HUDWidget->ActionTextVisibility(false);
				}
				if (pickupRef != NULL) {
					pickupRef = NULL;
				}
			}
		}
		else {
			if (HUDWidget) {
				HUDWidget->ActionTextVisibility(false);
				if (pickupRef != NULL) {
					pickupRef = NULL;
				}
			}
		}
	}
}

void AMyFPSCharacter::ShowHideInventory()
{
	if (bIsADS)
	{
		StopADS();
	}
	if (!InventoryWidget) {
		if (InventoryWidgetRef) {
			//InventoryWidget = CreateWidget<UUserWidget>(GetWorld()->GetGameInstance(), InventoryWidgetRef);
			InventoryWidget = CreateWidget<UMyUserWidget>(GetWorld()->GetGameInstance(), InventoryWidgetRef);
			if (InventoryWidget) {
				InventoryWidget->AddToViewport();
				InventoryWidget->BuildInventory();
				bIsInInventory = true;
				GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->bShowMouseCursor = true;
				GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->SetInputMode(FInputModeGameAndUI());
			}
		}
	}
	else {
		if (bIsInInventory)
		{
			bIsInInventory = false;
			InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
			GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->bShowMouseCursor = false;
			GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->SetInputMode(FInputModeGameOnly());
		}
		else {
			bIsInInventory = true;
			InventoryWidget->BuildInventory();
			InventoryWidget->SetVisibility(ESlateVisibility::Visible);
			GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->bShowMouseCursor = true;
			GetWorld()->GetFirstLocalPlayerFromController()->GetPlayerController(GetWorld())->SetInputMode(FInputModeGameAndUI());
		}
	}
}

void AMyFPSCharacter::InitWeaponInventory()
{
	
}

void AMyFPSCharacter::InitClothing()
{
	InitClothing_Server();
}

bool AMyFPSCharacter::InitClothing_Server_Validate() {
	return true;
}

void AMyFPSCharacter::InitClothing_Server_Implementation()
{
	AMyPlayerState* pS = Cast<AMyPlayerState>(PlayerState);
	TArray<FItemData> cArray;
	if (pS)
	{
		cArray = pS->GetClothing();
		if (cArray.Num() > 0)
		{
			for (int i = 0; i < cArray.Num(); i++)
			{
				if (HasAuthority())
				{
					AddItemToInventory(cArray[i], 1);
				}
				EquipCloth(cArray[i].ClothingClass->GetDefaultObject<AClothingBaseClass>());
			}
		}
	}
	else {
		PrintToScreen("WE DONE FUCKED UP!");
	}
}

bool AMyFPSCharacter::InitClothing_All_Validate() {
	return true;
}

void AMyFPSCharacter::InitClothing_All_Implementation()
{

}

void AMyFPSCharacter::ClientHandleClothing_Implementation()
{
	UGameInstanceSteam* gI = Cast<UGameInstanceSteam>(GetWorld()->GetGameInstance());
	if (gI)
	{
		AMyPlayerState* pS = Cast<AMyPlayerState>(PlayerState);
		if (pS)
		{
			pS->SetClothing(gI->Clothing_Feet);
			pS->SetClothing(gI->Clothing_Torso);
			pS->SetClothing(gI->Clothing_Legs);
			pS->SetClothing(gI->Clothing_Jacket);
			PrintToScreen("Done Handling clothing on client!");
			PrintToScreen(gI->Clothing_Feet.ItemName.ToString());
			PrintToScreen(gI->Clothing_Torso.ItemName.ToString());
			PrintToScreen(gI->Clothing_Legs.ItemName.ToString());
			PrintToScreen(gI->Clothing_Jacket.ItemName.ToString());
		}
	}
}

void AMyFPSCharacter::AddWeaponToInventory(AWeaponBaseClass * weapon)
{
	if (weapon->WeaponData.WeaponType == EWeaponType::WT_Pistol) {
		if (!WeaponInventory[2]) {
			WeaponInventory[2] = weapon;
			//HolsterWeapon(weapon);
		}
		return;
	}
	for (int i = 0; i < 2; i++)
	{
		if (!WeaponInventory[i]) {
			WeaponInventory[i] = weapon;
			//HolsterWeapon(weapon);
			break;
		}
	}
}

bool AMyFPSCharacter::CanPickupWeapon(TSubclassOf<AWeaponBaseClass> weapon)
{
	/*if (CurrentWeapon) {
		if (CurrentWeapon->GetClass() == weapon->GetClass())
		{
     			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Same weapon");
			return false;
		}
	}*/
	
	if (weapon->GetDefaultObject<AWeaponBaseClass>()->WeaponData.WeaponType == EWeaponType::WT_AR || weapon->GetDefaultObject<AWeaponBaseClass>()->WeaponData.WeaponType == EWeaponType::WT_SMG || weapon->GetDefaultObject<AWeaponBaseClass>()->WeaponData.WeaponType == EWeaponType::WT_Shotgun)
	{
		if (WeaponInventory[0] && WeaponInventory[1]) //// both slots for ar occupied
		{
			PrintToScreen("AR Slots Occupied");
			return false;
		}
	} 
	if (weapon->GetDefaultObject<AWeaponBaseClass>()->WeaponData.WeaponType == EWeaponType::WT_Pistol) {
		if (WeaponInventory[2])
		{
			PrintToScreen("Pistol slot Occupied");
			return false;
		}
	}

	//int HasNrOfWeapons = 0;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, weapon->GetName());
	for (int i = 0; i<3;i++)
	{
		if (WeaponInventory[i]) {
			//HasNrOfWeapons += 1;
			if (WeaponInventory[i]->IsA(weapon)) {
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, "Already have that weapon.");
				return false;
				break;
			}
		}

	}
	return true;
}

bool AMyFPSCharacter::CanPickupThisItem(APickupBaseClass * pickup)
{
	return false;
}

void AMyFPSCharacter::PrintToScreen(FString text) {

	/*if (!IsLocallyControlled())
	{
		return;
	}*/

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, text);
}


bool AMyFPSCharacter::RPC_Server_UnequipCloth_Validate(AClothingBaseClass * cloth) {
	return true;
}
void AMyFPSCharacter::UnequipCloth(AClothingBaseClass * cloth)
{
	if (HasAuthority()) {
		RPC_Server_UnequipCloth(cloth);
	}
	else {
		if (Role == ROLE_AutonomousProxy) {
			RPC_Server_UnequipCloth(cloth);
		}
		return;
	}
}

void AMyFPSCharacter::RPC_Server_UnequipCloth_Implementation(AClothingBaseClass * cloth)
{
	if (cloth)
	{
		if (cloth->ClothingData.ClothingType == EClothingType::CT_Boots)
		{
			Clothing_Feet = NULL;
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Torso)
		{
			Clothing_Torso = NULL;
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Jacket)
		{
			Clothing_Jacket = NULL;
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Pants)
		{
			Clothing_Legs = NULL;
		}
		//RPC_All_UnequipCloth(cloth);
		RPC_Client_EquipCloth(cloth);							//// this actually calls the OnReps on the client, nevermind the name...
		if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone)
		{
			OnRep_Feet();
			OnRep_Jacket();
			OnRep_Legs();
			OnRep_Torso();
		}
	}
}

void AMyFPSCharacter::RPC_All_UnequipCloth_Implementation(AClothingBaseClass * cloth) {
	if (cloth)
	{
		if (cloth->ClothingData.ClothingType == EClothingType::CT_Boots)
		{
			Mesh3P_Feet->SetSkeletalMesh(Default_Feet_Mesh);
			Mesh1P_Feet->SetSkeletalMesh(Default_Feet_Mesh);
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Torso)
		{
			Mesh3P_Torso->SetSkeletalMesh(Default_Torso_Mesh);
			Mesh1P_Arms_upper->SetSkeletalMesh(NULL);
			Mesh1P_Torso->SetSkeletalMesh(Default_Torso_Mesh);
			Mesh3P_Arms_upper->SetSkeletalMesh(NULL);
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Jacket)
		{
			Mesh3P_Jacket->SetSkeletalMesh(NULL);
			Mesh1P_Jacket->SetSkeletalMesh(NULL);
			if (Clothing_Torso)
			{
				if (Clothing_Torso->ClothingData.Mesh2)
				{
					Mesh1P_Arms_upper->SetSkeletalMesh(Clothing_Torso->ClothingData.Mesh2);
					Mesh3P_Arms_upper->SetSkeletalMesh(Clothing_Torso->ClothingData.Mesh2);
				}
			}
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Pants)
		{
			Mesh3P_Legs->SetSkeletalMesh(Default_Legs_Mesh);
			Mesh1P_Legs->SetSkeletalMesh(Default_Legs_Mesh);
		}
	}
}

void AMyFPSCharacter::RPC_All_EquipCloth_Implementation(AClothingBaseClass * cloth){
	if (cloth)
	{
		if (!cloth->ClothingData.Mesh)
		{
			PrintToScreen("No mesh on cloth");
			return;
		}
		if (cloth->ClothingData.ClothingType == EClothingType::CT_Boots)
		{
			Mesh3P_Feet->SetSkeletalMesh(cloth->ClothingData.Mesh);
			Mesh1P_Feet->SetSkeletalMesh(cloth->ClothingData.Mesh);
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Torso)
		{
			Mesh3P_Torso->SetSkeletalMesh(cloth->ClothingData.Mesh);
			Mesh1P_Torso->SetSkeletalMesh(cloth->ClothingData.Mesh);
			if (cloth->ClothingData.Mesh2) {
				if (!Clothing_Jacket)
				{
					Mesh1P_Arms_upper->SetSkeletalMesh(cloth->ClothingData.Mesh2);
					Mesh3P_Arms_upper->SetSkeletalMesh(cloth->ClothingData.Mesh2);
				}
			}
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Jacket)
		{
			Mesh3P_Jacket->SetSkeletalMesh(cloth->ClothingData.Mesh);
			Mesh1P_Jacket->SetSkeletalMesh(cloth->ClothingData.Mesh);
			if (Clothing_Torso)
			{
				Mesh1P_Arms_upper->SetSkeletalMesh(NULL);
				Mesh3P_Arms_upper->SetSkeletalMesh(NULL);
			}
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Pants)
		{
			Mesh3P_Legs->SetSkeletalMesh(cloth->ClothingData.Mesh);
			Mesh1P_Legs->SetSkeletalMesh(cloth->ClothingData.Mesh);
		}
	}
}

void AMyFPSCharacter::RPC_Server_EquipCloth_Implementation(AClothingBaseClass * cloth) {
	if (cloth)
	{
		if (!cloth->ClothingData.Mesh)
		{
			PrintToScreen("No mesh on cloth");
			return;
		}
		if (cloth->ClothingData.ClothingType == EClothingType::CT_Boots)
		{
			if (Clothing_Feet)
			{
				DropItem(Clothing_Feet->ItemData, FindItemInInventory(Clothing_Feet->ItemData));
			}
			Clothing_Feet = cloth;
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Torso)
		{
			if (Clothing_Torso)
			{
				DropItem(Clothing_Torso->ItemData, FindItemInInventory(Clothing_Torso->ItemData));
			}
			Clothing_Torso = cloth;
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Jacket)
		{
			if (Clothing_Jacket)
			{
				DropItem(Clothing_Jacket->ItemData, FindItemInInventory(Clothing_Jacket->ItemData));
			}
			Clothing_Jacket = cloth;
		}
		else if (cloth->ClothingData.ClothingType == EClothingType::CT_Pants)
		{
			if (Clothing_Legs)
			{
				DropItem(Clothing_Legs->ItemData, FindItemInInventory(Clothing_Legs->ItemData));
			}
			Clothing_Legs = cloth;
		}
		//RPC_All_EquipCloth(cloth);
		RPC_Client_EquipCloth(cloth);
		if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone)
		{
			OnRep_Feet();
			OnRep_Jacket();
			OnRep_Legs();
			OnRep_Torso();
		}
	}
}

bool AMyFPSCharacter::RPC_Server_EquipCloth_Validate(AClothingBaseClass * cloth) {
	return true;
}
void AMyFPSCharacter::EquipCloth(AClothingBaseClass * cloth)
{
	if (HasAuthority()) {
		RPC_Server_EquipCloth(cloth);
	}
	else {
		if (Role == ROLE_AutonomousProxy) {
			RPC_Server_EquipCloth(cloth);
		}
		return;
	}
}

void AMyFPSCharacter::RPC_Client_EquipCloth_Implementation(AClothingBaseClass * cloth)
{
	if (Role == ROLE_AutonomousProxy || GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone)
	{
		OnRep_Feet();
		OnRep_Jacket();
		OnRep_Legs();
		OnRep_Torso();
	}
}

UCameraComponent* AMyFPSCharacter::GetActiveCam()
{
	return ActiveCamera;
}

void AMyFPSCharacter::PlayWeaponFireSound()
{
	if (CurrentWeapon) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(),CurrentWeapon->GetFireSound(), CurrentWeapon->SK_FP_Mesh->GetSocketLocation(TEXT("Muzzle")), GetControlRotation());
	}
}

void AMyFPSCharacter::PlayWeaponReloadSound()
{
	if (CurrentWeapon) {
		/*if (AudioComponent->IsPlaying())
		{
			if (AudioComponent->Sound == CurrentWeapon->GetReloadSound())
			{
				AudioComponent->Stop();
				AudioComponent->
			}
		}*/
		if (AudioComponent->Sound != CurrentWeapon->GetReloadSound())
		{
			AudioComponent->SetSound(CurrentWeapon->GetReloadSound());

		}
		if (AudioComponent->Sound)
		{
			AudioComponent->Play(0.0f);
		}
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon->GetReloadSound(), CurrentWeapon->SK_FP_Mesh->GetSocketLocation(TEXT("Muzzle")), CurrentWeapon->SK_FP_Mesh->GetSocketRotation(TEXT("Muzzle")));
	}
}

void AMyFPSCharacter::WeaponReloadAnim_All_Implementation()
{
	CurrentWeapon->CallReloadAnimation();
}

void AMyFPSCharacter::PlayAnimation_FP_Implementation(UAnimMontage* anim)
{
	//ResetCharState();
	Mesh1P_Legs->GetAnimInstance()->Montage_Play(anim);
}

void AMyFPSCharacter::PlayAnimation_TP_Implementation(UAnimMontage* anim)
{
	Mesh3P_Legs->GetAnimInstance()->Montage_Play(anim);
}

void AMyFPSCharacter::WaitForMontageToEndAndEquip() {
	if (!Mesh1P_Legs->GetAnimInstance()->Montage_IsPlaying(NULL) && !Mesh3P_Legs->GetAnimInstance()->Montage_IsPlaying(NULL))
	{
		if (WeaponInventory[WeaponSlotBuffer]) {
			RPC_EquipWeapon_Client(WeaponInventory[WeaponSlotBuffer]);
			GetWorldTimerManager().ClearTimer(MontageHandler);
			return;
		}
		else {
			PrintToScreen("Null weapon buffer...");
			GetWorldTimerManager().ClearTimer(MontageHandler);
			return;
		}
		
		WeaponBuffer = nullptr;
		WeaponRef = nullptr;
		
	}
}

bool AMyFPSCharacter::SetbIsFPS_Validate(bool b) {
	return true;
}

void AMyFPSCharacter::SetbIsFPS_Implementation(bool b)
{
	bIsFPS = b;

}

bool AMyFPSCharacter::SetbIsADS_Validate(bool b) {
	return true;
}

void AMyFPSCharacter::SetbIsADS_Implementation(bool b)
{
	bIsADS = b;
}

bool AMyFPSCharacter::SetbUseHandIK_Validate(bool b) {
	return true;
}

void AMyFPSCharacter::SetbUseHandIK_Implementation(bool b)
{
	bUseHandIk = b;
}


void AMyFPSCharacter::PlayFootstepSFX()
{
	if (FootstepSFX)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FootstepSFX, Mesh3P_Legs->GetBoneLocation("Foot_L"));
	}
}

void AMyFPSCharacter::WaitForLevelStreaming_Implementation()
{
	GetWorld()->FlushLevelStreaming();
	GEngine->BlockTillLevelStreamingCompleted(GetWorld());
}

void AMyFPSCharacter::ShowTimer_Implementation(bool b)
{
	if (!HUDWidget)
	{
		PrintToScreen("No hudwidget...");
		return;
	}
	HUDWidget->ShowTimer(b);
}

bool AMyFPSCharacter::TryToAddItemToInventory_Validate(FItemData item) {
	return true;
}

void AMyFPSCharacter::TryToAddItemToInventory_Implementation(FItemData item)
{
	AddItemToInventory(item,1);
}

void AMyFPSCharacter::AddPitchRecoil_Implementation(const float value)
{
	AddControllerPitchInput(FMath::RandRange(0.0f,value*-1));
}

void AMyFPSCharacter::AddYawRecoil_Implementation(const float value)
{
	AddControllerYawInput(FMath::RandRange(value*-1, value));
}

void AMyFPSCharacter::EquipWeaponSlot1()
{
	if (WeaponInventory[0]) {
		WeaponSlotBuffer = 0;
		if (CurrentWeapon) {
			HolsterCurrentWeapon();
			WeaponBuffer = WeaponInventory[0];
			GetWorldTimerManager().SetTimer(MontageHandler, this, &AMyFPSCharacter::WaitForMontageToEndAndEquip, 0.3f, true);
			return;
		}
		if (WeaponInventory[0] != CurrentWeapon) {
			EquipWeapon(WeaponInventory[0]);
		}
		else if (WeaponInventory[0] == CurrentWeapon) {
			HolsterCurrentWeapon();
		}
	}
}

void AMyFPSCharacter::EquipWeaponSlot2()
{
	if (WeaponInventory[1]) {
		WeaponSlotBuffer = 1;
		if (CurrentWeapon) {
			HolsterCurrentWeapon();
			WeaponBuffer = WeaponInventory[1];
			GetWorldTimerManager().SetTimer(MontageHandler, this, &AMyFPSCharacter::WaitForMontageToEndAndEquip, 0.3f, true);
			return;
		}
		if (WeaponInventory[1] != CurrentWeapon) {
			EquipWeapon(WeaponInventory[1]);
		}
		else if (WeaponInventory[1] == CurrentWeapon) {
			HolsterCurrentWeapon();
		}
	}
}

void AMyFPSCharacter::EquipWeaponSlot3()
{
	if (WeaponInventory[2]) {
		WeaponSlotBuffer = 2;
		if (CurrentWeapon) {
			HolsterCurrentWeapon();
			WeaponBuffer = WeaponInventory[2];
			GetWorldTimerManager().SetTimer(MontageHandler, this, &AMyFPSCharacter::WaitForMontageToEndAndEquip, 0.3f, true);
			return;
		}
		if (WeaponInventory[2] != CurrentWeapon) {
			EquipWeapon(WeaponInventory[2]);
		}
		else if (WeaponInventory[2] == CurrentWeapon) {
			HolsterCurrentWeapon();
		}
	}
}

void AMyFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyFPSCharacter::TrytoJump);   //&ACharacter::Jump
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMyFPSCharacter::RPC_Client_Crouch);   
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyFPSCharacter::TrytoStopJump);   //&ACharacter::StopJumping
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMyFPSCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMyFPSCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &AMyFPSCharacter::Walk);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &AMyFPSCharacter::StopWalking);


	PlayerInputComponent->BindAction("Action", IE_Released, this, &AMyFPSCharacter::Action);

	PlayerInputComponent->BindAction("ChangeCamera", IE_Released, this, &AMyFPSCharacter::ChangeCurrentCamera);

	PlayerInputComponent->BindAction("EquipWeaponSlot1", IE_Released, this, &AMyFPSCharacter::EquipWeaponSlot1);
	PlayerInputComponent->BindAction("EquipWeaponSlot2", IE_Released, this, &AMyFPSCharacter::EquipWeaponSlot2);
	PlayerInputComponent->BindAction("EquipWeaponSlot3", IE_Released, this, &AMyFPSCharacter::EquipWeaponSlot3);

	PlayerInputComponent->BindAction("HolsterCurrentWeapon", IE_Released, this, &AMyFPSCharacter::HolsterCurrentWeapon);

	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &AMyFPSCharacter::ShowHideInventory);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyFPSCharacter::OnFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AMyFPSCharacter::StopFiring);

	//bind reload
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMyFPSCharacter::OnReload);

	// Bind ADS event
	PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &AMyFPSCharacter::AimDownSights);
	//PlayerInputComponent->BindAction("ADS", IE_Released, this, &AMyFPSCharacter::StopADS);

	//bind change firemode event
	PlayerInputComponent->BindAction("ChangeFireMode", IE_Released, this, &AMyFPSCharacter::ChangeFireMode);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyFPSCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AMyFPSCharacter::HandleMouseYaw);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyFPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyFPSCharacter::HandleMousePitch);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyFPSCharacter::LookUpAtRate);
}

bool AMyFPSCharacter::ReloadWeapon_Validate() {
	return true;
}

void AMyFPSCharacter::ReloadWeapon_Implementation() {
	if (CurrentWeapon)
	{
		if (!bIsJumping)
		{
			int32 totalAmmo = GetCurrentAmmoByType(CurrentWeapon->WeaponData.AmmoType);
			if (GetCurrentAmmoByType(CurrentWeapon->WeaponData.AmmoType) > 0)
			{
				int32 n = CurrentWeapon->NeedsAmmoAmount();
				if (n <= 0)
				{
					return;
				}
				PrintToScreen(FString::FromInt(n));
				if (totalAmmo >= n)
				{
					CurrentWeapon->ReloadAmount(n);
					DecreaseAmmoByType(CurrentWeapon->WeaponData.AmmoType, n);
				}
				else {
					CurrentWeapon->ReloadAmount(totalAmmo);
					DecreaseAmmoByType(CurrentWeapon->WeaponData.AmmoType, totalAmmo);
					bIsReloading = false;
				}
			}
			else {
				PrintToScreen("No ammo of this type");
			}
			bIsReloading = false;
		}
	}
}

void AMyFPSCharacter::OnReload()
{
	if (!bIsJumping)
	{
		if (!bIsADS)
		{
			if (!bIsReloading)
			{
				if (CurrentWeapon)
				{
					if (!Mesh1P_Legs->GetAnimInstance()->Montage_IsPlaying(NULL))
					{
						if (CurrentWeapon->NeedsAmmoAmount() == 0) {
							PrintToScreen("Mag Full");
							return;
						}

						if (GetCurrentAmmoByType(CurrentWeapon->WeaponData.AmmoType) > 0) {
							PlayReloadAnimationOfType(CurrentWeapon->WeaponData.WeaponType);
							PrintToScreen("OnReload function");
						}
					}
				}
			}
			else {
				PrintToScreen("Already reloading...");
			}
		}
	}
}

void AMyFPSCharacter::HandleMousePitch(float pitch) {
	if (!bIsInInventory) {
		AddControllerPitchInput(pitch);
	}
}

bool AMyFPSCharacter::SetPitch_Validate(float fPitch) {
	return true;
}

void AMyFPSCharacter::SetPitch_Implementation(float Pitch)
{
	fPitch = Pitch;
}

void AMyFPSCharacter::HandleMouseYaw(float yaw) {
	if (!bIsInInventory) {
		AddControllerYawInput(yaw);
	}
}

void AMyFPSCharacter::OnRep_WeaponRef()
{
	if (Role == ROLE_AutonomousProxy || GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone)
	{
		//PrintToScreen("weapon ref updated");
		HolsterWeapon_server(WeaponRef);
		if (!CurrentWeapon)
		{
			//PrintToScreen("Client234");
			RPC_EquipWeapon_Client(WeaponRef);
		}
		else {
			PrintToScreen("CurrentWeapon not null");
		}
		ChangeWeaponsVisibility(bIsFPS);
	}
}

void AMyFPSCharacter::OnRep_Torso()
{
	PrintToScreen("Torso replication called!");
		if (Clothing_Torso)
		{
			if (!Clothing_Torso->ClothingData.Mesh)
			{
				PrintToScreen("No mesh on cloth_torso");
				return;
			}
			if (Clothing_Torso->ClothingData.ClothingType == EClothingType::CT_Torso)
			{
				Mesh3P_Torso->SetSkeletalMesh(Clothing_Torso->ClothingData.Mesh);
				Mesh1P_Torso->SetSkeletalMesh(Clothing_Torso->ClothingData.Mesh);
				if (Clothing_Torso->ClothingData.Mesh2) {
					if (!Clothing_Jacket)
					{
						Mesh1P_Arms_upper->SetSkeletalMesh(Clothing_Torso->ClothingData.Mesh2);
						Mesh3P_Arms_upper->SetSkeletalMesh(Clothing_Torso->ClothingData.Mesh2);
					}
				}
			}
		}
		else {
			Mesh3P_Torso->SetSkeletalMesh(Default_Torso_Mesh);
			Mesh1P_Arms_upper->SetSkeletalMesh(NULL);
			Mesh3P_Arms_upper->SetSkeletalMesh(NULL);
			Mesh1P_Torso->SetSkeletalMesh(Default_Torso_Mesh);
		}
}

void AMyFPSCharacter::OnRep_Feet()
{
	/*if (Role == ROLE_AutonomousProxy || GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone)
	{
	}*/
	if (Clothing_Feet)
	{
		if (!Clothing_Feet->ClothingData.Mesh)
		{
			PrintToScreen("No mesh on cloth_feet");
			return;
		}
		if (Clothing_Feet->ClothingData.ClothingType == EClothingType::CT_Boots)
		{
			Mesh3P_Feet->SetSkeletalMesh(Clothing_Feet->ClothingData.Mesh);
			Mesh1P_Feet->SetSkeletalMesh(Clothing_Feet->ClothingData.Mesh);
		}
	}
	else {
		Mesh3P_Feet->SetSkeletalMesh(Default_Feet_Mesh);
		Mesh1P_Feet->SetSkeletalMesh(Default_Feet_Mesh);
	}
}

void AMyFPSCharacter::OnRep_Legs()
{
	if (Clothing_Legs)
	{
		if (!Clothing_Legs->ClothingData.Mesh)
		{
			PrintToScreen("No mesh on cloth_legs");
			return;
		}
		
		if (Clothing_Legs->ClothingData.ClothingType == EClothingType::CT_Pants)
		{
			Mesh3P_Legs->SetSkeletalMesh(Clothing_Legs->ClothingData.Mesh);
			Mesh1P_Legs->SetSkeletalMesh(Clothing_Legs->ClothingData.Mesh);
		}
	}
	else {
		Mesh3P_Legs->SetSkeletalMesh(Default_Legs_Mesh);
		Mesh1P_Legs->SetSkeletalMesh(Default_Legs_Mesh);
	}
}

void AMyFPSCharacter::OnRep_Jacket()
{
	if (Clothing_Jacket)
	{
		if (!Clothing_Jacket->ClothingData.Mesh)
		{
			PrintToScreen("No mesh on cloth_torso");
			return;
		}
		if (Clothing_Jacket->ClothingData.ClothingType == EClothingType::CT_Jacket)
		{
			Mesh3P_Jacket->SetSkeletalMesh(Clothing_Jacket->ClothingData.Mesh);
			Mesh1P_Jacket->SetSkeletalMesh(Clothing_Jacket->ClothingData.Mesh);
			if (Clothing_Torso)
			{
				Mesh1P_Arms_upper->SetSkeletalMesh(NULL);
				Mesh3P_Arms_upper->SetSkeletalMesh(NULL);
			}
		}
	}
	else {
		Mesh3P_Jacket->SetSkeletalMesh(NULL);
		Mesh1P_Jacket->SetSkeletalMesh(NULL);
		if (Clothing_Torso)
		{
			if (Clothing_Torso->ClothingData.Mesh2) {
				Mesh1P_Arms_upper->SetSkeletalMesh(Clothing_Torso->ClothingData.Mesh2);
				Mesh3P_Arms_upper->SetSkeletalMesh(Clothing_Torso->ClothingData.Mesh2);
			}
		}
	}
}

void AMyFPSCharacter::OnRep_Crouch()
{
	if (Role == ROLE_AutonomousProxy || GetNetMode() == ENetMode::NM_ListenServer)
	{
		//PrintToScreen("crouch b updated");
		if (bIsCrouching)
		{
			//PrintToScreen("Client");
			//CrouchOffsetDown();
		}
		else {
			//PrintToScreen("not crouching");
			//CrouchOffsetUp();
		}
	}
}

void AMyFPSCharacter::CrouchOffsetUp() {
	this->UnCrouch();
}

void AMyFPSCharacter::CrouchOffsetDown() {
	this->Crouch();
}

bool AMyFPSCharacter::CompareItems(FItemData a, FItemData b)
{
	return a.ItemName == b.ItemName && a.Description == b.Description && a.ItemID == b.ItemID && a.ItemType == b.ItemType && a.PickupClass == b.PickupClass;
}

int32 AMyFPSCharacter::FindItemInInventory(FItemData item)
{
	int32 index = 0;
	for (int32 i=0;i<Inventory.Num();i++)
	{
		if (CompareItems(item,Inventory[i]))
		{
			return i;
		}
	}

	return -1;
}

void AMyFPSCharacter::TimelineCallback(float val)
{
	
	if (bTimeline_isReverse)
	
	{
		TransitionCamera->SetWorldLocation(FMath::Lerp(TransitionCamera->GetComponentLocation(), FirstPersonCameraComponent->GetComponentLocation(), val/6));
		TransitionCamera->SetFieldOfView(FMath::Lerp(TransitionCamera->FieldOfView, FirstPersonCameraComponent->FieldOfView, val));
		if (!CurrentWeapon->HasSight())
		{
			CurrentWeapon->SK_FP_Mesh->SetRelativeRotation(FMath::Lerp(FRotator(CurrentWeapon->SK_FP_Mesh->RelativeRotation.Pitch, CurrentWeapon->SK_FP_Mesh->RelativeRotation.Yaw, CurrentWeapon->SK_FP_Mesh->RelativeRotation.Roll), WeaponOriginalT.Rotator(), val));
		}
		
	}
	else {
		
		TransitionCamera->SetWorldLocation(FMath::Lerp(TransitionCamera->GetComponentLocation(), ADSCameraComponent->GetComponentLocation(), val/2));
		TransitionCamera->SetFieldOfView(FMath::Lerp(TransitionCamera->FieldOfView, CurrentWeapon->GetFieldOfView(), val));

		if (val >= CamTimeline->GetTimelineLength() -0.2 )
		{
			if (!_aim)
			{
				PrintToScreen(FString::SanitizeFloat(val));
				SetWeaponAimingTrue();
				_aim = true;
			}
		}

		if (!CurrentWeapon->HasSight())
		{
			CurrentWeapon->SK_FP_Mesh->SetRelativeRotation(FMath::Lerp(FRotator(CurrentWeapon->SK_FP_Mesh->RelativeRotation.Pitch, CurrentWeapon->SK_FP_Mesh->RelativeRotation.Yaw, CurrentWeapon->SK_FP_Mesh->RelativeRotation.Roll), RotTarget, val));
			//PrintToScreen("Doesnt have sight!");
		}
	}
	
	//PrintToScreen("Interpolating shiiiiiiiit;");
}

void AMyFPSCharacter::TimelineFinishedCallback()
{
	if (bTimeline_isReverse)
	{
		_aim = false;
		TransitionCamera->Deactivate();
		ADSCameraComponent->Deactivate();
		ThirdPersonCameraComponent->Deactivate();
		FirstPersonCameraComponent->Activate();
		SetActiveCam(true);	//ActiveCamera = FirstPersonCameraComponent;
		SetbIsFPS(true);
		SetbIsADS(false);	//bIsADS = false;
		CanChangeCamera = true;
		//PrintToScreen("Timeline Reverse Done");
		
	}
	else {
		_aim = false;
		TransitionCamera->Deactivate();
		//TransitionCamera->SetWorldLocation(ADSCamera->GetComponentLocation());
		//CurrentWeapon->SetAimingTrue();
		//SetWeaponAimingTrue();
		ADSCameraComponent->SetFieldOfView(CurrentWeapon->GetFieldOfView());
		ADSCameraComponent->Activate();
		ADSCamera->bUsePawnControlRotation = true;
		PrintToScreen(ADSCamera->RelativeRotation.ToString());
		SetActiveCamComp(ADSCameraComponent);	//ActiveCamera = ADSCamera;
		//CurrentWeapon->SK_FP_Mesh->SetRelativeRotation(FRotator(CurrentWeapon->SK_FP_Mesh->RelativeRotation.Pitch, CurrentWeapon->SK_FP_Mesh->RelativeRotation.Yaw - CurrentWeapon->IronSightOffsetYaw, CurrentWeapon->SK_FP_Mesh->RelativeRotation.Roll - CurrentWeapon->IronSightOffsetRoll));  /////offset weapon so it can align sights
		//PrintToScreen("Timeline Done");
	}
}


void AMyFPSCharacter::PlayTimeline()
{
	if (CamTimeline != NULL)
	{
		CamTimeline->PlayFromStart();
	}
}

void AMyFPSCharacter::AddItemToInventory(FItemData itemData,int32 amount)
{
	int32 i;
	if (itemData.isStackable)
	{
		if (amount > 1)
		{
			PrintToScreen(FString::FromInt(amount));
			itemData.Amount = itemData.Amount + amount;
		}
	}
	i=Inventory.Add(itemData);
	Inventory[i].CurrentIndex = i;
}

int32 AMyFPSCharacter::GetCurrentAmmoByType(EAmmoType type)
{
	for (int32 i =0;i<Inventory.Num();i++)
	{
		if (Inventory[i].ItemType == EItemType::IT_Ammo)
		{
			if (Inventory[i].AmmoType == type)
			{
				return Inventory[i].Amount;
			}
		}
	}
	return -1;
}

void AMyFPSCharacter::DecreaseAmmoByType(EAmmoType type,int32 amount)
{
	for (int i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i].ItemType == EItemType::IT_Ammo)
		{
			if (Inventory[i].AmmoType == type)
			{
				if (Inventory[i].Amount > 0)
				{
					Inventory[i].Amount = Inventory[i].Amount - amount;
					if (Inventory[i].Amount <= 0)
					{
						Inventory[i].Amount = 0;
						RemoveItemFromInventoryAtIndex(i);
					}
				}
			}
		}
	}
}

bool AMyFPSCharacter::PlayReloadAnimationOfType_Validate(EWeaponType type) {
	return true;
}

void AMyFPSCharacter::PlayReloadAnimationOfType_Implementation(EWeaponType type)
{
	/* GETS RELOAD ANIMATION BASED ON A BOOL, TRUE IS FOR FIRST PERSON ANIM, FALSE FOR 3RD */
	PlayAnimation_FP(CurrentWeapon->GetCharReloadAnimation(true));
	PlayAnimation_TP(CurrentWeapon->GetCharReloadAnimation(false));
	WeaponReloadAnim_All();
	bIsReloading = true;
}

int32 AMyFPSCharacter::AddWeaponToItemInventory(FItemData itemData)
{
	return Inventory.Add(itemData);
}

void AMyFPSCharacter::UnequipArmor(FArmorData armorData)
{
	if (armorData.ArmorPart == EArmorPart::AP_Head && Armor_Head_Data != NULL)
	{
		Armor_Head_Data->Destroy();
		Armor_Head_Data = nullptr;
	}
	if (armorData.ArmorPart == EArmorPart::AP_Torso && Armor_Torso_Data != NULL)
	{
		Armor_Torso_Data->Destroy();
		Armor_Torso_Data = nullptr;
		TP_BodyArmor->SetSkeletalMesh(NULL);
		FP_BodyArmor->SetSkeletalMesh(NULL);
		if (HUDWidget)
		{
			HUDWidget->SetArmorIconTorsoVisibility(false);
		}
	}
	else if (Armor_Torso_Data == NULL)
	{
		PrintToScreen("Armor torso data null!");
	}
}

bool AMyFPSCharacter::RemoveItemFromInventoryAtIndex_Validate(int index) {
	return true;
}

void AMyFPSCharacter::RemoveItemFromInventoryAtIndex_Implementation(int index)
{
	Inventory.RemoveAt(index);
}

void AMyFPSCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AMyFPSCharacter, fPitch);
	DOREPLIFETIME(AMyFPSCharacter, bIsJumping);
	DOREPLIFETIME(AMyFPSCharacter, bIsCrouching);
	DOREPLIFETIME(AMyFPSCharacter, bIsReloading);
	DOREPLIFETIME(AMyFPSCharacter, bPistolEquipped);
	DOREPLIFETIME(AMyFPSCharacter, bRifleEquipped);
	DOREPLIFETIME(AMyFPSCharacter, bCanFire);
	DOREPLIFETIME(AMyFPSCharacter, bIsUnarmed);
	DOREPLIFETIME(AMyFPSCharacter, Inventory);
	DOREPLIFETIME(AMyFPSCharacter, WeaponInventory);
	DOREPLIFETIME(AMyFPSCharacter, CurrentWeapon);
	DOREPLIFETIME(AMyFPSCharacter, bUseHandIk);
	DOREPLIFETIME(AMyFPSCharacter, WeaponBuffer);
	DOREPLIFETIME(AMyFPSCharacter, WeaponRef);
	DOREPLIFETIME(AMyFPSCharacter, ActiveCamera);
	DOREPLIFETIME(AMyFPSCharacter, bIsFPS);
	DOREPLIFETIME(AMyFPSCharacter, bIsADS);
	DOREPLIFETIME(AMyFPSCharacter, bIsDead);
	DOREPLIFETIME(AMyFPSCharacter, fHealth);
	DOREPLIFETIME(AMyFPSCharacter, bFPPOnly);
	DOREPLIFETIME(AMyFPSCharacter, Clothing_Torso);
	DOREPLIFETIME(AMyFPSCharacter, Clothing_Feet);
	DOREPLIFETIME(AMyFPSCharacter, Clothing_Legs);
	DOREPLIFETIME(AMyFPSCharacter, Clothing_Jacket);
}

/*Crouch stuff*/

void AMyFPSCharacter::RPC_Client_Crouch_Implementation() {

		if (!bIsJumping) {
				if (bIsCrouching)
				{
					ThirdPersonCameraComponent->SetRelativeLocation(CamStartPos);
					//Mesh1P_Legs->SetRelativeLocation(Mesh3pStartPos);
				}
				else {
					ThirdPersonCameraComponent->SetRelativeLocation(FVector(CamStartPos.X, CamStartPos.Y, CamStartPos.Z - 45.f));
					//Mesh1P_Legs->SetRelativeLocation(FVector(Mesh3pStartPos.X, Mesh3pStartPos.Y, Mesh3pStartPos.Z + 30.f));
				} 
				RPC_Server_Crouch();
		}
}

void AMyFPSCharacter::RPC_Server_Crouch_Implementation() {
	if (bIsCrouching) {
		//GetCapsuleComponent()->SetCapsuleHalfHeight(CapsuleDefaultSize);
		bIsCrouching = false;
		ThirdPersonCameraComponent->SetRelativeLocation(CamStartPos);
		Mesh1P_Legs->SetRelativeLocation(Mesh3pStartPos);
		
	}
	else {
		//GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 33.f);
		bIsCrouching = true;
		ThirdPersonCameraComponent->SetRelativeLocation(FVector(CamStartPos.X, CamStartPos.Y, CamStartPos.Z - 45.f));
		Mesh1P_Legs->SetRelativeLocation(FVector(Mesh3pStartPos.X, Mesh3pStartPos.Y, Mesh3pStartPos.Z + 30.f));
	}
	//RPC_All_Crouch();
}

bool AMyFPSCharacter::RPC_Server_Crouch_Validate() {
	return true;
}

/*End of crouch stuff*/

/*Jump stuff*/

void AMyFPSCharacter::ResetJump() {
	b_CanJump = true;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "ResetJump");
}

bool AMyFPSCharacter::ResetCharState_Validate()
{
	return true;
}

void AMyFPSCharacter::ResetCharState_Implementation()
{
	bIsReloading = false;
	bCanFire = true;
	b_CanJump = true;
	if (CurrentWeapon)
	{
		CurrentWeapon->InterruptAllAnimation();
	}
}

void AMyFPSCharacter::TrytoJump() {
	FTimerHandle unusedHandle;
	if (bIsADS)
	{
		StopADS();
	}
	if(!bIsCrouching){
		if (b_CanJump) {
			if (!GetCharacterMovement()->IsFalling()) {
				if (!bIsJumping) {
					if (GetNetMode() == ENetMode::NM_Standalone)
					{
						RPC_Server_TrytoJump();
						b_CanJump = false;
						GetWorldTimerManager().SetTimer(unusedHandle, this, &AMyFPSCharacter::ResetJump, 1.0f, false);
					}
					else {
						if (HasAuthority()) {
							RPC_All_TrytoJump();
							GetWorldTimerManager().SetTimer(unusedHandle, this, &AMyFPSCharacter::RPC_All_TrytoStopJump, 0.5f, false);
						}
						else {
							if (Role == ROLE_AutonomousProxy) {
								if (!bIsCrouching) {
									RPC_Server_TrytoJump();
									b_CanJump = false;
									
									GetWorldTimerManager().SetTimer(unusedHandle, this, &AMyFPSCharacter::ResetJump, 1.0f, false);
								}
							}
						}
					}
					this->Jump();
					bIsJumping = true;
				}
			}
		}
	}
	//isJumping = true;
}

void AMyFPSCharacter::RPC_Server_TrytoJump_Implementation() {
	//&Super::Jump;
	if (!GetCharacterMovement()->IsFalling()) {
		if (!bIsJumping) {
			RPC_All_TrytoJump();
			FTimerHandle unusedHandle;
			GetWorldTimerManager().SetTimer(unusedHandle, this, &AMyFPSCharacter::RPC_All_TrytoStopJump, 0.5f, false);
		}
	}
}

bool AMyFPSCharacter::RPC_Server_TrytoJump_Validate() {
	return true;
}

void AMyFPSCharacter::RPC_All_TrytoJump_Implementation() {
	bIsJumping = true;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "TryJumping: " + isJumping ? "True" : "False");
}

void AMyFPSCharacter::TrytoStopJump() {
	return;
	if (HasAuthority()) {
		RPC_All_TrytoStopJump();
	}
	else {
		if (Role == ROLE_AutonomousProxy) {
			RPC_Server_TrytoStopJump();
		}
		return;
	}
	bIsJumping = false;
}

void AMyFPSCharacter::RPC_Server_TrytoStopJump_Implementation() {
	//&Super::StopJumping;

	AMyFPSCharacter::RPC_All_TrytoStopJump();
}

bool AMyFPSCharacter::RPC_Server_TrytoStopJump_Validate() {
	return true;
}

void AMyFPSCharacter::RPC_All_TrytoStopJump_Implementation() {
	bIsJumping = false;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,"StopJumping: " + isJumping ? "True" : "False"); 
}

/*	End of jump stuff	*/

/*	Sprint stuff	*/

void AMyFPSCharacter::Sprint() {

	if (bIsADS)
	{
		StopADS();
	}

	if (HasAuthority()) {
		RPC_All_Sprint();
	}
	else {
		if (Role == ROLE_AutonomousProxy) {
			RPC_Server_Sprint();
		}
		return;
	}
	this->StopJumping();
}

bool AMyFPSCharacter::RPC_Server_Sprint_Validate() {
	return true;
}

void AMyFPSCharacter::RPC_Server_Sprint_Implementation() {
	RPC_All_Sprint();
}

void AMyFPSCharacter::RPC_All_Sprint_Implementation() {
	UCharacterMovementComponent* cm = GetCharacterMovement();
	if (!bIsCrouching) {
		cm->MaxWalkSpeed = SprintSpeed;
	}
	else {
		cm->MaxWalkSpeed = WalkSpeed;
	}
}

void AMyFPSCharacter::StopSprinting() {
	if (HasAuthority()) {
		RPC_All_StopSprinting();
	}
	else {
		if (Role == ROLE_AutonomousProxy) {
			RPC_Server_StopSprinting();
		}
		return;
	}
}

void AMyFPSCharacter::OnRep_Health()
{
	PrintToScreen("Health replicated! Current health is: " + FString::SanitizeFloat(fHealth));
}

void AMyFPSCharacter::Death_All_Implementation()
{
	Mesh3P_Legs->SetAnimInstanceClass(NULL);
	Mesh3P_Legs->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	Mesh3P_Legs->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Mesh3P_Legs->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
}

void AMyFPSCharacter::Death_Client_Implementation()
{
	if (!IsLocallyControlled())
	{
		return;
	}
	//bIsDead = true;
	
	if (bIsFPS)
	{
		ChangeCurrentCamera();
	}
	/*Mesh1P_Legs->SetAnimInstanceClass(NULL);
	Mesh1P_Legs->SetSimulatePhysics(true);
	Mesh1P_Legs->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	Mesh1P_Legs->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);*/
	//APlayerController * c = Cast<APlayerController>(Controller);
	/*if (c)
	{
		c->GetPawn()->DisableInput(c);
		c->PlayerState->bOnlySpectator = true;
		c->ChangeState(NAME_Spectating);
		
	}*/
	if (HUDWidget)
	{
		HUDWidget->RemoveFromViewport();
		HUDWidget->RemoveFromRoot();
	}
}

bool AMyFPSCharacter::Death_Validate(APlayerController* pc) {
	return true;
}

void AMyFPSCharacter::Death_Implementation(APlayerController* pc) {
	//this->Destroy();
	Death_Client();
	DropWeapon(0, -1);
	DropWeapon(1, -1);
	DropWeapon(2, -1);

	AMyFPSGameMode* GM;
	GM = Cast<AMyFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		PrintToScreen(pc->GetName());
		GM->PlayerDied(LastDMGInstigator, pc);
	}
	bIsDead = true;
	DeathRep();
	//Death_All();
	this->Destroy();

}

bool AMyFPSCharacter::AddHealth_Validate(float amount) {
	return true;
}

void AMyFPSCharacter::AddHealth_Implementation(float amount)
{
	fHealth += amount;
}

bool AMyFPSCharacter::DecreaseHealth_Validate(float amount) {
	return true;
}

void AMyFPSCharacter::DecreaseHealth_Implementation(float amount)
{
	if (fHealth > 0)
	{
		fHealth -= amount;
	}
	else {
		PrintToScreen("IM DEAD BITCH");
	}
}


bool AMyFPSCharacter::RPC_Server_StopSprinting_Validate() {
	return true;
}

void AMyFPSCharacter::RPC_Server_StopSprinting_Implementation() {
	RPC_All_StopSprinting();
}

void AMyFPSCharacter::RPC_All_StopSprinting_Implementation() {
	UCharacterMovementComponent* cm = GetCharacterMovement();
	if (!bIsCrouching) {
		cm->MaxWalkSpeed = RunSpeed;
	}
	else {
		cm->MaxWalkSpeed = CrouchSpeed;
	}
}

/*	End of sprint stuff	*/

/* Walk */
void AMyFPSCharacter::Walk() {
	UCharacterMovementComponent* cm = GetCharacterMovement();
	if (!bIsCrouching) {
		cm->MaxWalkSpeed = WalkSpeed;
	}
	else {
		cm->MaxWalkSpeed = WalkSpeed / 2;
	}
}

void AMyFPSCharacter::StopWalking() {
	UCharacterMovementComponent* cm = GetCharacterMovement();
	if (!bIsCrouching) {
		cm->MaxWalkSpeed = RunSpeed;
	}
	else {
		cm->MaxWalkSpeed = CrouchSpeed;
	}
}

/*		*/
/*	Fire stuff	*/

bool AMyFPSCharacter::ServerFire_Validate(FVector origin) {
	return true;
}

void AMyFPSCharacter::ServerFire_Implementation(FVector origin) {
	GLog->Log(ActiveCamera->GetName());
	GLog->Log(ActiveCamera->GetComponentLocation().ToString());
	if (!bIsInInventory)
	{
		if (!bIsReloading)
		{
			if (CurrentWeapon) {
				if (bCanFire)
				{
					if (CurrentWeapon->CheckIfItCanFire())
					{
						PrintToScreen(ActiveCamera->GetName());
						CurrentWeapon->Fire(origin);
						FireAll();
					}
					else
					{
						PrintToScreen("Weapon cant fire");
					}
				}
				else {
					PrintToScreen("Char cant fire");
				}
			}
		}
	}
}

void AMyFPSCharacter::FireAll_Implementation() {

}


float AMyFPSCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	fHealth -= DamageAmount;
	LastDMGInstigator = EventInstigator;
	return DamageAmount;
}

void AMyFPSCharacter::OnFire()
{
	PrintToScreen(ActiveCamera->GetComponentLocation().ToString());
	GLog->Log(ActiveCamera->GetComponentLocation().ToString());
	ServerFire(ActiveCamera->GetComponentLocation());
}

void AMyFPSCharacter::StopFiring()
{
	StopFiring_Server();
}

bool AMyFPSCharacter::StopFiring_Server_Validate() {
	return true;
}

void AMyFPSCharacter::StopFiring_Server_Implementation()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
}

void AMyFPSCharacter::ChangeFireMode()
{
	ChangeFireMode_Server();
}
bool AMyFPSCharacter::ChangeFireMode_Server_Validate() {
	return true;
}

void AMyFPSCharacter::ChangeFireMode_Server_Implementation()
{
	if (CurrentWeapon)
	{
		if (CurrentWeapon->WeaponData.CanChangeFireMode)
		{
			CurrentWeapon->ChangeFireMode();
		}
	}
}

void AMyFPSCharacter::AimDownSights()
{
	if (bIsADS)
	{
		StopADS();
		return;
	}
	GLog->Log(ActiveCamera->GetName());
	if (!bIsInInventory)
	{
		//AimDownSights_Server();
		UCharacterMovementComponent* cm = GetCharacterMovement();
		
		if (cm->MaxWalkSpeed == SprintSpeed)
		{
			StopSprinting();
		}

		if (!bIsJumping && !bIsReloading)
		{
			if (CurrentWeapon && CurrentWeapon->CheckIfItCanFire())
			{
				if (!bIsFPS)
				{
					ChangeCurrentCamera();
				}
				//RefreshADSCamPos();
				SetbIsADS(true);	 //bIsADS = true;
				CanChangeCamera = false;
				if (TransitionCamera->GetComponentLocation() != FirstPersonCameraComponent->GetComponentLocation())
				{
					TransitionCamera->SetWorldLocation(FirstPersonCameraComponent->GetComponentLocation());
				}
				FirstPersonCameraComponent->Deactivate();
				ThirdPersonCameraComponent->Deactivate();
				TransitionCamera->Activate();
				SetActiveCamComp(TransitionCamera);	//ActiveCamera = TransitionCamera;
				TransitionCamera->bUsePawnControlRotation = true;
				bTimeline_isReverse = false;

				RotTarget = FRotator(WeaponOriginalT.Rotator().Pitch, WeaponOriginalT.Rotator().Yaw - CurrentWeapon->IronSightOffsetYaw, WeaponOriginalT.Rotator().Roll - CurrentWeapon->IronSightOffsetRoll);

				//FVector LocalPosition = ADSCamera->GetComponentLocation() - FirstPersonCameraComponent->GetComponentLocation();
					
				/*TransitionCamStartLoc = FirstPersonCameraComponent->GetComponentLocation();
				TransitionCamTargetLoc = ADSCamera->GetComponentLocation();*/
				CamTimeline->PlayFromStart();
				//SetWeaponAimingTrue();
				/*ADSCamera->Activate();
				ADSCamera->SetFieldOfView(60.f);
				ADSCamera->bUsePawnControlRotation = true;
				ActiveCamera = ADSCamera;*/
				//CurrentWeapon->SK_FP_Mesh->SetWorldRotation(FRotator(CurrentWeapon->SK_FP_Mesh->GetComponentRotation().Pitch, CurrentWeapon->SK_FP_Mesh->GetComponentRotation().Yaw - CurrentWeapon->IronSightOffsetYaw, CurrentWeapon->SK_FP_Mesh->GetComponentRotation().Roll - CurrentWeapon->IronSightOffsetRoll));  /////offset weapon so it can align sights

				//CurrentWeapon->SK_FP_Mesh->SetRelativeRotation(FRotator(CurrentWeapon->SK_FP_Mesh->RelativeRotation.Pitch, CurrentWeapon->SK_FP_Mesh->RelativeRotation.Yaw - CurrentWeapon->IronSightOffsetYaw, CurrentWeapon->SK_FP_Mesh->RelativeRotation.Roll - CurrentWeapon->IronSightOffsetRoll));  /////offset weapon so it can align sights

				//AMyFPSHUD* hud = Cast<AMyFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
				//hud->bShowHUD = false;
				//hud->showcross
				HUDWidget->ShowCrosshair(false);
			}
		}
	}
}

bool AMyFPSCharacter::AimDownSights_Server_Validate()
{
	return true;
}

void AMyFPSCharacter::AimDownSights_Server_Implementation()
{
	AimDownSights();
}

void AMyFPSCharacter::StopADS()
{
	//StopADS_Server();
	if (bIsADS)
	{
		if (TransitionCamera->GetComponentLocation() != ADSCameraComponent->GetComponentLocation())
		{
			//TransitionCamera->SetWorldLocation(ADSCamera->GetComponentLocation());
			GLog->Log("Transition cam not equal to adscam");
		}
		SetWeaponAimingFalse();
		ADSCameraComponent->Deactivate();
		CanChangeCamera = false;
		SetbIsADS(false);	//bIsADS = false;
		bTimeline_isReverse = true;
		CamTimeline->Reverse();
		if (bIsFPS)
		{
			TransitionCamera->Activate();
		}
		SetActiveCam(true);
		/*FirstPersonCameraComponent->Activate();
		ActiveCamera = FirstPersonCameraComponent;*/
		//CurrentWeapon->SK_FP_Mesh->SetRelativeRotation(FRotator(CurrentWeapon->SK_FP_Mesh->RelativeRotation.Pitch, CurrentWeapon->SK_FP_Mesh->RelativeRotation.Yaw + CurrentWeapon->IronSightOffsetYaw, CurrentWeapon->SK_FP_Mesh->RelativeRotation.Roll + CurrentWeapon->IronSightOffsetRoll));
		//AMyFPSHUD* hud = Cast<AMyFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
		//hud->bShowHUD = true;
		HUDWidget->ShowCrosshair(true);
	}
}

bool AMyFPSCharacter::StopADS_Server_Validate() {
	return true;
}

void AMyFPSCharacter::StopADS_Server_Implementation()
{
	StopADS();
}

void AMyFPSCharacter::Action()										/*Action logic*/
{
	//PrintToScreen(Role);
	RPC_Server_Action(ActiveCamera->GetComponentLocation());
	/*
	switch (Role)
	{
	case ROLE_AutonomousProxy: {
		PrintToScreen("AutonomousProxy");
		break; }
	case ROLE_SimulatedProxy: {
		PrintToScreen("SimulatedProxy");
		break; }

	case ROLE_Authority: {
		PrintToScreen("Authority");
		break; }
	}


	if (HasAuthority()) {
		//RPC_Server_Action(ActiveCamera->GetComponentLocation());
	}
	else {
		if (Role == ROLE_AutonomousProxy) {
			
		}
		return;
}	*/
}

void AMyFPSCharacter::OnRep_WeaponInvChanged()
{
	//PrintToScreen("happenin");
	ChangeWeaponsVisibility(bIsFPS);
}

bool AMyFPSCharacter::RPC_Server_Action_Validate(FVector origin) {
	return true;
}

void AMyFPSCharacter::RPC_Server_Action_Implementation(FVector origin)
{
	float distance;

	if (bIsFPS) {
		distance = traceDistance;
	}
	else {
		distance = 420.f;
	}

	FHitResult hit = TraceFromOrigin(origin, distance);
	DrawDebugLine(GetWorld(), hit.TraceStart, hit.TraceEnd, FColor::Blue, false, 5.0f, 0, 0.5f);
	if (hit.bBlockingHit) {
		if (hit.GetActor()->ActorHasTag("Pickup")) {
			ActionPickup = Cast<APickupBaseClass>(hit.GetActor());
			if (!ActionPickup) return;
			if (!ActionPickup->IsValidLowLevel()) return;
			if (ActionPickup) {
				if (ActionPickup->PickupData.PickupType == EPickupType::PT_Weapon)
				{
					AWeaponPickup* weaponPickup = Cast<AWeaponPickup>(ActionPickup);
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "This is a weapon.");
					if (!weaponPickup)
					{
						return;
					}
					TSubclassOf<AWeaponBaseClass> Obj = weaponPickup->WeaponClass;
					//WeaponRef->SK_FP_Mesh->AttachToComponent(Mesh1P_Arms, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
					if (!Obj)
					{
						PrintToScreen("Invalid weapon class on item...");
						return;
					}
					if (CanPickupWeapon(Obj))
					{
						GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "You may pickup the weapon.");
						WeaponBuffer = nullptr;
						WeaponRef = nullptr;
							UWorld* world = GetWorld();
							FVector Location = FVector(0.f, 0.f, 0.f);
							FRotator Rotation = GetActorRotation();
							FTransform T = FTransform(Rotation, Location, FVector(1, 1, 1));
							WeaponRef = world->SpawnActorDeferred<AWeaponBaseClass>(Obj, T);     ////spawns weapon
							WeaponRef->SetOwner(this);
							WeaponRef->Instigator = this;
							//WeaponRef->WeaponData.CurrentAmmo = weaponPickup->AmmoOnWeapon;
							WeaponRef->FinishSpawning(T);
						if (WeaponRef) {
							//PrintToScreen(WeaponRef->GetName());
							AddWeaponToInventory(WeaponRef);

							if (GetNetMode() == NM_DedicatedServer)
							{
								//OnRep_WeaponRef();
								//PrintToScreen("DediServer");
							}
							if (GetNetMode() == NM_ListenServer)
							{
								//PrintToScreen("ListenServer");
								OnRep_WeaponRef();
							}
							if (GetNetMode() == ENetMode::NM_Client)
							{
								//PrintToScreen("Client");
							}
							if (GetNetMode() == ENetMode::NM_Standalone)
							{
								//PrintToScreen("Standalone");
								OnRep_WeaponRef();
							}
							//OnRep_WeaponRef();
							//HolsterWeapon(WeaponRef);
							//ChangeWeaponsVisibility(bIsFPS);    ////after adding to inventory i can change visibility based on view perspective
							/*if (!CurrentWeapon) {
								EquipWeapon(WeaponRef);
							}*/
						}
					}
					else {
						//WeaponRef->Destroy();+++++++++++++++++++++++++++++++++++++
						GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "You may NOT pickup the weapon.");

						/*WeaponRef->Destroy();
						WeaponRef = nullptr;*/
						return;
					}
					//bIsUnarmed = false;
					
				}
				else if (ActionPickup->PickupData.PickupType == EPickupType::PT_Ammo)
				{
					if (ActionPickup->PickupData.ItemClass)
					{
						if (ActionPickup->PickupData.ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData.isStackable)
						{
							int32 a = FindItemInInventory(ActionPickup->PickupData.ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData);
							if (a == -1)
							{
								AddItemToInventory(ActionPickup->PickupData.ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData, ActionPickup->PickupData.Amount);
							}
							else {
								Inventory[a].Amount = Inventory[a].Amount + ActionPickup->PickupData.Amount;
							}
						}
					}
				}
				else if (ActionPickup->PickupData.PickupType == EPickupType::PT_Armor)
				{
					if (ActionPickup->PickupData.ItemClass)
					{
						AddItemToInventory(ActionPickup->PickupData.ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData, ActionPickup->PickupData.Amount);
					}
					else {
						PrintToScreen("No item class on item");
					}

					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "This is a piece of armor.");
					TSubclassOf<AArmorBaseClass> Obj = ActionPickup->PickupData.ArmorClass;
					AArmorBaseClass* armor = NULL;
					if (!armor) {
						UWorld* world = GetWorld();
						FVector Location = GetActorLocation();
						FRotator Rotation = GetActorRotation();
						armor = world->SpawnActor<AArmorBaseClass>(Obj, Location, Rotation);     ////spawns armor
						armor->SetOwner(this);
						if (armor->ArmorData.ArmorPart == EArmorPart::AP_Head)
						{
							Armor_Head_Data = armor;
							FP_BodyArmor->SetSkeletalMesh(Armor_Head_Data->SK_Mesh->SkeletalMesh);
							TP_BodyArmor->SetSkeletalMesh(Armor_Head_Data->SK_Mesh->SkeletalMesh);
						}
						if (armor->ArmorData.ArmorPart == EArmorPart::AP_Torso)
						{
							Armor_Torso_Data = armor;
							FP_BodyArmor->SetSkeletalMesh(Armor_Torso_Data->SK_Mesh->SkeletalMesh);
							TP_BodyArmor->SetSkeletalMesh(Armor_Torso_Data->SK_Mesh->SkeletalMesh);
							if (HUDWidget)
							{
								HUDWidget->SetArmorIconTorsoVisibility(true);
							}
						}
						armor->Destroy();
					}
				}
				else if (ActionPickup->PickupData.PickupType == EPickupType::PT_Medicine)
				{
					AddItemToInventory(ActionPickup->PickupData.ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData, ActionPickup->PickupData.Amount);
				}
				else if (ActionPickup->PickupData.PickupType == EPickupType::PT_Clothing)
				{
					AClothingPickupClass* ClothPickup = Cast<AClothingPickupClass>(ActionPickup);
					if (ClothPickup)
					{
						if (ClothPickup->ClothingClass)
						{
							AddItemToInventory(ClothPickup->ClothingClass->GetDefaultObject<AClothingBaseClass>()->ItemData, ActionPickup->PickupData.Amount);
						}
						else {
							PrintToScreen("No item class on item");
						}
						EquipCloth(ClothPickup->ClothingClass->GetDefaultObject<AClothingBaseClass>());
					}
				}
				else if (ActionPickup->PickupData.PickupType == EPickupType::PT_Attachments)
				{
					AddItemToInventory(ActionPickup->PickupData.ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData, ActionPickup->PickupData.Amount);
				}

				if (!ActionPickup->IsPendingKill()) {
					ActionPickup->OnPickup();
					return;
				}

			}
		}
		if (hit.GetActor()->ActorHasTag("Usable")) {

		}
	}
}

void AMyFPSCharacter::ChangeCurrentCamera() {

	//PrintToScreen(bFPPOnly ? "true" : "false");

	if (bFPPOnly)
	{
		return;
	}
	if (!bIsADS && CanChangeCamera)
	{
		ADSCameraComponent->Deactivate();
		TransitionCamera->Deactivate();
		if (bIsFPS) {
			FirstPersonCameraComponent->Deactivate();
			FP_BodyArmor->SetOwnerNoSee(true);
			Mesh1P_Jacket->SetOwnerNoSee(true);
			TP_BodyArmor->SetOwnerNoSee(false);
			Mesh1P_Legs->SetOwnerNoSee(true);
			Mesh1P_Arms_lower->SetOwnerNoSee(true);
			Mesh1P_Arms_upper->SetOwnerNoSee(true);
			Mesh1P_Feet->SetOwnerNoSee(true);
			Mesh1P_Torso->SetOwnerNoSee(true);
			Mesh3P_Legs->SetOwnerNoSee(false);
			Mesh3P_Arms_lower->SetOwnerNoSee(false);
			Mesh3P_Arms_upper->SetOwnerNoSee(false);
			Mesh3P_Feet->SetOwnerNoSee(false);
			Mesh3P_Torso->SetOwnerNoSee(false);
			Mesh3P_Jacket->SetOwnerNoSee(false);
			Mesh3P_Head->SetOwnerNoSee(false);
			ThirdPersonCameraComponent->Activate();

			if (GetWorld())
			{
				APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
				if (CameraManager)
				{
					CameraManager->ViewPitchMin = -70.f;
					//PrintToScreen(FString::SanitizeFloat(CameraManager->ViewPitchMin));
				}
				else {
					PrintToScreen("no cam mgr");
				}
			}
			else {
				PrintToScreen("no world");
			}

			SetActiveCam(false); ///true means fpp camera
			SetbIsFPS(false);	//bIsFPS = false;
			ChangeWeaponsVisibility(false);
		}
		else if (!bIsFPS) {
			FirstPersonCameraComponent->Activate();
			FP_BodyArmor->SetOwnerNoSee(false);
			TP_BodyArmor->SetOwnerNoSee(true);

			/*FP_Gun->SetOwnerNoSee(false);
			FP_Gun->SetOnlyOwnerSee(true);
			FP_Gun->SetVisibility(true);
			TP_Gun->SetOwnerNoSee(true);*/
			Mesh1P_Legs->SetOwnerNoSee(false);
			Mesh1P_Arms_lower->SetOwnerNoSee(false);
			Mesh1P_Arms_upper->SetOwnerNoSee(false);
			Mesh1P_Feet->SetOwnerNoSee(false);
			Mesh1P_Torso->SetOwnerNoSee(false);
			Mesh1P_Jacket->SetOwnerNoSee(false);
			Mesh3P_Legs->SetOwnerNoSee(true);
			//Mesh3P_Legs->SetOnlyOwnerSee(true);
			Mesh3P_Arms_lower->SetOwnerNoSee(true);
			//Mesh3P_Arms_lower->SetOnlyOwnerSee(true);
			Mesh3P_Arms_upper->SetOwnerNoSee(true);
			//Mesh3P_Arms_upper->SetOnlyOwnerSee(true);
			Mesh3P_Feet->SetOwnerNoSee(true);
			//Mesh3P_Feet->SetOnlyOwnerSee(true);
			Mesh3P_Torso->SetOwnerNoSee(true);
			//Mesh3P_Torso->SetOnlyOwnerSee(true);
			Mesh3P_Jacket->SetOwnerNoSee(true);
			//Mesh3P_Jacket->SetOnlyOwnerSee(true);
			Mesh3P_Head->SetOwnerNoSee(true);
			//Mesh3P_Head->SetOnlyOwnerSee(true);
			ThirdPersonCameraComponent->Deactivate();

			if (GetWorld())
			{
				APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0));
				if (CameraManager)
				{
					CameraManager->ViewPitchMin = -80.f;
					//PrintToScreen(FString::SanitizeFloat(CameraManager->ViewPitchMin));
				}
				else {
					PrintToScreen("no cam mgr");
				}
			}
			else {
				PrintToScreen("no world");
			}

			SetActiveCam(true);   //// false means tpp camera
			SetbIsFPS(true);	//bIsFPS = true;
			ChangeWeaponsVisibility(true);
		}
		//ChangeWeaponsVisibility(bIsFPS);
	}

}

void AMyFPSCharacter::ChangeCamera(bool b)       //false means tpp
{
	ADSCameraComponent->Deactivate();
	TransitionCamera->Deactivate();
	if (b) {
		FirstPersonCameraComponent->Deactivate();
		FP_BodyArmor->SetOwnerNoSee(true);
		Mesh1P_Jacket->SetOwnerNoSee(true);
		TP_BodyArmor->SetOwnerNoSee(false);
		Mesh1P_Legs->SetOwnerNoSee(true);
		Mesh1P_Arms_lower->SetOwnerNoSee(true);
		Mesh1P_Arms_upper->SetOwnerNoSee(true);
		Mesh1P_Feet->SetOwnerNoSee(true);
		Mesh1P_Torso->SetOwnerNoSee(true);
		Mesh3P_Legs->SetOwnerNoSee(false);
		Mesh3P_Arms_lower->SetOwnerNoSee(false);
		Mesh3P_Arms_upper->SetOwnerNoSee(false);
		Mesh3P_Feet->SetOwnerNoSee(false);
		Mesh3P_Torso->SetOwnerNoSee(false);
		Mesh3P_Jacket->SetOwnerNoSee(false);
		Mesh3P_Head->SetOwnerNoSee(false);
		ThirdPersonCameraComponent->Activate();

		

		SetActiveCam(false); ///true means fpp camera
		SetbIsFPS(false);	//bIsFPS = false;
		ChangeWeaponsVisibility(false);
	}
	else if (!b) {
		FirstPersonCameraComponent->Activate();
		FP_BodyArmor->SetOwnerNoSee(false);
		TP_BodyArmor->SetOwnerNoSee(true);

		/*FP_Gun->SetOwnerNoSee(false);
		FP_Gun->SetOnlyOwnerSee(true);
		FP_Gun->SetVisibility(true);
		TP_Gun->SetOwnerNoSee(true);*/
		Mesh1P_Legs->SetOwnerNoSee(false);
		Mesh1P_Arms_lower->SetOwnerNoSee(false);
		Mesh1P_Arms_upper->SetOwnerNoSee(false);
		Mesh1P_Feet->SetOwnerNoSee(false);
		Mesh1P_Torso->SetOwnerNoSee(false);
		Mesh1P_Jacket->SetOwnerNoSee(false);
		Mesh3P_Legs->SetOwnerNoSee(true);
		//Mesh3P_Legs->SetOnlyOwnerSee(true);
		Mesh3P_Arms_lower->SetOwnerNoSee(true);
		//Mesh3P_Arms_lower->SetOnlyOwnerSee(true);
		Mesh3P_Arms_upper->SetOwnerNoSee(true);
		//Mesh3P_Arms_upper->SetOnlyOwnerSee(true);
		Mesh3P_Feet->SetOwnerNoSee(true);
		//Mesh3P_Feet->SetOnlyOwnerSee(true);
		Mesh3P_Torso->SetOwnerNoSee(true);
		//Mesh3P_Torso->SetOnlyOwnerSee(true);
		Mesh3P_Jacket->SetOwnerNoSee(true);
		//Mesh3P_Jacket->SetOnlyOwnerSee(true);
		Mesh3P_Head->SetOwnerNoSee(true);
		//Mesh3P_Head->SetOnlyOwnerSee(true);
		ThirdPersonCameraComponent->Deactivate();

		SetActiveCam(true);   //// false means tpp camera
		SetbIsFPS(true);	//bIsFPS = true;
		ChangeWeaponsVisibility(true);
	}
}

bool AMyFPSCharacter::SetActiveCam_Validate(bool bFPS) {
	return true;
}

void AMyFPSCharacter::SetActiveCam_Implementation(bool bFPS)
{
	if (bFPS) {
		ActiveCamera = FirstPersonCameraComponent;
	}
	else {
		ActiveCamera = ThirdPersonCameraComponent;
	}
}

bool AMyFPSCharacter::SetActiveCamComp_Validate(UCameraComponent* cam) {
	return true;
}

void AMyFPSCharacter::SetActiveCamComp_Implementation(UCameraComponent* cam)
{
	ActiveCamera = cam;
}

void AMyFPSCharacter::ChangeWeaponsVisibility_Implementation(bool b)
{
	if (!IsLocallyControlled())
	{
		return;
	}



	PrintToScreen("ChangeWeaponsVis Called");
	if (b) {
		for (int i = 0;i<3;i++)
		{
			if (WeaponInventory[i]) {
				WeaponInventory[i]->SetVisibility(b);
				WeaponInventory[i]->SK_FP_Mesh->SetCastShadow(false);
				WeaponInventory[i]->SK_TP_Mesh->bCastHiddenShadow = true;
				/*
				WeaponInventory[i]->SK_FP_Mesh->SetOwnerNoSee(false);
				WeaponInventory[i]->SK_FP_Mesh->SetVisibility(true, true);				//SetOwnerNoSee(false)
				
				WeaponInventory[i]->SK_TP_Mesh->SetOwnerNoSee(true);				//
				WeaponInventory[i]->SK_TP_Mesh->SetVisibility(false, true);		*/		//SetOwnerNoSee(true)
			}
			else {
				//PrintToScreen("FPS: No weapon in index");
			}
		}
	}
	else {
		for (int i = 0; i < 3; i++)
		{
			if (WeaponInventory[i]) {
				WeaponInventory[i]->SetVisibility(b);
				WeaponInventory[i]->SK_FP_Mesh->SetCastShadow(false);
				WeaponInventory[i]->SK_TP_Mesh->bCastHiddenShadow = true;/*
				WeaponInventory[i]->SK_FP_Mesh->SetOwnerNoSee(true);
				WeaponInventory[i]->SK_FP_Mesh->SetVisibility(false, true);
				//
				WeaponInventory[i]->SK_TP_Mesh->SetOwnerNoSee(false);
				WeaponInventory[i]->SK_TP_Mesh->SetVisibility(true, true);*/
			}
			else {
				//PrintToScreen("TPS: No weapon in index");
			}
		}
	}
}

bool AMyFPSCharacter::SetCanFire_Validate(bool b) {
	return true;
}

void AMyFPSCharacter::SetCanFire_Implementation(bool b)
{
	bCanFire = b;
}

void AMyFPSCharacter::RPC_Client_AttachDetachToHandSocket_Implementation(bool b)
{
	if (b)
	{
		if (WeaponBuffer) {
			//CurrentWeapon = WeaponBuffer;
			/*CurrentWeapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Arms_lower, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));			////attaches it to 1p and 3p sockets
			CurrentWeapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Arms_lower, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint3P"));*/
			WeaponOriginalT = CurrentWeapon->SK_FP_Mesh->GetRelativeTransform();
		}
		if (CurrentWeapon)
		{
			ADSCamera->AttachToComponent(CurrentWeapon->SK_FP_Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			ADSCamera->SetRelativeLocation(CurrentWeapon->ADSCameraComponent->RelativeLocation);
			ADSCamera->SetRelativeRotation(FRotator(-0.771429, 90.126503, -1.086500));
			//ADSCamera->SetRelativeRotation(FRotator());
		}
	}

}

bool AMyFPSCharacter::AttachDetachToHandSocket_Validate(bool b) {
	return true;
}

void AMyFPSCharacter::AttachDetachToHandSocket_Implementation(bool b)
{
	RPC_All_AttachDetachToHandSocket(b);
	RPC_Client_AttachDetachToHandSocket(b);

	if (b)
	{
		if (WeaponBuffer) {
			CurrentWeapon = WeaponBuffer;
			/*CurrentWeapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Arms_lower, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));			////attaches it to 1p and 3p sockets
			CurrentWeapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Arms_lower, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint3P"));*/
			WeaponOriginalT = CurrentWeapon->SK_FP_Mesh->GetRelativeTransform();
		}
		if (CurrentWeapon)
		{
			if (!ADSCamera)
			{
				return;
			}

			ADSCamera->AttachToComponent(CurrentWeapon->SK_FP_Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			//ADSCamera->attac
			ADSCamera->SetRelativeLocation(CurrentWeapon->ADSCameraComponent->RelativeLocation);
			ADSCamera->SetRelativeRotation(FRotator(-0.771429, 90.126503, -1.086500));
		}
	}
	else {
		if (CurrentWeapon) {
			CurrentWeapon = nullptr;
			WeaponBuffer = nullptr;
			WeaponRef = nullptr;
		}
	}
	if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone)
	{
		OnRep_CurrentWeapon();
	}
	//
}

void AMyFPSCharacter::RPC_All_AttachDetachToHandSocket_Implementation(bool b)
{
	if (b)
	{
		if (WeaponBuffer) {
			GLog->Log(WeaponBuffer->GetName());
			if (!CurrentWeapon)
			{
				PrintToScreen("No current weapon on all attach detach");
				return;
			}
			
			CurrentWeapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Arms_lower, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));			////attaches it to 1p and 3p sockets
			CurrentWeapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Arms_lower, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint3P"));
			WeaponOriginalT = CurrentWeapon->SK_FP_Mesh->GetRelativeTransform();
		}
		else {
			GLog->Log("No weapon buffer on All!");
		}
	}
	else {
		if (CurrentWeapon) {
			if (CheckCurrentWeaponSlot() == 0) {
				if (CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_AR || CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_SMG || CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_Shotgun)
				{
					CurrentWeapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot1Socket"));			////attaches it to 1p and 3p sockets
					CurrentWeapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot1Socket"));
				}
			}
			else if (CheckCurrentWeaponSlot() == 1) {
				if (CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_AR || CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_SMG || CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_Shotgun)
				{
					CurrentWeapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot2Socket"));			////attaches it to 1p and 3p sockets
					CurrentWeapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot2Socket"));
				}
			}
			else if (CheckCurrentWeaponSlot() == 2) {
				if (CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_Pistol) {
					CurrentWeapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot3Socket"));			////attaches it to 1p and 3p sockets
					CurrentWeapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot3Socket"));
				}
			}
			//CurrentWeapon = nullptr;
			//WeaponBuffer = nullptr;
		}
	}
	//
}

bool AMyFPSCharacter::EquipWeapon_Validate(AWeaponBaseClass* weapon) {
	return true;
}

void AMyFPSCharacter::EquipWeapon_Implementation(AWeaponBaseClass* weapon)
{
	PrintToScreen("EquipWeapon called!");
	if (!Mesh1P_Legs->GetAnimInstance()->Montage_IsPlaying(NULL)) {
		CurrentWeapon = weapon;
		WeaponRef = nullptr;

		if (!weapon)
		{
			PrintToScreen("Error in equipweapon :(");
			return;
		}
			WeaponBuffer = weapon;

		if (weapon->WeaponData.WeaponType == EWeaponType::WT_Pistol) {
			if (PistolEquipAnimation_TP)
			{
				if (!Mesh3P_Legs->GetAnimInstance()->Montage_IsPlaying(PistolEquipAnimation_TP)) {
					PlayAnimation_TP(PistolEquipAnimation_TP);
				}
			}
			if (PistolEquipAnimation_FP) {
				if (!Mesh1P_Legs->GetAnimInstance()->Montage_IsPlaying(PistolEquipAnimation_FP)) {
					//Mesh1P_Arms_lower->GetAnimInstance()->Montage_Play(PistolEquipAnimation);
					PlayAnimation_FP(PistolEquipAnimation_FP);
				}
			}
			bPistolEquipped = true;
		}
		else if (weapon->WeaponData.WeaponType == EWeaponType::WT_AR) {
			if (RifleEquipAnimation_TP)
			{
				if (!Mesh3P_Legs->GetAnimInstance()->Montage_IsPlaying(RifleEquipAnimation_TP)) {
					PlayAnimation_TP(RifleEquipAnimation_TP);
				}
				PlayAnimation_FP(RifleEquipAnimation_FP);
			}
			if (RifleEquipAnimation_FP) {
				if (!Mesh1P_Legs->GetAnimInstance()->Montage_IsPlaying(RifleEquipAnimation_FP)) {
					PlayAnimation_FP(RifleEquipAnimation_FP);
				}
			}
			bRifleEquipped = true;
		}
		else if (weapon->WeaponData.WeaponType == EWeaponType::WT_SMG) {
			if (RifleEquipAnimation_TP)
			{
				if (!Mesh3P_Legs->GetAnimInstance()->Montage_IsPlaying(RifleEquipAnimation_TP)) {
					PlayAnimation_TP(RifleEquipAnimation_TP);
				}
			}
			if (RifleEquipAnimation_FP) {
				if (!Mesh1P_Legs->GetAnimInstance()->Montage_IsPlaying(RifleEquipAnimation_FP)) {
					PlayAnimation_FP(RifleEquipAnimation_FP);
				}
			}
			bRifleEquipped = true;
		}
		bIsUnarmed = false;
		SetbUseHandIK(true);		//bUseHandIk = true;

		if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone)
		{
			OnRep_CurrentWeapon();
		}
		//ADSCamera->AttachToComponent(weapon->SK_FP_Mesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("ADSCamSocket"));
		//ADSCamera->SetWorldTransform(weapon->SK_FP_Mesh->GetSocketTransform(TEXT("ADSCamSocket")));
	}
}

void AMyFPSCharacter::RPC_EquipWeapon_Client_Implementation(AWeaponBaseClass * weapon)
{
	EquipWeapon(weapon);
}

void AMyFPSCharacter::CheckWeaponStateDelay(AWeaponBaseClass * weapon) {
	GLog->Log("Waiting");
	RPC_EquipWeapon_Client(weapon);
}

int32 AMyFPSCharacter::CheckCurrentWeaponSlot() {
	if (CurrentWeapon) {
		if (CurrentWeapon == WeaponInventory[0]) {
			return 0;
		}
		if (CurrentWeapon == WeaponInventory[1]) {
			return 1;
		}
		if (CurrentWeapon == WeaponInventory[2]) {
			return 2;
		}
		return 99;
	}
	return 99;
}

void AMyFPSCharacter::OnRep_CurrentWeapon()
{
	//EquipWeapon(CurrentWeapon);
	if (!CurrentWeapon)
	{
		return;
	}
	PrintToScreen("OnRep current weapon!");
	CurrentWeapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Arms_lower, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));			////attaches it to 1p and 3p sockets
	CurrentWeapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Arms_lower, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint3P"));
}

bool AMyFPSCharacter::UnequipAttachment_Validate(FItemData attachment, AWeaponBaseClass * weapon, int32 slot, bool isEquipped) {
	return true;
}

void AMyFPSCharacter::UnequipAttachment_Implementation(FItemData attachment, AWeaponBaseClass * weapon, int32 slot, bool isEquipped)
{
	PrintToScreen("Unequip Attachment: INIT");
	/*if (FindItemInInventory(attachment) == -1)
	{
		PrintToScreen("Unequip Attachment: item not found in inventory");
		return;
	}
	else {
		PrintToScreen("Item found!");
	}*/
	if (WeaponInventory[slot])
	{
		PrintToScreen("123asdas");
		WeaponInventory[slot]->UnequipAttachment(attachment, weapon, slot,isEquipped);
		WeaponInventory[slot]->SetVisibility(bIsFPS);
	}
	else {
		PrintToScreen("Weapon slot referred to is empty");
	}
	
}

bool AMyFPSCharacter::HolsterCurrentWeapon_Validate() {
	return true;
}

void AMyFPSCharacter::HolsterCurrentWeapon_Implementation()
{
	if (CurrentWeapon) {
		/*if (!Mesh1P_Legs->GetAnimInstance()->Montage_IsPlaying(NULL)) {*/
			if (CheckCurrentWeaponSlot() == 0) {
				if (CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_AR || CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_SMG || CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_Shotgun)
				{
					if (RifleHolsterAnimation_FP)
					{
						if (!Mesh1P_Legs->GetAnimInstance()->Montage_IsPlaying(RifleHolsterAnimation_FP)) {
							PlayAnimation_FP(RifleHolsterAnimation_FP);
						}
					}
					if (RifleHolsterAnimation_TP)
					{
						if (!Mesh3P_Legs->GetAnimInstance()->Montage_IsPlaying(RifleHolsterAnimation_TP)) {
							PlayAnimation_TP(RifleHolsterAnimation_TP);
						}
					}
				}
				/*CurrentWeapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot1Socket"));			////attaches it to 1p and 3p sockets
				CurrentWeapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot1Socket"));*/
			}
			else
				if (CheckCurrentWeaponSlot() == 1) {
					if (CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_AR || CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_SMG || CurrentWeapon->WeaponData.WeaponType == EWeaponType::WT_Shotgun)
					{
						if (RifleHolsterAnimation_FP)
						{
							if (!Mesh1P_Legs->GetAnimInstance()->Montage_IsPlaying(RifleHolsterAnimation_FP)) {
								PlayAnimation_FP(RifleHolsterAnimation_FP);
							}
						}
						if (RifleHolsterAnimation_TP)
						{
							if (!Mesh3P_Legs->GetAnimInstance()->Montage_IsPlaying(RifleHolsterAnimation_TP)) {
								PlayAnimation_TP(RifleHolsterAnimation_TP);
							}
						}
					}
					/*CurrentWeapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot2Socket"));			////attaches it to 1p and 3p sockets
					CurrentWeapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot2Socket"));*/
				}
				else
					if (CheckCurrentWeaponSlot() == 2) {
						if (PistolHolsterAnimation_FP) {
							if (!Mesh1P_Legs->GetAnimInstance()->Montage_IsPlaying(PistolHolsterAnimation_FP)) {
								PlayAnimation_FP(PistolHolsterAnimation_FP);
							}
						}
						if(PistolHolsterAnimation_TP){
							if (!Mesh3P_Legs->GetAnimInstance()->Montage_IsPlaying(PistolHolsterAnimation_TP)) {
								PlayAnimation_TP(PistolHolsterAnimation_TP);
							}
						}
						/*CurrentWeapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot3Socket"));			////attaches it to 1p and 3p sockets
						CurrentWeapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot3Socket"));*/
					}
			//CurrentWeapon = nullptr;
			bIsUnarmed = true;
			bPistolEquipped = false;
			bRifleEquipped = false;
			WeaponBuffer = nullptr;
			SetbUseHandIK(false);//bUseHandIk = false;
		/*}*/
	}
}

bool AMyFPSCharacter::HolsterWeapon_Validate(AWeaponBaseClass* weapon) {
	return true;
}

void AMyFPSCharacter::HolsterWeapon_Implementation(AWeaponBaseClass * weapon)
{
	//PrintToScreen("HolsterWeapon: Called!");
	if (!weapon)
	{
		//PrintToScreen("HolsterWeapon: argument null");
		return;
	}
		if (weapon == WeaponInventory[0]) {
			weapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot1Socket"));			////attaches it to 1p and 3p sockets
			weapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot1Socket"));
		}
		else
			if (weapon == WeaponInventory[1]) {
				weapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot2Socket"));			////attaches it to 1p and 3p sockets
				weapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot2Socket"));
			}
			else
				if (weapon == WeaponInventory[2]) {
					weapon->SK_FP_Mesh->AttachToComponent(Mesh1P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot3Socket"));			////attaches it to 1p and 3p sockets
					weapon->SK_TP_Mesh->AttachToComponent(Mesh3P_Torso, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("WeaponSlot3Socket"));
				}
		//CurrentWeapon = nullptr;
		//bIsUnarmed = true;
		//bPistolEquipped = false;
		//bRifleEquipped = false;
}

bool AMyFPSCharacter::DropItem_Validate(FItemData itemData, int32 index) {
	return true;
}

FHitResult AMyFPSCharacter::TraceDown() {
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);

	FHitResult RV_Hit(ForceInit);
	FVector startTrace = GetActorLocation();
	FVector endTrace = (startTrace + ((GetActorUpVector()*500.f)*-1));
	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, startTrace, endTrace, ECC_Camera, RV_TraceParams)) {
		DrawLineTraces(GetWorld(), RV_Hit.TraceStart, RV_Hit.TraceEnd, TArray<FHitResult> {RV_Hit}, 7.5f);
	}
	return RV_Hit;
}

bool AMyFPSCharacter::SetWeaponAimingFalse_Validate() {
	return true;
}

void AMyFPSCharacter::SetWeaponAimingFalse_Implementation()
{
	CurrentWeapon->SetAimingFalse();
}

bool AMyFPSCharacter::SetWeaponAimingTrue_Validate() {
	return true;
}

void AMyFPSCharacter::SetWeaponAimingTrue_Implementation()
{
	CurrentWeapon->SetAimingTrue();
}

void AMyFPSCharacter::DropItem_Implementation(FItemData itemData, int32 index)
{
	if (FindItemInInventory(itemData) == -1)
	{
		PrintToScreen("Item not found on inventory, cheater!");
		return;
	}
	/*FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;
	RV_TraceParams.AddIgnoredActor(this);

	FHitResult RV_Hit(ForceInit);
	FVector startTrace = GetActorLocation();
	FVector endTrace = (startTrace+((GetActorUpVector()*500.f)*-1));
	if (GetWorld()->LineTraceSingleByChannel(RV_Hit, startTrace, endTrace, ECC_Camera, RV_TraceParams)) {
		DrawLineTraces(GetWorld(), RV_Hit.TraceStart, RV_Hit.TraceEnd, TArray<FHitResult> {RV_Hit}, 7.5f);
	}*/

	FHitResult RV_Hit = TraceDown();

	if (index >= 0)
	{
			Inventory.RemoveAt(index);          ///use this to remove from index
			PrintToScreen("Item succesfully removed from index");
	}
	else {
		PrintToScreen("Tried to remove item at index, and failed");
	}
	
	if (itemData.PickupClass)
	{
			UWorld* world = GetWorld();
			if (world)
			{
				APickupBaseClass* obj;
				obj = world->SpawnActorDeferred<APickupBaseClass>(itemData.PickupClass, FTransform(RV_Hit.Location), this, Instigator, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
				obj->FinishSpawning(FTransform(RV_Hit.Location));
			}
	}
	else {
		PrintToScreen("No pickup class...");
	}
	if (itemData.ItemType == EItemType::IT_Armor)
	{
		if (itemData.ArmorClass) {
			UnequipArmor(itemData.ArmorClass->GetDefaultObject<AArmorBaseClass>()->ArmorData);
		}
		else {
			PrintToScreen("No Armor class on armor piece...");
		}
	}
	if (itemData.ItemType == EItemType::IT_Clothing)
	{
		if (itemData.ClothingClass)
		{
			UnequipCloth(itemData.ClothingClass->GetDefaultObject<AClothingBaseClass>());
		}
	}
	BuildClientInventory();
}

bool AMyFPSCharacter::DropWeapon_Validate(int32 slot, int32 index)
{
	return true;
}

void AMyFPSCharacter::DropWeapon_Implementation(int32 slot,int32 index)
{
		if (WeaponInventory[slot]) {

			if (WeaponInventory[slot]->WeaponData.ItemClass)
			{
				FItemData weaponItem = WeaponInventory[slot]->WeaponData.ItemClass->GetDefaultObject<AItemBaseClass>()->ItemData;
				if (weaponItem.AmmoClass)
				{
					if (WeaponInventory[slot]->WeaponData.CurrentAmmo > 0)
					{
						FItemData ammoItem = weaponItem.AmmoClass->GetDefaultObject<AAmmoItemClass>()->ItemData;
						int32 a = FindItemInInventory(ammoItem);
						if (a != -1)
						{
							Inventory[a].Amount += WeaponInventory[slot]->WeaponData.CurrentAmmo;
						}
						else {
							AddItemToInventory(ammoItem, WeaponInventory[slot]->WeaponData.CurrentAmmo);
						}
					}
				}
				else { PrintToScreen("No ammo class on weapon item class..."); }
				/*DropItem(weaponItem, -1);*/
				FHitResult RV_Hit = TraceDown();

				//////// spawn weapoon pickup
				if (weaponItem.PickupClass)
				{
					UWorld* world = GetWorld();
					if (world)
					{
						APickupBaseClass* obj;
						obj = world->SpawnActorDeferred<APickupBaseClass>(weaponItem.PickupClass, FTransform(RV_Hit.Location), this, Instigator, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
						obj->FinishSpawning(FTransform(RV_Hit.Location));
					}
				}
				else {
					PrintToScreen("No pickup class...");
				}
			}
			else {
				PrintToScreen("No item class on weapon...");
			}

			if (CurrentWeapon == WeaponInventory[slot])
			{
				PrintToScreen("Dropping current Weapon");
				
				CurrentWeapon->Destroy();
				CurrentWeapon = nullptr;
				bRifleEquipped = false;
				bPistolEquipped = false;
				bIsUnarmed = true;
				SetbUseHandIK(false);
			}
			//PrintToScreen("Weapon on Slot 1");
			
			if (WeaponInventory[slot] != NULL)
			{
				WeaponInventory[slot]->Destroy();
				WeaponInventory[slot] = nullptr;
			}
			if (WeaponBuffer != NULL)
			{
				WeaponBuffer = nullptr;
			}
			BuildClientInventory();
			if (GetNetMode() == ENetMode::NM_ListenServer || GetNetMode() == ENetMode::NM_Standalone)
			{
				OnRep_CurrentWeapon();
			}
		}
}

void AMyFPSCharacter::BuildClientInventory_Implementation() {
	if (InventoryWidget)
	{
		//PrintToScreen("BUILD inv CALLED");
		InventoryWidget->BuildInventory();
	}
}

bool AMyFPSCharacter::VFX_EjectBulletCase_Validate() {
	return true;
}

void AMyFPSCharacter::VFX_EjectBulletCase_Implementation() {

	VFX_EjectBulletCase_ALL();
	if (CurrentWeapon)
	{
		CurrentWeapon->EjectBulletCase_Client();
	}
}

void AMyFPSCharacter::VFX_EjectBulletCase_ALL_Implementation()
{
	if (ROLE_AutonomousProxy && !IsLocallyControlled())
	{
		//PrintToScreen("VFXBULLETCASE CALLED");
		if (CurrentWeapon)
		{
			CurrentWeapon->EjectBulletCase();
		}
	}
}

/*	End of fire stuff	*/
void AMyFPSCharacter::MoveForward(float Value)
{
	if (!GetCharacterMovement()->IsFalling()) {
		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorForwardVector(), Value);
		}
		if (Value < 0.0f)
		{
			StopSprinting();
		}
	}
}

void AMyFPSCharacter::MoveRight(float Value)
{
	if (!GetCharacterMovement()->IsFalling()) {
		if (Value != 0.0f)
		{
			StopSprinting();
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
}

void AMyFPSCharacter::TurnAtRate(float Rate)
{
	if (!bIsInInventory)
	{
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AMyFPSCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if (!bIsInInventory)
	{
		AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

bool AMyFPSCharacter::HolsterWeapon_server_Validate(AWeaponBaseClass* weapon) {
	return true;
}

void AMyFPSCharacter::HolsterWeapon_server_Implementation(AWeaponBaseClass* weapon)
{
	HolsterWeapon(weapon);
}



bool AMyFPSCharacter::OnHit_Validate(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {
	return true;
}

void AMyFPSCharacter::OnHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, OtherActor->GetName());
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, OtherActor->GetOwner()->GetName());
	/*if (OtherActor->GetOwner() == this)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Silver, "Hitting yoself!");
		return;
	}*/
	/*
	if (OtherActor->ActorHasTag("Bullet"))
	{
		AMyFPSProjectile* p = Cast<AMyFPSProjectile>(OtherActor);
		if (p)
		{
			if (p->GetOwner() == this)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Silver, "Hitting yoself!");
				return;
			}
			TakeDamage(p->ProjectileDamage, FDamageEvent(), p->Instigator->Controller, p);
			PrintToScreen("Actor hit by : " + LastDMGInstigator->GetName() + " with a damage of: " + FString::SanitizeFloat(p->ProjectileDamage));
		//	DecreaseHealth(p->ProjectileDamage);
			OnHit_Client();
		}
		
	}*/
}

void AMyFPSCharacter::OnHit_Client_Implementation() {
	if (HUDWidget) {
		HUDWidget->ShowDamagedImage(fHealth);
	}
}

void AMyFPSCharacter::KeyboardEvent()
{
	if (!CurrentWeapon)
	{
		return;
	}
	PrintToScreen("MESH FP VISIBILITY: " + CurrentWeapon->SK_FP_Mesh->bVisible ? "True" : "False");
		PrintToScreen("MESH TP VISIBILITY: " + CurrentWeapon->SK_TP_Mesh->bVisible ? "True" : "False");
}

void AMyFPSCharacter::DeathRep()
{
	Mesh3P_Legs->SetAnimInstanceClass(NULL);
	Mesh3P_Legs->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	Mesh3P_Legs->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Mesh3P_Legs->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	Mesh3P_Legs->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
}

void AMyFPSCharacter::OnRep_IsDead()
{
	DeathRep();
}

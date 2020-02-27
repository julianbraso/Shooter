// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParticleDefinitions.h"
#include "MyFPSCharacter.h"
#include "MyFPSProjectile.generated.h"



UCLASS(config=Game)
class AMyFPSProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	class UCapsuleComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY()
		FVector StartLoc;

	UPROPERTY()
		FVector EndLoc;


public:
	AMyFPSProjectile();

	UPROPERTY()
		float ProjectileSpeed;

	UPROPERTY()
		float ProjectileDamage;

	UPROPERTY()
		float DistanceTraveled;

	UPROPERTY()
		float DistanceTraveledInM;

	UFUNCTION()
		void InitVelocity(const FVector& ShootDirection);

	UFUNCTION()
		UParticleSystem * GetFXFromSurface(FHitResult Hit);

	UPROPERTY()
		class UParticleSystem* impactFx;

	UPROPERTY(EditAnywhere, Category = ImpactFX)
		class UParticleSystem* impactFx_Wood;

	UPROPERTY(EditAnywhere, Category = ImpactFX)
		class UParticleSystem* impactFx_Dust;

	UPROPERTY(EditAnywhere, Category = ImpactFX)
		class UParticleSystem* impactFx_Grass;

	UPROPERTY(EditAnywhere, Category = ImpactFX)
		class UParticleSystem* impactFx_Glass;

	UPROPERTY(EditAnywhere, Category = ImpactFX)
		class UParticleSystem* impactFx_Concrete;

	UPROPERTY(EditAnywhere, Category = ImpactFX)
		class UParticleSystem* impactFx_Water;

	UPROPERTY(EditAnywhere, Category = ImpactFX)
		class UParticleSystem* impactFx_Flesh;

	UPROPERTY(EditAnywhere, Category = ImpactFX)
		class UMaterialInterface* BulletHitDecal;

	UFUNCTION()
		void SpawnFX(UWorld * World,
			UParticleSystem * EmitterTemplate,
			const FTransform & SpawnTransform,
			bool bAutoDestroy);

	UFUNCTION(NetMulticast,Reliable)
		void SpawnFX_All(UWorld * World, UParticleSystem * EmitterTemplate, const FTransform & SpawnTransform, bool bAutoDestroy);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnFX(UWorld * World, UParticleSystem * EmitterTemplate, const FTransform & SpawnTransform, bool bAutoDestroy);

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class UCapsuleComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

};


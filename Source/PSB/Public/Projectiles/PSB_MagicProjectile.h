// Copyright Mandirigma Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PSB_MagicProjectile.generated.h"


// Forward Declares
class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UNiagaraSystem;


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PSB_API APSB_MagicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APSB_MagicProjectile();

protected:
	
	// Basic Collision Component
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComp;

	// This will give it Velocity
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* MovementComp;

	//// Visual Component
	//UPROPERTY(VisibleAnywhere)
	//UParticleSystemComponent* EffectComp;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NEffectComp;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

// Mandirigma Games

#include "Projectiles/PSB_MagicProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
//#include "Particles/ParticleSystemComponent.h"

// Called when the game starts or when spawned
void APSB_MagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Sets default values
APSB_MagicProjectile::APSB_MagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	RootComponent = SphereComp;
	
	//NEffectComp = CreateDefaultSubobject<UNiagaraComponent>("NEffectComp");

	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(NEffectComp, SphereComp, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::KeepRelativeOffset, true);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 1000.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;


}



// Called every frame
void APSB_MagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


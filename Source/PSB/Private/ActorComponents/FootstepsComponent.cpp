// // Copyright Mandirigma Games. All Rights Reserved.


#include "ActorComponents/FootstepsComponent.h"
#include "PhysicalMaterials/PSB_PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "PSBCharacter.h"

#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<int32> CVarShowFootsteps(
	TEXT("ShowDebugFootsteps"),
	0,
	TEXT("Draws debug info about footsteps")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat);
	
	
	// Sets default values for this component's properties
UFootstepsComponent::UFootstepsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UFootstepsComponent::HandleFootstep(EFoot Foot)
{
	if (APSBCharacter* Character = Cast<APSBCharacter>(GetOwner()))
	{
		const int32 DebugShowFootsteps = CVarShowFootsteps.GetValueOnAnyThread();

		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			FHitResult HitResult;
			const FVector SocketLocation = Mesh->GetSocketLocation(Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector Location = SocketLocation + FVector::UpVector * 20;

			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(Character);

			if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + FVector::UpVector * -50.f, ECollisionChannel::ECC_WorldStatic, QueryParams))
			{
				if (HitResult.bBlockingHit)
				{
					if (HitResult.PhysMaterial.Get())
					{
						UPSB_PhysicalMaterial* PhysicalMaterial = Cast<UPSB_PhysicalMaterial>(HitResult.PhysMaterial.Get());

						if (PhysicalMaterial)
						{
							UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootstepSound, Location, 1.f);
						}
					}
				}
			}
		}
	}
}


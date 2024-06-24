// // Copyright Mandirigma Games. All Rights Reserved.


#include "ActorComponents/FootstepsComponent.h"

#include "PSBCharacter.h"

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

void UFootstepsComponent::HandleFootstep(EFoot Foot);
{

}


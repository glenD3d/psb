// // Copyright Mandirigma Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PSB_GameplayInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPSB_GameplayInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PSB_API IPSB_GameplayInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	void Interact(APawn* InstigatorPawn);
};

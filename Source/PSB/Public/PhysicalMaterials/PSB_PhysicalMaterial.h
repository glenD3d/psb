// // Copyright Mandirigma Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "PSB_PhysicalMaterial.generated.h"

/**
 * 
 */
UCLASS()
class PSB_API UPSB_PhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PhysicalMaterial)
	class USoundBase* FootstepSound = nullptr;
	
};

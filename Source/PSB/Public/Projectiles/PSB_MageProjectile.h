// // Copyright Mandirigma Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraActor.h"
#include "PSB_MageProjectile.generated.h"

class USphereComponent;
class UNiagaraSystem;
/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PSB_API APSB_MageProjectile : public ANiagaraActor
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* NEffectComp;
	
};

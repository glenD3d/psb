// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PSB_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	/* In Blueprints, this is Event Blueprint Initialize Animation. */
	virtual void NativeInitializeAnimation() override;

	/* In Blueprints, this is Event Blueprint Update Animation */
	virtual void NativeUpdateAnimation(float DeltaTime) override;


	/* Create variables */

	/* Forward declared variable, to expose to Blueprints it needs a UProperty. 
	To expose it to the Event Graph, set it to BlueprintReadOnly or BlueprintReadWrite. */
	
	UPROPERTY(BlueprintReadOnly)
	class APSB_Character* PSB_Character;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* PSB_CharacterMovement;

	/* Variable */
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;
	
};

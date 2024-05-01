// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "CharacterAnimInstance.generated.h"


class APSB_Character;
class UCustomMovementComponent;
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
	
	/* In Blueprints, this is Event Blueprint Update Animation. This is calculated every frame. */
	virtual void NativeUpdateAnimation(float DeltaTime) override;



	/* Create variables */

	/* Forward declared variable, to expose to Blueprints it needs a UProperty. 
	To expose it to the Event Graph, set it to BlueprintReadOnly or BlueprintReadWrite. */

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class APSB_Character* PSB_Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class UCharacterMovementComponent* PSB_CharacterMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UCustomMovementComponent* CustomMovementComponent;

	/* Variable */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float GroundSpeed;
	void GetGroundSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	float AirSpeed;
	void GetAirSpeed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	bool bShouldMove;
	void GetShouldMove();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	bool bIsFalling;
	void GetIsFalling();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	bool IsCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement | Character State ", meta = (AllowPrivateAccess = "true"))
	ECharacterState CharacterState;

public:



};

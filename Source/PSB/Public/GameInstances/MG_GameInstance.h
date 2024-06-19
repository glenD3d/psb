// Mandirigma Games

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MG_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PSB_API UMG_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;
	
};

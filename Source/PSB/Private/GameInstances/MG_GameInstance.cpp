// Mandirigma Games


#include "GameInstances/MG_GameInstance.h"
#include "AbilitySystemGlobals.h"

void UMG_GameInstance::Init()
{
	Super::Init();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

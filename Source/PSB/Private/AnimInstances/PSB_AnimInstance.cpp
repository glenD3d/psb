// // Copyright Mandirigma Games. All Rights Reserved.


#include "AnimInstances/PSB_AnimInstance.h"

#include "PSBCharacter.h"
#include "PSBGameTypes.h"
#include "Animation/AnimSequenceBase.h"
#include "Animation/BlendSpace.h"
#include "DataAssets/CharacterDataAsset.h"
#include "DataAssets/CharacterAnimDataAsset.h"


UBlendSpace* UPSB_AnimInstance::GetLocomotionBlendSpace() const
{
    if (APSBCharacter* PSBCharacter = Cast<APSBCharacter>(GetOwningActor()))
    {
        FCharacterData Data = PSBCharacter->GetCharacterData();

        if (Data.CharacterAnimDataAsset)
        {
            return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace;
        }
    }

    return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace : nullptr;

}

UAnimSequenceBase* UPSB_AnimInstance::GetIdleAnimation() const
{
    if (APSBCharacter* PSBCharacter = Cast<APSBCharacter>(GetOwningActor()))
    {
        FCharacterData Data = PSBCharacter->GetCharacterData();

        if (Data.CharacterAnimDataAsset)
        {
            return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset;
        }

    }

    return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationAsset : nullptr;
}

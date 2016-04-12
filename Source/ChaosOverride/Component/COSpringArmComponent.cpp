// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COSpringArmComponent.h"
#include "Character/COCharacter.h"

void UCOSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
	float Pitch = 0.0f;
	/*Update the Z and Y offset of the camera boom based when the character is aiming*/
	/*ENFAIT CA MARCHE PAS TROP*/
	ACOCharacter * COCharacter = Cast<ACOCharacter>(GetOwner());
	if (COCharacter)
	{
		//if(COCharacter->GetIsAiming())
		//SetRelativeLocation(COCharacter->GetClass()->GetDefaultObject<ACOCharacter>()->CameraBoom->RelativeLocation+FVector(0, 0, FMath::Lerp(0, 1000, Pitch/360)), true);
	}

}

void UCOSpringArmComponent::UpdateCameraLocation()
{
	ACOCharacter * COCharacter = Cast<ACOCharacter>(GetOwner());
	if (COCharacter)
	{
		if (COCharacter->GetIsAiming())
		{
			SetRelativeLocation(COCharacter->GetClass()->GetDefaultObject<ACOCharacter>()->CameraBoom->RelativeLocation + COCharacter->GetLocalSpringArmOffsetWhenAiming(), true);
			TargetOffset = COCharacter->GetSpringArmTargetOffsetWhenAiming();
			TargetArmLength *= COCharacter->GetTargetArmLengthMultiplierWhenAiming();
		}
		else
		{
			SetRelativeLocation(COCharacter->GetClass()->GetDefaultObject<ACOCharacter>()->CameraBoom->RelativeLocation);
			TargetOffset = COCharacter->GetClass()->GetDefaultObject<ACOCharacter>()->CameraBoom->TargetOffset;
			TargetArmLength = COCharacter->GetClass()->GetDefaultObject<ACOCharacter>()->CameraBoom->TargetArmLength;
		}
		
	}
}

// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COPlayerCameraManager.h"
#include "Character/COCharacter.h"

ACOPlayerCameraManager::ACOPlayerCameraManager(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ViewPitchMin = -70.0f;
	ViewPitchMax = 70.0f;
	bAlwaysApplyModifiers = true;
}


void ACOPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	ACOCharacter* COCharacter = PCOwner ? Cast<ACOCharacter>(PCOwner->GetPawn()) : nullptr;
	if (COCharacter)
	{
		/*Update the camera fov and zoom when dashing*/
		
		/*Update the camera fov and zoom when using hoverboots*/

		/*Update the camera fov and zoom when walking/running*/

		/*Update the camera fov and zoom when aiming*/
		if (COCharacter->GetIsAiming())
			SetFOV(COCharacter->GetFOVWhenAiming());
		
	}
	
	Super::UpdateCamera(DeltaTime);
}



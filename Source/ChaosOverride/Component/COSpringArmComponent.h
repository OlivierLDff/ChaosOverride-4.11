// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "COSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API UCOSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	/*Updates the desired arm location, calling BlendLocations to do the actual blending if a trace is done*/
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
	
	virtual void UpdateCameraLocation();
	
};

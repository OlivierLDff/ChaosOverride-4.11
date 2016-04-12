// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "Camera/PlayerCameraManager.h"
#include "COPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ACOPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	/* Performs per-tick camera update. Called once per tick after all other actors have been ticked. Non-local players replicate the POV if bUseClientSideCameraUpdates is true. */
	virtual void UpdateCamera(float DeltaTime) override;

public:

	ACOPlayerCameraManager(const class FObjectInitializer& ObjectInitializer);	
	float DefaultFOV;
};

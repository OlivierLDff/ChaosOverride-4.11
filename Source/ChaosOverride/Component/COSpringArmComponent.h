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

	/**
	*	A custom camera lag that handle different lag depending on the XYZ system
	*	This function is called from UpdateCamera every ticks
	*	This is only working with a COCharacter
	*	thanks to https://forums.unrealengine.com/showthread.php?56329-Feature-Request-Axis-specific-camera-lag-max-distance
	*
	*	@see UpdateCamera
	*/
	UFUNCTION(BlueprintCallable, Category = "Lag")
		virtual void CustomCameraLag(float DeltaTime);

	/**
	*	The lag speed for each axis for the custom camera lag system
	*
	*	@see	CustomCameraLag
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lag")
		FVector CameraLagSpeeds = FVector(10, 10, 10);

	/**
	*	The max distance for the camera lag
	*
	*	@see	CustomCameraLag
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lag")
		FVector CameraMaxDistances = FVector(200, 200, 100);

	/**
	*	The min distance for the camera lag
	*
	*	@see	CustomCameraLag
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lag")
		FVector CameraMinDistances = FVector(-200, -200, -300);

	/**
	 *	The lag speed in air
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lag")
		float ZLagInAir = 150.f;

	/**
	 *	The lag speed on ground
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lag")
		float ZLagOnGround = 10.f;

	/**
	 *	The speed of the interpolation between the ZLagInAir & ZLagOnGround
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Lag")
		float SpeedBetweenInAirAndOnGround = 1.f;
	
};

// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "Weapon/COWeapon.h"
#include "CODevastator.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ACODevastator : public ACOWeapon
{
	GENERATED_BODY()

public:
	ACODevastator(const FObjectInitializer& ObjectInitializer);

	/*------------------------*/
	/*---COMPONENT SECTION----*/
	/*------------------------*/

	/* The second arrow for the second muzzle flash */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Spawn")
		class UArrowComponent* SecondMuzzleFlashPoint;

	/*------------------------*/
	/*------FIRE SECTION------*/
	/*------------------------*/

protected:
	/*Called when the weapon needs to fire*/
	virtual void OnBurst() override;
	/*Return true if the weapon can actually fire, for exemple if fully equiped, have ammo, etc...*/
	virtual bool CanFire() override;

	/*Spawn a projectile at the desire location*/
	virtual bool SpawnProjectileAtLocation(FVector Location, FRotator Rotation);
	
	bool bFireWithMainGun = true;
	
};

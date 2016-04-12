// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "CODevastator.h"
#include "Weapon/Projectile/COWeaponProjectile.h"

ACODevastator::ACODevastator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	/*Override the default socket name*/
	WeaponSocketName = "DevastatorSocket";

	//SECOND ARROW
	SecondMuzzleFlashPoint = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("Second Projectile Spawn Point"));
	SecondMuzzleFlashPoint->AttachParent = Root;
}

void ACODevastator::OnBurst()
{
	Super::OnBurst();
	bool bFireWasSuccessfull = false;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%s say burst"), *GetName()));
	if (bFireWithMainGun)
	{
		bFireWasSuccessfull = SpawnProjectileAtLocation(MuzzleFlashPoint->GetComponentLocation(), MuzzleFlashPoint->GetComponentRotation());
		bFireWithMainGun = false;
	}
	else
	{
		bFireWasSuccessfull = SpawnProjectileAtLocation(SecondMuzzleFlashPoint->GetComponentLocation(), SecondMuzzleFlashPoint->GetComponentRotation());
		bFireWithMainGun = true;
	}
	if(bFireWasSuccessfull)	UnFillLoader(1);
	
}

bool ACODevastator::CanFire()
{
	return Super::CanFire();
}

bool ACODevastator::SpawnProjectileAtLocation(FVector Location, FRotator Rotation)
{
	ACOWeaponProjectile * Projectile = nullptr; //The projectile we are going to spawn
	if (ProjectileClass)
	{
		//FActorSpawnParameters SpawnParams;
		//SpawnParams.Instigator = GetInstigator();
		//SpawnParams.Owner = GetOwner();

		/*We spawn the projectile in the gun we want, it alternate*/
		//Projectile = GetWorld()->SpawnActor<ACOWeaponProjectile>(ProjectileClass, Location, Rotation, SpawnParams);

		ACOWeaponProjectile* Projectile = GetWorld()->SpawnActorDeferred<ACOWeaponProjectile>(ProjectileClass, FTransform(Rotation, Location), GetOwner(), GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Projectile)
		{
			Projectile->WeaponOwner = this;//...setstuff here..then finish spawn..
			Projectile->Instigator = GetInstigator();
			Projectile->SetOwner(GetOwner());
			UGameplayStatics::FinishSpawningActor(Projectile, FTransform(Rotation, Location));
		}
		
		/*This has to be replace with, I keep the error for now for debug purpose*/
		//return Projectile!=nullptr;
		if (Projectile)
		{
			//Projectile->SetOwner(GetOwner());
			//Projectile->Instigator = GetInstigator();
			return true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : Unable to spawn a projectile for the weapon %s"), *GetName()));
			return false;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning : Please add a default sub class of COWeaponProjectile for the weapon %s"), *GetName()));
		return false;
	}
	return false;
}

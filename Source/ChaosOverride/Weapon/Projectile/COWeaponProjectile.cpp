// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COWeaponProjectile.h"
#include "Character/COCharacter.h"
#include "Weapon/COWeapon.h"


// Sets default values
ACOWeaponProjectile::ACOWeaponProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	//PROJECTILE COMPONENT
	ProjectileComponent = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("Projectile Component"));
	ProjectileComponent->InitialSpeed = 2000.0f;

	//SPHERE
	DamagingSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("Damaging Sphere"));
	DamagingSphere->bGenerateOverlapEvents = true;	//To generate overlap event
	DamagingSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	DamagingSphere->OnComponentBeginOverlap.AddDynamic(this, &ACOWeaponProjectile::OnDamagingCapsuleOverlapBegin); //BINDING
	DamagingSphere->OnComponentEndOverlap.AddDynamic(this, &ACOWeaponProjectile::OnDamagingCapsuleOverlapEnd);
	RootComponent = DamagingSphere;
}

// Called when the game starts or when spawned
void ACOWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponOwner)
	{
		/*We initialize the actor that is targeted*/
		TargetActor = FindTargetActor();

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("The target is %s"), TargetActor ? *TargetActor->GetName() : *FString("nullptr")));

		/*We set the default orientation for the projectile depending on what the owner is focusing*/
		SetRotationToFocusPoint();

		/*If there is no target, the projectile just start in a normal way*/
		if (TargetActor)
		{
			/*The owner initial forward vector, set in the begin play when the actor is spawning*/
			InitialOwnerForwardVector = WeaponOwner->GetActorForwardVector();
			/*The initial world location when spawned, set in begin play when spawning */
			InitialProjectileWorldLocation = GetActorLocation();
			/*The initial forward vector of the projectile, set in begin play when spawning*/
			InitialForwardVector = GetActorForwardVector();
			/*The initial up vector of the projectile, set in begin play when spawning*/
			InitialUpVector = GetActorUpVector();
			/*The target location when spawning, set in the begin play when the actor is spawning*/
			InitialTargetLocation = TargetActor->GetActorLocation();
			/*The initial distance between the owner and the target, set in the begin play when the actor is spawning*/
			InitialDistanceBetweenOwnerAndTarget = (InitialTargetLocation - WeaponOwner->GetActorLocation()).Size();
			/*The square of the initial speed, V0² in the formula, set in the begin play when the actor is spawning*/
			SquareOfInitialSpeed = FMath::Square(GetVelocity().Size());
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Error/Warning : The projectile spawned %s owner's is %s that is not a COWeapon"), *GetName(), *GetOwner()->GetName()));
	}
}

// Called every frame
void ACOWeaponProjectile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (Role == ROLE_Authority)
	{
		//if (TargetActor)
		//	CorrectTrajectory(DeltaTime, TargetActor, 1.f);
	}

}

void ACOWeaponProjectile::OnDamagingCapsuleOverlapBegin(class AActor * OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{	
	if (Role == ROLE_Authority)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Begin Overlap")));
		/*We check that we are not shooting ourself and the actor is valid*/
		if (OtherActor && OtherActor != this)
		{
			/*An actor is valid for collision if he has no owner or a different owner than the weapon*/
			if (GetOwner() && GetOwner()->GetOwner() != OtherActor && OtherActor->GetOwner() != GetOwner() && OtherActor != GetOwner())
			{
				/*We do not hit ourself or an actor that have the same owner that the projectile*/
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("DEBUG : Begin Overlap with %s"), *OtherActor->GetName()));
				/*if (OtherActor->GetOwner())
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Begin Overlap with %s"), *OtherActor->GetOwner()->GetName()));
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Begin Overlap with %s who has no owner"), *OtherActor->GetName()));
				}*/
				OnCollisionWithValidActor(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("We overlap with self %s"), *OtherActor->GetName()));
			}
		}
		else
		{
			if (!OtherActor)
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Overlap with null")));
		}
	}
}

void ACOWeaponProjectile::OnDamagingCapsuleOverlapEnd(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
}

void ACOWeaponProjectile::OnCollisionWithValidActor(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	/*if (OtherActor->GetOwner())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Begin Overlap with %s"), *OtherActor->GetOwner()->GetName()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Begin Overlap with %s who has no owner"), *OtherActor->GetName()));
	}*/
	/*Test if we are hitting a COCharacter, if so we apply damage to him and we explose*/
	if (OtherActor)
	{
		/*We apply damage to the actor find*/
		if (GetOwner())
		{
			this->LastHitResult = SweepResult;
			FDamageEvent DamageEvent;
			DamageEvent.DamageTypeClass = DamageTypeClass;
			OtherActor->TakeDamage(Damage, DamageEvent, GetOwner()->GetInstigatorController(), this);
			if (OtherActor->GetOwner())	OtherActor->GetOwner()->TakeDamage(Damage, DamageEvent, GetOwner()->GetInstigatorController(), this);
		}
		ACOCharacter * COCharacterHit = Cast<ACOCharacter>(OtherActor);
		if (COCharacterHit == nullptr)
		{
			COCharacterHit = Cast<ACOCharacter>(OtherActor->GetOwner());
			if (COCharacterHit == nullptr)
			{
				/*If we are not damaging a COCharacter then we bounce*/
				Bounce(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}
			else
			{
				Explose(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}
		}
		else
		{
			Explose(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		}
	}
}

bool ACOWeaponProjectile::IsOwnerAiming() const
{
	if (WeaponOwner)
	{
		return WeaponOwner->GetIsOwnerAiming();
	}
	return false;
}

void ACOWeaponProjectile::SetRotationToFocusPoint()
{
	/*If the weapon that spawned this projectile is aiming we want to set the rotation to the focus, if not we call the set default rotation that will be reimplement for each weapon*/
	if (IsOwnerAiming())
	{
		FocusPoint = GetFocusPointFromOwner();
		FVector NewVelocityCarryingVector = (FocusPoint - GetActorLocation());
		/*The rotation that is going to be applied to the projectile*/
		FRotator NewRotation = NewVelocityCarryingVector.Rotation();
		/*Don't know if this is very useful*/
		SetActorRotation(NewRotation);

		/*We recalculate the velocity to go in the direction we want*/
		NewVelocityCarryingVector.Normalize();

		FVector PendingVelocity = ProjectileComponent->Velocity;
		float VelocitySize = PendingVelocity.Size();

		ProjectileComponent->Velocity = NewVelocityCarryingVector*VelocitySize;
	}
	else
	{
		SetDefaultRotation();
	}	
}

void ACOWeaponProjectile::SetDefaultRotation()
{
	/*By default we do nothing, this have to be reimplement for exemple for weapon that need focus point or follow a target*/
}

FVector ACOWeaponProjectile::GetFocusPointFromOwner() const
{
	if (WeaponOwner)
	{
		return WeaponOwner->GetFocusPoint();
	}
	return FVector();
}

AActor * ACOWeaponProjectile::GetTargetActor() const
{
	return TargetActor;
}

void ACOWeaponProjectile::CorrectTrajectory(float DeltaTime, AActor * Target, float TrajectoryCorrectionStrength)
{
	if (Target == nullptr) return;
	/*Set teta depending on the . product of FWD shooter & right look at. When colinear, teta is negative*/
	FRotator RightLookAtRot = FRotationMatrix::MakeFromX(InitialTargetLocation-InitialProjectileWorldLocation).Rotator();
	/*In this rotator we only take a look at the yaw*/
	FRotator RightLookAtRotYaw = RightLookAtRot;
	/*In this rotator we need the yaw & pitch, it will be used in the second part of the formula*/
	FRotator RightLookAtRotPitchAndYaw = RightLookAtRot;
	/*We only need the yaw information or yaw&pitch*/
	RightLookAtRotYaw.Pitch = 0;
	RightLookAtRotYaw.Roll = 0;
	RightLookAtRotPitchAndYaw.Roll = 0;
	/*The forward & right vector from the rotator*/
	FVector ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocation = FRotationMatrix(RightLookAtRotYaw).GetScaledAxis(EAxis::X);
	FVector RightVectorFromInitialProjectileWorldLocationToInitialTargetLocation = FRotationMatrix(RightLookAtRotYaw).GetScaledAxis(EAxis::Y);
	FVector ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocationWithPitch = FRotationMatrix(RightLookAtRotPitchAndYaw).GetScaledAxis(EAxis::X);

	/*----Calculation of the Yaw Gravity----*/

	FVector ProjectionOfLookAtFromInitialLocationToTargetOnToForwardVector = ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocation.ProjectOnTo(InitialForwardVector);

	/*We calculate teta*/
	float Teta = FMath::Acos(ProjectionOfLookAtFromInitialLocationToTargetOnToForwardVector.Size() / InitialForwardVector.Size());

	/*If the dot product of the owner forward vector with the vector from the spawning point to the target is greater than 0 we need to multiply teta by -1*/
	if (FVector::DotProduct(InitialOwnerForwardVector, ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocation) > 0)
	{
		Teta *= -1.f;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Positive"), SquareOfInitialSpeed));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Neg %f"), FVector::DotProduct(InitialOwnerForwardVector, ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocation)));
	}

	/*We calculate the direction of the "gravity" we need to apply, what I call gravity is the correction
	*	H = mg = m*V0²*sin(2teta) / (d - x0)
	*	H is the "heaviness" effect
	*	We suppose here that the mass is 1kg.
	*/
	float YawHeaviness = (SquareOfInitialSpeed * FMath::Sin(2 * Teta) / (InitialDistanceBetweenOwnerAndTarget));

	ProjectileComponent->Velocity += YawHeaviness*RightVectorFromInitialProjectileWorldLocationToInitialTargetLocation*DeltaTime;

	/*----Calculation of the Pitch Gravity----*/

	//TODO

	/*Find pitch in degree*/
	FVector ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocationWithPitchProjectOnToTheVectorWithoutPitch = ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocationWithPitch.ProjectOnTo(ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocation);
	float Beta = FMath::Acos(ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocationWithPitchProjectOnToTheVectorWithoutPitch.Size() / ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocationWithPitch.Size());

	/*If the dot product of the owner up vector with the vector from the spawning point to the target is greater than 0 we need to multiply beta by -1*/
	if (FVector::DotProduct(InitialUpVector, ForwardVectorFromInitialProjectileWorldLocationToInitialTargetLocation) > 0) Beta *= -1.f;

	/*We calculate the direction of the "gravity" we need to apply, what I call gravity is the correction
	*	H = mg = m*V0²*sin(2beta) / (d - x0)
	*	H is the "heaviness" effect
	*	We suppose here that the mass is 1kg.
	*/
	float PitchHeaviness = (SquareOfInitialSpeed * FMath::Sin(2 * Beta) / (InitialDistanceBetweenOwnerAndTarget));
	ProjectileComponent->Velocity += PitchHeaviness*FVector::UpVector*DeltaTime;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Projectile component velocity after : %s"), *ProjectileComponent->Velocity.ToString()));
	
}

void ACOWeaponProjectile::SetTargetActor(AActor * newTargetActor)
{
	TargetActor = newTargetActor;
}

AActor * ACOWeaponProjectile::FindTargetActor()
{
	if (WeaponOwner)
	{
		/*Only if the owner is aiming we want to get target actor, if the character holding the weapon isn't targeting we return nullptr*/
		if (WeaponOwner->GetIsOwnerAiming())
		{
			/*We get the best actor to target & return it*/
			return WeaponOwner->GetBestTargetedActor(bRemoveActorFromTargetActorArrayWhenSpawningAProjectile);
		}
		else
			return nullptr;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Error/Warning : The owner of %s isn't a COWeapon"), *GetName()));
		return nullptr;
	}
	//if (GetOwner())
	//{
	//	//ACOWeapon * WeaponOwner = Cast<ACOWeapon>(GetOwner());
	//	
	//}
	//else
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : No owner for %s, so we can't find the targeted actor or if the weapon is currently targeting"), *GetName()));
	//	return nullptr;
	//}
}

void ACOWeaponProjectile::Bounce(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	
	if (BounceNumber <= 0 || BounceDone >= BounceNumber)
	{
		/*We don't bounce, we explose*/
		Explose(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
	else
	{		
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Bounce")));
		//FVector VectorOnPlane = FVector::VectorPlaneProject(ProjectileComponent->Velocity, SweepResult.ImpactNormal);
		//float Angle = FMath::Acos(FVector::DotProduct(VectorOnPlane, ProjectileComponent->Velocity)/(ProjectileComponent->Velocity.Size()* VectorOnPlane.Size()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Angle %f"), Angle));
		//FVector RotateVector = FVector(1, 0, 0);
		//RotateVector.RotateAngleAxis(Angle, FVector(1, 0, 0));

		/*We search the correct normal of what we hit*/
		FHitResult OutHit;
		const FVector Start = GetActorLocation()-GetActorForwardVector()*200.0f;
		const FVector End = GetActorLocation() + GetActorForwardVector()*200.0f;
		FCollisionQueryParams Params;
		FCollisionResponseParams ResponseParam;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(GetOwner());

		if (GetWorld())
		{
			if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, Params))
			{
				//if (OutHit.GetActor())
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Collision found with %s"), *OutHit.GetActor()->GetName()));

				FPlane Plane = FPlane(OutHit.ImpactNormal);
				FVector NewDirection = ProjectileComponent->Velocity.MirrorByPlane(Plane);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Impact normal : %s"), *OutHit.ImpactNormal.ToString()));
				//SetActorRotation(OutHit.ImpactNormal.Rotation());
				//SetActorRotation(FRotator(90,0,0)); //TEST PURPOSE

				OnBounce(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Collision not found")));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : The world you isn't valid in bounce for %s"), *GetName()));
		}
	}
}

void ACOWeaponProjectile::OnBounce(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	++BounceDone;
	OnBounceBlueprint(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ACOWeaponProjectile::OnBounceBlueprint_Implementation(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	/*Call the same function on client not working yet*/
}

void ACOWeaponProjectile::ApplyDamage(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
}

void ACOWeaponProjectile::Explose(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	OnExploding(OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void ACOWeaponProjectile::OnExploding(AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Explose %s"), *GetName()));
	Destroy();
}


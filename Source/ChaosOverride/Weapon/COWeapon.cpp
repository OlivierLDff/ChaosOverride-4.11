// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COWeapon.h"
#include "Character/COCharacter.h"
#include "Gameplay/COAIController.h"



// Sets default values
ACOWeapon::ACOWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//ROOT
	Root = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Defaut Scene Root"));
	SetRootComponent(Root);

	//WEAPON MESH
	WeaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Weapon Mesh"));
	WeaponMesh->AttachParent = Root;

	//ARROW
	MuzzleFlashPoint = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("Projectile Spawn Point"));
	MuzzleFlashPoint->AttachParent = Root;

	//DEFAULT TARGET
	ActorThatCanBeTarget.Add(ACOCharacter::StaticClass());
}

// Called when the game starts or when spawned
void ACOWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("The owner of %s is %s"), *GetName(), *GetOwner()->GetName()));
		COCharacter = Cast<ACOCharacter>(GetOwner());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : %s has No owner, this should never occured"), *GetName()));
	}
}

// Called every frame
void ACOWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	//if (GetIsOwnerAiming())
	//	CheckForTarget();

}

void ACOWeapon::EquipWeapon()
{
	Super::EquipWeapon();
	/*Play equip animation*/
	if (EquipAnimation)
	{
		WeaponMesh->PlayAnimation(EquipAnimation, false);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Weapon equip for %s"), *GetName()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You need to add an equipanimation for %s"), *GetName()));
	}
}

void ACOWeapon::OnEquip()
{
	Super::OnEquip();
	/*If the trigger is pressed we want to try to shoot*/
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("OnEquip")));
	if (GetTriggerPressed())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("We try to burst")));
		Burst();
	}
}

void ACOWeapon::UnEquipWeapon(bool bWithAnimation)
{
	Super::UnEquipWeapon();
	/*Play equip animation*/
	if (UnEquipAnimation)
	{
		WeaponMesh->PlayAnimation(UnEquipAnimation, false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You need to add an unequipanimation for %s"), *GetName()));
	}
}

void ACOWeapon::OnUnEquip()
{
	Super::OnUnEquip();
}

FName ACOWeapon::GetWeaponSocketName() const
{
	Super::GetWeaponSocketName();
	return WeaponSocketName;
}

void ACOWeapon::SetIndexInInventory(int8 newIndex)
{
	IndexInInventory = newIndex;
}

void ACOWeapon::PressTrigger()
{
	bTriggerPressed = true;
	OnTriggerPressed();
}

void ACOWeapon::ReleaseTrigger()
{
	bTriggerPressed = false;
	OnTriggerReleased();
}

void ACOWeapon::OnTriggerPressed()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Trigger pressed")));
	if (CanFire())
	{
		/*To respect the fire rate*/
		const float GameTime = GetWorld()->GetTimeSeconds();
		if (LastTimeWeaponFire > 0.0f && (1.0f / FireRate) > 0.0f && LastTimeWeaponFire + (1.0f / FireRate) > GameTime)
		{
			GetWorldTimerManager().SetTimer(HandleOnBurstTimer, this, &ACOWeapon::Burst, LastTimeWeaponFire + (1.0f / FireRate) - GameTime, false);
		}
		else
		{
			Burst();
		}
	}
}

void ACOWeapon::OnTriggerReleased()
{
	/*We clear the timer that was responsible for reiterating burst*/
	GetWorldTimerManager().ClearTimer(HandleRepeatBurst);
}

bool ACOWeapon::GetTriggerPressed() const
{
	return bTriggerPressed;
}

void ACOWeapon::Burst()
{
	if (GetTriggerPressed())
	{
		if (FireRate > 0.0f)
		{
			GetWorldTimerManager().SetTimer(HandleRepeatBurst, this, &ACOWeapon::Burst, (1.0f/FireRate), false);
		}
		if (CanFire())
		{
			/*We shoot if we are not empty*/
			OnBurst();
		}
	}
}

void ACOWeapon::OnBurst()
{
	LastTimeWeaponFire = GetWorld()->GetTimeSeconds();
	MakeNoise(1.0f, GetInstigator(), GetActorLocation(), 0.0f, FName("Weapon"));
	/*MUST BE OVERRIDEN*/
}

bool ACOWeapon::CanFire()
{
	return bIsEquip && !IsEmpty();
}

bool ACOWeapon::GetIsFiring() const
{
	return bIsFiring;
}

bool ACOWeapon::RefillLoader(int32 ammo)
{
	if (MaxAmmo > 0)
	{
		if (MaxAmmo >= CurrentAmmo)
		{		
			if (CurrentAmmo < MaxAmmo)
			{
				OnLoaderRefill(ammo);
				CurrentAmmo += ammo;
				if (CurrentAmmo > MaxAmmo) CurrentAmmo = MaxAmmo;
				if (CurrentAmmo == MaxAmmo) OnLoaderFull();
				return true;
			}
		}
		else
		{
			if (MaxAmmo == CurrentAmmo) return false;
			/*That case should never occured, an error must have be done somewhere*/
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : There is more current ammo than max ammo for %s. That case should never occured"), *GetName()));
		}
	}
	else
	{
		/*The ammo are infinite we do not need to refill the loader*/
		return true;
	}
	return false;
}

bool ACOWeapon::UnFillLoader(int32 ammo)
{
	if (MaxAmmo > 0)
	{
		if (CurrentAmmo > 0)
		{
			OnLoaderUnfill(ammo);
			CurrentAmmo -= ammo;
			if (CurrentAmmo < 0) CurrentAmmo = 0;
			if (CurrentAmmo == 0) OnLoaderEmpty();
			return true;
		}
	}
	else
	{
		/*The ammo are infinite we do not need to refill the loader*/
	}
	return false;
}

void ACOWeapon::OnLoaderFull()
{

}

void ACOWeapon::OnLoaderEmpty()
{

}

void ACOWeapon::OnLoaderRefill(int32 ammo)
{
}

void ACOWeapon::OnLoaderUnfill(int32 ammo)
{

}

void ACOWeapon::OnTryToFireWithEmptyLoader()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s have no more ammo"), *GetName()));
}

int32 ACOWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

bool ACOWeapon::IsEmpty() const
{
	return CurrentAmmo == 0 && MaxAmmo>0;
}

bool ACOWeapon::GetIsAOneHandWeapon() const
{
	return WeaponType == ECOWeaponType::WT_OneHanded;
}

bool ACOWeapon::GetIsATwoHandWeapon() const
{
	return WeaponType == ECOWeaponType::WT_TwoHanded;
}

bool ACOWeapon::GetIsAMeleeWeaponType() const
{
	return WeaponType == ECOWeaponType::WT_Melee;
}

FVector ACOWeapon::GetFocusPoint()
{
	AActor * PotentialTarget = GetBestTargetedActor();
	if (PotentialTarget)
	{
		return PotentialTarget->GetActorLocation();
	}
	else
	{
		if (COCharacter)
		{
			AController * Controller = COCharacter->GetController();
			if (Controller)
			{
				/*The start of the begin trace*/
				FVector PlayerViewPointLocation;
				FRotator PlayerViewPointRotation;

				Controller->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);
				FVector LineTraceCarryingVector = PlayerViewPointRotation.Vector() * MaxTargetDistance;

				/*We line trace to find the focus point*/
				if (GetWorld())
				{
					/*We line trace*/
					FHitResult OutHit;
					const FVector Start = PlayerViewPointLocation;
					const FVector End = LineTraceCarryingVector + PlayerViewPointLocation;
					FCollisionQueryParams Params;
					FCollisionResponseParams ResponseParam;
					Params.AddIgnoredActor(this);
					Params.AddIgnoredActor(GetOwner());

					/*DEBUG LINE*/
					if (bDrawGetFocusPointDebugLine)
						DrawDebugLine(
							GetWorld(),
							Start,
							End,
							FColor(255, 0, 0),
							false, -1, 0,
							2
							);
					if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, Params))
					{
						/*DEBUG POINT*/
						if (bDrawGetFocusPointDebugLine)
							DrawDebugPoint(
								GetWorld(),
								OutHit.ImpactPoint,
								20,  					//size
								FColor(255, 0, 0),		//red
								false,  				//persistent (never goes away)
								2.f 					//point leaves a trail on moving object
								);
						/*We find a correct focus point*/
						return OutHit.Location;
					}
					else
					{
						/*We didn't find any point to focus so we focus at the distance of FocusDistance*/
						LineTraceCarryingVector.Normalize();
						return LineTraceCarryingVector*FocusDistance + Start;
					}
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : %s owner's isn't controlled by a Controller"), *GetName()));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : Unable to get the focus point because the weapon %s hasn't a valid COcharacter owner"), *GetName()));
		}
	}
	return FVector();
}

AActor * ACOWeapon::GetBestTargetedActor(bool bRemoveActorFromTargetActorArrayWhenSpawningAProjectile)
{
	//CheckForTarget(); //Not the best method
	return TargetActor.Num() > 0 ? TargetActor[0] : nullptr;
}

void ACOWeapon::CheckForTarget()
{
	TArray<AActor *> PotentialTargetArray;
	/*We clean the target actor array because we want to refill it*/
	TargetActor.Empty();
	/*We browse all the actor default sub class that can be targeted*/
	for (int i = 0; i < ActorThatCanBeTarget.Num(); ++i)
	{
		if (ActorThatCanBeTarget[i])
		{
			TArray<AActor *> TempPotentialTargetArray;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ActorThatCanBeTarget[i], TempPotentialTargetArray);
			PotentialTargetArray.Append(TempPotentialTargetArray);
		}
	}
	
	/*Now we have all the potential actor that can be targeted*/
	if (GetOwner())
	{
		/*We don't want to target the owner*/
		PotentialTargetArray.Remove(GetOwner());
		
		APawn * OwnerPawn = Cast<APawn>(GetOwner());

		/*We first check if we have a target in the AI*/
		if (COCharacter)
		{
			AController * C = COCharacter->GetController();
			ACOAIController * AIController = Cast<ACOAIController>(C);
			if (AIController)
			{
				if (COCharacter->AITargetActor)
				{
					TargetActor.Add(COCharacter->AITargetActor);
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("We find the target for %s that is %s"), *GetName(), *COCharacter->AITargetActor->GetName()));
					return;
				}	
			}
		}

		/*We check vector colinearity and remove the actor that are not good*/
		for (AActor* Actor : PotentialTargetArray)
		{		
			if (Actor && OwnerPawn)
			{
				/*The location of the player point of view*/
				FVector PlayerViewPointLocation;
				/*The rotation of the player point of view*/
				FRotator PlayerViewPointRotator;
				/*The actor we are analysing location*/
				FVector ActorLocation;
				/*The actor we are analysing rotation*/
				FRotator ActorRotator;
				/*A vector that represent the rotation of the controller*/
				FVector ControlRotationVector;
				if (OwnerPawn->GetController())
				{
					/*We get the player view point*/
					OwnerPawn->GetController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotator);
					ControlRotationVector = PlayerViewPointRotator.Vector();
					ActorLocation = Actor->GetActorLocation();
					ActorRotator = Actor->GetActorRotation();
					//ControlRotationVector = OwnerPawn->GetController()->GetControlRotation().Vector();
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("ControlRotationVector = %s"), *ControlRotationVector.ToString()));
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("ActorLocation - PlayerViewPointLocation = %s"), *(ActorLocation - PlayerViewPointLocation).ToString()));
					/*We remove all the actor that are not ok*/
					FVector NormalizedViewPointToActor = (ActorLocation - PlayerViewPointLocation);
					NormalizedViewPointToActor.Normalize();
					FVector ControlRotationVectorNormalized = ControlRotationVector;
					ControlRotationVectorNormalized.Normalize();
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("DotProduct = %f"), FVector::DotProduct(NormalizedViewPointToActor, ControlRotationVectorNormalized)));

					if (FVector::DotProduct(NormalizedViewPointToActor, ControlRotationVectorNormalized) >= TargetDoubt /*&& (ActorLocation - GetOwner()->GetActorLocation()).Size() <= MaxTargetDistance*/)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Dot product if ok for %s"), *Actor->GetName()));
						if (GetWorld())
						{
							/*We line trace*/
							FHitResult OutHit;
							const FVector Start = PlayerViewPointLocation;
							const FVector End = NormalizedViewPointToActor * MaxTargetDistance+PlayerViewPointLocation;
							FCollisionQueryParams Params;
							FCollisionResponseParams ResponseParam;
							Params.AddIgnoredActor(this);
							Params.AddIgnoredActor(GetOwner());

							//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Actor location : %s"), *GetActorLocation().ToString()));
							if (bDrawCheckForTargetDebugLine)
							DrawDebugLine(
								GetWorld(),
								Start,
								End,
								FColor(255, 0, 0),
								true, -1, 0,
								2
								);
							if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_WorldDynamic, Params))
							{
								//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("We hit : %s"), OutHit.GetActor()? *OutHit.GetActor()->GetName() : TEXT("none")));
								if (OutHit.GetActor() && (OutHit.GetActor() == Actor || OutHit.GetActor()->GetOwner() == Actor))
								{
									if(bDrawCheckForTargetDebugLine)
									DrawDebugPoint(
										GetWorld(),
										OutHit.ImpactPoint,
										20,  					//size
										FColor(255, 0, 0),		//red
										false,  				//persistent (never goes away)
										0.5f 					//point leaves a trail on moving object
										);
									
									//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("We found a target")));
									TargetActor.Add(OutHit.GetActor());
								}
							}
							else
							{
								//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("We hit : nothing")));
							}
						}
					}			
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : %s have not a valid controller, we can't get the player view point to target"), *OwnerPawn->GetName()));
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : %s isn't a Pawn, we can't target"), *GetOwner()->GetName()));
			}
		}
		/*DEBUG*/
		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Actor found : ")));
		for (int i = 0; i < PotentialTargetArray.Num(); ++i)
		{
			if (PotentialTargetArray[i])
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%d : %s"), i, *PotentialTargetArray[i]->GetName()));
		}*/
		return;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : The weapon %s has no owner, we can't target"), *GetName()));
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : The weapon %s has no owner, we can't target"), *GetOwner()->GetName()));

	//if(GetClass()->GetDefaultObject<ACOCharacter>())
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Class : %s"), *GetClass()->GetDefaultObject<ACOCharacter>()->GetName()));

}

inline bool ACOWeapon::GetFindAtLeastOneTarget() const
{
	return TargetActor.Num() > 0;
}

bool ACOWeapon::GetIsOwnerAiming() const
{
	return COCharacter && COCharacter->GetIsAiming();
}

void ACOWeapon::RefillLoaderBlueprint(int32 ammo)
{
	RefillLoader(ammo);
}

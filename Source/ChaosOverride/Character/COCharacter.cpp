// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COCharacter.h"
#include "ChaosOverrideGameMode.h"
#include "Gameplay/COPlayerController.h"
#include "Gameplay/COAIController.h"
#include "Gameplay/COPlayerState.h"
#include "Character/COCharacterMovementComponent.h"
#include "Component/COSpringArmComponent.h"
#include "Character/COInventoryComponent.h"
#include "Weapon/COWeapon.h"
#include "Weapon/MeleeWeapon/COMeleeWeapon.h"
#include "Weapon/Projectile/DamageType/CODamageType.h"
#include "Weapon/Projectile/DamageType/DevastatorDamageType.h"



// Sets default values
ACOCharacter::ACOCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCOCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*------------------------*/
	/*-----CAMERA SECTION-----*/
	/*------------------------*/

	//On crée le Spring Arm et on l'attache à la capsule
	CameraBoom = ObjectInitializer.CreateDefaultSubobject<UCOSpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachParent = this->GetCapsuleComponent();

	//On déclare les paramètres par défault
	CameraBoom->bUsePawnControlRotation = true;
	//Hauteur par défault
	CameraBoom->SetRelativeLocation(FVector(0, 0, 100), false);
	//Longueur par défault
	CameraBoom->TargetArmLength = 400;

	/*Camera Lag*/
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraLagSpeed = 10;
	CameraBoom->CameraRotationLagSpeed = 15;

	bUseControllerRotationYaw = false;

	/*Camera init*/
	// On crée la caméra et on l'attache au spring arm
	FollowCamera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	FollowCamera->AttachParent = CameraBoom;

	/*------------------------*/
	/*----MOUVEMENT SECTION---*/
	/*------------------------*/

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->GravityScale = 4.5;
		MoveComp->JumpZVelocity = 1500;
		MoveComp->AirControl = 1;
		MoveComp->MaxWalkSpeed = 900;
	}

	/*------------------------*/
	/*---INVENTORY SECTION----*/
	/*------------------------*/

	Inventory = ObjectInitializer.CreateDefaultSubobject<UCOInventoryComponent>(this, TEXT("Inventory"));
	Inventory->CharacterOwner = this;

}

// Called when the game starts or when spawned
void ACOCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACOCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Inventory->DestroyInventory(true);
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ACOCharacter::Tick( float DeltaTime )
{
	//CameraBoom->CustomCameraLag(DeltaTime);

	Super::Tick( DeltaTime );
	/*We fill in the target array*/
	CheckIfAiming();
}

// Called to bind functionality to input
void ACOCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void ACOCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//JUMP
	DOREPLIFETIME(ACOCharacter, bIsDoubleJumping);
	DOREPLIFETIME(ACOCharacter, bJumpPressed);

	//GLIDE
	DOREPLIFETIME(ACOCharacter, bIsGliding);

	//HOVERBOOTS
	DOREPLIFETIME(ACOCharacter, bAreHoverbootsActive);
	DOREPLIFETIME(ACOCharacter, HoverbootsInputValue);

	//DASH
	DOREPLIFETIME(ACOCharacter, bIsDashing);

	//JETPACK
	DOREPLIFETIME(ACOCharacter, bIsJetPackActive);
	DOREPLIFETIME(ACOCharacter, JetPackInputValue);

	//AIMING
	DOREPLIFETIME(ACOCharacter, bIsAiming);
	
	//WEAPON
	//DOREPLIFETIME(ACOCharacter, CurrenlyInHandWeapon);
	//DOREPLIFETIME(ACOCharacter, CurrentWeapon);
	//DOREPLIFETIME(ACOCharacter, MeleeWeapon);
}

ACOPlayerController * ACOCharacter::GetCOController() const
{
	return COPlayerController;
}

void ACOCharacter::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);
	/*Called only on server*/

	/*Set up default value for the player controller & the player state*/
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("PossessedBy called for %s by %s"), *GetName(), *NewController->GetName())); //test
	COPlayerController = Cast<ACOPlayerController>(Controller);
	//if(COPlayerController)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("BamBamBam"))); //test
	COPlayerState = Cast<ACOPlayerState>(PlayerState);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Player State is: %s for %s"), PlayerState? *PlayerState->GetName() : *FString("null"), *GetName())); //test
	if (COPlayerState)
	{
		/*We create all the default inventory that are stored in the player state*/
		Inventory->CreateDefaultWeapon();
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning : The player state for %s isn't valid, we can't spawn the default weapon"), *GetName())); //test
	}
	if (!COPlayerController)
	{
		/*We are not controlled by a COPlayerController, maybe an AI?*/
		COAIController = Cast<ACOAIController>(NewController);
		if (COAIController)
		{
			/*We change the walk speed because an AI is always slower than a character*/
			//GetCharacterMovement()->MaxWalkSpeed = AIMaxRunningSpeed;
			//Actually everything is directly done in the movement compoenent
		}
	}
	

}

void ACOCharacter::UnPossessed()
{
	//if (COPlayerController)GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("UNPOSSESSEDCALLED"))); //test

	Super::UnPossessed();
	COPlayerController = nullptr;
	COPlayerState = nullptr;
	// TODO DeleteAllWeapon; ?
}

void ACOCharacter::MoveForward(float Value)
{
	SetMoveForwardValue(Value);
	// find out which way is forward
	if ((Controller != NULL) && (Value != 0.0f))
	{
		FRotator Rotation = Controller->GetControlRotation();
		// Limit pitch when walking or falling
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			Rotation.Pitch = 0.0f;
		}
		// add movement in that direction
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}

}

void ACOCharacter::MoveRight(float Value)
{
	SetMoveRightValue(Value);
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ACOCharacter::Turn(float Value)
{
	if (Controller != NULL && Value != 0.0f)
	{
		if (bIsTurnRateReverse)
		{
			AddControllerYawInput((-1.0f)*Value);
		}
		else
		{
			AddControllerYawInput(Value);
		}
	}
}

void ACOCharacter::LookUp(float Value)
{
	if (Controller != NULL && Value != 0.0f)
	{
		if (bIsLookUpRateReverse)
		{
			AddControllerPitchInput(Value);
		}
		else
		{
			AddControllerPitchInput((-1.0f)*Value);
		}
	}
}

void ACOCharacter::TurnRate(float Value)
{
	if (Controller != NULL && Value != 0.0f)
	{
		if (bIsTurnRateReverse)
		{
			AddControllerYawInput(Value*UGameplayStatics::GetWorldDeltaSeconds(this)*BaseTurnRate*(-1.0f));
		}
		else
		{
			AddControllerYawInput(Value*UGameplayStatics::GetWorldDeltaSeconds(this)*BaseTurnRate);
		}
	}
}

void ACOCharacter::LookUpRate(float Value)
{
	if (Controller != NULL && Value != 0.0f)
	{
		if (bIsLookUpRateReverse)
		{
			AddControllerPitchInput(Value*UGameplayStatics::GetWorldDeltaSeconds(this)*BaseLookUpRate*(-1.0f));
		}
		else
		{
			AddControllerPitchInput(Value*UGameplayStatics::GetWorldDeltaSeconds(this)*BaseLookUpRate);
		}
	}
}

void ACOCharacter::HandleJump()
{
	if (bPrintJumpDebugInfo) 
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Handle Jump Called On Server")));

	if (GetAreHoverbootsActive() && NumberOfTimeHandleJumpCalledSinceLastLanded == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("hoverboot jump called")));
		DoubleJump(); //When we are with the hoverboots
		NumberOfTimeHandleJumpCalledSinceLastLanded=2;//To glide if never we are falling down
		return;
	}

	//Incrémentation du nombre de fois ou le bouton sauter a été pressé
	if (NumberOfTimeHandleJumpCalledSinceLastLanded > 0)
	{
		++NumberOfTimeHandleJumpCalledSinceLastLanded;
	}
	else
	{
		//Si on est déjà entrain de sauter, par exemple si on a été projeté vers le haut lors d'un coup, sur un trampoline etc... On veut direct faire un double saut
		//Peut être faudra t il ajouter bCanJump?
		if (GetVelocity().Z > 100 || GetVelocity().Z < -2000) NumberOfTimeHandleJumpCalledSinceLastLanded = 2;
	}
	/* La première fois que l'on appuie sur le bouton */
	if (GetCanJump() && NumberOfTimeHandleJumpCalledSinceLastLanded == 0)
	{
		if (bPrintJumpDebugInfo) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("JUMP called on server"))); }		
		++NumberOfTimeHandleJumpCalledSinceLastLanded;//On incrémente la première fois dans cet condition pour ne pas incrémenter s'il ne peut pas sauter
		Jump();
		return;
	}
	/* On gère le double saut ici */
	if ((!bIsDoubleJumping) && (NumberOfTimeHandleJumpCalledSinceLastLanded == 2))
	{
		if (bPrintJumpDebugInfo) { GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("DOUBLE JUMP"))); }

		//On delay le double saut si le perso n'est pas assez haut dans son saut
		if (GetVelocity().Z > 200)
		{
				/*
				Pour le calcul du delay c'est très simple, il s'agit d'un produit en croix principalement fait avec la fonction Lerp(linear interpolation blabla)
				On delay le temps d'une valeur entre 0 et 0.2 s qui est proportionnel à la velocity au moment du test
				Il faut une valeur entre 0 et 1 pour la valeur alpha du lerp. On prends donc la velocité suivant Z que l'on clamp entre 200 et la valeur du double saut que l'on divise par la plus grande
				valeur possible(ici le double saut). Et bam ça fait des chocapics
				*/
			GetWorldTimerManager().SetTimer(DoubleJumpTimer, this, &ACOCharacter::DoubleJump, FMath::Lerp(0.0f, 0.2f, FMath::Clamp(GetVelocity().Z, 200.0f, DoubleJumpZVelocity) / DoubleJumpZVelocity), true);
			return;
		}
		else
		{
			DoubleJump();
			return;
		}
	}
	ServerGlide();
}

void ACOCharacter::Jump()
{
	if (GetCharacterMovement()) this->LaunchCharacter(FVector(0, 0, GetCharacterMovement()->JumpZVelocity), false, true);
}

void ACOCharacter::DoubleJump()
{
	bAllowToGlide = false;
	FVector LaunchVector = FVector(0, 0, DoubleJumpZVelocity);
	if (bAreHoverbootsActive) LaunchVector += GetActorForwardVector()*FMath::Lerp(0.0f, JumpForwardPower, FVector(MoveForwardValue, MoveRightValue, 0).Size());
	LaunchCharacter(LaunchVector, false, true);
	bIsDoubleJumping = true;

	//Arrêt du timer
	GetWorldTimerManager().ClearTimer(DoubleJumpTimer);
}

void ACOCharacter::SetJumpPressed(bool Value)
{
	bJumpPressed = Value;
}

bool ACOCharacter::GetJumpPressed() const
{
	return bJumpPressed;
}

bool ACOCharacter::GetIsDoubleJumping() const
{	
	return bIsDoubleJumping;
}

bool ACOCharacter::GetCanJump() const
{
	/* On ne peut pas sauter quand
	* On est entrain de dasher !bIsDashing
	* On est entrain de se prendre des dégâts !bIsTakingDamage
	* On est entrain de tomber !GetCharacterMovement->IsFalling()
	*
	*/
	return !GetIsDashing() && /*!bIsTakingDamages && */!GetCharacterMovement()->IsFalling();
}

void ACOCharacter::Landed(const FHitResult & Hit)
{
	/*Called on client and on server*/
	Super::Landed(Hit);
	if (Role == ROLE_Authority)
	{
		ResetJump();
		ServerStopGliding();
		bAllowToDash = true;//Reallow to dash 
	}
}

void ACOCharacter::ResetJump()
{
	if(Role == ROLE_Authority)
	if (bPrintJumpDebugInfo) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("RESET JUMP EVENT CALLED on Server")));

	//Reset des valeurs
	bIsDoubleJumping = false;
	//On réinit le nombre de fois ou le bouton sauter à été pressé
	NumberOfTimeHandleJumpCalledSinceLastLanded = 0;
}

bool ACOCharacter::IsFalling() const
{
	if (GetCharacterMovement())
	{
		return bIsDashing ? true : GetCharacterMovement()->IsFalling();
	}
	else
	{
		return false;
	}
}

bool ACOCharacter::IsNearGround() const
{
	/* We trace from the center of the character to the ground 
	 * I will maybe need to change the axis if a custom gravity system is implemented
	 * TODO
	 */

	FHitResult OutHit;
	const FVector Start = GetActorLocation();
	const FVector End = GetActorLocation() - FVector(0, 0, NearDistanceOfGround);
	FCollisionQueryParams Params;
	FCollisionResponseParams ResponseParam;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("bool : %d"), GetWorld() && GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, Params) && GetCharacterMovement()->Velocity.Z <= 0 && IsFalling()));

	/*If the world is valid, we hit something and we are falling down*/
	return GetWorld() && GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, Params) && GetCharacterMovement()->Velocity.Z<=0 && IsFalling();
}

bool ACOCharacter::GetIsGliding() const
{
	return bIsGliding;
}

FVector ACOCharacter::GetGlideVector() const
{
	return FVector(0, 0, -GlideZVelocity);
}

void ACOCharacter::HandleHoverbootsInput(float Value)
{
	/*We activate the hoverboots if the value is different of 0, maybe change if we change the deadzone from the controller, somethiong like <0.25 maybe?*/
	if (Value != 0)
	{
		HoverbootsInputValue = Value; /*For the character movement*/
		/*We activate the hoverboots*/
		if (!bAreHoverbootsActive)
			ActivateHoverboots();
	}
	else
	{
		if (bAreHoverbootsActive)
			DeactivateHoverboots();
	}
	/*if (Role == ROLE_Authority)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("We are on server : %f"), MinMultiplierHoverbootsCoeff));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("We are on client : %f"), MinMultiplierHoverbootsCoeff));
	}*/
}

void ACOCharacter::ActivateHoverboots()
{
	if (GetCharacterMovement())
	{
		bAreHoverbootsActive = true;	//We activate the hoverboots, replicate to client for the anim instance
		GetCharacterMovement()->GroundFriction = HoverbootsGroundFriction;		//We modify physics but we stay in walk mode
		GetCharacterMovement()->MaxAcceleration = HoverbootsMaxAcceleration;
		ClientActualiseHoverbootsMovement(true);
	}
}

void ACOCharacter::DeactivateHoverboots()
{
	if (GetCharacterMovement())
	{
		bAreHoverbootsActive = false;
		GetCharacterMovement()->GroundFriction = GetClass()->GetDefaultObject<ACOCharacter>()->GetCharacterMovement()->GroundFriction;
		GetCharacterMovement()->MaxAcceleration = GetClass()->GetDefaultObject<ACOCharacter>()->GetCharacterMovement()->MaxAcceleration;
		ClientActualiseHoverbootsMovement(false);
	}
}

void ACOCharacter::ClientActualiseHoverbootsMovement_Implementation(bool bActivate)
{
	if (GetCharacterMovement())
	{
		if (bActivate)
		{
			GetCharacterMovement()->GroundFriction = HoverbootsGroundFriction;		//We modify physics but we stay in walk mode
			GetCharacterMovement()->MaxAcceleration = HoverbootsMaxAcceleration;
		}
		else
		{
			GetCharacterMovement()->GroundFriction = GetClass()->GetDefaultObject<ACOCharacter>()->GetCharacterMovement()->GroundFriction;
			GetCharacterMovement()->MaxAcceleration = GetClass()->GetDefaultObject<ACOCharacter>()->GetCharacterMovement()->MaxAcceleration;
		}
	}
}

bool ACOCharacter::GetAreHoverbootsActive() const
{
	return bAreHoverbootsActive;
}

float ACOCharacter::GetMinMultiplierHoverbootsCoeff() const
{
	return MinMultiplierHoverbootsCoeff;
}

float ACOCharacter::GetMaxMultiplierHoverbootsCoeff() const
{
	return MaxMultiplierHoverbootsCoeff;
}

float ACOCharacter::GetHoverbootsInputValue() const
{
	return HoverbootsInputValue;
}

bool ACOCharacter::CanDash() const
{
	return !bIsDashing && bAllowToDash;
}

void ACOCharacter::HandleDash()
{
	if (GetCharacterMovement())
	{
		if (CanDash())
		{
			bIsDashing = true;//Inform the anim instance and the character movement that we are dashing
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);//On se met en mode fly pour ne pas être soumis à la gravité
			GetCharacterMovement()->bOrientRotationToMovement = false;//When we are aiming
			ClientReplicateOrientationToMovement(false);
			bAllowToDash = false; /*Not allowed to dash*/

			GetWorldTimerManager().SetTimer(DashTimer, this, &ACOCharacter::StopDash, DashTime, true);//Delay the reinit of the dash
		}
	}
}

void ACOCharacter::StopDash()
{
	//NEED TO ADD SOMETHING WHEN WE DASH IN THE AIR TODO
	if (GetCharacterMovement())
	{
		GetWorldTimerManager().ClearTimer(DashTimer);
		bIsDashing = false; //Stop to dash
		GetCharacterMovement()->SetMovementMode(MOVE_Walking); //Reset movement to walk to have gravity
		GetCharacterMovement()->bOrientRotationToMovement = true;//Reorient the rotation to the movement
		ClientReplicateOrientationToMovement(true);
		ResetJump();
		GetCharacterMovement()->Velocity *= 0.5f*FVector(1,1,0); //Reduce the velocity by half
		GetWorldTimerManager().SetTimer(DashEndTimer, this, &ACOCharacter::ReinitDash, TimeBetweenTwoDash, false);
	}
}

FVector ACOCharacter::GetDashDirection() const
{
	if (bIsAiming)
	{
		return FVector();
	}
	else
	{
		return GetActorForwardVector();
	}
}

float ACOCharacter::GetDashSpeed() const
{
	return DashSpeed;
}

bool ACOCharacter::GetIsDashing() const
{
	return bIsDashing;
}

void ACOCharacter::ReinitDash()
{
	if (!GetCharacterMovement()->IsFalling()) bAllowToDash = true;
	GetWorldTimerManager().ClearTimer(DashEndTimer);
}

void ACOCharacter::ClientReplicateOrientationToMovement_Implementation(bool newValue)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = newValue;
	}
}

bool ACOCharacter::GetIsAiming() const
{
	return bIsAiming;
}

void ACOCharacter::SetIsAiming(bool Value)
{
	bIsAiming = Value;
	if (CameraBoom)
		CameraBoom->UpdateCameraLocation();
}

FVector ACOCharacter::GetLocalSpringArmOffsetWhenAiming() const
{
	return LocalSpringArmOffsetWhenAiming;
}

FVector ACOCharacter::GetSpringArmTargetOffsetWhenAiming() const
{
	return SpringArmTargetOffsetWhenAiming;
}

float ACOCharacter::GetFOVWhenAiming() const
{
	return FOVWhenAiming;
}

float ACOCharacter::GetTargetArmLengthMultiplierWhenAiming() const
{
	return TargetArmLengthMultiplierWhenAiming;
}

void ACOCharacter::CheckIfAiming()
{
	if (GetIsAiming())
	{
		if (Inventory->CurrentWeapon)
		{
			Inventory->CurrentWeapon->CheckForTarget();
		}
		else
		{
			Inventory->EquipWeapon();
		}		
	}
}

bool ACOCharacter::HasATarget() const
{
	if (Inventory->CurrentWeapon)
	{
		return Inventory->CurrentWeapon->GetFindAtLeastOneTarget();
	}
	return false;
}

void ACOCharacter::HandleMeleeWeapon(bool bPunchInputPressed)
{
	//TODO
	if (bPunchInputPressed)
	{
		Inventory->EquipMeleeWeapon();
	}
	else
	{

	}
}

void ACOCharacter::OnMeleeWeaponEquip()
{
	Inventory->CurrenlyInHandWeapon = ECOWeapon::W_MeleeWeapon;
}

void ACOCharacter::OnUnEquipMeleeWeapon()
{

}

bool ACOCharacter::GetIsHoldingAOneHandWeapon() const
{
	return Inventory->GetIsHoldingAOneHandWeapon();
}

bool ACOCharacter::GetIsHoldingTwoOneHandWeapon() const
{
	return Inventory->GetIsHoldingTwoOneHandWeapon();
}

bool ACOCharacter::GetIsHoldingATwoHandWeapon() const
{
	return Inventory->GetIsHoldingATwoHandWeapon();
}

bool ACOCharacter::GetIsHoldingAMeleeWeapon() const
{
	return Inventory->GetIsHoldingAMeleeWeapon();
}

bool ACOCharacter::GetIsAllowedToFire() const
{
	//For now he is allowed to shoot everywhere, maybe i will need to add a variable for trigger volume where shooting isn't allowed, or if the character is targetting a friend
	return true;
}

void ACOCharacter::HandleFire(bool bFirePressed)
{
	if (GetIsAllowedToFire())
	{
		if (bFirePressed)
		{
			OnFirePressed();
		}
		else
		{
			OnFireReleased();
		}
	}
}

void ACOCharacter::OnFirePressed()
{
	/*When the fire input is pressed, we equip the current weapon*/
	if (Inventory->CurrenlyInHandWeapon != ECOWeapon::W_Weapon) Inventory->EquipWeapon();

	/*The weapon is currently in hand we can pressed it*/
	if (Inventory->CurrentWeapon)
	{
		Inventory->CurrentWeapon->PressTrigger();
	}
	else
	{
		//No weapon are equiped, this case should never occured
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error: %s have a weapon in his hand but the current weapon isn't valid"), *GetName()));
	}
}

void ACOCharacter::OnFireReleased()
{
	if (Inventory->CurrentWeapon)
	{
		Inventory->CurrentWeapon->ReleaseTrigger(); //TODO
	}
}

void ACOCharacter::HandleNextWeapon(bool bNextWeaponInputPressed)
{
	if (bNextWeaponInputPressed)
	{
		/*OnNextWeaponPressed*/
		OnNextWeaponPressed();
	}
	else
	{
		/*OnNextWeaponReleased*/
		OnNextWeaponReleased();
	}
}

void ACOCharacter::OnNextWeaponPressed()
{
	/*To limit in time the change of weapon*/	
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (Inventory->LastTimeWeaponChanged > 0.0f && Inventory->TimeBetweenTwoChangedWeapon > 0.0f && Inventory->LastTimeWeaponChanged + Inventory->TimeBetweenTwoChangedWeapon > GameTime)
	{
		GetWorldTimerManager().SetTimer(Inventory->HandleChangeWeaponTimer, Inventory, &UCOInventoryComponent::SetNextWeapon, Inventory->LastTimeWeaponChanged + Inventory->TimeBetweenTwoChangedWeapon - GameTime, false);
	}
	else
	{
		Inventory->SetNextWeapon();
	}
}

void ACOCharacter::OnNextWeaponReleased()
{
}

void ACOCharacter::HandlePrevWeapon(bool bPrevWeaponInputPressed)
{
	if (bPrevWeaponInputPressed)
	{
		OnPrevWeaponPressed();
	}
	else
	{
		OnPrevWeaponReleased();
	}
}

void ACOCharacter::OnPrevWeaponPressed()
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (Inventory->LastTimeWeaponChanged > 0.0f && Inventory->TimeBetweenTwoChangedWeapon > 0.0f && Inventory->LastTimeWeaponChanged + Inventory->TimeBetweenTwoChangedWeapon > GameTime)
	{
		GetWorldTimerManager().SetTimer(Inventory->HandleChangeWeaponTimer, Inventory, &UCOInventoryComponent::SetPreviousWeapon, Inventory->LastTimeWeaponChanged + Inventory->TimeBetweenTwoChangedWeapon - GameTime, false);
	}
	else
	{
		Inventory->SetPreviousWeapon();
	}
}

void ACOCharacter::OnPrevWeaponReleased()
{

}

void ACOCharacter::HandleJetPack(float Value)
{
	if ((JetPackInputValue = Value) == 0)
	{
		DeactivateJetPack();
	}
	else
	{
		if (CanActivateJetPack()) ActivateJetPack();
	}
}

void ACOCharacter::ActivateJetPack()
{
	if(!bIsJetPackActive)
	bIsJetPackActive = true;
}

void ACOCharacter::DeactivateJetPack()
{
	if (bIsJetPackActive)
	bIsJetPackActive = false;
	ServerGlide();
}

bool ACOCharacter::CanActivateJetPack() const
{
	/*Can't activate the jetpack if out of gaz, dashing,*/
	return true;
}

bool ACOCharacter::GetIsJetPackActive() const
{
	return bIsJetPackActive;
}

float ACOCharacter::GetJetPackInput() const
{
	return JetPackInputValue;
}

float ACOCharacter::GetJetPackMaxZVelocity() const
{
	return JetPackMaxZVelocity;
}

float ACOCharacter::GetJetPackMinZVelocity() const
{
	return JetPackMinZVelocity;
}

void ACOCharacter::ServerGlide_Implementation()
{	
	ACOPlayerController * COControler = GetCOController();
	if (COControler)
	{	
		if ((NumberOfTimeHandleJumpCalledSinceLastLanded >= 2) && (bIsDoubleJumping) && bAllowToGlide && (GetCharacterMovement()->Velocity.Z < 0) && COControler->bJumpPressed)
		{
			bIsGliding = true;
		}
	}
}
bool ACOCharacter::ServerGlide_Validate()
{
	return true;
}

void ACOCharacter::ServerStopGliding_Implementation()
{
	bIsGliding = false;
}
bool ACOCharacter::ServerStopGliding_Validate()
{
	return true;
}

void ACOCharacter::ServerAllowToGlide_Implementation()
{
	bAllowToGlide = true;
	ServerGlide();
}
bool ACOCharacter::ServerAllowToGlide_Validate()
{
	return true;
}

void ACOCharacter::SetMoveForwardValue_Implementation(float Value)
{
	MoveForwardValue = Value;
}
bool ACOCharacter::SetMoveForwardValue_Validate(float Value) { return true; }

void ACOCharacter::SetMoveRightValue_Implementation(float Value)
{
	MoveRightValue = Value;
}
bool ACOCharacter::SetMoveRightValue_Validate(float Value) { return true; }

void ACOCharacter::ResetInvincibleMomentumDamageTime()
{
	bIsInvincibleToMomentumDamage = false;
}

float ACOCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	if (Health <= 0.f)
	{
		return 0.f;
	}
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.0f)
	{
		ActualDamage = FMath::Clamp(ActualDamage, 0.f, Health);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("ActualDamage : %f"), ActualDamage)); //DEBUG
		Health -= ActualDamage;

		/*Handle Assist, EventInstigator!=GetInstigatorController() is here to avoid assist kill ourself if we inflict some damage to self*/
		if (EventInstigator && EventInstigator!=GetInstigatorController())
		{
			AssistArray.Add(EventInstigator);
		}

		/*Handle if we die or if we took a simple damage*/
		if (Health <= 0.f)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			APawn* Pawn = EventInstigator ? EventInstigator->GetPawn() : nullptr;
			PlayHit(ActualDamage, DamageEvent, EventInstigator, DamageCauser, false);
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Life : %f"), Health));

	return ActualDamage;
}

bool ACOCharacter::CanDie(float KillingDamage, FDamageEvent const & DamageEvent, AController * Killer, AActor * DamageCauser) const
{
	return !bPendingDying || bIsDead;
}

bool ACOCharacter::Die(float KillingDamage, FDamageEvent const & DamageEvent, AController * Killer, AActor * DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = 0.0f;

	OnDeath(KillingDamage, DamageEvent, Killer, DamageCauser);
	return false;
}

void ACOCharacter::OnDeath(float KillingDamage, FDamageEvent const & DamageEvent, AController * Killer, AActor * DamageCauser)
{
	if (bPendingDying)
	{
		return;
	}
	bPendingDying = true;

	ACOPlayerController * COKillerController = Cast<ACOPlayerController>(Killer);
	ACOAIController * COAIKillerController = Cast<ACOAIController>(Killer);

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("OnDeath")));

	/*Other controller stats*/

	/*Die stupidly by falling, suicide, etc... The last assist controller in assist array is the killer or if we kill ourself by a grenade etc...*/
	if ((Killer == nullptr || Killer != GetInstigatorController()) && AssistArray.Num() > 0)
	{
		COKillerController = Cast<ACOPlayerController>(AssistArray[AssistArray.Num() - 1]);
		COAIKillerController = Cast<ACOAIController>(AssistArray[AssistArray.Num() - 1]);
	}

	/*Inform all the HUD of a death*/
	if (GetWorld())
	{
		AGameMode * Gm = UGameplayStatics::GetGameMode(GetWorld());
		if (Gm)
		{
			AChaosOverrideGameMode * COGameMode = Cast<AChaosOverrideGameMode>(Gm);
			if (COGameMode)
				COGameMode->InformEveryHUDOfAKill(DamageEvent, Killer, Controller);
		}
	}

	/*Increment kill*/
	if (COKillerController)
	{
		COKillerController->IncrementKill(Controller);
		AssistArray.Remove(COKillerController);
	}
	else if (COAIKillerController)
	{
		COAIKillerController->IncrementKill(Controller);
		AssistArray.Remove(COAIKillerController);
		//DO ai stuff here
		COAIKillerController->OnTargetDeath();
	}

	ACOPlayerController * COAssistPlayerController = nullptr;
	ACOAIController * COAIAssistPlayerController = nullptr;

	while (AssistArray.Num() > 0)
	{
		AController * TempC = AssistArray[0];
		if (TempC)
		{
			COAssistPlayerController = Cast<ACOPlayerController>(TempC);
			COAIAssistPlayerController = Cast<ACOAIController>(TempC);

			AssistArray.Remove(TempC);

			if (COAssistPlayerController)
			{
				COAssistPlayerController->IncrementAssist(GetController());
			}
			else if (COAIAssistPlayerController)
			{
				COAIAssistPlayerController->IncrementAssist(COPlayerController);
				//Do AI stuff here
				COAIAssistPlayerController->OnTargetDeath();
			}

			COAssistPlayerController = nullptr;
			COAIAssistPlayerController = nullptr;
		}	
	}

	/*Self stats*/
	if (COPlayerController)
	{
		COPlayerController->RequestRespawn();
		COPlayerController->IncrementDeath(Killer);
	}
	else if (COAIController)
	{
		COAIController->RequestRespawn();
		COAIController->IncrementDeath(Killer);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning, The controller isn't a subclass of ACOPlayerController or ACOAIController, %s can't respawn"), *GetName()));
	}	


	PlayHit(KillingDamage, DamageEvent, Killer, DamageCauser, true);

	FVector impulse = GetActorForwardVector()*(-700) + FVector(0, 0, 300);
	//LaunchCharacter(impulse, true, true);

	Inventory->DestroyInventory(true);

	/*To replace with disable input and a delay*/
	DetachFromControllerPendingDestroy();
	
	/* Disable all collision on capsule */
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	/*Handle dissolving here*/
	USkeletalMeshComponent* Mesh = GetMesh();
	if (Mesh)
	{

		Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
		Mesh->SetAllBodiesSimulatePhysics(true);
		Mesh->SetSimulatePhysics(true);
		Mesh->WakeAllRigidBodies();
		Mesh->SetAllPhysicsLinearVelocity(impulse);
		Mesh->bBlendPhysics = true;
	}
	SetActorEnableCollision(true);

	SetLifeSpan(20.f);

	bIsDead = true;
}

void ACOCharacter::FellOutOfWorld(const UDamageType & DmgType)
{
	Die(Health, FDamageEvent(DmgType.GetClass()), nullptr, nullptr);
	Super::FellOutOfWorld(DmgType);

}

void ACOCharacter::PlayHit(float DamageTaken, FDamageEvent const & DamageEvent, AController * Killer, AActor * DamageCauser, bool bKilled)
{
	if (DamageTaken > 0.f)
	{
		APawn * PawnInstigator = Killer ? Killer->GetPawn() : nullptr;
		ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
	}
}

void ACOCharacter::ApplyDamageMomentum(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn * PawnInstigator, AActor* DamageCauser)
{
	/*To keep the control of the character when being shoot*/
	if (bIsInvincibleToMomentumDamage)
		return;

	if(DamageEvent.DamageTypeClass)
	{
		Super::ApplyDamageMomentum(DamageTaken, DamageEvent, PawnInstigator, DamageCauser);
		float const ImpulseScale = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>()->DamageImpulse;
		UCODamageType const * CODamageType = Cast<UCODamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());

		if (CODamageType)
		{
			/*The bullet that cause damage*/
			if (DamageCauser)
			{
				FVector BulletVelocity = DamageCauser->GetVelocity();
				BulletVelocity.Normalize();
				BulletVelocity.Z = 0;
				LaunchCharacter(BulletVelocity * ImpulseScale + FVector(0, 0, 500), true, true);
				SetActorRotation((BulletVelocity*-1.f).Rotation());
				bIsInvincibleToMomentumDamage = true;
				GetWorldTimerManager().SetTimer(ResetbIsInvincibleToMomentumDamageTimer, this, &ACOCharacter::ResetInvincibleMomentumDamageTime, InvincibleMomentumDamageTime, false);
			}
		}
	}
	else
	{
		if (DamageCauser)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning, Please add a default damage type for %s"), DamageCauser? *DamageCauser->GetName(): *FString("nullptr")));
		}	
	}
}

bool ACOCharacter::Heal(float HealthAmount, bool bRefill)
{
	if (Health >= MaxHealth) return false;

	if (HealthAmount > 0)
	{
		Health = FMath::Clamp(Health+HealthAmount, 0.f, MaxHealth);
		return true;
	}
	return false;
}

void ACOCharacter::HandleAim(bool bAimPressed)
{
	if(bAimPressed)
	{
		OnAimPressed();
	}
	else
	{
		OnAimReleased();
	}
}

void ACOCharacter::OnAimPressed()
{
	if(Inventory && Inventory->CurrentWeapon)
		Inventory->CurrentWeapon->CheckForTarget();

	SetIsAiming(true);

	bUseControllerRotationYaw = true;

	GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ACOCharacter::OnAimReleased()
{
	SetIsAiming(false);

	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

bool ACOCharacter::HealBlueprint(float HealthAmount, bool bRefill)
{
	return Heal(HealthAmount, bRefill);
}
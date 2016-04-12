// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COPlayerController.h"
#include "Gameplay/COPlayerState.h"
#include "Character/COCharacter.h"
#include "Gameplay/ProfileSaveGame.h"
#include "Character/COPlayerCameraManager.h"
#include "ChaosOverrideGameMode.h"

ACOPlayerController::ACOPlayerController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ACOPlayerCameraManager::StaticClass();
}

/*Called when spawned in the world*/
void ACOPlayerController::BeginPlay()
{
	Super::BeginPlay();
	/*Only the player controller on the server have a player state that is replicate to every client. For the player controller on client the player state is NULL*/
	if (Role == ROLE_Authority)
	{
		if (GetCOPlayerState() == nullptr)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: Unable to get the playerState for : %s in the begin play on SERVER"), *GetName()));
		}
	}
	
	//TEST TO DISCOVER THE TRUTH
	/*if ((COPlayerState = Cast<ACOPlayerState>(PlayerState)) == nullptr)
	{
		if(Role == ROLE_Authority)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: Unable to get the playerState for : %s in the begin play on SERVER"), *GetName()));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: Unable to get the playerState for : %s in the begin play on CLIENT"), *GetName()));
		}
	}
	if (PlayerState)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("For %s the player state is %s"), *GetName(), *PlayerState->GetName()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("For %s the player state is null"), *GetName()));
	}*/
}

// Called to bind functionality to input
void ACOPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//AXIS MAPPING
	InputComponent->BindAxis("MoveForward", this, &ACOPlayerController::MoveForward);

	InputComponent->BindAxis("MoveRight", this, &ACOPlayerController::MoveRight);

	InputComponent->BindAxis("Turn", this, &ACOPlayerController::Turn);

	InputComponent->BindAxis("LookUp", this, &ACOPlayerController::LookUp);

	InputComponent->BindAxis("TurnRate", this, &ACOPlayerController::TurnRate);

	InputComponent->BindAxis("LookUpRate", this, &ACOPlayerController::LookUpRate);

	InputComponent->BindAxis("Hoverboots", this, &ACOPlayerController::Hoverboots);

	InputComponent->BindAxis("JetPack", this, &ACOPlayerController::Jetpack);

	//ACTION MAPPING
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACOPlayerController::JumpOnPressed);
	InputComponent->BindAction("Jump", IE_Released, this, &ACOPlayerController::JumpOnReleased);

	InputComponent->BindAction("Punch", IE_Pressed, this, &ACOPlayerController::PunchOnPressed);
	InputComponent->BindAction("Punch", IE_Released, this, &ACOPlayerController::PunchOnReleased);

	InputComponent->BindAction("Interaction", IE_Pressed, this, &ACOPlayerController::InteractionOnPressed);
	InputComponent->BindAction("Interaction", IE_Released, this, &ACOPlayerController::InteractionOnReleased);

	InputComponent->BindAction("WeaponWheel", IE_Pressed, this, &ACOPlayerController::WeaponWheelOnPressed);
	InputComponent->BindAction("WeaponWheel", IE_Released, this, &ACOPlayerController::WeaponWheelOnReleased);

	InputComponent->BindAction("Aim", IE_Pressed, this, &ACOPlayerController::AimOnPressed);
	InputComponent->BindAction("Aim", IE_Released, this, &ACOPlayerController::AimOnReleased);

	InputComponent->BindAction("Start", IE_Pressed, this, &ACOPlayerController::StartOnPressed);
	InputComponent->BindAction("Start", IE_Released, this, &ACOPlayerController::StartOnReleased);

	InputComponent->BindAction("Select", IE_Pressed, this, &ACOPlayerController::SelectOnPressed);
	InputComponent->BindAction("Select", IE_Released, this, &ACOPlayerController::SelectOnReleased);

	InputComponent->BindAction("Fire", IE_Pressed, this, &ACOPlayerController::FireOnPressed);
	InputComponent->BindAction("Fire", IE_Released, this, &ACOPlayerController::FireOnReleased);

	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &ACOPlayerController::PrevWeaponOnPressed);
	InputComponent->BindAction("PrevWeapon", IE_Released, this, &ACOPlayerController::PrevWeaponOnReleased);

	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &ACOPlayerController::NextWeaponOnPressed);
	InputComponent->BindAction("NextWeapon", IE_Released, this, &ACOPlayerController::NextWeaponOnReleased);
}

void ACOPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ACOPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Player state in controller %s"), *PlayerState->GetName()));
	COPlayerState = Cast<ACOPlayerState>(PlayerState);
}

void ACOPlayerController::CleanupPlayerState()
{
	Super::CleanupPlayerState();
	COPlayerState = nullptr;
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------SAVE SYSTEM------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

bool ACOPlayerController::SetProfileToController(UProfileSaveGame* newProfile)
{
	if (newProfile == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Error: Tried to assign a null ProfileSaveGame to the controller"));
		return false;
	}
	/*We set the profile as the new profile*/
	Profile = newProfile;
	return LoadFromProfile();
}

bool ACOPlayerController::SaveProfile()
{
	/*For debug purpose, we need to know which local player we are currently working on*/
	ULocalPlayer*  LocalPlayer = GetLocalPlayer();
	/*Check if is valid*/
	if (LocalPlayer)
	{
		/*Check if the profile is valid*/
		if (Profile)
		{
			/*Get the player State where all the info is stocked*/
			/*Check if the player state is valid*/
			ACOPlayerState * COPlayerState =GetCOPlayerState();
			if (COPlayerState)
			{
				/*Copy all the needed info from the player state in the profile*/
				if (COPlayerState->SaveInProfile(Profile))
				{
					/*Save the profile*/
					if (UGameplayStatics::SaveGameToSlot(Profile, Profile->Pseudo, 0))
					{
						GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, FString::Printf(TEXT("Game sucessfully save to slot %s for controller ID %d"), *Profile->Pseudo, LocalPlayer->GetControllerId()));
						return true;
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: When trying to save game to slot %s for controller ID %d"), *Profile->Pseudo, LocalPlayer->GetControllerId()));
					}

				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: When trying to copy the information contain in the player state into the profile for controller ID %d"), LocalPlayer->GetControllerId()));
				}
				
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: The player state isn't valid for local controller ID %d"), LocalPlayer->GetControllerId()));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: Failed to save the profile of local player number %d because the profile isn't valid, mostly occured because it had not been initialized yet"), LocalPlayer->GetControllerId()));
		}	
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: Unable to get the ULocalPlayer for the controller when trying to save the profile")));
	}

	return false;
}

bool ACOPlayerController::LoadFromProfile()
{
	/*For debug purpose, we need to know which local player we are currently working on*/
	ULocalPlayer*  LocalPlayer = GetLocalPlayer();
	/*Check if is valid*/
	if (LocalPlayer)
	{
		/*Check if the profile is valid*/
		if (Profile)
		{
			/*Get the player State where all the info is stocked*/
			/*Check if the player state is valid*/
			if (COPlayerState)
			{
				/*Copy all the needed info from the player state in the profile*/
				if (COPlayerState->LoadFromProfile(Profile))
				{
					return true;
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: When trying to copy the information contain in the player state into the profile for controller ID %d"), LocalPlayer->GetControllerId()));
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: The player state isn't valid for local controller ID %d"), LocalPlayer->GetControllerId()));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: Failed to save the profile of local player number %d because the profile isn't valid, mostly occured because it had not been initialized yet"), LocalPlayer->GetControllerId()));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: Unable to get the ULocalPlayer for the controller when trying to save the profile")));
	}

	return false;
}

void ACOPlayerController::GameHasEnded(AActor * EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);
	SaveProfile();
}

/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------INPUT SYSTEM-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

void ACOPlayerController::MoveForward(float Value)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), Value));
	/*Check if the CO is valid, if we are on server and if move input are authorize*/
	ACOCharacter * PossessedPawn = GetCOCharacter();
	if (PossessedPawn && CanMove() && Value !=0)
	{
		PossessedPawn->MoveForward(Value);
	}
}

void ACOPlayerController::MoveRight(float Value)
{
	/*Check if the CO is valid, if we are on server and if move input are authorize*/
	ACOCharacter * PossessedPawn = GetCOCharacter();
	if (PossessedPawn && CanMove() && Value != 0)
	{
		PossessedPawn->MoveRight(Value);
	}
}

void ACOPlayerController::Turn(float Value)
{
	/*Check if the CO is valid, and if look around is authorize*/
	ACOCharacter * PossessedPawn = GetCOCharacter();
	if (PossessedPawn && CanLookAround() && Value != 0)
	{
		PossessedPawn->Turn(Value);
	}
}

void ACOPlayerController::TurnRate(float Value)
{
	/*Check if the CO is valid, and if look around is authorize*/
	ACOCharacter * PossessedPawn = GetCOCharacter();
	if (PossessedPawn && CanLookAround() && Value != 0)
	{
		PossessedPawn->TurnRate(Value);
	}
}

void ACOPlayerController::LookUp(float Value)
{
	/*Check if the CO is valid, and if look around is authorize*/
	ACOCharacter * PossessedPawn = GetCOCharacter();
	if (PossessedPawn && CanLookAround() && Value != 0)
	{
		PossessedPawn->LookUp(Value);
	}
}

void ACOPlayerController::LookUpRate(float Value)
{
	/*Check if the CO is valid, and if look around is authorize*/
	ACOCharacter * PossessedPawn = GetCOCharacter();
	if (PossessedPawn && CanLookAround() && Value != 0)
	{
		PossessedPawn->LookUpRate(Value);
	}
}

void ACOPlayerController::Hoverboots_Implementation(float Value)
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
	{
		/*Hoverboot standard input, ie acceleration*/
		if(CanActivateHoverboots())COCharacter->HandleHoverbootsInput(Value);
		if (Value >= 0.25)
		{
			if (bFirstTimeDashButtonIsPressed)
			{
				if (bDashButtonHasBeenReleased)
				{
					COCharacter->HandleDash();//Dash
					bDashLaunched = true;
				}
			}
			else
			{
				bFirstTimeDashButtonIsPressed = true;
				GetWorldTimerManager().SetTimer(TimerDashAbort, this, &ACOPlayerController::AbortDash, TimeElapseBetweenTwoPressedOfHoverbootsInput, true);//Abort dash after x seconds
			}
		}
		else
		{

			if (bFirstTimeDashButtonIsPressed && !bDashButtonHasBeenReleased && !bDashLaunched && !bDashAbort)
			{
				GetWorldTimerManager().ClearTimer(TimerDashAbort);//Cancel the dash abort
				bDashButtonHasBeenReleased = true;
				GetWorldTimerManager().SetTimer(TimerDashAbort, this, &ACOPlayerController::AbortDash, TimeElapseBetweenTwoPressedOfHoverbootsInput, true); //Delay another abortion if it is not repressed after x seconds
			}
			if (bDashLaunched || bDashAbort)
			{
				AbortDash();//Reset du dash (abort)
				bDashAbort = false;
			}
		}
	}
}
bool ACOPlayerController::Hoverboots_Validate(float Value) { return true; }

void ACOPlayerController::Jetpack_Implementation(float Value)
{
	if (CanUseJetPack())
	{
		ACOCharacter * COCharacter = GetCOCharacter();
		if (COCharacter)
		{
			COCharacter->HandleJetPack(Value);
		}
	}
}
bool ACOPlayerController::Jetpack_Validate(float Value) { return true; }

void ACOPlayerController::JumpOnPressed_Implementation()
{
	SetJumpPressed(true);
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter && CanJump())
	{
		COCharacter->HandleJump();
	}
}
bool ACOPlayerController::JumpOnPressed_Validate() { return true; }
void ACOPlayerController::JumpOnReleased_Implementation()
{
	SetJumpPressed(false);
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
	{
		COCharacter->ServerStopGliding();
	}
}
bool ACOPlayerController::JumpOnReleased_Validate() { return true; }

void ACOPlayerController::SetJumpPressed(bool Value)
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
		COCharacter->SetJumpPressed(Value);
	bJumpPressed = Value;
}
bool ACOPlayerController::GetJumpPressed() const
{
	return bJumpPressed;
}

void ACOPlayerController::PunchOnPressed_Implementation()
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
	{
		COCharacter->HandleMeleeWeapon(true);
	}
}
bool ACOPlayerController::PunchOnPressed_Validate() { return true; }
void ACOPlayerController::PunchOnReleased_Implementation()
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
	{
		COCharacter->HandleMeleeWeapon(false);
	}
}
bool ACOPlayerController::PunchOnReleased_Validate() { return true; }

void ACOPlayerController::InteractionOnPressed()
{

}
void ACOPlayerController::InteractionOnReleased()
{

}

void ACOPlayerController::WeaponWheelOnPressed()
{

}
void ACOPlayerController::WeaponWheelOnReleased()
{

}

void ACOPlayerController::AimOnPressed_Implementation()
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
		COCharacter->HandleAim(true);
	
}
bool ACOPlayerController::AimOnPressed_Validate() { return true; }
void ACOPlayerController::AimOnReleased_Implementation()
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
		COCharacter->HandleAim(false);
}
bool ACOPlayerController::AimOnReleased_Validate() { return true; }

void ACOPlayerController::FireOnPressed_Implementation()
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
	{
		COCharacter->HandleFire(true);
	}
}
bool ACOPlayerController::FireOnPressed_Validate() { return true; }
void ACOPlayerController::FireOnReleased_Implementation()
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
	{
		COCharacter->HandleFire(false);
	}
}
bool ACOPlayerController::FireOnReleased_Validate() { return true; }

void ACOPlayerController::StartOnPressed()
{

}
void ACOPlayerController::StartOnReleased()
{

}

void ACOPlayerController::SelectOnPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Select pressed")));
	//GetWorld()->SeamlessTravel("Map1");
	//ServerTravel("Map1");
}
void ACOPlayerController::SelectOnReleased()
{

}

void ACOPlayerController::PrevWeaponOnPressed_Implementation()
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
	{
		COCharacter->HandlePrevWeapon(true);
	}
}
bool ACOPlayerController::PrevWeaponOnPressed_Validate() { return true; }
void ACOPlayerController::PrevWeaponOnReleased_Implementation()
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
	{
		COCharacter->HandlePrevWeapon(false);
	}
}
bool ACOPlayerController::PrevWeaponOnReleased_Validate() { return true; }

void ACOPlayerController::NextWeaponOnPressed_Implementation()
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
	{
		COCharacter->HandleNextWeapon(true);
	}
}
bool ACOPlayerController::NextWeaponOnPressed_Validate()
{
	return true;
}
void ACOPlayerController::NextWeaponOnReleased_Implementation()
{
	ACOCharacter * COCharacter = GetCOCharacter();
	if (COCharacter)
	{
		COCharacter->HandleNextWeapon(false);
	}
}
bool ACOPlayerController::NextWeaponOnReleased_Validate() { return true; }

ACOCharacter * ACOPlayerController::GetCOCharacter() const
{
	return Cast<ACOCharacter>(GetCharacter());
}

ACOPlayerState * ACOPlayerController::GetCOPlayerState() const
{
	return COPlayerState;
}

bool ACOPlayerController::CanMove() const
{
	return true;
}

bool ACOPlayerController::CanLookAround() const
{
	return true;
}

bool ACOPlayerController::CanJump() const
{
	return true;
}

bool ACOPlayerController::CanActivateHoverboots() const
{
	return true;
}

bool ACOPlayerController::CanDash() const
{
	return true;
}

void ACOPlayerController::ReinitHoverbootDoublePressedTimer()
{
	/*Reinit the bool bHoverbootInputReleaseSinceLastTimePressed to false and clear the timer*/
	GetWorldTimerManager().ClearTimer(HoverbootsDoublePressedTimer);
	bDashAbort = bDashLaunched = bHoverbootInputReleaseSinceLastTimePressed = bHoverboootInputPressed = false;
}

void ACOPlayerController::AbortDash()
{
	bDashAbort = true;
	bDashLaunched = bFirstTimeDashButtonIsPressed = bDashButtonHasBeenReleased = false;
	GetWorldTimerManager().ClearTimer(TimerDashAbort);
}

bool ACOPlayerController::CanUseJetPack() const
{
	return true;
}

void ACOPlayerController::RequestRespawn()
{
	AChaosOverrideGameMode * COGameMode = Cast<AChaosOverrideGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (COGameMode)
	{
		FTimerDelegate OnDestroyWeaponDelegate = FTimerDelegate::CreateUObject(COGameMode, &AChaosOverrideGameMode::HandleRequestRespawn, Cast<AController>(this));
		GetWorldTimerManager().SetTimer(HandleRespawnTimer, OnDestroyWeaponDelegate, COGameMode->RespawnTime, false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning, The game mode isn't a subclass of ACOGameMode, %s pawn can't respawn"), *GetName()));
	}
}

void ACOPlayerController::IncrementDeath(AController * Killer)
{
	AChaosOverrideGameMode * COGameMode = Cast<AChaosOverrideGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	/*We increment the player state death*/
	if (GetCOPlayerState())
	{
		++GetCOPlayerState()->Deaths;
		++GetCOPlayerState()->MatchDeaths;
		if (COGameMode)
		{
			GetCOPlayerState()->Score += COGameMode->DeathScore;
			GetCOPlayerState()->Experience += COGameMode->DeathExperience;
		}
		/*We keep a reference to everyone that killed us*/
		if (Killer)
		{
			GetCOPlayerState()->KilledByPlayer.Add(Killer->PlayerState);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning: The player state isn't ACOPlayerState for %s"), *GetName()));
	}
}

void ACOPlayerController::IncrementKill(AController * KilledController)
{
	AChaosOverrideGameMode * COGameMode = Cast<AChaosOverrideGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	/*We increment the player state kills*/
	if (GetCOPlayerState())
	{
		++GetCOPlayerState()->Kills;
		++GetCOPlayerState()->MatchKills;
		if (COGameMode)
		{
			GetCOPlayerState()->Score += COGameMode->KillScore;
			GetCOPlayerState()->Experience += COGameMode->KillExperience;
		}
		/*We keep a reference to everyone that we killed*/
		if (KilledController)
		{
			GetCOPlayerState()->KilledPlayer.Add(KilledController->PlayerState);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning: The player state isn't ACOPlayerState for %s"), *GetName()));
	}
}

void ACOPlayerController::IncrementAssist(AController * KilledByAssistController)
{
	AChaosOverrideGameMode * COGameMode = Cast<AChaosOverrideGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	/*We increment the player state assist kills*/
	if (GetCOPlayerState())
	{
		++GetCOPlayerState()->Assists;
		++GetCOPlayerState()->MatchAssists;
		if (COGameMode)
		{
			GetCOPlayerState()->Score += COGameMode->AssistScore;
			GetCOPlayerState()->Experience += COGameMode->AssistExperience;
		}
		/*We keep a reference to everyone that assist on killing us*/
		if (KilledByAssistController)
		{
			GetCOPlayerState()->AssistKilledPlayer.Add(KilledByAssistController->PlayerState);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning: The player state isn't ACOPlayerState for %s"), *GetName()));
	}
}

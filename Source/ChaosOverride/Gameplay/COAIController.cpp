// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COAIController.h"
#include "Character/COCharacter.h"
#include "ChaosOverrideGameMode.h"
#include "Gameplay/COPlayerState.h"

ACOAIController::ACOAIController(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	bWantsPlayerState = true;
}

void ACOAIController::InitPlayerState()
{
	Super::InitPlayerState();
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Player state in controller %s"), *PlayerState->GetName()));
	COPlayerState = Cast<ACOPlayerState>(PlayerState);
}

void ACOAIController::ExecuteAICombatMode(AActor * Target)
{
	if (Target)
	{
		APawn * pawn = GetPawn();
		if (pawn)
		{
			ACOCharacter * COChar = Cast<ACOCharacter>(pawn);
			if (COChar)
			{
				COChar->AITargetActor = Target;
				TargetActor = Target;
				COChar->HandleAim(true);		
				COChar->HandleFire(true);
			}
		}
	}
}

void ACOAIController::FinishAICombatMode(AActor * Target)
{
	APawn * pawn = GetPawn();
	if (pawn)
	{
		ACOCharacter * COChar = Cast<ACOCharacter>(pawn);
		if (COChar)
		{
			COChar->AITargetActor = nullptr;
			TargetActor = nullptr;
			COChar->HandleAim(false);
			COChar->HandleFire(false);

		}
	}
}


void ACOAIController::RequestRespawn()
{
	FinishAICombatMode();
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

void ACOAIController::IncrementDeath(AController * Killer)
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

void ACOAIController::IncrementKill(AController * KilledController)
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

void ACOAIController::IncrementAssist(AController * KilledByAssistController)
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

ACOPlayerState * ACOAIController::GetCOPlayerState() const
{
	return COPlayerState;
}

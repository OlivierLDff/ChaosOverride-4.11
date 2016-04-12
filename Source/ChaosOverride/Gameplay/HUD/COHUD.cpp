// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COHUD.h"

void ACOHUD::HandleMatchIsWaitingToStart()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Init of the game -- HUD for %s"), *GetName()));
}

void ACOHUD::HandleMatchHasStarted()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Match has started")));
}

void ACOHUD::HandleMatchHasEnded()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Match has ended")));
}

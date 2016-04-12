// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/PlayerStart.h"
#include "COPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ACOPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TEAM)
		int32 TeamID = 1;
	
	
	
	
};

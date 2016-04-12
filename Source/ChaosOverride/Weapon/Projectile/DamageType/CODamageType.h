// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/DamageType.h"
#include "CODamageType.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API UCODamageType : public UDamageType
{
	GENERATED_BODY()

public:
	/** icon displayed in death messages log when killed with this weapon */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon KillIcon;

	/** force feedback effect to play on a player hit by this damage type */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UForceFeedbackEffect *HitForceFeedback;

	/** force feedback effect to play on a player killed by this damage type */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UForceFeedbackEffect *KilledForceFeedback;
	
	
	
	
};

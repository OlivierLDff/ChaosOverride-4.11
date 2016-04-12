// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "Engine/TargetPoint.h"
#include "COInterestPoint.generated.h"

UENUM(BlueprintType)
namespace ECOInterestPointType
{
	enum uint8
	{
		MeetingPoint,	
		CorridorCorner,
		TeamBase,	
	};
}

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ACOInterestPoint : public ATargetPoint
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = INTEREST)
		TEnumAsByte<ECOInterestPointType::uint8> Interest = ECOInterestPointType::MeetingPoint;

	/*A filter because only one team can be interest by a point, <=0 mean no filter*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = INTEREST)
		int32 TeamNotInterestIn = 0;
	
	
};

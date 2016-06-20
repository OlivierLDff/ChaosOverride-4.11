// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COSpringArmComponent.h"
#include "Character/COCharacter.h"

void UCOSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
	
	ACOCharacter * COCharacter = Cast<ACOCharacter>(GetOwner());
	if (COCharacter)
	{
		if (COCharacter->IsFalling() || COCharacter->IsNearGround())
			CameraLagSpeeds.Z = ZLagInAir;
		else
			CameraLagSpeeds.Z = FMath::FInterpTo(CameraLagSpeeds.Z, ZLagOnGround, DeltaTime, SpeedBetweenInAirAndOnGround);
		CustomCameraLag(DeltaTime);
	}

}

void UCOSpringArmComponent::UpdateCameraLocation()
{
	ACOCharacter * COCharacter = Cast<ACOCharacter>(GetOwner());
	if (COCharacter)
	{
		if (COCharacter->GetIsAiming())
		{
			SetRelativeLocation(COCharacter->GetClass()->GetDefaultObject<ACOCharacter>()->CameraBoom->RelativeLocation + COCharacter->GetLocalSpringArmOffsetWhenAiming(), true);
			TargetOffset = COCharacter->GetSpringArmTargetOffsetWhenAiming();
			TargetArmLength *= COCharacter->GetTargetArmLengthMultiplierWhenAiming();
		}
		else
		{
			SetRelativeLocation(COCharacter->GetClass()->GetDefaultObject<ACOCharacter>()->CameraBoom->RelativeLocation);
			TargetOffset = COCharacter->GetClass()->GetDefaultObject<ACOCharacter>()->CameraBoom->TargetOffset;
			TargetArmLength = COCharacter->GetClass()->GetDefaultObject<ACOCharacter>()->CameraBoom->TargetArmLength;
		}
		
	}
}

void UCOSpringArmComponent::CustomCameraLag(float DeltaTime)
{
	//The last position of the camera, used to interpolate between this to point with lag
	static FVector LastDesiredCameraPos = FVector();
	static bool bInit = false;

	//The pawn owner
	const ACOCharacter* COCharacter = Cast<ACOCharacter>(GetOwner());
	
	if (COCharacter)
	{
		//The camera of the COCharacter
		const UCameraComponent * FollowCamera = COCharacter->FollowCamera;
		
		if (FollowCamera)
		{
			if (!bInit)
			{
				LastDesiredCameraPos = COCharacter->FollowCamera->GetComponentLocation();
				bInit = true;
			}
				
			FRotator ControlRotation = COCharacter->GetControlRotation();
			ControlRotation.Pitch = ControlRotation.Roll = 0.f;
			const FTransform T = FTransform(ControlRotation, FollowCamera->GetComponentLocation(), FVector(1, 1, 1));

			//The camera position in local space
			const FVector CamLocalPosition = T.InverseTransformPosition(COCharacter->GetActorLocation()) + COCharacter->FinalGunCamOffset;
			//The last camera position in local space
			const FVector LastCamLocalPosition = T.InverseTransformPosition(LastDesiredCameraPos);

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("CamLocalPosition %s"), *CamLocalPosition.ToString()));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("LastCamLocalPosition %s"), *LastCamLocalPosition.ToString()));

			//The new X cam location component in local space
			const float XComponent = FMath::FInterpTo(CamLocalPosition.X, LastCamLocalPosition.X, DeltaTime, CameraLagSpeeds.X);
			//The new Y cam location component in local space
			const float YComponent = FMath::FInterpTo(CamLocalPosition.Y, LastCamLocalPosition.Y, DeltaTime, CameraLagSpeeds.Y);
			//The new Z cam location component in local space
			const float ZComponent = FMath::FInterpTo(CamLocalPosition.Z, LastCamLocalPosition.Z, DeltaTime, CameraLagSpeeds.Z);


			//The new cam position in world space
			FVector NewCamPositionInWorldSpace = T.TransformPosition(FVector(XComponent, YComponent, ZComponent));

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("FVector(XComponent, YComponent, ZComponent) %s"), *FVector(XComponent, YComponent, ZComponent).ToString()));

			//We clamp the lag

			FVector VectorToClamp = ControlRotation.RotateVector(NewCamPositionInWorldSpace - COCharacter->GetActorLocation());

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("VectorToClamp.x before %f"), VectorToClamp.X));

			VectorToClamp.X = FMath::Clamp(VectorToClamp.X, CameraMinDistances.X, CameraMaxDistances.X);
			VectorToClamp.Y = FMath::Clamp(VectorToClamp.Y, CameraMinDistances.Y, CameraMaxDistances.Y);
			VectorToClamp.Z = FMath::Clamp(VectorToClamp.Z, CameraMinDistances.Z, CameraMaxDistances.Z);

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("VectorToClamp.x after %f"), VectorToClamp.X));

			LastDesiredCameraPos = ControlRotation.UnrotateVector(VectorToClamp) + COCharacter->GetActorLocation();

			this->SocketOffset = FTransform(ControlRotation, COCharacter->GetActorLocation(), FVector(1, 1, 1)).InverseTransformPosition(LastDesiredCameraPos) + FVector(0, 0, 0);
		}
	}
}
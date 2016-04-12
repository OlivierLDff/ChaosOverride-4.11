// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/Actor.h"
#include "COWeaponProjectile.generated.h"

UCLASS()
class CHAOSOVERRIDE_API ACOWeaponProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACOWeaponProjectile(const FObjectInitializer& ObjectInitializer);

	/*------------------------*/
	/*---COMPONENT SECTION----*/
	/*------------------------*/

	/* The component responsible for the projectile movement */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = COMPONENT)
		class UProjectileMovementComponent * ProjectileComponent;

	/* The sphere that handle the damage */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = COMPONENT)
		class USphereComponent * DamagingSphere;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/*------------------------*/
	/*-----Overlap SECTION----*/
	/*------------------------*/

	UFUNCTION()
		void OnDamagingCapsuleOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void OnDamagingCapsuleOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*Called when we collide with a valid actor*/
	virtual void OnCollisionWithValidActor(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/*------------------------*/
	/*------OWNER SECTION-----*/
	/*------------------------*/

	/*The weapon responsible for spawning the projectile, initialize in begin play*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	class ACOWeapon * WeaponOwner = nullptr;

	/*------------------------*/
	/*------TARGET SECTION----*/
	/*------------------------*/

private:
	/*---Initial Value, should be initialize when spawning----*/

	/*The initial world location when spawned, set in begin play when spawning */
	FVector InitialProjectileWorldLocation = FVector(0, 0, 0);
	/*The initial forward vector of the projectile, set in begin play when spawning*/
	FVector InitialForwardVector = FVector(0, 0, 0);
	/*The initial up vector of the projectile, set in begin play when spawning*/
	FVector InitialUpVector = FVector(0, 0, 0);
	/*The target location when spawning, set in the begin play when the actor is spawning*/
	FVector InitialTargetLocation = FVector(0, 0, 0);
	/*The owner initial forward vector, set in the begin play when the actor is spawning*/
	FVector InitialOwnerForwardVector = FVector(0, 0, 0);
	/*The initial distance between the owner and the target, set in the begin play when the actor is spawning*/
	float InitialDistanceBetweenOwnerAndTarget = 0.f;
	/*The square of the initial speed, V0² in the formula, set in the begin play when the actor is spawning*/
	float SquareOfInitialSpeed = 0.f;

	/*The focus point, this value can change if the character focus is moving and we apply a continious correction*/
	FVector FocusPoint = FVector(0,0,0);


public:
	/*Return if the owner is aiming*/
	bool IsOwnerAiming() const;
	/*
	*	When spawned the projectile must orient itself to the focus point, this function is called during the begin play event
	*	We ask the focus point to the weapon that spawned the projectile with GetFocusPointFromOwner
	*	If there is no focus we call SetDefaultRotation function
	*/
	virtual void SetRotationToFocusPoint();

	/*Called if the owner of the gun isn't aiming, can be override for different response depending on weapon*/
	virtual void SetDefaultRotation();

	/*Return the point where the projectile has to focus, this can be an actor location or just a point in the air
	* This function is called during the initialization by SetRotationToFocusPoint*/
	FVector GetFocusPointFromOwner() const;

	/*
	*	If checked the targetActor array will be remove from an actor every time the function GetBestTargetedActor is called
	*	This should be use for multi target weapon
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TARGET)
		bool bRemoveActorFromTargetActorArrayWhenSpawningAProjectile = false;

	/*
	*	The strenght of the correction when following a character, if low it will just be a little correction
	*	1 mean a full continious trajectory correction, can't miss the target execpt if there is a wall
	*	0 mean no correction
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TARGET)
		float ContinuiousTrajectoryCorrectionStrength = 0.1;
	/*
	*	The strength of the correction when spawning, this correction depends only on SPAWNING CONDITION
	*	So the projectile will go to point where the targeted actor was when the projectile SPAWN
	*	1 mean a full continious trajectory correction, can't miss the target execpt if there is a wall
	*	0 mean no correction
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TARGET)
		float BeginPlayTrajectoryCorrectionStrength = 0.f;

	/*Do we want a rotation correction on spawned?*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TARGET)
		bool bBeginPlayRotationTrajectoryCorrection = true;

	/*The actor that is targeted*/
	AActor * TargetActor = nullptr;
	/*@return: the actual targeted actor*/
	AActor * GetTargetActor() const;
	/*Set the TargetActor, should be called when spawing a projectile if the projectile need a correction*/
	void SetTargetActor(AActor * newTargetActor);

	/*	Called when spawning in begin play, find the actor targeted by this projectile using the owner, usually it is a COWeapon so the GetBestTargetedActor is available
	*	@return : return the actor found
	*/
	AActor * FindTargetActor();

	/*
	*	Correct the orientation to the target, only called if a correction is needed(ie checked in the editor)
	*	This function can be called at any time, but if ContinuiousTrajectoryCorrectionStrength is greater than 0 this function is called on every tick
	*	If bBeginPlayTrajectoryCorrection is true this function is called at the end of begin play
	*	If no targetActor is set, nothing will happened
	*	@param : DeltaTime, the time between two frames
	*	@param : Target, the actor we want to use as a trajectory collision. ActorLocation will be use
	*	@param : TrajectoryCorrectionStrength, the strength of the correction, if 0 nothing will happen, if 1 full correction
	*/
	virtual void CorrectTrajectory(float DeltaTime, AActor * Target, float TrajectoryCorrectionStrength);


	/*------------------------*/
	/*-----BOUNCE SECTION-----*/
	/*------------------------*/

	/*The number of bounce before explosing, if <=0 then never bounce*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = BOUNCE)
		int32 BounceNumber = 3;
	/*The number of bounce already done*/
	int32 BounceDone = 0;
	/*Make the projectile bounce on a surface*/
	virtual void Bounce(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	/*Called when the projectile bounce*/
	virtual void OnBounce(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	/*Called when the projectile bounce, for cosmetic purpose, called on server & client from server*/
	UFUNCTION(BlueprintNativeEvent, Category = BOUNCE)
		void OnBounceBlueprint(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/*------------------------*/
	/*-----DAMAGE SECTION-----*/
	/*------------------------*/

	/*the ammount of damage the weapon is doing, if < 0 the projectile is healing, if ==0 nothing will hapenned*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = DAMAGE)
		float Damage = 10.0f;
	/*The sub class of damage type*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = DAMAGE)
		TSubclassOf< class UDamageType > DamageTypeClass;
	/*Information about the last hit, read by a COCharacter when damaging*/
	FHitResult LastHitResult;
	
	/*Apply damage to the actor*/
	virtual void ApplyDamage(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	virtual void Explose(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	virtual void OnExploding(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);	
	
};

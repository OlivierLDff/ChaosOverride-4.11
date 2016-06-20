// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/Character.h"
#include "COCharacter.generated.h"

UCLASS()
class CHAOSOVERRIDE_API ACOCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACOCharacter(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called when the game end
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Replicate variable to client
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------COMPONENT--------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/* La camera permettant de tourner autour du perso, c'est le point de vue du joueur */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "COMPONENT")
	class UCameraComponent* FollowCamera;

	/* La perche ou est attaché la caméra */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "COMPONENT")
	class UCOSpringArmComponent* CameraBoom;

	/* La perche ou est attaché la caméra */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "COMPONENT")
	class UCOInventoryComponent* Inventory;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------AI----------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/*The AI controller*/
	class ACOAIController * COAIController = nullptr;

	/*The target actor of the AI*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = AI)
		class AActor * AITargetActor = nullptr;
	
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------INPUT SYSTEM-----------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	class ACOPlayerController * GetCOController() const;
	/*The COPlayerController that is currently possessing this pawn*/
	class ACOPlayerController * COPlayerController = nullptr;
	/*The player state, actualise in possessedBy*/
	class ACOPlayerState * COPlayerState = nullptr;
	/*Called when this Pawn is possessed. Only called on the server (or in standalone).*/
	void PossessedBy(AController* NewController) override;
	/*Called when our Controller no longer possesses us.*/
	void UnPossessed() override;

	/*------------------------*/
	/*----MOUVEMENT SECTION---*/
	/*------------------------*/

	virtual void MoveForward(float Value);
	virtual void MoveRight(float Value);

	/*The value from the input moveForward*/
	float MoveForwardValue;
	/*The value from the input moveRight*/
	float MoveRightValue;

	UFUNCTION(Server, Reliable, WithValidation)
	void SetMoveForwardValue(float Value);
	UFUNCTION(Server, Reliable, WithValidation)
	void SetMoveRightValue(float Value);

	/*------------------------*/
	/*-----CAMERA SECTION-----*/
	/*------------------------*/

	virtual void Turn(float Value);
	virtual void TurnRate(float Value);
	virtual void LookUp(float Value);
	virtual void LookUpRate(float Value);

	/*Is the turn rate Reverse?*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CAMERA)
		bool bIsTurnRateReverse = false;

	/*The base turn rate, only for gamepad*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CAMERA)
		float BaseTurnRate = 80.0f;

	/*Is the turn rate Reverse?*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CAMERA)
		bool bIsLookUpRateReverse = false;

	/*The base turn rate, only for gamepad*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CAMERA)
		float BaseLookUpRate = 100.0f;

	/*Camera offset when watching from the bottom, to avoid having the character in front of the screen*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = CAMERA)
		float CameraZOffsetWhenWatchingDown = 50.0f;

	/*------------------------*/
	/*-------JUMP SECTION-----*/
	/*------------------------*/

	bool bPrintJumpDebugInfo = false;

	/*Called by the controller to handle the jump, determine if we are jumping or double jumping or gliding*/
	void HandleJump();

	/*Override from the function of character.*/
	virtual void Jump() override;

	/*Make the player execute a second jump*/
	virtual void DoubleJump();

	/*Return if the player is pressing the jump button*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Replicated)
	bool bJumpPressed = false;
	void SetJumpPressed(bool Value);
	bool GetJumpPressed() const;

	/*The number of time the function handle jump is called, it used to know if we are */
	uint32 NumberOfTimeHandleJumpCalledSinceLastLanded = 0;

	/*Return whether or not the player can jump*/
	bool GetCanJump() const;

	/*Return if the pawn has double jump after a simple jump, reinit at each land event*/
	bool GetIsDoubleJumping() const;
	/*If we have double jump between the time we are in falling mode and reset jump*/
	UPROPERTY(Replicated)
	bool bIsDoubleJumping = false;

	/** Le timer permettant de delayer le double saut si on n'est pas en haut(velocite > 400) */
	FTimerHandle DoubleJumpTimer;

	/** La velocite a ajouter lors du double saut, par default 1200 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = JUMP)
	float DoubleJumpZVelocity = 1200.0f;

	/*Called on client & server when landing on a surface*/
	virtual void Landed(const FHitResult& Hit) override;
	/*Called when the player land after a delay TimeBetweenTwoJump*/
	void ResetJump();

	/**
	 *	Indicate if the player is really falling, which means he is dashing
	 *
	 *	@return	If the character is falling
	 */
	UFUNCTION(BlueprintCallable, Category = JUMP)
		virtual bool IsFalling() const;

	/**
	 *	Is the player near ground, usefull to know if the player is at NearDistanceOfGround meter of the ground
	 *	
	 *	@return	If the player is falling a near ground
	 */
	UFUNCTION(BlueprintCallable, Category = JUMP)
		virtual bool IsNearGround() const;

	/**
	 *	The distance at which the player can be consider near the ground when he is falling
	 *
	 *	@see	IsNearGround
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JUMP")
		float NearDistanceOfGround = 100.f;


	/*------------------------*/
	/*------GLIDE SECTION-----*/
	/*------------------------*/

	/** Permet de savoir si le perso plane */
	UPROPERTY(Replicated)
	bool bIsGliding;
	bool GetIsGliding() const;

	/** La velocite a ajouter quand on plane, par default 200 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GLIDE")
		float GlideZVelocity = 200.0f;

	/** Permet de savoir si le a le droit de planer */
	bool bAllowToGlide;
	/*Set allow to glide to true when the double jump is finished, should be called by the anim instance*/
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "GLIDE")
	void ServerAllowToGlide();

	/*Check if glide is possible, called when glide is allowed and when we press the jump button*/
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerGlide();
	/*Stop to glide, should be called when unpressing the jump button or when landing*/
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopGliding();

	/*The direction of the glide, may be useful for implement more complexe gravity system*/
	FVector GetGlideVector() const;

	/*------------------------*/
	/*---HOVERBOOTS SECTION---*/
	/*------------------------*/

	/*Indicate if the hoverboots are currently active*/
	UPROPERTY(Replicated)
		bool bAreHoverbootsActive = false;
	/*The value from the hoverboots input*/
	UPROPERTY(Replicated)
		float HoverbootsInputValue = 0.0f;

	/*The coef of ground friction when the character is using hoverboots*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HOVERBOOTS)
		float HoverbootsGroundFriction = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HOVERBOOTS)
		float HoverbootsMaxAcceleration = 4096.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HOVERBOOTS)
		float MinMultiplierHoverbootsCoeff = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HOVERBOOTS)
		float MaxMultiplierHoverbootsCoeff = 2.0f;
	/*The maximum power for the jump forward when we jump using the hoverboots*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HOVERBOOTS)
		float JumpForwardPower = 1000.0f;

	virtual void HandleHoverbootsInput(float Value);
	virtual void ActivateHoverboots();
	virtual void DeactivateHoverboots();
	/*Acutalise the groud friction & max acceleration on client because it isn't replicated*/
	UFUNCTION(Client, Reliable)
	void ClientActualiseHoverbootsMovement(bool bActivate);

	bool GetAreHoverbootsActive() const;
	float GetMinMultiplierHoverbootsCoeff() const;
	float GetMaxMultiplierHoverbootsCoeff() const;
	float GetHoverbootsInputValue() const;
	/*If the hoverboots are graduate with the hoverboot input*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HOVERBOOTS)
		bool bHoverbootGraduable = false;


	/*------------------------*/
	/*------DASH SECTION------*/
	/*------------------------*/

	/*Return whether or not the character can dash*/
	bool CanDash() const;
	bool bAllowToDash = true;
	/*Make the character dash in the direction he is heading*/
	void HandleDash();
	void StopDash();
	/*The timer that handle the dash, call the stop dash*/
	FTimerHandle DashTimer;
	/*Calculate the dash direction*/
	FVector GetDashDirection() const;
	/*The time that the dash last*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DASH)
		float DashTime = 0.1f;
	/*The dash speed*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DASH)
		float DashSpeed = 5000.0f;
	float GetDashSpeed() const;
	/*If the character is currently Dashing, read by the character movement that handle the dash, replicated for the anim instance to stay inform*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = DASH, Replicated)
	bool bIsDashing = false;
	/*Return bIsDashing*/
	bool GetIsDashing() const;

	FTimerHandle DashEndTimer;
	/*The time that the dash last*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DASH)
		float TimeBetweenTwoDash = 0.5f;
	/*Reinit the dash after time between two dash seconds*/
	virtual void ReinitDash();
	/*Replicate the orientation to movement variable on client*/
	UFUNCTION(Client, Reliable)
	void ClientReplicateOrientationToMovement(bool newValue);

	/*------------------------*/
	/*-----JETPACK SECTION----*/
	/*------------------------*/

	/*Called by the controller to handle the jetpack input*/
	virtual void HandleJetPack(float Value);
	/*Active the jet pack*/
	virtual void ActivateJetPack();
	/*Deactive the jetpack*/
	virtual void DeactivateJetPack();
	/*Return if the jetpack can be activate*/
	virtual bool CanActivateJetPack() const;

	/*The current value for the jetpack input, ie L2, replicated for movement purpose*/
	UPROPERTY(Replicated)
		float JetPackInputValue = 0.0f;
	/*If the jetpack is active, replicated for cosmetic purpose to everyone*/
	UPROPERTY(Replicated)
		bool bIsJetPackActive = false;
	/*Return if the jetpack is active, ie bIsJetPackActive*/
	bool GetIsJetPackActive() const;
	/*Return the jetpack input value, @return JetPackInputValue*/
	float GetJetPackInput() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = JETPACK)
		float JetPackMaxZVelocity = 700.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = JETPACK)
		float JetPackMinZVelocity = 100.0f;

	float GetJetPackMaxZVelocity() const;
	float GetJetPackMinZVelocity() const;

	/*------------------------*/
	/*-----AIMING SECTION-----*/
	/*------------------------*/

	/*Handle the aim input*/
	UFUNCTION(BlueprintCallable, Category = AIM)
	virtual void HandleAim(bool bAimPressed);
	/*Called when the aim button is pressed*/
	virtual void OnAimPressed();
	/*Called when the aim button is released*/
	virtual void OnAimReleased();

	/*If the character is currently aiming, replicated for HUD (draw the weapon viewFinder) and for the anim instance to calculate the camera pitch*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = AIMING, Replicated)
	bool bIsAiming = false;
	/*Return if the character is aiming*/
	bool GetIsAiming() const;
	/*Call update springarm position*/
	void SetIsAiming(bool Value);

	/*The local offset when aiming*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = AIMING)
		FVector LocalSpringArmOffsetWhenAiming = FVector(0, 0, 70);
	/*Return the local location offset when aiming*/
	FVector GetLocalSpringArmOffsetWhenAiming() const;
	/*The local target offset when aiming*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = AIMING)
		FVector SpringArmTargetOffsetWhenAiming = FVector(0, 50, 0);
	/*Return the local target offset when aiming*/
	FVector GetSpringArmTargetOffsetWhenAiming() const;
	/*The FOV when aiming*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = AIMING)
		float FOVWhenAiming = 105.f;
	/*Return the custom FOV when aiming*/
	float GetFOVWhenAiming() const;
	/*The target arm length multiplier when aiming, default = 0.8f*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = AIMING)
		float TargetArmLengthMultiplierWhenAiming = 0.8f;
	/*Return the target arm length multiplier when aiming, default = 0.8f*/
	float GetTargetArmLengthMultiplierWhenAiming() const;

	/*Check if the character is aiming, if so ask the gun to fill the target array, called every frame*/
	virtual void CheckIfAiming();

	/*Return for the HUD if the weapon is actually targeting*/
	UFUNCTION(BlueprintCallable, Category = AIMING)
		bool HasATarget() const;

	//ALLOW
	/*Return whether or not the character is allowed to shoot*/
	virtual bool GetIsAllowedToFire() const;

	/*------------------------*/
	/*-----WEAPON SECTION-----*/
	/*------------------------*/

	void HandleMeleeWeapon(bool bPunchInputPressed);
	/*End equip the melee weapon, called by a timer and authorize to punch with the melee weapon*/
	void OnMeleeWeaponEquip();
	/*Called when the melee weapon is unequiped*/
	void OnUnEquipMeleeWeapon();

	//COSMETIC
	/*Return if the character is actually holding a one hand weapon(current weapon && equiped), should be called by the anim instance*/
	bool GetIsHoldingAOneHandWeapon() const;
	/*Return if the character is actually holding two hand weapon(current weapon && equiped), should be called by the anim instance NOT IMPLEMENT YET TODO*/
	bool GetIsHoldingTwoOneHandWeapon() const;
	/*Return if the character is actually holding a two hand weapon(current weapon && equiped), should be called by the anim instance*/
	bool GetIsHoldingATwoHandWeapon() const;
	/*Return if the character is actually holding a melee weapon(can be the effective melee weapon or a ACOWeapon that work as a melee weapon), should be called by the anim instance*/
	bool GetIsHoldingAMeleeWeapon() const;

	//INPUT

	/*Handle the fire input*/
	UFUNCTION(BlueprintCallable, Category = WEAPON) 
	virtual void HandleFire(bool bFirePressed);
	/*Called when the fire input is pressed*/
	virtual void OnFirePressed();
	/*Called when the fire input is released*/
	virtual void OnFireReleased();
	/*Handle the nextweaponbutton*/
	virtual void HandleNextWeapon(bool bNextWeaponInputPressed);
	/*Called when the next weapon button is pressed*/
	virtual void OnNextWeaponPressed();
	/*Called when the next weapon button is released*/
	virtual void OnNextWeaponReleased();
	/*Handle the prevWeaponButton*/
	virtual void HandlePrevWeapon(bool bPrevWeaponInputPressed);
	/*Called when the previous weapon button is pressed*/
	virtual void OnPrevWeaponPressed();
	/*Called when the previous weapon button is released*/
	virtual void OnPrevWeaponReleased();

	/**
	 *	The offset of the camera when possessing a weapon
	 *	Not useful now
	 */
	FVector FinalGunCamOffset = FVector(0, 0, 0);

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------DAMAGE--------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/*------------------------*/
	/*-----DAMAGE SECTION-----*/
	/*------------------------*/

	//PROPERTIES\\

	/*The actual health of the character*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = HEALTH)
	float Health = 100.0f;
	/*The max health of the character*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = HEALTH)
	float MaxHealth = 100.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = HEALTH)
	bool bIsDead = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = HEALTH)
	bool bPendingDying = false;
	/*An array that contain all the controller that damaged the character*/
	TArray<class AController *> AssistArray;

	/*The time during the player can't take damage momentum in seconds*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = DAMAGE)
		float InvincibleMomentumDamageTime = 5.f;
	/*Is the character invincible to momentum damage?*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = DAMAGE)
		bool bIsInvincibleToMomentumDamage;
	/*The timer that handle the reset of bIsInvincibleToMomentumDamage*/
	FTimerHandle ResetbIsInvincibleToMomentumDamageTimer;
	/*Reset the invincible time, called after InvincibleMomentumDamageTime*/
	void ResetInvincibleMomentumDamageTime();

	//HANDLE DAMAGE\\

	/*Apply damage to this actor*/
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;
	/*Return if a character can die*/
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;
	/*Make the character die*/
	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);
	/*Event called when the character die*/
	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, AController * Killer, AActor* DamageCauser);
	/*Event called when the character felloutofworld*/
	virtual void FellOutOfWorld(const class UDamageType& DmgType) override;
	/*Play the hit*/
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, AController * Killer, AActor* DamageCauser, bool bKilled);
	/*Replicate the hit information*/
	//void ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);
	/*Apply the damage due to the speed of impact*/
	virtual void ApplyDamageMomentum(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn * PawnInstigator, AActor* DamageCauser) override;


	//COSMETIC ASPECT\\

	//virtual void HandleDevastatorImpact();

	/*------------------------*/
	/*------HEAL SECTION------*/
	/*------------------------*/

	/*Heal the character bu the health amount, if health amount<=0 nothing happen*/
	virtual bool Heal(float HealthAmount, bool bRefill = false);
	/*Called when the health is full*/

	/*	Heal the character bu the health amount, if health amount<=0 nothing happen
	*	This function execute only on server
	*	@param HealthAmount The amount of health add
	*	@param bRefill If you want to refill
	*/
	UFUNCTION(BlueprintCallable, Category = HEAL)
		virtual bool HealBlueprint(float HealthAmount, bool bRefill = false);
	

};

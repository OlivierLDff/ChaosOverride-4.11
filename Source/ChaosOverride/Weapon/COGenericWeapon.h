// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/Actor.h"
#include "COGenericWeapon.generated.h"

UCLASS()
class CHAOSOVERRIDE_API ACOGenericWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACOGenericWeapon(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/*------------------------*/
	/*---REPLICATION SECTION--*/
	/*------------------------*/

	// Replicate variable to client
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*------------------------*/
	/*-----EQUIP SECTION------*/
	/*------------------------*/


	//EQUIP

	/*Equip the weapon to the character*/
	virtual void EquipWeapon();
	/*Called when the weapon is equip after a timer*/
	virtual void OnEquip();
	/*The timer that handle the delay of equip*/
	FTimerHandle EquipTimerHandle;
	/*The time needed to equip the weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EQUIP)
		float EquipTime = 0.2f;

	/*Indicate if the weapon is pending equiped, replicate for cosmetic*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, Category = EQUIP)
		bool bPendingEquip = false;
	/*Indicate if the weapon is equiped*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, Category = EQUIP)
		bool bIsEquip = false;
	/*Indicate if the weapon is pending unequiped*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, Category = EQUIP)
		bool bPendingUnEquip = false;

	//UNEQUIP

	/*Unequip the weapon*/
	virtual void UnEquipWeapon(bool bWithAnimation = true);
	/*Called when the weapon is unequiped after a timer if the animation is played*/
	virtual void OnUnEquip();

	/*The timer that handle the delay of unequip*/
	FTimerHandle UnEquipTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = EQUIP)
		float UnEquipTime = 0.1f;

	//SOCKET

	/*Return where the weapon have to be attached*/
	virtual FName GetWeaponSocketName() const;

	/*------------------------*/
	/*-----DAMAGE SECTION-----*/
	/*------------------------*/

	/*The amount of damage the weapon is making*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
		float DamageAmount = 20.0f;
	
	
};

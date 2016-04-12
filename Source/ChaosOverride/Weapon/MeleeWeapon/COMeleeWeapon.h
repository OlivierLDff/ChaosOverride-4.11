// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "Weapon/COGenericWeapon.h"
#include "COMeleeWeapon.generated.h"

UCLASS()
class CHAOSOVERRIDE_API ACOMeleeWeapon : public ACOGenericWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACOMeleeWeapon(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/*---------------------------*/
	/*-----COMPONENT SECTION-----*/
	/*---------------------------*/

	/** Le root de l'objet */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Root")
	class USceneComponent* Root;

	/* Le mesh de l'arme */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skeletal Mesh")
	class USkeletalMeshComponent* MeleeWeaponMesh;

	/*The capsule that is applying damage*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
	class UCapsuleComponent* DamagingCapsule;

	/*---------------------------*/
	/*-----ANIMATION SECTION-----*/
	/*---------------------------*/


	/*The animation to play when we equip the weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		class UAnimationAsset * EquipAnimation = nullptr;
	/*The animation to play when we unequip the weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		class UAnimationAsset * UnEquipAnimation = nullptr;
	/*The animation to play when we attack with the weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		class UAnimationAsset * AttackAnimation = nullptr;

	/*------------------------*/
	/*-----EQUIP SECTION------*/
	/*------------------------*/

	/*Equip the weapon to the character*/
	virtual void EquipWeapon() override;
	/*Called when the weapon is equip after a timer*/
	virtual void OnEquip() override;

	/*Unequip the weapon*/
	virtual void UnEquipWeapon(bool bWithAnimation = false) override;
	/*Called when the weapon is unequiped after a timer if the animation is played*/
	virtual void OnUnEquip() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EQUIP)
		FName MeleeWeaponSocketName = "MeleeWeaponSocket";

	FName GetWeaponSocketName() const override;

	/*------------------------*/
	/*-----DAMAGE SECTION-----*/
	/*------------------------*/

	/*Attack with the weapon*/
	void Attack();


	/*------------------------*/
	/*-----Overlap SECTION----*/
	/*------------------------*/
	UFUNCTION()
		void OnDamagingCapsuleOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void OnDamagingCapsuleOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};

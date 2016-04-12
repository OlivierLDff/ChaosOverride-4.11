// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COMeleeWeapon.h"


// Sets default values
ACOMeleeWeapon::ACOMeleeWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ROOT
	Root = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Defaut Scene Root"));
	SetRootComponent(Root);

	//MELEEWEAPON MESH
	MeleeWeaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("Melee Weapon Mesh"));
	MeleeWeaponMesh->AttachParent = Root;

	//DAMAGE CAPSULE
	DamagingCapsule = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("Damage Capsule"));
	DamagingCapsule->AttachParent = MeleeWeaponMesh;
	DamagingCapsule->SetCollisionResponseToAllChannels(ECR_Overlap);
	//DamagingCapsule->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Overlap);
	DamagingCapsule->OnComponentBeginOverlap.AddDynamic(this, &ACOMeleeWeapon::OnDamagingCapsuleOverlapBegin);
	DamagingCapsule->OnComponentEndOverlap.AddDynamic(this, &ACOMeleeWeapon::OnDamagingCapsuleOverlapEnd);
	DamagingCapsule->bGenerateOverlapEvents = true;
}

// Called when the game starts or when spawned
void ACOMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACOMeleeWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ACOMeleeWeapon::EquipWeapon()
{
	Super::EquipWeapon();
	/*Play equip animation*/
	if (EquipAnimation)
	{
		//MeleeWeaponMesh->PlayAnimation(EquipAnimation, false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You need to add an equipanimation for the current meleeWeapon!")));
	}
}

void ACOMeleeWeapon::OnEquip()
{
	Super::OnEquip();
	DamagingCapsule->bGenerateOverlapEvents = true;//DEBUG, will need change, only generate overlap event when attacking
}

void ACOMeleeWeapon::UnEquipWeapon(bool bWithAnimation)
{
	Super::UnEquipWeapon();
	if (bWithAnimation)
	{
		if (UnEquipAnimation)
		{
			//MeleeWeaponMesh->PlayAnimation(UnEquipAnimation, true);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You need to add an unequipanimation for the current meleeWeapon!")));
		}
	}
}

void ACOMeleeWeapon::OnUnEquip()
{
	Super::OnUnEquip();
	DamagingCapsule->bGenerateOverlapEvents = false;
}

FName ACOMeleeWeapon::GetWeaponSocketName() const
{
	Super::GetWeaponSocketName();
	return MeleeWeaponSocketName;
}

void ACOMeleeWeapon::Attack()
{
	if (bIsEquip)
		DamagingCapsule->bGenerateOverlapEvents = true;
}


void ACOMeleeWeapon::OnDamagingCapsuleOverlapBegin(class AActor * OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("BEGIN OVERLAP!!")));
}

void ACOMeleeWeapon::OnDamagingCapsuleOverlapEnd(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("END OVERLAP!!")));
}


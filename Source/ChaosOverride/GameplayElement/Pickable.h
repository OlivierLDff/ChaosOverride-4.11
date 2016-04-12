// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/Actor.h"
#include "Pickable.generated.h"

UCLASS()
class CHAOSOVERRIDE_API APickable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickable();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};

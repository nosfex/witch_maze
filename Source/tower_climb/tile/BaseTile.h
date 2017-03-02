// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BaseTile.generated.h"

UCLASS()
class TOWER_CLIMB_API ABaseTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseTile();

protected:
	bool StartingPoint;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool GetStartingPoint() { return StartingPoint; }
	void SetStartingPoint() { StartingPoint = true; }

	
};

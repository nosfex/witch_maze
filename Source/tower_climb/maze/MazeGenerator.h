// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "../tile/BaseTile.h"
#include "../room/Room.h"
#include "MazeGenerator.generated.h"

UCLASS(Blueprintable)
class TOWER_CLIMB_API AMazeGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMazeGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Size of the map will be combined
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapSize)
	int32 MapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapBaseTile)
	TSubclassOf<ABaseTile> MapTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapInitialLocation)
	FVector InitialLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapSize)
	float TileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room)
	int32 RoomMaxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room)
	int32 RoomMinSize;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room)
	int32 RoomMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Room)
	float FloodFillErrorMax;

private:

	TArray<Room*> Rooms;
	
	TArray<TArray<int32>> TileMap2D;

	void InitializeMap();

	void PickPosition(int32 x, int32 y);

	TArray<ABaseTile*> Tiles;
	UFUNCTION(BlueprintCallable)
	void AddRoom();

	void SelectSquare(int32 x, int32 y, int32 distance);
	
	void SelectDiamond(int32 x, int32 y, int32 distance);
	
	bool CreateQuadRoom(int32 x, int32 y, int32 sizeX, int32 sizeY);
	// GH: Generate a walkway into a room. 
	void AddWalkway(Room* start);

	void InitBlocks();
	
	FVector GetRoomLocation(Room* room);

	void FixEdges();
};

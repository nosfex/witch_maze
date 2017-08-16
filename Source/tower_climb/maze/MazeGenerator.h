// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "../tile/BaseTile.h"
#include "../room/Room.h"
#include "MazeGenerator.generated.h"

UENUM(Blueprintable)
enum EMazeType 
{
	EFloodFill  UMETA(DisplayName= "Flood Fill"),
	EBackTracking UMETA(DisplayName= "Back Tracking")
};

enum EDirection
{
	ENorth	= 1,
	ESouth	= 2,
	EEast	= 3,
	EWest	= 4,
	EError	= 0
};

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

	// Size of the map will be squared
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapSize)
	int32 MapSize;
	// GH: Class ref for the bp we want to instantiate as our maptile
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MapBaseTile)
	TArray<TSubclassOf<ABaseTile>> MapTile;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MazeType)
	TEnumAsByte<EMazeType> MazeType;

private:
	TArray<EDirection> Directions;

	TArray<Room*> Rooms; 
	
	TArray<TArray<int32>> TileMap2D;

	void InitializeMap();

	void PickPosition(int32 x, int32 y);

	TArray<ABaseTile*> Tiles;
	UFUNCTION(BlueprintCallable)
	void AddRoom();

	bool CreateQuadRoom(int32 x, int32 y, int32 sizeX, int32 sizeY);
	// GH: Generate a walkway into a room. 
	void AddWalkway(Room* start);

	void InitBlocks();

	// ------------------- Backtrack block -------------------
	void CarvePassageFrom(int32 x, int32 y);
	
	EDirection GetOppositeDirection(EDirection dir);

	void GetNextPosFromDir(EDirection dir, int32 &x, int32 &y);

	EDirection GetRandomDirection();

	FString DirectionToString(EDirection dir);

	// ------------------- General Use -------------------
	FVector GetRoomLocation(Room* room);
	
	void FixEdges();
};

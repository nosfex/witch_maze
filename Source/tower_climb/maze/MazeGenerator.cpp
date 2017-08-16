// Fill out		your copyright notice in the Description page of Project Settings.

#include "tower_climb.h"
#include "MazeGenerator.h"


// Sets default values
//-----------------------------------------------------------------------------
AMazeGenerator::AMazeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RoomMinSize = 2;
}
//-----------------------------------------------------------------------------
// Called when the game starts or when spawned
void AMazeGenerator::BeginPlay()
{
	Super::BeginPlay();
	InitializeMap();
	switch(MazeType)
	{ 
		case EMazeType::EFloodFill:
			AddRoom();
			FixEdges();
			InitBlocks();
			break;

		case EMazeType::EBackTracking:
			CarvePassageFrom(0, 0);
			//FixEdges();
			InitBlocks();

			break;

	}
}
//-----------------------------------------------------------------------------
// Called every frame
void AMazeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
//-----------------------------------------------------------------------------
void AMazeGenerator::InitializeMap()
{
	for(int32 i = 0; i < MapSize; i++)
	{
		TArray<int32> xTile;
		for (int32 k = 0; k < MapSize; k++)
		{
			int32 yTile = 0;
			xTile.Add(yTile);
		}
		TileMap2D.Add(xTile);
	}

	Directions = {EDirection::ENorth, EDirection::ESouth, EDirection::EEast, EDirection::EWest};
}
//-----------------------------------------------------------------------------
void AMazeGenerator::InitBlocks()
{
	switch (MazeType)
	{
		

		case EMazeType::EBackTracking:
			for(int x = 0 ; x < TileMap2D.Num() ; x ++)
			{
				for(int y = 0; y < TileMap2D.Num() ; y++)
				{
					ABaseTile* Tile = GetWorld()->SpawnActor<ABaseTile>(MapTile[TileMap2D[x][y]]);
					Tile->SetActorLocation(InitialLocation + FVector(TileSize * x, TileSize * y, 0.0f));
					if (Tiles.Num() == 0)
					{
						Tile->SetStartingPoint();
					}
					Tiles.Add(Tile);
				}
			}
					
			break;
		default:
			for (int x = 0; x < TileMap2D.Num(); x++)
			{
				for (int y = 0; y < TileMap2D[x].Num(); y++)
				{
					if (TileMap2D[x][y] == 0)
					{
						ABaseTile* Tile = GetWorld()->SpawnActor<ABaseTile>(MapTile[TileMap2D[x][y]]);
						Tile->SetActorLocation(InitialLocation + FVector(TileSize * x, TileSize * y, 0.0f));
						if (Tiles.Num() == 0)
						{
							Tile->SetStartingPoint();
						}
						Tiles.Add(Tile);
					}
				}
			}
			break;
	}
}
//-----------------------------------------------------------------------------
void AMazeGenerator::FixEdges()
{
	for (int i = 0; i < TileMap2D.Num(); i++)
	{
		TileMap2D[0][i]						= 0;
		TileMap2D[TileMap2D.Num() -1][i]	= 0;
		TileMap2D[i][0]						= 0;
		TileMap2D[i][TileMap2D.Num() - 1]	= 0;
	}
}
//-----------------------------------------------------------------------------
void AMazeGenerator::PickPosition(int32 x, int32 y)
{
	TileMap2D[x][y] = 1;
}
//-----------------------------------------------------------------------------
FVector AMazeGenerator::GetRoomLocation(Room* room)
{
	int32 x, y = 0;
	room->GetXY(x, y);

	return InitialLocation + FVector(TileSize * x, TileSize * y, 0.0f);
}
//-----------------------------------------------------------------------------
void AMazeGenerator::AddRoom()
{
	int32 max = RoomMax;

	while(max > 0)
	{
		// Get a random mid point for the room
		int32 x = FMath::RandRange(0, MapSize - 1);
		int32 y = FMath::RandRange(0, MapSize - 1);

		// Leave the equation
		if (TileMap2D[x][y] == 1)
		{
			// GH: Check % on room completion
			float xPer = 0.0f;
			for(int32 i = 0; i < TileMap2D.Num(); i++)
			{
				for(int32 j = 0; j < TileMap2D.Num(); j++)
				{
					if(TileMap2D[i][j] == 1)
					{
						xPer++;
					}
				}
			}

			if((xPer / (float)(TileMap2D.Num() *TileMap2D.Num())) >= FloodFillErrorMax)
			{
				max = -1;
			}

			continue;
		}

		int32 width	 = FMath::RandRange(RoomMinSize, RoomMaxSize);
		int32 height = FMath::RandRange(RoomMinSize, RoomMaxSize);

		if (CreateQuadRoom(x, y, width, height))
		{
			max--;
		}
		if (Rooms.Num() > 1)
		{
			// GH: auto parenting?
			Rooms.Last()->SetParent(Rooms[Rooms.Num() - 2]);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("ROOMS: %i"), Rooms.Num());
	// GH: Connect the rooms. Get Beginning + End and figure out how to move from there
	Room* end = Rooms.Last();
	for (int i = Rooms.Num() -1; i > 1 ; i--)
	{
		AddWalkway(Rooms[i]);
	}

	int32 x, y = 0;
	Rooms[0]->GetXY(x, y);
	AController* controller = GetWorld()->GetFirstPlayerController();
	controller->GetPawn()->SetActorLocation(InitialLocation + FVector(x * TileSize, y * TileSize, 0.0f));
}
//-----------------------------------------------------------------------------
void AMazeGenerator::AddWalkway(Room* start)
{
	// GH: Linearity is best boi
	Room* parent = start->GetParent();
	// GH: Room positions, start+parent
	int32 x = 0;
	int32 y = 0;
	int32 pX = 0;
	int32 pY = 0;
	// GH: Get map position
	start->GetXY(x, y);
	parent->GetXY(pX, pY);
	// GH: Get distance
	int32 dX = abs(x - pX);
	int32 dY = abs(y - pY);
	// GH: get direction
	int32 xDir = x - pX < 0 ? 1 : -1;
	int32 yDir = y - pY < 0 ? 1 : -1;

	UE_LOG(LogTemp, Warning, TEXT("x: %i pX: %i dX: %i xDir: %i"), x, pX, dX, xDir);
	for (int32 i = 0; i < dX; i++)
	{
		PickPosition(x + i * xDir, y);
	
	}
	// GH: Start from X last position
	for (int32 i = 0; i < dY; i++)
	{
		PickPosition(x + (dX * xDir), y + i * yDir);
	}
	
}
//-----------------------------------------------------------------------------
bool AMazeGenerator::CreateQuadRoom(int32 x, int32 y, int32 sizeX, int32 sizeY)
{
	// GH: adjacent coordinates
	int32 left		= x - sizeX;
	int32 right		= x + sizeX;
	int32 top		= y + sizeY;
	int32 bottom	= y - sizeY;
	// GH: Bound check
	if (left < 0 || right >= TileMap2D.Num())
	{
		return false;
	}

	if (bottom < 0 || top >= TileMap2D.Num())
	{
		return false;
	}

	TArray<FVector2D> positions;
	for (int minX = left; minX < right; minX++)
	{
		for (int minY = bottom; minY < top; minY++)
		{
			if(TileMap2D[minX][minY] == 1)
			{
				return false;
			}
			positions.Add(FVector2D(minX, minY));
		}
	}

	for (int i = 0; i < positions.Num(); i++)
	{
		PickPosition(positions[i].X, positions[i].Y);
	}
	
	// GH: generate the room data structure, for funsies.
	Room* newRoom = new Room(x, y, sizeX, sizeY);
	Rooms.Add(newRoom);
	newRoom->SetParent(NULL);
	return true;
}
//-----------------------------------------------------------------------------
void AMazeGenerator::CarvePassageFrom(int32 x, int32 y)
{
	// GH: Grab the 4 directions and open up
	int32 nx, ny = 0;
	EDirection dir;

	// GH: Reset the direction arr
	Directions.Sort([this](const EDirection a, const EDirection b)
	{
		return FMath::FRand() < 0.5f;
	});

	for(int i = 0 ; i < Directions.Num() ; i ++)
	{
		// GH: Grab a random dir
		dir = Directions[i];
		nx = x;
		ny = y;
		//  GH: update the new position
		GetNextPosFromDir(dir, nx, ny);
		//  GH: Bound and tile check
		if ( (ny >= 0 && ny < TileMap2D.Num()) && (nx >= 0 && nx < TileMap2D.Num()) && TileMap2D[nx][ny] == 0)
		{
			// GH: Update the new direction
			TileMap2D[x][y] = dir;
			// GH: set the next tile direction
			TileMap2D[nx][ny] = GetOppositeDirection(dir);
			FVector drawpos(TileSize * x, TileSize * y, 0.0f);
			DrawDebugString(GEngine->GetWorldFromContextObject(this), drawpos, DirectionToString(dir), nullptr, FColor::White, 1000000000.0f);
			// GH: Recursivitis
			CarvePassageFrom(nx, ny);
		}
	}
}
//-----------------------------------------------------------------------------
FString AMazeGenerator::DirectionToString(EDirection dir)
{
	
	switch (dir)
	{
		case EDirection::ENorth:
			return FString("NORTH");
			break;

		case EDirection::ESouth:
			return FString("SOUTH");
			break;

		case EDirection::EEast:
			return FString("EAST");
			break;

		case EDirection::EWest:
			return FString("WEST");;
			break;

	}
	return FString("ERROR");
}
//-----------------------------------------------------------------------------
EDirection AMazeGenerator::GetOppositeDirection(EDirection dir)
{
	switch (dir)
	{

		case EDirection::ENorth:	
			return EDirection::ESouth;
			break;

		case EDirection::ESouth:
			return EDirection::ENorth;
			break;

		case EDirection::EEast:
			return EDirection::EWest;
			break;

		case EDirection::EWest:
			return EDirection::EEast;
			break;

	}

	return EDirection::EError;
}
//-----------------------------------------------------------------------------
void AMazeGenerator::GetNextPosFromDir(EDirection dir, int32 &x, int32 &y)
{

	switch (dir)
	{
		case EDirection::ENorth:
			x = x;
			y = y - 1;
			break;

		case EDirection::ESouth:
			x = x;
			y = y + 1;
			break;

		case EDirection::EEast:
			x = x + 1;
			y = y;
			break;

		case EDirection::EWest:
			x = x - 1;
			y = y;
			break;
	}
}
//-----------------------------------------------------------------------------
EDirection AMazeGenerator::GetRandomDirection()
{
	int32 rnd = FMath::RandRange(0, Directions.Num() - 1);
	EDirection dir = Directions[rnd];
	Directions.RemoveAt(rnd);
	return dir;
}
//-----------------------------------------------------------------------------


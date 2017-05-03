// Fill out		your copyright notice in the Description page of Project Settings.

#include "tower_climb.h"
#include "MazeGenerator.h"


// Sets default values
AMazeGenerator::AMazeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMazeGenerator::BeginPlay()
{
	Super::BeginPlay();
	InitializeMap();
	AddRoom();
	InitBlocks();
}

// Called every frame
void AMazeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

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
	/*for (int32 i = 0; i < MapSize; i++)
	{
		for (int32 k = 0; k < MapSize; k++)
		{
			PickPosition(i, k);
		}
	}*/
}
void AMazeGenerator::InitBlocks()
{
	for (int x = 0; x < TileMap2D.Num(); x++)
	{
		for (int y = 0; y < TileMap2D[x].Num(); y++)
		{
			if (TileMap2D[x][y] == 0)
			{
				ABaseTile* Tile = GetWorld()->SpawnActor<ABaseTile>(MapTile);
				Tile->SetActorLocation(InitialLocation + FVector(TileSize * x, TileSize * y, 0.0f));
				if (Tiles.Num() == 0)
				{
					Tile->SetStartingPoint();
				}
				Tiles.Add(Tile);
			}
		}
	}
}

void AMazeGenerator::PickPosition(int32 x, int32 y)
{
	TileMap2D[x][y] = 1;
}
void AMazeGenerator::RemovePosition(int32 x, int32 y)
{
	for (int i = 0; i < Tiles.Num(); i++)
	{
		FVector pos = InitialLocation + FVector(TileSize * x, TileSize * y, 0.0f);
		if(Tiles[i]->GetActorLocation() == pos)
		{
			GetWorld()->RemoveActor(Tiles[i], true);
			TileMap2D[x][y] = 0;
		}
	}
}
void AMazeGenerator::AddRoom()
{
	int32 max = RoomMax;

	while (max >= 0)
	{
		// Get a random mid point for the room
		int32 x = FMath::RandRange(0, MapSize - 1);
		int32 y = FMath::RandRange(0, MapSize - 1);

		// Leave the equation
		if (TileMap2D[x][y] == 1)
		{

			// GH: Check % on room completion
			float xPer = 0.0f;

			for (int32 i = 0; i < TileMap2D.Num(); i++)
			{
				for (int32 j = 0; j < TileMap2D.Num(); j++)
				{
					if (TileMap2D[i][j] == 1)
					{
						xPer++;
					}
				}
			}

			if ((xPer / (float)(TileMap2D.Num() *TileMap2D.Num())) >= FloodFillErrorMax)
			{
				max = -1;
			}

			continue;

		}

		int32 width = FMath::RandRange(2, RoomMaxSize);
		int32 height = FMath::RandRange(2, RoomMaxSize);

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

	// GH: Connect the rooms. Get Beginning + End and figure out how to move from there
	Room* end = Rooms.Last();

	for (int i = Rooms.Num() -1; i > 1 ; i--)
	{
		AddWalkway(Rooms[i]);
	}
}

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

	UE_LOG(LogTemp, Warning, TEXT("pxy: %d"), parent);
	
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
	for (int32 j = 0; j < dY; j++)
	{
		PickPosition(x , y + j * yDir);
	}
}


bool AMazeGenerator::CreateQuadRoom(int32 x, int32 y, int32 sizeX, int32 sizeY)
{
	// GH: adjacent coordinates
	int32 left		= x - sizeX;
	int32 right		= x + sizeX;
	int32 top		= y + sizeY;
	int32 bottom	= y - sizeY;

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

void AMazeGenerator::SelectSquare(int32 x, int32 y, int32 distance)
{
	int32 left = x - distance;
	int32 right = x + distance;
	int32 top = y + distance;
	int32 bottom = y - distance;
	
	if (left >= 0 && bottom >= 0)
	{
		// GH: Occupied, break
		if (TileMap2D[left][bottom] == 1)
		{
			return;
		}
	}

}

void AMazeGenerator::SelectDiamond(int32 x, int32 y, int32 distance)
{
	
	int32 left		= x - distance;
	int32 right		= x + distance;
	int32 top		= y + distance;
	int32 bottom	= y - distance;

	// GH: 4 picks


}




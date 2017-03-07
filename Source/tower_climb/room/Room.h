// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
// GH: Fucking includes
#include "GameFramework/Actor.h"
class TOWER_CLIMB_API Room 
{
public:	
	// Sets default values for this actor's properties
	Room();	
	Room(int32 nX, int32 nY, int32 w, int32 h);
	virtual ~Room();

	// GH: Basic bitches write setters
	int32 GetWidth()					{ return width; }
	void SetWidth(int32 w)				{ width = w; }
	
	int32 GetHeight()					{ return height; }
	void SetHeight(int32 h)				{ height = h; }
	
	void GetXY(int32 &rX, int32 & rY)	{ rX = x; rY = y; }
	void SetXY(int32 rX, int32 rY)		{ x = rX; y = rY; }

	void SetParent(Room* parent)		{ Parent = parent; }
	Room* GetParent()					{ return Parent; }

	TArray<Room*> GetNearestRooms()		{ return NearestRooms; }

private:

	int32 x, y;
	int32 width, height;
	
	Room* Parent;
	TArray<Room*> NearestRooms;

};

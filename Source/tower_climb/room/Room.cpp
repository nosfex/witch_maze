// Fill out your copyright notice in the Description page of Project Settings.

#include "tower_climb.h"
#include "Room.h"


// Sets default values
Room::Room()
{
}

Room::Room(int32 nX, int32 nY, int32 w, int32 h) :
	x(nX),
	y(nY),
	width(w),
	height(h),
	Parent(nullptr)
{
}


Room::~Room() {}

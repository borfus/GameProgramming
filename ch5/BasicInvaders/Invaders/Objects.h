#pragma once

#include "math.h"
#include "stdlib.h"
#include "MyFiles.h"
#include <vector>
#include "surface.h"
#include "Input.h"

inline float Rand(float a_Range) { return ((float)rand() / RAND_MAX) * a_Range; } // put it hear
enum Types {
	Bullet,
	Missile1,
	Missile2,
	Alien,
	AShooter,
	AShelter
};

class Objects
{


public:
	Objects();
	Objects(char* Filename, MyFiles* FileHandler);  // in anticipation of the system being overloaded
	~Objects();


	Types Type;
	float Xpos, Ypos;
	float Xspeed, Yspeed;

	Surface* Image;



};


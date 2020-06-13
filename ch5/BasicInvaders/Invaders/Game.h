// Main Game loop control

#pragma once
#include <vector>
#include "MyFiles.h"
#include "surface.h"
#include "Objects.h"
#include "Input.h"




#define SCRWIDTH 320
#define SCRHEIGHT 240


using namespace std;


class Game
{
public:
	
	Game();
	~Game();
	
	
	void Update(float DTime, MyFiles* FileHandler, Input* InputHandler,Surface* a_Screen);
	bool Init(MyFiles* FileHandler);

	Objects* Bob;



private:
	
	 bool InitDone ;
	
	
};


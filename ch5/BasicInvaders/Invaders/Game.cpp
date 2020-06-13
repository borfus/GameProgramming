
#include "Game.h"

#include <string.h>

Game::Game()
{
	InitDone = false;
	
}
Game::~Game()
{
// make sure everything is cleared out	
}

extern std::vector<Objects*> MyObjects;




void Game::Update(float DTime, MyFiles* a_FileHandler, Input* a_InputHandler,Surface* a_Screen)
{
	if (this->InitDone == false)
	{
		Init(a_FileHandler);
		
	}

	



} // update 


bool Game::Init(MyFiles* a_FileHandler)
{

// set up your "objects" here



	InitDone = true;
	return true;
}
	

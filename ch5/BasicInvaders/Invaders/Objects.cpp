#include "Objects.h"





Objects::Objects()
{
	
}

// its perhaps not clear in the book but you will be asked to create new overloaded constructors for your derived classes
// which will mean calling this method to load your images

Objects::Objects(char* FN, MyFiles* FH)  // in anticipation of the system being overloaded
{
	Image = new Surface(FN, FH);

}


Objects::~Objects()
{
//	delete Image;
	Image = 0;
}


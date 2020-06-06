
#pragma once

#include "MyFiles.h"
#include <stdio.h>  
#include <math.h>



#define BLUEMASK	(0x00ff0000)
#define GREENMASK	(0x0000ff00) 
#define REDMASK		(0x000000ff)
#define ALPHAMASK	(0xff000000)


	typedef unsigned long Pixel;

	inline Pixel AddBlend(Pixel a_Color1, Pixel a_Color2)
	{
		const unsigned int r = (a_Color1 & REDMASK) + (a_Color2 & REDMASK);
		const unsigned int g = (a_Color1 & GREENMASK) + (a_Color2 & GREENMASK);
		const unsigned int b = (a_Color1 & BLUEMASK) + (a_Color2 & BLUEMASK);
		const unsigned r1 = (r & REDMASK) | (REDMASK * (r >> 24));
		const unsigned g1 = (g & GREENMASK) | (GREENMASK * (g >> 16));
		const unsigned b1 = (b & BLUEMASK) | (BLUEMASK * (b >> 8));
		const unsigned a1 = (r1 + g1 + b1 == 0x00ffffff) ? 0:0xff000000;
		return (a1 +r1 + g1 + b1);
	}

	// subtractive blending
	inline Pixel SubBlend(Pixel a_Color1, Pixel a_Color2)
	{
		int red = (a_Color1 & REDMASK) - (a_Color2 & REDMASK);
		int green = (a_Color1 & GREENMASK) - (a_Color2 & GREENMASK);
		int blue = (a_Color1 & BLUEMASK) - (a_Color2 & BLUEMASK);
		if (red < 0) red = 0;
		if (green < 0) green = 0;
		if (blue < 0) blue = 0;
		return (Pixel)(red + green + blue);
	}

	inline Pixel PlotNoAlpha(Pixel a_Color1)
	{
		const unsigned int r = (a_Color1 & REDMASK) ;
		const unsigned int g = (a_Color1 & GREENMASK);
		const unsigned int b = (a_Color1 & BLUEMASK) ;
	
		const unsigned a = (r + g + b == 0x00ffffff) ? 0 : 0xff000000;
		return (a + r + g + b);
	}


	class Surface
	{
		enum
		{
			OWNER = 1
		};

	public:
		// constructor / destructor
		Surface(int a_Width, int a_Height, Pixel* a_Buffer);
		Surface(int a_Width, int a_Height);
		Surface(char* a_File, MyFiles* FileHandler);
		~Surface();
		// member data access
		Pixel* GetBuffer() { return m_Buffer; }
		void SetBuffer(Pixel* a_Buffer) { m_Buffer = a_Buffer; }
		int GetWidth() { return m_Width; }
		int GetHeight() { return m_Height; }
		int GetPitch() { return m_Pitch; }
		void SetPitch(int a_Pitch) { m_Pitch = a_Pitch; }

		void ClearBuffer(Pixel a_Color);
		void Line(float x1, float y1, float x2, float y2, Pixel color);
		void Plot(int x, int y, Pixel c);
		void LoadImage(char* a_File);
		void CopyTo(Surface* a_Dst, int a_X, int a_Y);
		void BlendCopyTo(Surface* a_Dst, int a_X, int a_Y);
		void CopyAlphaPlot(Surface* a_Dst, int a_X, int a_Y, int a_Alpha = 0xff000000);

		void ScaleColor(unsigned int a_Scale);
		void DrawBox(int x1, int y1, int x2, int y2, Pixel color);
		void Bar(int x1, int y1, int x2, int y2, Pixel color);
		void Resize(int a_Width, int a_Height, Surface* a_Orig);


		void CopyBox(Surface* Dst, int X, int Y, int Width, int Height, int srcPitch, int DestX, int DestY, Pixel Colour);
		void CopyColourBox(Surface* Dest, int SourceX, int SourceY, int Width, int Height, int dstPitch, int DestX, int DestY);
	private:
		// Attributes
		Pixel* m_Buffer;	
		int m_Width, m_Height, m_Pitch;	
		
	};



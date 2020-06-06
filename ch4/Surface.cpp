// Surface v3
// Original version IGAD/NHTV - Jacco Bikker - 2006-2009
// Modifed and adapted 2015 Brian Beuken


#define MALLOC64(x) _aligned_malloc(x,64)
#define FREE64(x) _aligned_free(x)

#include "stdio.h"
#include "math.h"
#include "string.h"
#include "stdlib.h"
#include "surface.h"  
 




	void NotifyUser(char* s);

	// -----------------------------------------------------------
	// True-color surface class implementation
	// -----------------------------------------------------------

	Surface::Surface(int a_Width, int a_Height, Pixel* a_Buffer)
		: m_Width(a_Width)
		, m_Height(a_Height)
		, m_Buffer(a_Buffer) 
		, m_Pitch(a_Width)

	{

	}

	Surface::Surface(int a_Width, int a_Height)
		: m_Width(a_Width)
		, m_Height(a_Height)
		, m_Pitch(a_Width)

	{
		m_Buffer = (Pixel*)malloc(a_Width * a_Height * sizeof(Pixel));
	}

	Surface::Surface(char* a_File, MyFiles* FileHandler)
		: m_Buffer(NULL)
		, m_Width(0)
		, m_Height(0)
		
	{
		// use the name to load the image and set up Height and Width
		m_Buffer = (Pixel*)FileHandler->Load(a_File, &m_Width, &m_Height);

		if (m_Buffer == NULL)
		{
			printf("File %s cannot be loaded, check name and dir \n", a_File);
		}
		else
		{
			printf("Buffer loaded with %s image, size is %i,%i\n",  a_File, m_Width, m_Height);
		}
		SetPitch(m_Width);

	}


	Surface::~Surface()
	{
		free(m_Buffer);
	}

	void Surface::ClearBuffer(Pixel a_Color)
	{
		//loop through and set to clear
		for (int i = 0; i < m_Width * m_Height; i++) m_Buffer[i] = a_Color;
	

	}

	

	void Surface::Resize(int a_Width, int a_Height, Surface* a_Orig)
	{
		unsigned int newpitch = (a_Width + 16) & 0xffff0; // calculate the new pitch size
		
		Pixel* src = a_Orig->GetBuffer(); 
		Pixel* dst = m_Buffer;  // set up source and destination for moves
		
		int u, v, owidth = a_Orig->GetWidth();
		int	oheight = a_Orig->GetHeight();
		int destx = (owidth << 10) / a_Width;
		int	desty = (oheight << 10) / a_Height;
		
		for (v = 0; v < a_Height; v++)
		{
			for (u = 0; u < a_Width; u++)
			{
				int su = u * destx, sv = v * desty;
		
				Pixel* s = src + (su >> 10) + (sv >> 10) * owidth;
				
				int ufrac = su & 1023, vfrac = sv & 1023;
				
				int w4 = (ufrac * vfrac) >> 12;
				int w3 = ((1023 - ufrac) * vfrac) >> 12;
				int w2 = (ufrac * (1023 - vfrac)) >> 12;
				int w1 = ((1023 - ufrac) * (1023 - vfrac)) >> 12;
				int x2 = ((su + destx) > ((owidth - 1) << 10)) ? 0 : 1;
				int y2 = ((sv + desty) > ((oheight - 1) << 10)) ? 0 : 1;
				Pixel p1 = *s, p2 = *(s + x2), p3 = *(s + owidth * y2), p4 = *(s + owidth * y2 + x2);
				unsigned int r = (((p1 & REDMASK) * w1 + (p2 & REDMASK) * w2 + (p3 & REDMASK) * w3 + (p4 & REDMASK) * w4) >> 8) & REDMASK;
				unsigned int g = (((p1 & GREENMASK) * w1 + (p2 & GREENMASK) * w2 + (p3 & GREENMASK) * w3 + (p4 & GREENMASK) * w4) >> 8) & GREENMASK;
				unsigned int b = (((p1 & BLUEMASK) * w1 + (p2 & BLUEMASK) * w2 + (p3 & BLUEMASK) * w3 + (p4 & BLUEMASK) * w4) >> 8) & BLUEMASK;
				*(dst + u + v * newpitch) = (Pixel)(r + g + b);
			}
		}
		// reset the members to new values
		m_Width = a_Width;
		m_Height = a_Height;
		m_Pitch = newpitch;
	}

	void Surface::Line(float x1, float y1, float x2, float y2, Pixel c)
	{
		if ((x1 < 0) || (y1 < 0) || (x1 >= m_Width) || (y1 >= m_Height) ||
			(x2 < 0) || (y2 < 0) || (x2 >= m_Width) || (y2 >= m_Height))
		{
			return;
		}
		float b = x2 - x1;
		float h = y2 - y1;
		float l = fabsf(b);
		if (fabsf(h) > l) l = fabsf(h);
		int il = (int)l;
		float dx = b / (float)l;
		float dy = h / (float)l;
		for (int i = 0; i <= il; i++)
		{
			*(m_Buffer + (int)x1 + (int)y1 * m_Pitch) = c;
			x1 += dx, y1 += dy;
		}
	}

	void Surface::Plot(int xpos, int ypos, Pixel p)
	{ 
		if ((xpos >= 0) && (ypos >= 0) && (xpos < m_Width) && (ypos < m_Height)) 
		{
			m_Buffer[xpos + (ypos * m_Pitch)] = p;
		}
	}

	void Surface::DrawBox(int xTL, int yTL, int xBR, int yBR, Pixel p)
	{
		Line((float)xTL, (float)yTL, (float)xBR, (float)yBR, p);
		Line((float)xBR, (float)yTL, (float)xBR, (float)yBR, p);
		Line((float)xTL, (float)yBR, (float)xBR, (float)yBR, p);
		Line((float)xTL, (float)yTL, (float)xTL, (float)yBR, p);
	}

	void Surface::Bar(int x1, int y1, int x2, int y2, Pixel c)
	{
		Pixel* a = x1 + y1 * m_Pitch + m_Buffer;
		for (int y = y1; y <= y2; y++)
		{
			for (int x = 0; x <= (x2 - x1); x++) a[x] = c;
			a += m_Pitch;
		}
	}

	void Surface::CopyTo(Surface* a_Dst, int a_X, int a_Y)
	{
		Pixel* dst = a_Dst->GetBuffer();
		Pixel* src = m_Buffer;
		if ((src) && (dst)) 
		{
			int srcwidth = m_Width;
			int srcheight = m_Height;
			int srcpitch = m_Pitch;
			int dstwidth = a_Dst->GetWidth();
			int dstheight = a_Dst->GetHeight();
			int dstpitch = a_Dst->GetPitch();
			if ((srcwidth + a_X) > dstwidth) srcwidth = dstwidth - a_X;
			if ((srcheight + a_Y) > dstheight) srcheight = dstheight - a_Y;
			if (a_X < 0) src -= a_X, srcwidth += a_X, a_X = 0;
			if (a_Y < 0) src -= a_Y * srcpitch, srcheight += a_Y, a_Y = 0;
			if ((srcwidth > 0) && (srcheight > 0))
			{
				dst += a_X + dstpitch * a_Y;
				for (int y = 0; y < srcheight; y++)
				{
					memcpy(dst, src, srcwidth * 4);
					dst += dstpitch;
					src += srcpitch;
				}
			}
		}
	}

	void Surface::BlendCopyTo(Surface* a_Dst, int a_X, int a_Y)
	{
		Pixel* dst = a_Dst->GetBuffer();
		Pixel* src = m_Buffer;
		if ((src) && (dst)) 
		{
			int srcwidth = m_Width;
			int srcheight = m_Height;
			int srcpitch = m_Pitch;
			int dstwidth = a_Dst->GetWidth();
			int dstheight = a_Dst->GetHeight();
			int dstpitch = a_Dst->GetPitch();
			if ((srcwidth + a_X) > dstwidth) srcwidth = dstwidth - a_X;
			if ((srcheight + a_Y) > dstheight) srcheight = dstheight - a_Y;
			if (a_X < 0) src -= a_X, srcwidth += a_X, a_X = 0;
			if (a_Y < 0) src -= a_Y * srcpitch, srcheight += a_Y, a_Y = 0;
			if ((srcwidth > 0) && (srcheight > 0))
			{
				dst += a_X + dstpitch * a_Y;
				for (int y = 0; y < srcheight; y++)
				{
					for (int x = 0; x < srcwidth; x++) dst[x] = AddBlend(dst[x], src[x]);
					dst += dstpitch;
					src += srcpitch;
				}
			}
		}
	}

	
	void Surface::CopyAlphaPlot(Surface* a_Dst, int a_X, int a_Y, int a_Alpha ) 
	{
		Pixel* dst = a_Dst->GetBuffer();
		Pixel* src = m_Buffer;
		Pixel p;
		if ((src) && (dst))
		{
			int srcwidth = m_Width;
			int srcheight = m_Height;
			int srcpitch = m_Pitch;
			int dstwidth = a_Dst->GetWidth();
			int dstheight = a_Dst->GetHeight();
			int dstpitch = a_Dst->GetPitch();
			if ((srcwidth + a_X) > dstwidth) srcwidth = dstwidth - a_X;
			if ((srcheight + a_Y) > dstheight) srcheight = dstheight - a_Y;
			if (a_X < 0) src -= a_X, srcwidth += a_X, a_X = 0;
			if (a_Y < 0) src -= a_Y * srcpitch, srcheight += a_Y, a_Y = 0;
			if ((srcwidth > 0) && (srcheight > 0))
			{
				dst += a_X + dstpitch * a_Y;
				for (int y = 0; y < srcheight; y++)
				{
					
					for (int x = 0; x < srcwidth; x++)
					{
						
						if (src[x] & ALPHAMASK) dst[x] = src[x]+a_Alpha; // this will stop us drawing colours but alpha 0
					}
					dst += dstpitch;
					src += srcpitch;
				}
			}
		}
	}



// this is almost exclusivly used by a tilefont system
	void Surface::CopyBox(Surface* Dest, int SourceX, int SourceY,int Width, int Height, int dstPitch, int DestX, int DestY, Pixel colour)
	{
		Pixel* dst = Dest->GetBuffer();
		Pixel* src = GetBuffer();
		src += SourceY*(this->GetPitch()*Height);
		src += SourceX*(Width);
		if ((src) && (dst))
		{
			dst += DestX + (dstPitch * DestY);
				for (int y = 0; y < Height; y++)
				{
					for (int x = 0; x < Width; x++)
					{
						if (src[x] & ALPHAMASK) dst[x] = colour;
					}
					dst += dstPitch;
					src += GetPitch();
				}
		}
	}

// this is a normal colour box copy
void Surface::CopyColourBox(Surface* Dest, int SourceX, int SourceY, int Width, int Height, int dstPitch, int DestX, int DestY)
{
	Pixel* dst = Dest->GetBuffer();
	Pixel* src = GetBuffer();
	src += SourceY*(this->GetPitch()*Height);
	src += SourceX*(Width);
	if ((src) && (dst))
	{
		dst += DestX + (dstPitch * DestY);
		for (int y = 0; y < Height; y++)
		{ 
		if (DestY >= Dest->GetHeight()) continue ;

			for (int x = 0; x < Width; x++)
			{
				dst[x] = src[x];
			}
			DestY++;
			dst += dstPitch;
			src += GetPitch();
		}
	}
}




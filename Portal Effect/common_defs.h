

#ifndef COMMON_DEF
	#define COMMON_DEF

	// Global defs
//	#define		USE_MESH

//	#define		USE_SHADOWS

//	#define		USE_SHADERS

//	#define		USE_JPEG


	#define TEX_SIZE		2048	
	#define LIGHT_NEAR		0.5
	#define LIGHT_FAR		300.0
	#define DEGtoRAD		(3.141592/180.0)

	#ifdef _MSC_VER
		#include <windows.h>
	#else
		typedef unsigned int		DWORD;
	#endif

	#define COLOR(r,g,b)	( (DWORD(r*255.0f)<<24) | (DWORD(g*255.0f)<<16) | (DWORD(b*255.0f)<<8) )
	#define COLORA(r,g,b,a)	( (DWORD(r*255.0f)<<24) | (DWORD(g*255.0f)<<16) | (DWORD(b*255.0f)<<8) | DWORD(a*255.0f) )
	#define RED(c)			(float((c>>24) & 0xFF)/255.0)
	#define GRN(c)			(float((c>>16) & 0xFF)/255.0)
	#define BLUE(c)			(float((c>>8) & 0xFF)/255.0)
	#define ALPH(c)			(float(c & 0xFF)/255.0)
#endif



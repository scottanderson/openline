#if WIN32

/* (c)2003 Hotsprings Inc. Licensed under GPL - see LICENSE in HotlineSources diresctory */

#include "UMath.h"

#undef cos
#undef sin
#undef tan
#undef acos
#undef asin
#undef atan
#undef sqrt

// UMath.h declares this "extern const fast_float pi;" -- a const at namespace scope
// has internal linkage by default in C++ unless marked extern too, so without it this
// silently created a second, unrelated "pi" instead of defining the declared one.
extern const double pi = 3.14159265358979323846;

/* -------------------------------------------------------------------------- */

Int32 UMath::CalcRandomSeed()
{
	return ::GetTickCount();
}

extern "C" {
double cos(double);
double sin(double);
double tan(double);
double acos(double);
double asin(double);
double atan(double);
double sqrt(double);
}

fast_float UMath::Cosine(fast_float x)
{
	return cos(x);
}

fast_float UMath::Sine(fast_float x)
{
	return sin(x);
}

fast_float UMath::Tangent(fast_float x)
{
	return tan(x);
}

fast_float UMath::ArcCosine(fast_float x)
{
	return acos(x);
}

fast_float UMath::ArcSine(fast_float x)
{
	return asin(x);
}

fast_float UMath::ArcTangent(fast_float x)
{
	return atan(x);
}

fast_float UMath::SquareRoot(fast_float x)
{
	return sqrt(x);
}




#endif

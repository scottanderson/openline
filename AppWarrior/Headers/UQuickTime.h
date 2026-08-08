/* (c)2003 Hotsprings Inc. Licensed under GPL - see LICENSE in HotlineSources diresctory */

// HL_NO_QUICKTIME: the vendored Apple QuickTime SDK (which supplied QTML.h/Movies.h/
// Gestalt.h and, on Windows, a Mac-Toolbox compatibility layer) was removed from this
// repo for licensing reasons. When HL_NO_QUICKTIME is defined we skip these headers
// entirely and provide just enough of the classic Mac/QuickTime types below to keep
// the (otherwise platform-agnostic) view-hierarchy headers like CView.h and
// CQuickTimeView.h compiling. See UOperatingSystem::InitQuickTime() (always returns
// false in this build) and CQuickTimeView(NoQT).cp for the corresponding stub bodies.
#ifndef HL_NO_QUICKTIME

#include <QTML.h>
#include <Movies.h>
#include <Gestalt.h>

#include "HL_Handler.h"

#else // HL_NO_QUICKTIME

typedef int Boolean;
#define pascal

typedef void* Movie;
typedef void* MovieController;
typedef void* ComponentInstance;
typedef void* Handle;

struct Rect
{
	short top, left, bottom, right;
};

struct EventRecord
{
	unsigned short what;
	unsigned long message;
	unsigned long when;
	long whereH, whereV;
	unsigned short modifiers;
};

#endif // HL_NO_QUICKTIME

/* (c)2003 Hotsprings Inc. Licensed under GPL - see LICENSE in HotlineSources diresctory */

// HL_NO_QUICKTIME stub: this file replaces CQuickTimeView.cp when building without the
// (removed, for licensing reasons) Apple QuickTime SDK. CQuickTimeView is instantiated
// unconditionally by the client's UI code (see Apps/Client/Source/Hotline.h /
// HotlineWindows.cp), so the class still needs to exist and link -- it just never
// actually plays any media, since UOperatingSystem::IsQuickTimeAvailable() is always
// false in this build and every caller already checks that before doing real work.
//
// Only built when HL_NO_QUICKTIME is defined; CMakeLists.txt picks either this file or
// the real CQuickTimeView.cp, never both. The real CQuickTimeView.cp is left completely
// untouched so a full QuickTime build stays possible if the SDK is ever reinstated.

#include "CQuickTimeView.h"

Uint32 CQuickTimeView::sMovieCount = 0L;

CQuickTimeView::CQuickTimeView(CViewHandler *inHandler, const SRect& inBounds, Uint16 inResizeOptions, Uint16 inOptions)
	: CLaunchUrlView(inHandler, inBounds)
{
	mMovie = nil;
	mController = nil;
	mInstance = nil;

	mIsVideoTrack = false;
	mTempFile = nil;

	mResizeOptions = inResizeOptions;
	mOptions = inOptions;
	mMaxHorizSize = 0;
	mMaxVertSize = 0;
}

CQuickTimeView::~CQuickTimeView()
{
	CloseMovie();
}

bool CQuickTimeView::SelectMovie()						{ return false; }
bool CQuickTimeView::StreamMovie(Int8 * /* inAddress */)	{ return false; }
bool CQuickTimeView::SetMovie(void*, Uint32, Uint32)		{ return false; }

bool CQuickTimeView::StartMovie()							{ return false; }
bool CQuickTimeView::StopMovie()							{ return false; }
void CQuickTimeView::CloseMovie()							{ /* nothing ever opened */ }
bool CQuickTimeView::StopStreamMovie()						{ return false; }
void CQuickTimeView::SetMaxMovieSize(Uint32 inMaxHorizSize, Uint32 inMaxVertSize)
{
	mMaxHorizSize = inMaxHorizSize;
	mMaxVertSize = inMaxVertSize;
}

bool CQuickTimeView::SetBounds(const SRect& inBounds)		{ return CLaunchUrlView::SetBounds(inBounds); }
bool CQuickTimeView::SetVisible(bool inVisible)			{ return CLaunchUrlView::SetVisible(inVisible); }
bool CQuickTimeView::SetEnable(bool inEnable)				{ return CLaunchUrlView::SetEnable(inEnable); }
bool CQuickTimeView::ChangeState(Uint16 inState)			{ return CLaunchUrlView::ChangeState(inState); }

bool CQuickTimeView::UpdateQuickTimeBounds()				{ return false; }
void CQuickTimeView::UpdateQuickTime()						{ }
void CQuickTimeView::SendToQuickTime(const EventRecord&)	{ }
bool CQuickTimeView::SaveMovieAs()							{ return false; }

// no format is "supported" -- there's no QuickTime to hand it to
bool CQuickTimeView::IsSupported(Uint32 /* inTypeCode */)	{ return false; }
bool CQuickTimeView::IsVideoTrack()						{ return mIsVideoTrack; }

void CQuickTimeView::MouseDown(const SMouseMsgData& inInfo)	{ CLaunchUrlView::MouseDown(inInfo);	}
void CQuickTimeView::MouseUp(const SMouseMsgData& inInfo)		{ CLaunchUrlView::MouseUp(inInfo);		}
void CQuickTimeView::MouseEnter(const SMouseMsgData& inInfo)	{ CLaunchUrlView::MouseEnter(inInfo);	}
void CQuickTimeView::MouseMove(const SMouseMsgData& inInfo)	{ CLaunchUrlView::MouseMove(inInfo);	}
void CQuickTimeView::MouseLeave(const SMouseMsgData& inInfo)	{ CLaunchUrlView::MouseLeave(inInfo);	}

bool CQuickTimeView::MakeMovieFromFile(TFSRefObj*)					{ return false; }
bool CQuickTimeView::MakeMovieFromUrlHandle(Handle, bool)				{ return false; }
bool CQuickTimeView::MakeController(TWindow, Movie)					{ return false; }
bool CQuickTimeView::MakeCustomMenu(EventRecord*)						{ return false; }
void CQuickTimeView::CalculateMovieSize(Rect&, Uint32& outWidth, Uint32& outHeight)
{
	outWidth = 0;
	outHeight = 0;
}

void CQuickTimeView::SetWindowSize()						{ }
void CQuickTimeView::SetMovieSize(const Rect&)				{ }
void CQuickTimeView::GetMovieSize(Rect& outBounds)
{
	outBounds.top = outBounds.left = outBounds.bottom = outBounds.right = 0;
}

bool CQuickTimeView::MovieContains(const SPoint& /* inLocation */)	{ return false; }

/* (c)2003 Hotsprings Inc. Licensed under GPL - see LICENSE in HotlineSources diresctory */

// This is a stand-in for the CodeWarrior "prefix file" (AWHeaders(W).h / the .pch
// files under AppWarrior/Libraries) that the original Metrowerks project force-included
// into every translation unit via project settings. Since CMake/GCC has no equivalent
// project setting, we force-include this file on the command line instead (see
// target_compile_options(... -include ...) in AppWarrior/CMakeLists.txt and
// Apps/Client/CMakeLists.txt). That's why hundreds of .cp files in this codebase use
// things like UMemory, UText, SPoint, or plain Win32 types without including any
// header themselves -- they always relied on the prefix file for that.
//
// Include order below is hand-sorted (roughly) by dependency depth; a handful of
// AppWarrior headers assume types from "earlier" headers are already visible rather
// than including them directly (again, relying on the prefix file), so order matters.
#pragma once

#ifndef WIN32
#define WIN32 1
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#ifndef _WIN32_IE
#define _WIN32_IE 0x0500
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shellapi.h>
#include <ole2.h>
#include <oleauto.h>
#include <mmsystem.h>
#include <wininet.h>

// --- fundamentals --------------------------------------------------------------
#include "typedefs.h"
#include "MoreTypes.h"
#include "GrafTypes.h"
#include "ANSI.h"

// --- generic data structures -----------------------------------------------------
#include "CBoolArray.h"
#include "CLinkedList.h"
#include "CPtrList.h"
#include "CPtrTree.h"

// --- graphics primitives (defines TImage, needed by ImageTypes.h below) -----------
#include "UGraphics.h"
#include "UIcon.h"
#include "UPixmap.h"
#include "URegion.h"
#include "ImageTypes.h"

// --- core utility classes ---------------------------------------------------------
#include "UMemory.h"
#include "UText.h"
#include "UError.h"
#include "UDebug.h"
#include "UMath.h"
#include "UDateTime.h"
#include "UGUID.h"
#include "UDigest.h"
#include "UMime.h"
#include "UBitString.h"
#include "UFieldData.h"
#include "UIDVarArray.h"
#include "UMessageSys.h"
#include "URez.h"
#include "UZlibCompress.h"

// CFlatten needs SDateTimeStamp (from UDateTime.h above)
#include "CFlatten.h"

#include "UFileSys.h"

// --- application/system services ---------------------------------------------------
#include "UApplication.h"
#include "UTimer.h"
#include "UProgramCleanup.h"
#include "UOperatingSystem.h"
#include "UClipboard.h"
#include "UKeyboard.h"
#include "UMouse.h"
#include "USound.h"
#include "UExternalApp.h"
#include "UOleAutomation.h"
#include "UService.h"

// --- networking ---------------------------------------------------------------
#include "UTransport.h"
#include "UTransact.h"
#include "URegularTransport.h"
#include "UHttpTransport.h"
#include "UHttpTransact.h"
#include "UNntpTransact.h"

// --- view hierarchy (CView.h pulls in its own UGraphics/UDragAndDrop/UApplication/
// UTimer/UQuickTime deps) -------------------------------------------------------
#include "UDragAndDrop.h"
#include "CView.h"
#include "CDragAndDroppable.h"

#include "UUserInterface.h"
#include "UTooltip.h"
#include "UProgressBar.h"
#include "UScrollBar.h"
#include "UButton.h"
#include "UCheckBox.h"
#include "UIconButton.h"
#include "UEditText.h"
#include "UWindow.h"

#include "CApplication.h"
#include "CWindow.h"
#include "CWizard.h"
#include "MsgBox.h"

#include "CContainerView.h"
#include "CViewContainer.h"
#include "CScrollerView.h"
#include "CPaneView.h"
#include "CBoxView.h"
#include "CColorView.h"
#include "CSeparatorView.h"
#include "CButtonView.h"
#include "CCheckBoxView.h"
#include "CIconButtonView.h"
#include "CSimpleIconButtonView.h"
#include "CIconView.h"
#include "CImageView.h"
#include "CDecompressImage.h"
#include "CDecompressBitmap.h"
#include "CDecompressGif.h"
#include "CDecompressJpeg.h"
#include "CDecompressPict.h"
#include "CAnimatedGifView.h"
#include "CPictureView.h"
#include "CLabelView.h"
#include "CLabelUrlView.h"
#include "CLaunchUrlView.h"
#include "CQuickTimeView.h"
#include "CTextView.h"
#include "CPasswordTextView.h"
#include "CProgressView.h"
#include "CItemsView.h"
#include "CListView.h"
#include "CGeneralListView.h"
#include "CTabbedItemsView.h"
#include "CTabbedListView.h"
#include "CGeneralTabbedListView.h"
#include "CTreeView.h"
#include "CTabbedTreeView.h"
#include "CTabbedView.h"

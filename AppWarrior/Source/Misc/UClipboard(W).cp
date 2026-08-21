#if WIN32

/* (c)2003 Hotsprings Inc. Licensed under GPL - see LICENSE in HotlineSources diresctory */

#include "UClipboard.h"

static bool _CLIsOpen = false;

HGLOBAL _HdlToWinHdl(THdl inHdl, Uint32 *outSize);

Uint32 _MIMEToWinClipFormat(const void *inText, Uint32 inSize);

void _FailLastWinError(const Int8 *inFile, Uint32 inLine);
void _FailWinError(Int32 inWinError, const Int8 *inFile, Uint32 inLine);
#if DEBUG
	#define FailLastWinError()		_FailLastWinError(__FILE__, __LINE__)
	#define FailWinError(id)		_FailWinError(id, __FILE__, __LINE__)
#else
	#define FailLastWinError()		_FailLastWinError(nil, 0)
	#define FailWinError(id)		_FailWinError(id, nil, 0)
#endif

TImage _NewDCImage(HDC inDC, Uint32 inWidth, Uint32 inHeight);

/* -------------------------------------------------------------------------- */

// returns nil if no data
THdl UClipboard::GetData(const Int8 *inType)
{
	Uint32 format = _MIMEToWinClipFormat(inType, strlen(inType));
	THdl hdl = nil;

	if (format && ::OpenClipboard(NULL))
	{
		// CF_TEXT alone loses characters like the Mac Roman versions of the trademark
		// sign, infinity, root, etc: Windows only stores CF_UNICODETEXT for Unicode
		// clipboard sources and synthesizes CF_TEXT from it on demand using its own
		// lossy "best fit" substitution (eg turning them into unrelated ASCII
		// look-alikes) before we ever see the data. Read CF_UNICODETEXT directly and
		// map straight to Mac Roman by code point to avoid that lossy detour.
		if (format == CF_TEXT && ::IsClipboardFormatAvailable(CF_UNICODETEXT))
		{
			HANDLE hUni = ::GetClipboardData(CF_UNICODETEXT);
			if (hUni)
			{
				extern Uint8 _UTUnicodeCharToMacRoman(Uint16 inChar);
				Uint16 *pUni = (Uint16 *)::GlobalLock(hUni);
				try
				{
					Uint32 sUni = 0;
					while (pUni[sUni]) sUni++;

					hdl = UMemory::NewHandle(sUni);
					Uint8 *q = (Uint8 *)UMemory::Lock(hdl);
					for (Uint32 i = 0; i < sUni; i++)
						q[i] = _UTUnicodeCharToMacRoman(pUni[i]);
					UMemory::Unlock(hdl);
				}
				catch(...)
				{
					::GlobalUnlock(hUni);
					::CloseClipboard();
					UMemory::Dispose(hdl);
					throw;
				}
				::GlobalUnlock(hUni);

				UMemory::SearchAndReplaceAll(hdl, 0, "\x0A", 1, nil, 0);
				::CloseClipboard();
				return hdl;
			}
		}

		HANDLE h = ::GetClipboardData(format);
		if (h)
		{
			Uint32 s = ::GlobalSize(h);
			void *p = ::GlobalLock(h);
			try
			{
				if (format == CF_TEXT)
					s = strlen((char *)p);
				
				hdl = UMemory::NewHandle(p, s);
				
				if (format == CF_TEXT)
				{
					// CF_TEXT is CP1252; convert to Mac Roman, this app's internal encoding.
					extern const char _UTCharMap_PCToAW[];
					Uint8 *q = (Uint8 *)UMemory::Lock(hdl);
					for (Uint32 i = 0; i < s; i++)
						q[i] = (Uint8)_UTCharMap_PCToAW[q[i]];
					UMemory::Unlock(hdl);
				}

				// strip linefeed chars
				UMemory::SearchAndReplaceAll(hdl, 0, "\x0A", 1, nil, 0);
			}
			catch(...)
			{
				::GlobalUnlock(h);
				::CloseClipboard();
				UMemory::Dispose(hdl);
				throw;
			}
			::GlobalUnlock(h);
		}
		::CloseClipboard();
	}
	
	return hdl;
}

// builds a CF_UNICODETEXT handle from Mac Roman <inData>, expanding CR to CRLF like the
// CF_TEXT path does, so text with characters outside CP1252 (eg the Mac Roman trademark,
// infinity, root, etc.) round-trips losslessly to any Unicode-aware paste target instead
// of degrading to '?'. Returns NULL on empty input.
static HANDLE _CLMakeUnicodeHandle(const void *inData, Uint32 inDataSize)
{
	extern const Uint16 _UTCharMap_AWToUnicode[256];
	const Uint8 *p = (const Uint8 *)inData;

	Uint32 nChars = 0;
	for (Uint32 i = 0; i < inDataSize; i++)
		nChars += (p[i] == 0x0D) ? 2 : 1;

	HANDLE h = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (nChars+1) * sizeof(Uint16));
	if (h == NULL) Fail(errorType_Memory, memError_NotEnough);

	Uint16 *q = (Uint16 *)::GlobalLock(h);
	for (Uint32 i = 0; i < inDataSize; i++)
	{
		if (p[i] == 0x0D)
		{
			*q++ = 0x0D;
			*q++ = 0x0A;
		}
		else
			*q++ = _UTCharMap_AWToUnicode[p[i]];
	}
	*q = 0;
	::GlobalUnlock(h);

	return h;
}

// if you just want one item on the clipboard, you don't have to call BeginSet/EndSet
void UClipboard::SetData(const Int8 *inType, const void *inData, Uint32 inDataSize)
{
	Uint32 err, format;
	Uint8 *p;
	HANDLE h;
	HANDLE hUni = NULL;

	format = _MIMEToWinClipFormat(inType, strlen(inType));
	if (format == 0) return;

	if (format == CF_TEXT)
	{
		h = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, inDataSize+1+4);
		if (h == NULL) Fail(errorType_Memory, memError_NotEnough);
		
		p = (Uint8 *)::GlobalLock(h);
		*(*(Uint32 **)&p)++ = inDataSize+1;
		::CopyMemory(p, inData, inDataSize);
		p[inDataSize] = 0;		// must be null-terminated

		// mirror of GetData()'s conversion: Mac Roman -> CP1252 going out to the clipboard.
		{
			extern const char _UTCharMap_AWToPC[];
			for (Uint32 i = 0; i < inDataSize; i++)
				p[i] = (Uint8)_UTCharMap_AWToPC[p[i]];
		}

		::GlobalUnlock(h);

		// need to convert CR (AW) to CRLF (DOS/windoze)
		try
		{
			UMemory::SearchAndReplaceAll((THdl)h, 0, "\x0D", 1, "\x0D\x0A", 2);
			_HdlToWinHdl((THdl)h, nil);
		}
		catch(...)
		{
			::GlobalFree(h);
			throw;
		}

		// also offer CF_UNICODETEXT so Unicode-aware paste targets get every Mac Roman
		// character losslessly, not just the ones that also exist in CP1252.
		try
		{
			hUni = _CLMakeUnicodeHandle(inData, inDataSize);
		}
		catch(...)
		{
			::GlobalFree(h);
			throw;
		}
	}
	else
	{
		h = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, inDataSize);
		if (h == NULL) Fail(errorType_Memory, memError_NotEnough);
		
		p = (Uint8 *)::GlobalLock(h);
		::CopyMemory(p, inData, inDataSize);
		::GlobalUnlock(h);
	}
	
	if (!_CLIsOpen && !::OpenClipboard(NULL))
	{
		err = ::GetLastError();
		::GlobalFree(h);
		if (hUni) ::GlobalFree(hUni);
		FailWinError(err);
	}
	
	if (!::EmptyClipboard())
	{
		err = ::GetLastError();
		::GlobalFree(h);
		if (hUni) ::GlobalFree(hUni);
		FailWinError(err);
	}
	
	if (::SetClipboardData(format, h) == NULL)
	{
		err = ::GetLastError();
		::GlobalFree(h);
	}
	else
		err = 0;
	
	if (hUni)
	{
		if (::SetClipboardData(CF_UNICODETEXT, hUni) == NULL)
			::GlobalFree(hUni);
	}

	if (!_CLIsOpen) ::CloseClipboard();
	
	if (err) FailWinError(err);
}

void UClipboard::BeginSet()
{
	if (_CLIsOpen)
	{
		DebugBreak("UClipboard - BeginSet() already called");
		Fail(errorType_Misc, error_Protocol);
	}
	
	if (!::OpenClipboard(NULL)) FailLastWinError();
	_CLIsOpen = true;
}

void UClipboard::EndSet()
{
	if (_CLIsOpen)
	{
		_CLIsOpen = false;
		::CloseClipboard();
	}
}

// does not modify nor take ownership of <inImage>
void UClipboard::SetImageData(TImage inImage, const SRect *inRect, Uint32 /* inOptions */)
{
	#pragma unused(inRect)
	Require(inImage);
	Fail(errorType_Misc, error_Unimplemented);
}

TImage UClipboard::GetImageData(SRect *outRect, Uint32 /* inOptions */)
{
	TImage img = nil;
	HDC dc = NULL;
	HBITMAP bm = NULL;
	
	if (::OpenClipboard(NULL))
	{
		HANDLE h = (HBITMAP)::GetClipboardData(CF_DIB);
		if (h)
		{
			BITMAPINFO *bmi = (BITMAPINFO *)::GlobalLock(h);
			try
			{
				dc = ::CreateCompatibleDC(NULL);
				if (dc == NULL) Fail(errorType_Memory, memError_NotEnough);
				
				void *dibBits;
				bm = CreateDIBSection(dc, bmi, DIB_RGB_COLORS, &dibBits, NULL, 0);
				if (bm == NULL) FailLastWinError();
				GdiFlush();

				::CopyMemory(dibBits, BPTR(bmi) + bmi->bmiHeader.biSize + (bmi->bmiHeader.biClrUsed * sizeof(RGBQUAD)), bmi->bmiHeader.biSizeImage);

				::SelectObject(dc, bm);
				
				if (outRect) outRect->Set(0, 0, bmi->bmiHeader.biWidth, abs(bmi->bmiHeader.biHeight));
				
				img = _NewDCImage(dc, bmi->bmiHeader.biWidth, abs(bmi->bmiHeader.biHeight));
			}
			catch(...)
			{
				::GlobalUnlock(h);
				if (dc) DeleteDC(dc);
				if (bm) DeleteObject(bm);
				::CloseClipboard();
				throw;
			}
			::GlobalUnlock(h);
		}
		::CloseClipboard();
	}
	
	return img;
}

// does not modify nor take ownership of <inHdl>
void UClipboard::SetSoundData(THdl inHdl, Uint32 /* inOptions */)
{
	Require(inHdl);
	Fail(errorType_Misc, error_Unimplemented);
}

THdl UClipboard::GetSoundData(Uint32 /* inOptions */)
{
	Fail(errorType_Misc, error_Unimplemented);
	return nil;
}


/* -------------------------------------------------------------------------- */
#pragma mark -

Uint32 _MIMEToWinClipFormat(const void *inText, Uint32 inSize)
{
	if (inSize == 10 && UMemory::Equal(inText, "text/plain", 10))
		return CF_TEXT;
	
	return 0;
}



#endif /* WIN32 */

#if WIN32

/* (c)2003 Hotsprings Inc. Licensed under GPL - see LICENSE in HotlineSources diresctory */

#include "UText.h"

/* -------------------------------------------------------------------------- */
// Maps Mac OS Roman (the encoding AppWarrior/the Hotline protocol store text in, a legacy of
// this app's classic-Mac origins) to Windows-1252/CP1252 (what Win32 GDI text APIs such as
// DrawTextA expect) so text drawn/exported on Windows renders with the right glyphs.
//
// The previous version of this table mapped dozens of ordinary, frequently-used Mac Roman
// characters (curly quotes, em/en dashes, bullet, trademark, accented capitals, etc.) to
// \x00. Since callers pass an explicit byte count rather than relying on a null terminator,
// those bytes were still "drawn" -- as an empty/tofu glyph -- but any code that treats the
// mapped buffer as a null-terminated C string (e.g. the CF_TEXT drag-export path in
// UDragAndDrop(W).cp, which reuses this table) would stop at that spurious embedded NUL,
// silently truncating the string. This is why server names/messages containing any of those
// characters rendered with stray boxes on screen and were truncated at the first such
// character when copied out via drag-and-drop.
//
// Fixed by mapping every Mac Roman character (0x80-0xFF) to its correct CP1252 equivalent.
// A small number of Mac Roman characters (math symbols, ligatures, the Apple logo, etc.) have
// no CP1252 equivalent; those map to '?' (0x3F) instead of \x00 so they degrade to a visible,
// non-terminating placeholder rather than corrupting the string.
extern const char _UTCharMap_AWToPC[] = ""
	"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
	"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
	"\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"
	"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"
	"\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F"
	"\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\x5B\x5C\x5D\x5E\x5F"
	"\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F"
	"\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\x7B\x7C\x7D\x7E\x7F"
	"\xC4\xC5\xC7\xC9\xD1\xD6\xDC\xE1\xE0\xE2\xE4\xE3\xE5\xE7\xE9\xE8"
	"\xEA\xEB\xED\xEC\xEE\xEF\xF1\xF3\xF2\xF4\xF6\xF5\xFA\xF9\xFB\xFC"
	"\x86\xB0\xA2\xA3\xA7\x95\xB6\xDF\xAE\xA9\x99\xB4\xA8\x3F\xC6\xD8"
	"\x3F\xB1\x3F\x3F\xA5\xB5\x3F\x3F\x3F\x3F\x3F\xAA\xBA\x3F\xE6\xF8"
	"\xBF\xA1\xAC\x3F\x83\x3F\x3F\xAB\xBB\x85\xA0\xC0\xC3\xD5\x8C\x9C"
	"\x96\x97\x93\x94\x91\x92\xF7\x3F\xFF\x9F\x3F\x80\x8B\x9B\x3F\x3F"
	"\x87\xB7\x82\x84\x89\xC2\xCA\xC1\xCB\xC8\xCD\xCE\xCF\xCC\xD3\xD4"
	"\x3F\xD2\xDA\xDB\xD9\x3F\x88\x98\xAF\x3F\x3F\x3F\xB8\x3F\x3F\x3F";




Uint32 UText::GetCaretTime()
{
	return ::GetCaretBlinkTime();
}




#endif /* WIN32 */

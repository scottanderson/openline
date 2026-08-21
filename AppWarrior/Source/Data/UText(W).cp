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

// Inverse of _UTCharMap_AWToPC: CP1252 (keyboard/clipboard input) -> Mac Roman (this app's
// internal encoding), needed so extended characters round-trip. Built from the real Mac
// Roman/CP1252 code points rather than inverted from AWToPC, which isn't bijective (several
// Mac Roman chars have no CP1252 equivalent and collide on '?'); unmapped bytes are '?' too.
extern const char _UTCharMap_PCToAW[] = ""
	"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
	"\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
	"\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"
	"\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"
	"\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F"
	"\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\x5B\x5C\x5D\x5E\x5F"
	"\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F"
	"\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\x7B\x7C\x7D\x7E\x7F"
	"\xDB\x3F\xE2\xC4\xE3\xC9\xA0\xE0\xF6\xE4\x3F\xDC\xCE\x3F\x3F\x3F"
	"\x3F\xD4\xD5\xD2\xD3\xA5\xD0\xD1\xF7\xAA\x3F\xDD\xCF\x3F\x3F\xD9"
	"\xCA\xC1\xA2\xA3\x3F\xB4\x3F\xA4\xAC\xA9\xBB\xC7\xC2\x3F\xA8\xF8"
	"\xA1\xB1\x3F\x3F\xAB\xB5\xA6\xE1\xFC\x3F\xBC\xC8\x3F\x3F\x3F\xC0"
	"\xCB\xE7\xE5\xCC\x80\x81\xAE\x82\xE9\x83\xE6\xE8\xED\xEA\xEB\xEC"
	"\x3F\x84\xF1\xEE\xEF\xCD\x85\x3F\xAF\xF4\xF2\xF3\x86\x3F\x3F\xA7"
	"\x88\x87\x89\x8B\x8A\x8C\xBE\x8D\x8F\x8E\x90\x91\x93\x92\x94\x95"
	"\x3F\x96\x98\x97\x99\x9B\x9A\xD6\xBF\x9D\x9C\x9E\x9F\x3F\x3F\xD8";

// Unlike _UTCharMap_PCToAW (CP1252 byte -> Mac Roman), this maps an actual Unicode
// code point straight to Mac Roman, so callers reading CF_UNICODETEXT off the
// clipboard can represent every Mac Roman character losslessly -- including the
// ~40 that have no CP1252 equivalent (math symbols, extra Greek letters, ligatures,
// spacing modifier letters) and would otherwise have already been mangled by
// Windows' lossy Unicode -> CP1252 "best fit" substitution before this app ever
// saw them (eg turning the Mac Roman trademark/infinity/root/etc. characters into
// unrelated ASCII look-alikes rather than '?').
Uint8 _UTUnicodeCharToMacRoman(Uint16 inChar)
{
	if (inChar <= 0x7F)
		return (Uint8)inChar;

	switch (inChar)
	{
		case 0x00C4:	return 0x80;	// Ä
		case 0x00C5:	return 0x81;	// Å
		case 0x00C7:	return 0x82;	// Ç
		case 0x00C9:	return 0x83;	// É
		case 0x00D1:	return 0x84;	// Ñ
		case 0x00D6:	return 0x85;	// Ö
		case 0x00DC:	return 0x86;	// Ü
		case 0x00E1:	return 0x87;	// á
		case 0x00E0:	return 0x88;	// à
		case 0x00E2:	return 0x89;	// â
		case 0x00E4:	return 0x8A;	// ä
		case 0x00E3:	return 0x8B;	// ã
		case 0x00E5:	return 0x8C;	// å
		case 0x00E7:	return 0x8D;	// ç
		case 0x00E9:	return 0x8E;	// é
		case 0x00E8:	return 0x8F;	// è
		case 0x00EA:	return 0x90;	// ê
		case 0x00EB:	return 0x91;	// ë
		case 0x00ED:	return 0x92;	// í
		case 0x00EC:	return 0x93;	// ì
		case 0x00EE:	return 0x94;	// î
		case 0x00EF:	return 0x95;	// ï
		case 0x00F1:	return 0x96;	// ñ
		case 0x00F3:	return 0x97;	// ó
		case 0x00F2:	return 0x98;	// ò
		case 0x00F4:	return 0x99;	// ô
		case 0x00F6:	return 0x9A;	// ö
		case 0x00F5:	return 0x9B;	// õ
		case 0x00FA:	return 0x9C;	// ú
		case 0x00F9:	return 0x9D;	// ù
		case 0x00FB:	return 0x9E;	// û
		case 0x00FC:	return 0x9F;	// ü
		case 0x2020:	return 0xA0;	// †
		case 0x00B0:	return 0xA1;	// °
		case 0x00A2:	return 0xA2;	// ¢
		case 0x00A3:	return 0xA3;	// £
		case 0x00A7:	return 0xA4;	// §
		case 0x2022:	return 0xA5;	// •
		case 0x00B6:	return 0xA6;	// ¶
		case 0x00DF:	return 0xA7;	// ß
		case 0x00AE:	return 0xA8;	// ®
		case 0x00A9:	return 0xA9;	// ©
		case 0x2122:	return 0xAA;	// ™
		case 0x00B4:	return 0xAB;	// ´
		case 0x00A8:	return 0xAC;	// ¨
		case 0x2260:	return 0xAD;	// ≠
		case 0x00C6:	return 0xAE;	// Æ
		case 0x00D8:	return 0xAF;	// Ø
		case 0x221E:	return 0xB0;	// ∞
		case 0x00B1:	return 0xB1;	// ±
		case 0x2264:	return 0xB2;	// ≤
		case 0x2265:	return 0xB3;	// ≥
		case 0x00A5:	return 0xB4;	// ¥
		case 0x00B5:	return 0xB5;	// µ
		case 0x2202:	return 0xB6;	// ∂
		case 0x2211:	return 0xB7;	// ∑
		case 0x220F:	return 0xB8;	// ∏
		case 0x03C0:	return 0xB9;	// π
		case 0x222B:	return 0xBA;	// ∫
		case 0x00AA:	return 0xBB;	// ª
		case 0x00BA:	return 0xBC;	// º
		case 0x03A9:	return 0xBD;	// Ω
		case 0x00E6:	return 0xBE;	// æ
		case 0x00F8:	return 0xBF;	// ø
		case 0x00BF:	return 0xC0;	// ¿
		case 0x00A1:	return 0xC1;	// ¡
		case 0x00AC:	return 0xC2;	// ¬
		case 0x221A:	return 0xC3;	// √
		case 0x0192:	return 0xC4;	// ƒ
		case 0x2248:	return 0xC5;	// ≈
		case 0x2206:	return 0xC6;	// ∆
		case 0x00AB:	return 0xC7;	// «
		case 0x00BB:	return 0xC8;	// »
		case 0x2026:	return 0xC9;	// …
		case 0x00A0:	return 0xCA;	// (nbsp)
		case 0x00C0:	return 0xCB;	// À
		case 0x00C3:	return 0xCC;	// Ã
		case 0x00D5:	return 0xCD;	// Õ
		case 0x0152:	return 0xCE;	// Œ
		case 0x0153:	return 0xCF;	// œ
		case 0x2013:	return 0xD0;	// –
		case 0x2014:	return 0xD1;	// —
		case 0x201C:	return 0xD2;	// "
		case 0x201D:	return 0xD3;	// "
		case 0x2018:	return 0xD4;	// '
		case 0x2019:	return 0xD5;	// '
		case 0x00F7:	return 0xD6;	// ÷
		case 0x25CA:	return 0xD7;	// ◊
		case 0x00FF:	return 0xD8;	// ÿ
		case 0x0178:	return 0xD9;	// Ÿ
		case 0x2044:	return 0xDA;	// ⁄
		case 0x20AC:	return 0xDB;	// €
		case 0x2039:	return 0xDC;	// ‹
		case 0x203A:	return 0xDD;	// ›
		case 0xFB01:	return 0xDE;	// ﬁ
		case 0xFB02:	return 0xDF;	// ﬂ
		case 0x2021:	return 0xE0;	// ‡
		case 0x00B7:	return 0xE1;	// ·
		case 0x201A:	return 0xE2;	// ‚
		case 0x201E:	return 0xE3;	// „
		case 0x2030:	return 0xE4;	// ‰
		case 0x00C2:	return 0xE5;	// Â
		case 0x00CA:	return 0xE6;	// Ê
		case 0x00C1:	return 0xE7;	// Á
		case 0x00CB:	return 0xE8;	// Ë
		case 0x00C8:	return 0xE9;	// È
		case 0x00CD:	return 0xEA;	// Í
		case 0x00CE:	return 0xEB;	// Î
		case 0x00CF:	return 0xEC;	// Ï
		case 0x00CC:	return 0xED;	// Ì
		case 0x00D3:	return 0xEE;	// Ó
		case 0x00D4:	return 0xEF;	// Ô
		case 0xF8FF:	return 0xF0;	// (Apple logo)
		case 0x00D2:	return 0xF1;	// Ò
		case 0x00DA:	return 0xF2;	// Ú
		case 0x00DB:	return 0xF3;	// Û
		case 0x00D9:	return 0xF4;	// Ù
		case 0x0131:	return 0xF5;	// ı
		case 0x02C6:	return 0xF6;	// ˆ
		case 0x02DC:	return 0xF7;	// ˜
		case 0x00AF:	return 0xF8;	// ¯
		case 0x02D8:	return 0xF9;	// ˘
		case 0x02D9:	return 0xFA;	// ˙
		case 0x02DA:	return 0xFB;	// ˚
		case 0x00B8:	return 0xFC;	// ¸
		case 0x02DD:	return 0xFD;	// ˝
		case 0x02DB:	return 0xFE;	// ˛
		case 0x02C7:	return 0xFF;	// ˇ
		default:		return 0x3F;	// '?' -- no Mac Roman equivalent
	}
}

// Inverse of _UTUnicodeCharToMacRoman: Mac Roman byte -> real Unicode code point (not the
// lossy CP1252 detour), so text copied to the clipboard can be written as CF_UNICODETEXT
// and round-trip losslessly to any Unicode-aware paste target, including the ~40 Mac Roman
// characters with no CP1252 equivalent.
extern const Uint16 _UTCharMap_AWToUnicode[256] =
{
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
	0x00C4, 0x00C5, 0x00C7, 0x00C9, 0x00D1, 0x00D6, 0x00DC, 0x00E1, 0x00E0, 0x00E2, 0x00E4, 0x00E3, 0x00E5, 0x00E7, 0x00E9, 0x00E8,
	0x00EA, 0x00EB, 0x00ED, 0x00EC, 0x00EE, 0x00EF, 0x00F1, 0x00F3, 0x00F2, 0x00F4, 0x00F6, 0x00F5, 0x00FA, 0x00F9, 0x00FB, 0x00FC,
	0x2020, 0x00B0, 0x00A2, 0x00A3, 0x00A7, 0x2022, 0x00B6, 0x00DF, 0x00AE, 0x00A9, 0x2122, 0x00B4, 0x00A8, 0x2260, 0x00C6, 0x00D8,
	0x221E, 0x00B1, 0x2264, 0x2265, 0x00A5, 0x00B5, 0x2202, 0x2211, 0x220F, 0x03C0, 0x222B, 0x00AA, 0x00BA, 0x03A9, 0x00E6, 0x00F8,
	0x00BF, 0x00A1, 0x00AC, 0x221A, 0x0192, 0x2248, 0x2206, 0x00AB, 0x00BB, 0x2026, 0x00A0, 0x00C0, 0x00C3, 0x00D5, 0x0152, 0x0153,
	0x2013, 0x2014, 0x201C, 0x201D, 0x2018, 0x2019, 0x00F7, 0x25CA, 0x00FF, 0x0178, 0x2044, 0x20AC, 0x2039, 0x203A, 0xFB01, 0xFB02,
	0x2021, 0x00B7, 0x201A, 0x201E, 0x2030, 0x00C2, 0x00CA, 0x00C1, 0x00CB, 0x00C8, 0x00CD, 0x00CE, 0x00CF, 0x00CC, 0x00D3, 0x00D4,
	0xF8FF, 0x00D2, 0x00DA, 0x00DB, 0x00D9, 0x0131, 0x02C6, 0x02DC, 0x00AF, 0x02D8, 0x02D9, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7,
};

Uint32 UText::GetCaretTime()
{
	return ::GetCaretBlinkTime();
}




#endif /* WIN32 */

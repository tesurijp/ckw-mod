/*----------------------------------------------------------------------------
 * Copyright 2004-2012  Kazuo Ishii <kish@wb3.so-net.ne.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *--------------------------------------------------------------------------*/

#include "ckw.h"
#include <WinNls.h>
#include "encoding.h"

UINT surrogate_to_ucs(const UINT16 _src_high, const UINT16 _src_low)
{
	UINT src_high = _src_high - 0xd800;
	UINT src_low  = _src_low  - 0xdc00;
	return (src_high << 10 | src_low) + 0x10000;
}

static const UINT  _is_dblchar_table []={
	0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
	0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x7FFFFFFF,0x000003FF,0xFFFFFFFF,
	0xFFFFFFFF,0x00000000,0x00000000,0x00000000,0x00000000,0xFFFF0000,0xFFFFFFFF,0xFFFFFFFF,
	0x0001FFFF,0x00000000,0x00000000,0x00000000,0x007F0000,0x00000000,0x00000000,0x00000000,
	0x00000000,0xC9000000,0x207BEFA2,0xA1E0C080,0x012BC69B,0x60040500,0x8BC08704,0x60000617,
	0x00000004,0x00000000,0x00000000,0x000AAAA0,0x00000000,0x00000000,0x00000000,0x01000100,
	0x00000000,0x00000000,0x48000000,0x00578097,0x00000000,0x00000000,0x00000000,0x00000000,
	0x00000000,0xFDFFFF00,0xFDFFFF01,0x00000001,0x01000000,0xFFFFFF80,0xFFFFFFFF,0xEE66F37F,
	0x00905A01,0x00000000,0x3D002000,0x00000000,0x00000020,0x00000000,0x50000000,0x8C009004,
	0x00000010,0xFEF03000,0x0007FE1F,0x0007FE04,0x00060000,0x00002800,0x1A000001,0x53C84513,
	0x0061E0BF,0xE6000822,0x98000199,0x40044001,0x01000000,0x00000000,0x00000000,0x00000800,
	0x0000000C,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
	0x00000000,0xFE000000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFF7,0xFFFFFFFF,
	0xFFFFFFFF,0xFFFFFE1F,0xFE001FFF,0xF60079FF,0x86619807,0x78000793,0xC0000100,0x00A06184,
	0x0A000000,0x76000000,0x0000016F,0x01800000,0xE1800000,0x17FFFF7F,0x01FFFFFE,0x00000000,
	0x00400000,0x00000000,0xFFFF8000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
	0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
	0xFEFFFFFF,0x0007FFFF,0xFFFFFE00,0xFFFFFFFF,0x000001FF,0x00000000,0x00000000,0x00000000,
	0xFE000000,0xFFFFFFFF,0xFFFFFFFF,0x03FFFFFF,0x00000000,0x00000000,0x00000000,0xFE000000,
	0x00400000,
};

inline bool _is_dblchar(int idx){
	return ( _is_dblchar_table[idx>>5] & 1<<(idx&31) ) ? true : false;
}

bool  is_dblchar(UINT n){
	if(n <= 0x004DFF){
		if(n <= 0x0010FF) return false;	/* U+000000 ~ U+0010FF (4352)*/
		if(n <= 0x00115F) return true;	/* U+001100 ~ U+00115F (96)*/
		if(n <= 0x002328) return false;	/* U+001160 ~ U+002328 (4553)*/
		if(n <= 0x00232A) return true;	/* U+002329 ~ U+00232A (2)*/
		if(n <= 0x002E7F) return false;	/* U+00232B ~ U+002E7F (2901)*/
		if(n <= 0x00303F) return _is_dblchar(n-0x002E80+0);	/* U+002E80 ~ U+00303F (448)*/
		if(n <= 0x003247) return true;	/* U+003040 ~ U+003247 (520)*/
		if(n <= 0x00324F) return false;	/* U+003248 ~ U+00324F (8)*/
		if(n <= 0x004DBF) return true;	/* U+003250 ~ U+004DBF (7024)*/
		if(n <= 0x004DFF) return false;	/* U+004DC0 ~ U+004DFF (64)*/
	}
	else  if(n <= 0x00FFE6){
		if(n <= 0x00A4CF) return true;	/* U+004E00 ~ U+00A4CF (22224)*/
		if(n <= 0x00A95F) return false;	/* U+00A4D0 ~ U+00A95F (1168)*/
		if(n <= 0x00A97C) return true;	/* U+00A960 ~ U+00A97C (29)*/
		if(n <= 0x00ABFF) return false;	/* U+00A97D ~ U+00ABFF (643)*/
		if(n <= 0x00D7A3) return true;	/* U+00AC00 ~ U+00D7A3 (11172)*/
		if(n <= 0x00F8FF) return false;	/* U+00D7A4 ~ U+00F8FF (8540)*/
		if(n <= 0x00FAFF) return true;	/* U+00F900 ~ U+00FAFF (512)*/
		if(n <= 0x00FE0F) return false;	/* U+00FB00 ~ U+00FE0F (784)*/
		if(n <= 0x00FFE6) return _is_dblchar(n-0x00FE10+448);	/* U+00FE10 ~ U+00FFE6 (471)*/
	}
	else{
		if(n <= 0x01AFFF) return false;	/* U+00FFE7 ~ U+01AFFF (45081)*/
		if(n <= 0x01B001) return true;	/* U+01B000 ~ U+01B001 (2)*/
		if(n <= 0x01F1FF) return false;	/* U+01B002 ~ U+01F1FF (16894)*/
		if(n <= 0x01F2FF) return true;	/* U+01F200 ~ U+01F2FF (256)*/
		if(n <= 0x01FFFF) return false;	/* U+01F300 ~ U+01FFFF (3328)*/
		if(n <= 0x02FFFD) return true;	/* U+020000 ~ U+02FFFD (65534)*/
		if(n <= 0x02FFFF) return false;	/* U+02FFFE ~ U+02FFFF (2)*/
		if(n <= 0x03FFFD) return true;	/* U+030000 ~ U+03FFFD (65534)*/
		if(n <= 0x10FFFF) return false;	/* U+03FFFE ~ U+10FFFF (851970)*/
	}
	return false;
}

bool  is_dblchar_cjk(UINT n){
	if(n <= 0x004DFF){
		if(n <= 0x000451) return _is_dblchar(n-0x000000+919);	/* U+000000 ~ U+000451 (1106)*/
		if(n <= 0x0010FF) return false;	/* U+000452 ~ U+0010FF (3246)*/
		if(n <= 0x00115F) return true;	/* U+001100 ~ U+00115F (96)*/
		if(n <= 0x00200F) return false;	/* U+001160 ~ U+00200F (3760)*/
		if(n <= 0x00277F) return _is_dblchar(n-0x002010+2025);	/* U+002010 ~ U+00277F (1904)*/
		if(n <= 0x002B54) return false;	/* U+002780 ~ U+002B54 (981)*/
		if(n <= 0x002B59) return true;	/* U+002B55 ~ U+002B59 (5)*/
		if(n <= 0x002E7F) return false;	/* U+002B5A ~ U+002E7F (806)*/
		if(n <= 0x00303F) return _is_dblchar(n-0x002E80+3929);	/* U+002E80 ~ U+00303F (448)*/
		if(n <= 0x004DBF) return true;	/* U+003040 ~ U+004DBF (7552)*/
		if(n <= 0x004DFF) return false;	/* U+004DC0 ~ U+004DFF (64)*/
	}
	else if(n <= 0x00FFFD){
		if(n <= 0x00A4CF) return true;	/* U+004E00 ~ U+00A4CF (22224)*/
		if(n <= 0x00A95F) return false;	/* U+00A4D0 ~ U+00A95F (1168)*/
		if(n <= 0x00A97C) return true;	/* U+00A960 ~ U+00A97C (29)*/
		if(n <= 0x00ABFF) return false;	/* U+00A97D ~ U+00ABFF (643)*/
		if(n <= 0x00D7A3) return true;	/* U+00AC00 ~ U+00D7A3 (11172)*/
		if(n <= 0x00DFFF) return false;	/* U+00D7A4 ~ U+00DFFF (2140)*/
		if(n <= 0x00FAFF) return true;	/* U+00E000 ~ U+00FAFF (6912)*/
		if(n <= 0x00FDFF) return false;	/* U+00FB00 ~ U+00FDFF (768)*/
		if(n <= 0x00FFFD) return _is_dblchar(n-0x00FE00+4377);	/* U+00FE00 ~ U+00FFFD (510)*/
	}
	else if(n <= 0x03FFFD){
		if(n <= 0x01AFFF) return false;	/* U+00FFFE ~ U+01AFFF (45058)*/
		if(n <= 0x01B001) return true;	/* U+01B000 ~ U+01B001 (2)*/
		if(n <= 0x01F0FF) return false;	/* U+01B002 ~ U+01F0FF (16638)*/
		if(n <= 0x01F77F) return true;	/* U+01F100 ~ U+01F77F (1664)*/
		if(n <= 0x01FFFF) return false;	/* U+01F780 ~ U+01FFFF (2176)*/
		if(n <= 0x02FFFD) return true;	/* U+020000 ~ U+02FFFD (65534)*/
		if(n <= 0x02FFFF) return false;	/* U+02FFFE ~ U+02FFFF (2)*/
		if(n <= 0x03FFFD) return true;	/* U+030000 ~ U+03FFFD (65534)*/
	}
	else{
		if(n <= 0x0E00FF) return false;	/* U+03FFFE ~ U+0E00FF (655618)*/
		if(n <= 0x0E01EF) return true;	/* U+0E0100 ~ U+0E01EF (240)*/
		if(n <= 0x0EFFFF) return false;	/* U+0E01F0 ~ U+0EFFFF (65040)*/
		if(n <= 0x0FFFFD) return true;	/* U+0F0000 ~ U+0FFFFD (65534)*/
		if(n <= 0x0FFFFF) return false;	/* U+0FFFFE ~ U+0FFFFF (2)*/
		if(n <= 0x10FFFD) return true;	/* U+100000 ~ U+10FFFD (65534)*/
		if(n <= 0x10FFFF) return false;	/* U+10FFFE ~ U+10FFFF (2)*/
	}
	return false;
}

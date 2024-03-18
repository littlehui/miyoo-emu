
#include "scaler.h"

#define AVERAGE(z, x) ((((z) & 0xF7DEF7DE) >> 1) + (((x) & 0xF7DEF7DE) >> 1))
#define AVERAGEHI(AB) ((((AB) & 0xF7DE0000) >> 1) + (((AB) & 0xF7DE) << 15))
#define AVERAGELO(CD) ((((CD) & 0xF7DE) >> 1) + (((CD) & 0xF7DE0000) >> 17))

extern uint32 palettetranslate[65536 * 4];

/*
	Upscale 256x224 -> 320x240

	Horizontal upscale:
		320/256=1.25  --  do some horizontal interpolation
		8p -> 10p
		4dw -> 5dw

		coarse interpolation:
		[ab][cd][ef][gh] -> [ab][(bc)c][de][f(fg)][gh]

		fine interpolation
		[ab][cd][ef][gh] -> [a(0.25a+0.75b)][(0.5b+0.5c)(0.75c+0.25d)][de][(0.25e+0.75f)(0.5f+0.5g)][(0.75g+0.25h)h]


	Vertical upscale:
		Bresenham algo with simple interpolation

	Parameters:
	uint32 *dst - pointer to 320x240x16bpp buffer
	uint8 *src - pointer to 256x224x8bpp buffer
	palette is taken from palettetranslate[]
	no pitch corrections are made!
*/

void upscale_320x240(uint32 *dst, uint8 *src)
{
	int midh = 240;// * 3 / 4;
	int Eh = 0;
	int source = 0;
	int dh = 0;
	int y, x;

	for (y = 0; y < 240; y++)
	{
		source = dh * 256;

		for (x = 0; x < 320/10; x++)
		{
			register uint32 ab, cd, ef, gh;

			//__builtin_prefetch(dst + 4, 1);			//�������Ը��Ż�
			__builtin_prefetch(src + source + 4, 0);

			ab = palettetranslate[*(uint16 *)(src + source)] & 0xF7DEF7DE;
			cd = palettetranslate[*(uint16 *)(src + source + 2)] & 0xF7DEF7DE;
			ef = palettetranslate[*(uint16 *)(src + source + 4)] & 0xF7DEF7DE;
			gh = palettetranslate[*(uint16 *)(src + source + 6)] & 0xF7DEF7DE;

			if(Eh >= midh) { // average + 256
				ab = AVERAGE(ab, palettetranslate[*(uint16 *)(src + source + 256)]) & 0xF7DEF7DE; // to prevent overflow
				cd = AVERAGE(cd, palettetranslate[*(uint16 *)(src + source + 256 + 2)]) & 0xF7DEF7DE; // to prevent overflow
				ef = AVERAGE(ef, palettetranslate[*(uint16 *)(src + source + 256 + 4)]) & 0xF7DEF7DE; // to prevent overflow
				gh = AVERAGE(gh, palettetranslate[*(uint16 *)(src + source + 256 + 6)]) & 0xF7DEF7DE; // to prevent overflow
			}

			*dst++ = ab;
			*dst++  = ((ab >> 17) + ((cd & 0xFFFF) >> 1)) + (cd << 16);
			*dst++  = (cd >> 16) + (ef << 16);
			*dst++  = (ef >> 16) + (((ef & 0xFFFF0000) >> 1) + ((gh & 0xFFFF) << 15));
			*dst++  = gh;

			source += 8;

		}
		Eh += 224; if(Eh >= 240) { Eh -= 240; dh++; }
	}
}

/*
	Upscale 256x224 -> 384x240 (for 400x240)

	Horizontal interpolation
		384/256=1.5
		4p -> 6p
		2dw -> 3dw

		for each line: 4 pixels => 6 pixels (*1.5) (64 blocks)
		[ab][cd] => [a(ab)][bc][(cd)d]

	Vertical upscale:
		Bresenham algo with simple interpolation

	Parameters:
	uint32 *dst - pointer to 400x240x16bpp buffer
	uint8 *src - pointer to 256x224x8bpp buffer
	palette is taken from palettetranslate[]
	pitch correction
*/

void upscale_384x240(uint32 *dst, uint8 *src)
{
	int midh = 240 * 3 / 4;
	int Eh = 0;
	int source = 0;
	int dh = 0;
	int y, x;

	dst += (400 - 384) / 4;

	for (y = 0; y < 240; y++)
	{
		source = dh * 256;

		for (x = 0; x < 384/6; x++)
		{
			register uint32 ab, cd;

			__builtin_prefetch(dst + 4, 1);
			__builtin_prefetch(src + source + 4, 0);

			ab = palettetranslate[*(uint16 *)(src + source)] & 0xF7DEF7DE;
			cd = palettetranslate[*(uint16 *)(src + source + 2)] & 0xF7DEF7DE;

			if(Eh >= midh) { // average + 256
				ab = AVERAGE(ab, palettetranslate[*(uint16 *)(src + source + 256)]) & 0xF7DEF7DE; // to prevent overflow
				cd = AVERAGE(cd, palettetranslate[*(uint16 *)(src + source + 256 + 2)]) & 0xF7DEF7DE; // to prevent overflow
			}

			*dst++ = (ab & 0xFFFF) + AVERAGEHI(ab);
			*dst++ = (ab >> 16) + ((cd & 0xFFFF) << 16);
			*dst++ = (cd & 0xFFFF0000) + AVERAGELO(cd);

			source += 4;

		}
		dst += (400 - 384) / 2; 
		Eh += 224; if(Eh >= 240) { Eh -= 240; dh++; }
	}
}

/*
	Upscale 256x224 -> 384x272 (for 480x240)

	Horizontal interpolation
		384/256=1.5
		4p -> 6p
		2dw -> 3dw

		for each line: 4 pixels => 6 pixels (*1.5) (64 blocks)
		[ab][cd] => [a(ab)][bc][(cd)d]

	Vertical upscale:
		Bresenham algo with simple interpolation

	Parameters:
	uint32 *dst - pointer to 480x272x16bpp buffer
	uint8 *src - pointer to 256x224x8bpp buffer
	palette is taken from palettetranslate[]
	pitch correction
*/

void upscale_384x272(uint32 *dst, uint8 *src)
{
	int midh = 272 * 3 / 4;
	int Eh = 0;
	int source = 0;
	int dh = 0;
	int y, x;

	dst += (480 - 384) / 4;

	for (y = 0; y < 272; y++)
	{
		source = dh * 256;

		for (x = 0; x < 384/6; x++)
		{
			register uint32 ab, cd;

			__builtin_prefetch(dst + 4, 1);
			__builtin_prefetch(src + source + 4, 0);

			ab = palettetranslate[*(uint16 *)(src + source)] & 0xF7DEF7DE;
			cd = palettetranslate[*(uint16 *)(src + source + 2)] & 0xF7DEF7DE;

			if(Eh >= midh) { // average + 256
				ab = AVERAGE(ab, palettetranslate[*(uint16 *)(src + source + 256)]) & 0xF7DEF7DE; // to prevent overflow
				cd = AVERAGE(cd, palettetranslate[*(uint16 *)(src + source + 256 + 2)]) & 0xF7DEF7DE; // to prevent overflow
			}

			*dst++ = (ab & 0xFFFF) + AVERAGEHI(ab);
			*dst++ = (ab >> 16) + ((cd & 0xFFFF) << 16);
			*dst++ = (cd & 0xFFFF0000) + AVERAGELO(cd);

			source += 4;

		}
		dst += (480 - 384) / 2; 
		Eh += 224; if(Eh >= 272) { Eh -= 272; dh++; }
	}
}

/*
    Upscale 256x224 -> 480x272

    Horizontal interpolation
        480/256=1.875
        16p -> 30p
        8dw -> 15dw

        For each line 16 pixels => 30 pixels (*1.875) (32 blocks)
        Coarse:
            [ab][cd][ef][gh][ij][kl][mn][op]
                =>
            [aa][bb][cc][d(de)][ef][fg][gh][hi][ij][jk][kl][(lm)m][nn][oo][pp]
        Fine:
            ab` = a, (0.875a + 0.125b)
            cd` = b, (0.75b  + 0.25c)
            ef` = c, (0.625c + 0.375d)
            gh` = d, (0.5d   + 0.5e)
            ij` = e, (0.375e + 0.625f)
            kl` = f, (0.25f  + 0.75g)
            mn` = g, (0.125g + 0.875h)
            op` = h, i
            qr` = (0.875i + 0.125j), j
            st` = (0.75j  + 0.25k),  k
            uv` = (0.625k + 0.375l), l
            wx` = (0.5l   + 0.5m),   m
            yz` = (0.375m + 0.625n), n
            12` = (0.25n  + 0.75o),  o
            34` = (0.125o + 0.875p), p

    Vertical upscale:
        Bresenham algo with simple interpolation
*/

void upscale_480x272(uint32 *dst, uint8 *src)
{
    int midh = 272 / 2;
    int Eh = 0;
    int source = 0;
    int dh = 0;
    int y, x;

    for (y = 0; y < 272; y++)
    {
        source = dh * 256;

        for (x = 0; x < 480/30; x++)
        {
            register uint32 ab, cd, ef, gh, ij, kl, mn, op;

            __builtin_prefetch(dst + 4, 1);
            __builtin_prefetch(src + source + 4, 0);

            ab = palettetranslate[*(uint16 *)(src + source)] & 0xF7DEF7DE;
            cd = palettetranslate[*(uint16 *)(src + source + 2)] & 0xF7DEF7DE;
            ef = palettetranslate[*(uint16 *)(src + source + 4)] & 0xF7DEF7DE;
            gh = palettetranslate[*(uint16 *)(src + source + 6)] & 0xF7DEF7DE;
            ij = palettetranslate[*(uint16 *)(src + source + 8)] & 0xF7DEF7DE;
            kl = palettetranslate[*(uint16 *)(src + source + 10)] & 0xF7DEF7DE;
            mn = palettetranslate[*(uint16 *)(src + source + 12)] & 0xF7DEF7DE;
            op = palettetranslate[*(uint16 *)(src + source + 14)] & 0xF7DEF7DE;

            if(Eh >= midh) {
                ab = AVERAGE(ab, palettetranslate[*(uint16 *)(src + source + 256)]) & 0xF7DEF7DE;
                cd = AVERAGE(cd, palettetranslate[*(uint16 *)(src + source + 256 + 2)]) & 0xF7DEF7DE;
                ef = AVERAGE(ef, palettetranslate[*(uint16 *)(src + source + 256 + 4)]) & 0xF7DEF7DE;
                gh = AVERAGE(gh, palettetranslate[*(uint16 *)(src + source + 256 + 6)]) & 0xF7DEF7DE;
                ij = AVERAGE(ij, palettetranslate[*(uint16 *)(src + source + 256 + 8)]) & 0xF7DEF7DE;
                kl = AVERAGE(kl, palettetranslate[*(uint16 *)(src + source + 256 + 10)]) & 0xF7DEF7DE;
                mn = AVERAGE(mn, palettetranslate[*(uint16 *)(src + source + 256 + 12)]) & 0xF7DEF7DE;
                op = AVERAGE(op, palettetranslate[*(uint16 *)(src + source + 256 + 14)]) & 0xF7DEF7DE;
            }

            *dst++ = (ab & 0xFFFF) + (ab << 16);            // [aa]
            *dst++ = (ab >> 16) + (ab & 0xFFFF0000);        // [bb]
            *dst++ = (cd & 0xFFFF) + (cd << 16);            // [cc]
            *dst++ = (cd >> 16) + (((cd & 0xF7DE0000) >> 1) + ((ef & 0xF7DE) << 15)); // [d(de)]
            *dst++ = ef;                                    // [ef]
            *dst++ = (ef >> 16) + (gh << 16);               // [fg]
            *dst++ = gh;                                    // [gh]
            *dst++ = (gh >> 16) + (ij << 16);               // [hi]
            *dst++ = ij;                                    // [ij]
            *dst++ = (ij >> 16) + (kl << 16);               // [jk]
            *dst++ = kl;                                    // [kl]
            *dst++ = (((kl & 0xF7DE0000) >> 17) + ((mn & 0xF7DE) >> 1)) + (mn << 16); // [(lm)m]
            *dst++ = (mn >> 16) + (mn & 0xFFFF0000);        // [nn]
            *dst++ = (op & 0xFFFF) + (op << 16);            // [oo]
            *dst++ = (op >> 16) + (op & 0xFFFF0000);        // [pp]

            source += 16;
        }
        Eh += 224; if(Eh >= 272) { Eh -= 272; dh++; }
    }
}

/*
 * Approximately bilinear scaler, 256x224 to 320x240
 *
 * Copyright (C) 2014 hi-ban, Nebuleon <nebuleon.fumika@gmail.com>
 *
 * This function and all auxiliary functions are free software; you can
 * redistribute them and/or modify them under the terms of the GNU Lesser
 * General Public License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * These functions are distributed in the hope that they will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

// Support math
#define Half(A) (((A) >> 1) & 0x7BEF)
#define Quarter(A) (((A) >> 2) & 0x39E7)
// Error correction expressions to piece back the lower bits together
#define RestHalf(A) ((A) & 0x0821)
#define RestQuarter(A) ((A) & 0x1863)

// Error correction expressions for quarters of pixels
#define Corr1_3(A, B)     Quarter(RestQuarter(A) + (RestHalf(B) << 1) + RestQuarter(B))
#define Corr3_1(A, B)     Quarter((RestHalf(A) << 1) + RestQuarter(A) + RestQuarter(B))

// Error correction expressions for halves
#define Corr1_1(A, B)     ((A) & (B) & 0x0821)

// Quarters
#define Weight1_3(A, B)   (Quarter(A) + Half(B) + Quarter(B) + Corr1_3(A, B))
#define Weight3_1(A, B)   (Half(A) + Quarter(A) + Quarter(B) + Corr3_1(A, B))

// Halves
#define Weight1_1(A, B)   (Half(A) + Half(B) + Corr1_1(A, B))

/* Upscales a 256x224 image to 320x240 using an approximate bilinear
 * resampling algorithm that only uses integer math.
 *
 * Input:
 *   src: A packed 256x224 pixel image. The pixel format of this image is
 *     RGB 565.
 *   width: The width of the source image. Should always be 256.
 * Output:
 *   dst: A packed 320x240 pixel image. The pixel format of this image is
 *     RGB 565.
 */
void upscale_320x240_bilinearish_clip(uint32_t* dst, uint8 *src, int width)
{
	uint8* Src16 = (uint8*) src;
	uint16_t* Dst16 = (uint16_t*) dst;
	// There are 80 blocks of 3 pixels horizontally, and 14 of 16 vertically.
	// Each block of 3x16 becomes 4x17.
	uint32_t BlockX, BlockY;
	uint8* BlockSrc;
	uint16_t* BlockDst;
	for (BlockY = 0; BlockY < 14; BlockY++)
	{
		BlockSrc = Src16 + BlockY * 256 * 16;
		BlockDst = Dst16 + BlockY * 320 * 17;
		for (BlockX = 0; BlockX < 80; BlockX++)
		{
			/* Horizontally:
			 * Before(3):
			 * (a)(b)(c)
			 * After(4):
			 * (a)(abbb)(bbbc)(c)
			 *
			 * Vertically:
			 * Before(16): After(17):
			 * (a)       (a)
			 * (b)       (b)
			 * (c)       (c)
			 * (d)       (cddd)
			 * (e)       (deee)
			 * (f)       (efff)
			 * (g)       (fggg)
			 * (h)       (ghhh)
			 * (i)       (hi)
			 * (j)       (iiij)
			 * (k)       (jjjk)
			 * (l)       (kkkl)
			 * (m)       (lllm)
			 * (n)       (mmmn)
			 * (o)       (n)
			 * (p)       (o)
			 *           (p)
			 */

			// -- Row 1 --
			uint16_t  _1 = palettetranslate[*(BlockSrc                + 8)];
			*(BlockDst            + 0) = _1;
			uint16_t  _2 = palettetranslate[*(BlockSrc            + 1 + 8)];
			*(BlockDst            + 1) = Weight1_3( _1,  _2);
			uint16_t  _3 = palettetranslate[*(BlockSrc            + 2 + 8)];
			*(BlockDst            + 2) = Weight3_1( _2,  _3);
			*(BlockDst            + 3) = _3;

			// -- Row 2 --
			uint16_t  _4 = palettetranslate[*(BlockSrc + 256 *  1     + 8)];
			*(BlockDst + 320 *  1 + 0) = _4;
			uint16_t  _5 = palettetranslate[*(BlockSrc + 256 *  1 + 1 + 8)];
			*(BlockDst + 320 *  1 + 1) = Weight1_3( _4,  _5);
			uint16_t  _6 = palettetranslate[*(BlockSrc + 256 *  1 + 2 + 8)];
			*(BlockDst + 320 *  1 + 2) = Weight3_1( _5,  _6);
			*(BlockDst + 320 *  1 + 3) = _6;

			// -- Row 3 --
			uint16_t  _7 = palettetranslate[*(BlockSrc + 256 *  2     + 8)];
			*(BlockDst + 320 *  2 + 0) = _7;
			uint16_t  _8 = palettetranslate[*(BlockSrc + 256 *  2 + 1 + 8)];
			*(BlockDst + 320 *  2 + 1) = Weight1_3( _7, _8);
			uint16_t  _9 = palettetranslate[*(BlockSrc + 256 *  2 + 2 + 8)];
			*(BlockDst + 320 *  2 + 2) = Weight3_1(_8, _9);
			*(BlockDst + 320 *  2 + 3) = _9;

			// -- Row 4 --
			uint16_t _10 = palettetranslate[*(BlockSrc + 256 *  3     + 8)];
			*(BlockDst + 320 *  3 + 0) = Weight1_3( _7, _10);
			uint16_t _11 = palettetranslate[*(BlockSrc + 256 *  3 + 1 + 8)];
			*(BlockDst + 320 *  3 + 1) = Weight1_3(Weight1_3( _7, _8), Weight1_3(_10, _11));
			uint16_t _12 = palettetranslate[*(BlockSrc + 256 *  3 + 2 + 8)];
			*(BlockDst + 320 *  3 + 2) = Weight1_3(Weight3_1(_8, _9), Weight3_1(_11, _12));
			*(BlockDst + 320 *  3 + 3) = Weight1_3(_9, _12);

			// -- Row 5 --
			uint16_t _13 = palettetranslate[*(BlockSrc + 256 *  4     + 8)];
			*(BlockDst + 320 *  4 + 0) = Weight1_3(_10, _13);
			uint16_t _14 = palettetranslate[*(BlockSrc + 256 *  4 + 1 + 8)];
			*(BlockDst + 320 *  4 + 1) = Weight1_3(Weight1_3(_10, _11), Weight1_3(_13, _14));
			uint16_t _15 = palettetranslate[*(BlockSrc + 256 *  4 + 2 + 8)];
			*(BlockDst + 320 *  4 + 2) = Weight1_3(Weight3_1(_11, _12), Weight3_1(_14, _15));
			*(BlockDst + 320 *  4 + 3) = Weight1_3(_12, _15);

			// -- Row 6 --
			uint16_t _16 = palettetranslate[*(BlockSrc + 256 *  5     + 8)];
			*(BlockDst + 320 *  5 + 0) = Weight1_3(_13, _16);
			uint16_t _17 = palettetranslate[*(BlockSrc + 256 *  5 + 1 + 8)];
			*(BlockDst + 320 *  5 + 1) = Weight1_3(Weight1_3(_13, _14), Weight1_3(_16, _17));
			uint16_t _18 = palettetranslate[*(BlockSrc + 256 *  5 + 2 + 8)];
			*(BlockDst + 320 *  5 + 2) = Weight1_3(Weight3_1(_14, _15), Weight3_1(_17, _18));
			*(BlockDst + 320 *  5 + 3) = Weight1_3(_15, _18);

			// -- Row 7 --
			uint16_t _19 = palettetranslate[*(BlockSrc + 256 *  6     + 8)];
			*(BlockDst + 320 *  6 + 0) = Weight1_3(_16, _19);
			uint16_t _20 = palettetranslate[*(BlockSrc + 256 *  6 + 1 + 8)];
			*(BlockDst + 320 *  6 + 1) = Weight1_3(Weight1_3(_16, _17), Weight1_3(_19, _20));
			uint16_t _21 = palettetranslate[*(BlockSrc + 256 *  6 + 2 + 8)];
			*(BlockDst + 320 *  6 + 2) = Weight1_3(Weight3_1(_17, _18), Weight3_1(_20, _21));
			*(BlockDst + 320 *  6 + 3) = Weight1_3(_18, _21);

			// -- Row 8 --
			uint16_t _22 = palettetranslate[*(BlockSrc + 256 *  7     + 8)];
			*(BlockDst + 320 *  7 + 0) = Weight1_3(_19, _22);
			uint16_t _23 = palettetranslate[*(BlockSrc + 256 *  7 + 1 + 8)];
			*(BlockDst + 320 *  7 + 1) = Weight1_3(Weight1_3(_19, _20), Weight1_3(_22, _23));
			uint16_t _24 = palettetranslate[*(BlockSrc + 256 *  7 + 2 + 8)];
			*(BlockDst + 320 *  7 + 2) = Weight1_3(Weight3_1(_20, _21), Weight3_1(_23, _24));
			*(BlockDst + 320 *  7 + 3) = Weight1_3(_21, _24);

			// -- Row 9 --
			uint16_t _25 = palettetranslate[*(BlockSrc + 256 *  8     + 8)];
			*(BlockDst + 320 *  8 + 0) = Weight1_1(_22, _25);
			uint16_t _26 = palettetranslate[*(BlockSrc + 256 *  8 + 1 + 8)];
			*(BlockDst + 320 *  8 + 1) = Weight1_1(Weight1_3(_22, _23), Weight1_3(_25, _26));
			uint16_t _27 = palettetranslate[*(BlockSrc + 256 *  8 + 2 + 8)];
			*(BlockDst + 320 *  8 + 2) = Weight1_1(Weight3_1(_23, _24), Weight3_1(_26, _27));
			*(BlockDst + 320 *  8 + 3) = Weight1_1(_24, _27);

			// -- Row 10 --
			uint16_t _28 = palettetranslate[*(BlockSrc + 256 *  9     + 8)];
			*(BlockDst + 320 *  9 + 0) = Weight3_1(_25, _28);
			uint16_t _29 = palettetranslate[*(BlockSrc + 256 *  9 + 1 + 8)];
			*(BlockDst + 320 *  9 + 1) = Weight3_1(Weight1_3(_25, _26), Weight1_3(_28, _29));
			uint16_t _30 = palettetranslate[*(BlockSrc + 256 *  9 + 2 + 8)];
			*(BlockDst + 320 *  9 + 2) = Weight3_1(Weight3_1(_26, _27), Weight3_1(_29, _30));
			*(BlockDst + 320 *  9 + 3) = Weight3_1(_27, _30);

			// -- Row 11 --
			uint16_t _31 = palettetranslate[*(BlockSrc + 256 * 10     + 8)];
			*(BlockDst + 320 * 10 + 0) = Weight3_1(_28, _31);
			uint16_t _32 = palettetranslate[*(BlockSrc + 256 * 10 + 1 + 8)];
			*(BlockDst + 320 * 10 + 1) = Weight3_1(Weight1_3(_28, _29), Weight1_3(_31, _32));
			uint16_t _33 = palettetranslate[*(BlockSrc + 256 * 10 + 2 + 8)];
			*(BlockDst + 320 * 10 + 2) = Weight3_1(Weight3_1(_29, _30), Weight3_1(_32, _33));
			*(BlockDst + 320 * 10 + 3) = Weight3_1(_30, _33);

			// -- Row 12 --
			uint16_t _34 = palettetranslate[*(BlockSrc + 256 * 11     + 8)];
			*(BlockDst + 320 * 11 + 0) = Weight3_1(_31, _34);
			uint16_t _35 = palettetranslate[*(BlockSrc + 256 * 11 + 1 + 8)];
			*(BlockDst + 320 * 11 + 1) = Weight3_1(Weight1_3(_31, _32), Weight1_3(_34, _35));
			uint16_t _36 = palettetranslate[*(BlockSrc + 256 * 11 + 2 + 8)];
			*(BlockDst + 320 * 11 + 2) = Weight3_1(Weight3_1(_32, _33), Weight3_1(_35, _36));
			*(BlockDst + 320 * 11 + 3) = Weight3_1(_33, _36);

			// -- Row 13 --
			uint16_t _37 = palettetranslate[*(BlockSrc + 256 * 12     + 8)];
			*(BlockDst + 320 * 12 + 0) = Weight3_1(_34, _37);
			uint16_t _38 = palettetranslate[*(BlockSrc + 256 * 12 + 1 + 8)];
			*(BlockDst + 320 * 12 + 1) = Weight3_1(Weight1_3(_34, _35), Weight1_3(_37, _38));
			uint16_t _39 = palettetranslate[*(BlockSrc + 256 * 12 + 2 + 8)];
			*(BlockDst + 320 * 12 + 2) = Weight3_1(Weight3_1(_35, _36), Weight3_1(_38, _39));
			*(BlockDst + 320 * 12 + 3) = Weight3_1(_36, _39);

			// -- Row 14 --
			uint16_t _40 = palettetranslate[*(BlockSrc + 256 * 13     + 8)];
			*(BlockDst + 320 * 13 + 0) = Weight3_1(_37, _40);
			uint16_t _41 = palettetranslate[*(BlockSrc + 256 * 13 + 1 + 8)];
			*(BlockDst + 320 * 13 + 1) = Weight3_1(Weight1_3(_37, _38), Weight1_3(_40, _41));
			uint16_t _42 = palettetranslate[*(BlockSrc + 256 * 13 + 2 + 8)];
			*(BlockDst + 320 * 13 + 2) = Weight3_1(Weight3_1(_38, _39), Weight3_1(_41, _42));
			*(BlockDst + 320 * 13 + 3) = Weight3_1(_39, _42);

			// -- Row 15 --
			*(BlockDst + 320 * 14 + 0) = _40;
			*(BlockDst + 320 * 14 + 1) = Weight1_3(_40, _41);
			*(BlockDst + 320 * 14 + 2) = Weight3_1(_41, _42);
			*(BlockDst + 320 * 14 + 3) = _42;

			// -- Row 16 --
			uint16_t _43 = palettetranslate[*(BlockSrc + 256 * 14     + 8)];
			*(BlockDst + 320 * 15 + 0) = _43;
			uint16_t _44 = palettetranslate[*(BlockSrc + 256 * 14 + 1 + 8)];
			*(BlockDst + 320 * 15 + 1) = Weight1_3(_43, _44);
			uint16_t _45 = palettetranslate[*(BlockSrc + 256 * 14 + 2 + 8)];
			*(BlockDst + 320 * 15 + 2) = Weight3_1(_44, _45);
			*(BlockDst + 320 * 15 + 3) = _45;

			// -- Row 17 --
			uint16_t _46 = palettetranslate[*(BlockSrc + 256 * 15     + 8)];
			*(BlockDst + 320 * 16 + 0) = _46;
			uint16_t _47 = palettetranslate[*(BlockSrc + 256 * 15 + 1 + 8)];
			*(BlockDst + 320 * 16 + 1) = Weight1_3(_46, _47);
			uint16_t _48 = palettetranslate[*(BlockSrc + 256 * 15 + 2 + 8)];
			*(BlockDst + 320 * 16 + 2) = Weight3_1(_47, _48);
			*(BlockDst + 320 * 16 + 3) = _48;

			BlockSrc += 3;
			BlockDst += 4;
		}
	}
}

void upscale_320x240_bilinearish_noclip(uint32_t* dst, uint8 *src, int width)
{
	uint8* Src16 = (uint8*) src;
	uint16_t* Dst16 = (uint16_t*) dst;
	// There are 64 blocks of 4 pixels horizontally, and 14 of 16 vertically.
	// Each block of 4x16 becomes 5x17.
	uint32_t BlockX, BlockY;
	uint8* BlockSrc;
	uint16_t* BlockDst;
	for (BlockY = 0; BlockY < 14; BlockY++)
	{
		BlockSrc = Src16 + BlockY * 256 * 16;
		BlockDst = Dst16 + BlockY * 320 * 17;
		for (BlockX = 0; BlockX < 64; BlockX++)
		{
			/* Horizontally:
			 * Before(4):
			 * (a)(b)(c)(d)
			 * After(5):
			 * (a)(abbb)(bc)(cccd)(d)
			 *
			 * Vertically:
			 * Before(16): After(17):
			 * (a)       (a)
			 * (b)       (b)
			 * (c)       (c)
			 * (d)       (cddd)
			 * (e)       (deee)
			 * (f)       (efff)
			 * (g)       (fggg)
			 * (h)       (ghhh)
			 * (i)       (hi)
			 * (j)       (iiij)
			 * (k)       (jjjk)
			 * (l)       (kkkl)
			 * (m)       (lllm)
			 * (n)       (mmmn)
			 * (o)       (n)
			 * (p)       (o)
			 *           (p)
			 */

			// -- Row 1 --
			uint16_t  _1 = palettetranslate[*(BlockSrc               )];
			*(BlockDst            + 0) = _1;
			uint16_t  _2 = palettetranslate[*(BlockSrc            + 1)];
			*(BlockDst            + 1) = Weight1_3( _1,  _2);
			uint16_t  _3 = palettetranslate[*(BlockSrc            + 2)];
			*(BlockDst            + 2) = Weight1_1( _2,  _3);
			uint16_t  _4 = palettetranslate[*(BlockSrc            + 3)];
			*(BlockDst            + 3) = Weight3_1( _3,  _4);
			*(BlockDst            + 4) = _4;

			// -- Row 2 --
			uint16_t  _5 = palettetranslate[*(BlockSrc + 256 *  1    )];
			*(BlockDst + 320 *  1 + 0) = _5;
			uint16_t  _6 = palettetranslate[*(BlockSrc + 256 *  1 + 1)];
			*(BlockDst + 320 *  1 + 1) = Weight1_3( _5,  _6);
			uint16_t  _7 = palettetranslate[*(BlockSrc + 256 *  1 + 2)];
			*(BlockDst + 320 *  1 + 2) = Weight1_1( _6,  _7);
			uint16_t  _8 = palettetranslate[*(BlockSrc + 256 *  1 + 3)];
			*(BlockDst + 320 *  1 + 3) = Weight3_1( _7,  _8);
			*(BlockDst + 320 *  1 + 4) = _8;

			// -- Row 3 --
			uint16_t  _9 = palettetranslate[*(BlockSrc + 256 *  2    )];
			*(BlockDst + 320 *  2 + 0) = _9;
			uint16_t  _10 = palettetranslate[*(BlockSrc + 256 *  2 + 1)];
			*(BlockDst + 320 *  2 + 1) = Weight1_3( _9, _10);
			uint16_t  _11 = palettetranslate[*(BlockSrc + 256 *  2 + 2)];
			*(BlockDst + 320 *  2 + 2) = Weight1_1(_10, _11);
			uint16_t  _12 = palettetranslate[*(BlockSrc + 256 *  2 + 3)];
			*(BlockDst + 320 *  2 + 3) = Weight3_1(_11, _12);
			*(BlockDst + 320 *  2 + 4) = _12;

			// -- Row 4 --
			uint16_t _13 = palettetranslate[*(BlockSrc + 256 *  3    )];
			*(BlockDst + 320 *  3 + 0) = Weight1_3( _9, _13);
			uint16_t _14 = palettetranslate[*(BlockSrc + 256 *  3 + 1)];
			*(BlockDst + 320 *  3 + 1) = Weight1_3(Weight1_3( _9, _10), Weight1_3(_13, _14));
			uint16_t _15 = palettetranslate[*(BlockSrc + 256 *  3 + 2)];
			*(BlockDst + 320 *  3 + 2) = Weight1_3(Weight1_1(_10, _11), Weight1_1(_14, _15));
			uint16_t _16 = palettetranslate[*(BlockSrc + 256 *  3 + 3)];
			*(BlockDst + 320 *  3 + 3) = Weight1_3(Weight3_1(_11, _12), Weight3_1(_15, _16));
			*(BlockDst + 320 *  3 + 4) = Weight1_3(_12, _16);

			// -- Row 5 --
			uint16_t _17 = palettetranslate[*(BlockSrc + 256 *  4    )];
			*(BlockDst + 320 *  4 + 0) = Weight1_3(_13, _17);
			uint16_t _18 = palettetranslate[*(BlockSrc + 256 *  4 + 1)];
			*(BlockDst + 320 *  4 + 1) = Weight1_3(Weight1_3(_13, _14), Weight1_3(_17, _18));
			uint16_t _19 = palettetranslate[*(BlockSrc + 256 *  4 + 2)];
			*(BlockDst + 320 *  4 + 2) = Weight1_3(Weight1_1(_14, _15), Weight1_1(_18, _19));
			uint16_t _20 = palettetranslate[*(BlockSrc + 256 *  4 + 3)];
			*(BlockDst + 320 *  4 + 3) = Weight1_3(Weight3_1(_15, _16), Weight3_1(_19, _20));
			*(BlockDst + 320 *  4 + 4) = Weight1_3(_16, _20);

			// -- Row 6 --
			uint16_t _21 = palettetranslate[*(BlockSrc + 256 *  5    )];
			*(BlockDst + 320 *  5 + 0) = Weight1_3(_17, _21);
			uint16_t _22 = palettetranslate[*(BlockSrc + 256 *  5 + 1)];
			*(BlockDst + 320 *  5 + 1) = Weight1_3(Weight1_3(_17, _18), Weight1_3(_21, _22));
			uint16_t _23 = palettetranslate[*(BlockSrc + 256 *  5 + 2)];
			*(BlockDst + 320 *  5 + 2) = Weight1_3(Weight1_1(_18, _19), Weight1_1(_22, _23));
			uint16_t _24 = palettetranslate[*(BlockSrc + 256 *  5 + 3)];
			*(BlockDst + 320 *  5 + 3) = Weight1_3(Weight3_1(_19, _20), Weight3_1(_23, _24));
			*(BlockDst + 320 *  5 + 4) = Weight1_3(_20, _24);

			// -- Row 7 --
			uint16_t _25 = palettetranslate[*(BlockSrc + 256 *  6    )];
			*(BlockDst + 320 *  6 + 0) = Weight1_3(_21, _25);
			uint16_t _26 = palettetranslate[*(BlockSrc + 256 *  6 + 1)];
			*(BlockDst + 320 *  6 + 1) = Weight1_3(Weight1_3(_21, _22), Weight1_3(_25, _26));
			uint16_t _27 = palettetranslate[*(BlockSrc + 256 *  6 + 2)];
			*(BlockDst + 320 *  6 + 2) = Weight1_3(Weight1_1(_22, _23), Weight1_1(_26, _27));
			uint16_t _28 = palettetranslate[*(BlockSrc + 256 *  6 + 3)];
			*(BlockDst + 320 *  6 + 3) = Weight1_3(Weight3_1(_23, _24), Weight3_1(_27, _28));
			*(BlockDst + 320 *  6 + 4) = Weight1_3(_24, _28);

			// -- Row 8 --
			uint16_t _29 = palettetranslate[*(BlockSrc + 256 *  7    )];
			*(BlockDst + 320 *  7 + 0) = Weight1_3(_25, _29);
			uint16_t _30 = palettetranslate[*(BlockSrc + 256 *  7 + 1)];
			*(BlockDst + 320 *  7 + 1) = Weight1_3(Weight1_3(_25, _26), Weight1_3(_29, _30));
			uint16_t _31 = palettetranslate[*(BlockSrc + 256 *  7 + 2)];
			*(BlockDst + 320 *  7 + 2) = Weight1_3(Weight1_1(_26, _27), Weight1_1(_30, _31));
			uint16_t _32 = palettetranslate[*(BlockSrc + 256 *  7 + 3)];
			*(BlockDst + 320 *  7 + 3) = Weight1_3(Weight3_1(_27, _28), Weight3_1(_31, _32));
			*(BlockDst + 320 *  7 + 4) = Weight1_3(_28, _32);

			// -- Row 9 --
			uint16_t _33 = palettetranslate[*(BlockSrc + 256 *  8    )];
			*(BlockDst + 320 *  8 + 0) = Weight1_1(_29, _33);
			uint16_t _34 = palettetranslate[*(BlockSrc + 256 *  8 + 1)];
			*(BlockDst + 320 *  8 + 1) = Weight1_1(Weight1_3(_29, _30), Weight1_3(_33, _34));
			uint16_t _35 = palettetranslate[*(BlockSrc + 256 *  8 + 2)];
			*(BlockDst + 320 *  8 + 2) = Weight1_1(Weight1_1(_30, _31), Weight1_1(_34, _35));
			uint16_t _36 = palettetranslate[*(BlockSrc + 256 *  8 + 3)];
			*(BlockDst + 320 *  8 + 3) = Weight1_1(Weight3_1(_31, _32), Weight3_1(_35, _36));
			*(BlockDst + 320 *  8 + 4) = Weight1_1(_32, _36);

			// -- Row 10 --
			uint16_t _37 = palettetranslate[*(BlockSrc + 256 *  9    )];
			*(BlockDst + 320 *  9 + 0) = Weight3_1(_33, _37);
			uint16_t _38 = palettetranslate[*(BlockSrc + 256 *  9 + 1)];
			*(BlockDst + 320 *  9 + 1) = Weight3_1(Weight1_3(_33, _34), Weight1_3(_37, _38));
			uint16_t _39 = palettetranslate[*(BlockSrc + 256 *  9 + 2)];
			*(BlockDst + 320 *  9 + 2) = Weight3_1(Weight1_1(_34, _35), Weight1_1(_38, _39));
			uint16_t _40 = palettetranslate[*(BlockSrc + 256 *  9 + 3)];
			*(BlockDst + 320 *  9 + 3) = Weight3_1(Weight3_1(_35, _36), Weight3_1(_39, _40));
			*(BlockDst + 320 *  9 + 4) = Weight3_1(_36, _40);

			// -- Row 11 --
			uint16_t _41 = palettetranslate[*(BlockSrc + 256 * 10    )];
			*(BlockDst + 320 * 10 + 0) = Weight3_1(_37, _41);
			uint16_t _42 = palettetranslate[*(BlockSrc + 256 * 10 + 1)];
			*(BlockDst + 320 * 10 + 1) = Weight3_1(Weight1_3(_37, _38), Weight1_3(_41, _42));
			uint16_t _43 = palettetranslate[*(BlockSrc + 256 * 10 + 2)];
			*(BlockDst + 320 * 10 + 2) = Weight3_1(Weight1_1(_38, _39), Weight1_1(_42, _43));
			uint16_t _44 = palettetranslate[*(BlockSrc + 256 * 10 + 3)];
			*(BlockDst + 320 * 10 + 3) = Weight3_1(Weight3_1(_39, _40), Weight3_1(_43, _44));
			*(BlockDst + 320 * 10 + 4) = Weight3_1(_40, _44);

			// -- Row 12 --
			uint16_t _45 = palettetranslate[*(BlockSrc + 256 * 11    )];
			*(BlockDst + 320 * 11 + 0) = Weight3_1(_41, _45);
			uint16_t _46 = palettetranslate[*(BlockSrc + 256 * 11 + 1)];
			*(BlockDst + 320 * 11 + 1) = Weight3_1(Weight1_3(_41, _42), Weight1_3(_45, _46));
			uint16_t _47 = palettetranslate[*(BlockSrc + 256 * 11 + 2)];
			*(BlockDst + 320 * 11 + 2) = Weight3_1(Weight1_1(_42, _43), Weight1_1(_46, _47));
			uint16_t _48 = palettetranslate[*(BlockSrc + 256 * 11 + 3)];
			*(BlockDst + 320 * 11 + 3) = Weight3_1(Weight3_1(_43, _44), Weight3_1(_47, _48));
			*(BlockDst + 320 * 11 + 4) = Weight3_1(_44, _48);

			// -- Row 13 --
			uint16_t _49 = palettetranslate[*(BlockSrc + 256 * 12    )];
			*(BlockDst + 320 * 12 + 0) = Weight3_1(_45, _49);
			uint16_t _50 = palettetranslate[*(BlockSrc + 256 * 12 + 1)];
			*(BlockDst + 320 * 12 + 1) = Weight3_1(Weight1_3(_45, _46), Weight1_3(_49, _50));
			uint16_t _51 = palettetranslate[*(BlockSrc + 256 * 12 + 2)];
			*(BlockDst + 320 * 12 + 2) = Weight3_1(Weight1_1(_46, _47), Weight1_1(_50, _51));
			uint16_t _52 = palettetranslate[*(BlockSrc + 256 * 12 + 3)];
			*(BlockDst + 320 * 12 + 3) = Weight3_1(Weight3_1(_47, _48), Weight3_1(_51, _52));
			*(BlockDst + 320 * 12 + 4) = Weight3_1(_48, _52);

			// -- Row 14 --
			uint16_t _53 = palettetranslate[*(BlockSrc + 256 * 13    )];
			*(BlockDst + 320 * 13 + 0) = Weight3_1(_49, _53);
			uint16_t _54 = palettetranslate[*(BlockSrc + 256 * 13 + 1)];
			*(BlockDst + 320 * 13 + 1) = Weight3_1(Weight1_3(_49, _50), Weight1_3(_53, _54));
			uint16_t _55 = palettetranslate[*(BlockSrc + 256 * 13 + 2)];
			*(BlockDst + 320 * 13 + 2) = Weight3_1(Weight1_1(_50, _51), Weight1_1(_54, _55));
			uint16_t _56 = palettetranslate[*(BlockSrc + 256 * 13 + 3)];
			*(BlockDst + 320 * 13 + 3) = Weight3_1(Weight3_1(_51, _52), Weight3_1(_55, _56));
			*(BlockDst + 320 * 13 + 4) = Weight3_1(_52, _56);

			// -- Row 15 --
			*(BlockDst + 320 * 14 + 0) = _53;
			*(BlockDst + 320 * 14 + 1) = Weight1_3(_53, _54);
			*(BlockDst + 320 * 14 + 2) = Weight1_1(_54, _55);
			*(BlockDst + 320 * 14 + 3) = Weight3_1(_55, _56);
			*(BlockDst + 320 * 14 + 4) = _56;

			// -- Row 16 --
			uint16_t _57 = palettetranslate[*(BlockSrc + 256 * 14    )];
			*(BlockDst + 320 * 15 + 0) = _57;
			uint16_t _58 = palettetranslate[*(BlockSrc + 256 * 14 + 1)];
			*(BlockDst + 320 * 15 + 1) = Weight1_3(_57, _58);
			uint16_t _59 = palettetranslate[*(BlockSrc + 256 * 14 + 2)];
			*(BlockDst + 320 * 15 + 2) = Weight1_1(_58, _59);
			uint16_t _60 = palettetranslate[*(BlockSrc + 256 * 14 + 3)];
			*(BlockDst + 320 * 15 + 3) = Weight3_1(_59, _60);
			*(BlockDst + 320 * 15 + 4) = _60;

			// -- Row 17 --
			uint16_t _61 = palettetranslate[*(BlockSrc + 256 * 15    )];
			*(BlockDst + 320 * 16 + 0) = _61;
			uint16_t _62 = palettetranslate[*(BlockSrc + 256 * 15 + 1)];
			*(BlockDst + 320 * 16 + 1) = Weight1_3(_61, _62);
			uint16_t _63 = palettetranslate[*(BlockSrc + 256 * 15 + 2)];
			*(BlockDst + 320 * 16 + 2) = Weight1_1(_62, _63);
			uint16_t _64 = palettetranslate[*(BlockSrc + 256 * 15 + 3)];
			*(BlockDst + 320 * 16 + 3) = Weight3_1(_63, _64);
			*(BlockDst + 320 * 16 + 4) = _64;

			BlockSrc += 4;
			BlockDst += 5;
		}
	}
}
/*
 * JPEG2000 tables
 * Copyright (c) 2007 Kamil Nowosad
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVCODEC_J2K_H
#define AVCODEC_J2K_H

/**
 * JPEG2000 tables
 * @file
 * @author Kamil Nowosad
 */

#include "mqc.h"
#include "j2k_dwt.h"

enum Jpeg2000Markers{
    JPEG2000_SOC = 0xff4f, ///< start of codestream
    JPEG2000_SIZ = 0xff51, ///< image and tile size
    JPEG2000_COD,          ///< coding style default
    JPEG2000_COC,          ///< coding style component
    JPEG2000_TLM = 0xff55, ///< packed packet headers, tile-part header
    JPEG2000_PLM = 0xff57, ///< tile-part lengths
    JPEG2000_PLT,          ///< packet length, main header
    JPEG2000_QCD = 0xff5c, ///< quantization default
    JPEG2000_QCC,          ///< quantization component
    JPEG2000_RGN,          ///< region of interest
    JPEG2000_POC,          ///< progression order change
    JPEG2000_PPM,          ///< packet length, tile-part header
    JPEG2000_PPT,          ///< packed packet headers, main header
    JPEG2000_CRG = 0xff63, ///< component registration
    JPEG2000_COM,          ///< comment
    JPEG2000_SOT = 0xff90, ///< start of tile-part
    JPEG2000_SOP,          ///< start of packet
    JPEG2000_EPH,          ///< end of packet header
    JPEG2000_SOD,          ///< start of data
    JPEG2000_EOC = 0xffd9, ///< end of codestream
};

enum Jpeg2000Quantsty{ ///< quantization style
    JPEG2000_QSTY_NONE, ///< no quantization
    JPEG2000_QSTY_SI,   ///< scalar derived
    JPEG2000_QSTY_SE    ///< scalar expoounded
};

#define JPEG2000_MAX_CBLKW 64
#define JPEG2000_MAX_CBLKH 64

#define JPEG2000_MAX_RESLEVELS 33

// T1 flags
// flags determining significance of neighbour coefficients
#define JPEG2000_T1_SIG_N  0x0001
#define JPEG2000_T1_SIG_E  0x0002
#define JPEG2000_T1_SIG_W  0x0004
#define JPEG2000_T1_SIG_S  0x0008
#define JPEG2000_T1_SIG_NE 0x0010
#define JPEG2000_T1_SIG_NW 0x0020
#define JPEG2000_T1_SIG_SE 0x0040
#define JPEG2000_T1_SIG_SW 0x0080
#define JPEG2000_T1_SIG_NB (JPEG2000_T1_SIG_N | JPEG2000_T1_SIG_E | JPEG2000_T1_SIG_S | JPEG2000_T1_SIG_W \
                      |JPEG2000_T1_SIG_NE | JPEG2000_T1_SIG_NW | JPEG2000_T1_SIG_SE | JPEG2000_T1_SIG_SW)
// flags determining sign bit of neighbour coefficients
#define JPEG2000_T1_SGN_N  0x0100
#define JPEG2000_T1_SGN_S  0x0200
#define JPEG2000_T1_SGN_W  0x0400
#define JPEG2000_T1_SGN_E  0x0800

#define JPEG2000_T1_VIS    0x1000
#define JPEG2000_T1_SIG    0x2000
#define JPEG2000_T1_REF    0x4000

#define JPEG2000_T1_SGN    0x8000

// Codeblock coding styles
#define JPEG2000_CBLK_BYPASS    0x01 // Selective arithmetic coding bypass
#define JPEG2000_CBLK_RESET     0x02 // Reset context probabilities
#define JPEG2000_CBLK_TERMALL   0x04 // Terminate after each coding pass
#define JPEG2000_CBLK_VSC       0x08 // Vertical stripe causal context formation
#define JPEG2000_CBLK_PREDTERM  0x10 // Predictable termination
#define JPEG2000_CBLK_SEGSYM    0x20 // Segmentation symbols present

// Coding styles
#define JPEG2000_CSTY_PREC      0x01 // Precincts defined in coding style
#define JPEG2000_CSTY_SOP       0x02 // SOP marker present
#define JPEG2000_CSTY_EPH       0x04 // EPH marker present

typedef struct {
    int data[JPEG2000_MAX_CBLKW][JPEG2000_MAX_CBLKH];
    int flags[JPEG2000_MAX_CBLKW+2][JPEG2000_MAX_CBLKH+2];
    MqcState mqc;
} Jpeg2000T1Context;

typedef struct Jpeg2000TgtNode {
    uint8_t val;
    uint8_t vis;
    struct Jpeg2000TgtNode *parent;
} Jpeg2000TgtNode;

typedef struct Jpeg2000CodingStyle {
    uint8_t nreslevels;       // number of resolution levels
    uint8_t log2_cblk_width,
            log2_cblk_height; // exponent of codeblock size
    uint8_t transform;        // DWT type
    uint8_t csty;             // coding style
    uint8_t log2_prec_width,
            log2_prec_height; // precinct size
    uint8_t nlayers;          // number of layers
    uint8_t mct;              // multiple component transformation
    uint8_t cblk_style;       // codeblock coding style
    uint8_t prog_order;       // progression order
    uint8_t log2_prec_widths[JPEG2000_MAX_RESLEVELS];  // precincts size according resolution levels
    uint8_t log2_prec_heights[JPEG2000_MAX_RESLEVELS]; // TODO: initialize prec_size array with 0?
} Jpeg2000CodingStyle;

typedef struct {
    uint8_t  expn[32 * 3]; ///< quantization exponent
    uint16_t mant[32 * 3]; ///< quantization mantissa
    uint8_t  quantsty;     ///< quantization style
    uint8_t  nguardbits;   ///< number of guard bits
} Jpeg2000QuantStyle;

typedef struct {
    uint16_t rate;
    int64_t disto;
} Jpeg2000Pass;

typedef struct {
    uint8_t npasses;
    uint8_t ninclpasses; ///< number coding of passes included in codestream
    uint8_t nonzerobits;
    uint16_t length;
    uint16_t lengthinc;
    uint8_t lblock;
    uint8_t zero;
    uint8_t data[8192];
    Jpeg2000Pass passes[100];
} Jpeg2000Cblk; ///< code block

typedef struct {
    uint16_t xi0, xi1, yi0, yi1; ///< codeblock indexes ([xi0, xi1))
    Jpeg2000TgtNode *zerobits;
    Jpeg2000TgtNode *cblkincl;
} Jpeg2000Prec; ///< precinct

typedef struct {
    uint16_t coord[2][2]; ///< border coordinates {{x0, x1}, {y0, y1}}
    uint16_t codeblock_width, codeblock_height;
    uint16_t cblknx, cblkny;
    uint32_t stepsize; ///< quantization stepsize (* 2^13)
    Jpeg2000Prec *prec;
    Jpeg2000Cblk *cblk;
} Jpeg2000Band; ///< subband

typedef struct {
    uint8_t nbands;
    uint16_t coord[2][2]; ///< border coordinates {{x0, x1}, {y0, y1}}
    uint16_t num_precincts_x, num_precincts_y; ///< number of precincts in x/y direction
    uint8_t log2_prec_width, log2_prec_height; ///< exponent of precinct size
    Jpeg2000Band *band;
} Jpeg2000ResLevel; ///< resolution level

typedef struct {
   Jpeg2000ResLevel *reslevel;
   DWTContext dwt;
   int *data;
   uint16_t coord[2][2]; ///< border coordinates {{x0, x1}, {y0, y1}}
} Jpeg2000Component;

/* debug routines */
#if 0
#undef fprintf
#undef printf
void ff_j2k_printv(int *tab, int l);
void ff_j2k_printu(uint8_t *tab, int l);
#endif

/* misc tools */
static inline int ff_jpeg2000_ceildivpow2(int a, int b)
{
    return (a + (1 << b) - 1)>> b;
}

static inline int ff_jpeg2000_ceildiv(int a, int b)
{
    return (a + b - 1) / b;
}

/* tag tree routines */
Jpeg2000TgtNode *ff_j2k_tag_tree_init(int w, int h);

/* TIER-1 routines */
void ff_j2k_init_tier1_luts(void);

void ff_j2k_set_significant(Jpeg2000T1Context *t1, int x, int y, int negative);

extern uint8_t ff_jpeg2000_sigctxno_lut[256][4];

static inline int ff_j2k_getsigctxno(int flag, int bandno)
{
    return ff_jpeg2000_sigctxno_lut[flag&255][bandno];
}

/* Get context label (number in range[14..16]) of a coefficient for magnitude
 * refinement pass. */
static inline int ff_jpeg2000_getrefctxno(int flag)
{
    static const uint8_t refctxno_lut[2][2] = { { 14, 15 }, { 16, 16 } };
    return refctxno_lut[(flag>>14)&1][(flag & 255) != 0];
}

extern uint8_t ff_jpeg2000_sgnctxno_lut[16][16];
extern uint8_t ff_jpeg2000_xorbit_lut[16][16];

/* Get context label (number in range[9..13]) for sign decoding. */
static inline int ff_jpeg2000_getsgnctxno(int flag, int *xorbit)
{
    *xorbit = ff_jpeg2000_xorbit_lut[flag&15][(flag>>8)&15];
    return  ff_jpeg2000_sgnctxno_lut[flag&15][(flag>>8)&15];
}

int ff_j2k_init_component(Jpeg2000Component *comp, Jpeg2000CodingStyle *codsty, Jpeg2000QuantStyle *qntsty, int cbps, int dx, int dy);
void ff_j2k_reinit(Jpeg2000Component *comp, Jpeg2000CodingStyle *codsty);
void ff_j2k_cleanup(Jpeg2000Component *comp, Jpeg2000CodingStyle *codsty);

#endif /* AVCODEC_J2K_H */

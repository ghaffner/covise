/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Sun Microsystems, Inc.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

/* @(#)s_log1p.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/* double log1p(double x)
 *
 * Method :                  
 *   1. Argument Reduction: find k and f such that 
 *			1+x = 2^k * (1+f), 
 *	   where  sqrt(2)/2 < 1+f < sqrt(2) .
 *
 *      Note. If k=0, then f=x is exact. However, if k!=0, then f
 *	may not be representable exactly. In that case, a correction
 *	term is need. Let u=1+x rounded. Let c = (1+x)-u, then
 *	log(1+x) - log(u) ~ c/u. Thus, we proceed to compute log(u),
 *	and add back the correction term c/u.
 *	(Note: when x > 2**53, one can simply return log(x))
 *
 *   2. Approximation of log1p(f).
 *	Let s = f/(2+f) ; based on log(1+f) = log(1+s) - log(1-s)
 *		 = 2s + 2/3 s**3 + 2/5 s**5 + .....,
 *	     	 = 2s + s*R
 *      We use a special Reme algorithm on [0,0.1716] to generate 
 * 	a polynomial of degree 14 to approximate R The maximum error 
 *	of this polynomial approximation is bounded by 2**-58.45. In
 *	other words,
 *		        2      4      6      8      10      12      14
 *	    R(z) ~ Lp1*s +Lp2*s +Lp3*s +Lp4*s +Lp5*s  +Lp6*s  +Lp7*s
 *  	(the values of Lp1 to Lp7 are listed in the program)
 *	and
 *	    |      2          14          |     -58.45
 *	    | Lp1*s +...+Lp7*s    -  R(z) | <= 2 
 *	    |                             |
 *	Note that 2s = f - s*f = f - hfsq + s*hfsq, where hfsq = f*f/2.
 *	In order to guarantee error in log below 1ulp, we compute log
 *	by
 *		log1p(f) = f - (hfsq - s*(hfsq+R)).
 *	
 *	3. Finally, log1p(x) = k*ln2 + log1p(f).  
 *		 	     = k*ln2_hi+(f-(hfsq-(s*(hfsq+R)+k*ln2_lo)))
 *	   Here ln2 is split into two floating point number: 
 *			ln2_hi + ln2_lo,
 *	   where n*ln2_hi is always exact for |n| < 2000.
 *
 * Special cases:
 *	log1p(x) is NaN with signal if x < -1 (including -INF) ; 
 *	log1p(+INF) is +INF; log1p(-1) is -INF with signal;
 *	log1p(NaN) is that NaN with no signal.
 *
 * Accuracy:
 *	according to an error analysis, the error is always less than
 *	1 ulp (unit in the last place).
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following 
 * constants. The decimal values may be used, provided that the 
 * compiler will convert from decimal to binary accurately enough 
 * to produce the hexadecimal values shown.
 *
 * Note: Assuming log() return accurate answer, the following
 * 	 algorithm can be used to compute log1p(x) to within a few ULP:
 *	
 *		u = 1+x;
 *		if(u==1.0) return x ; else
 *			   return log(u)*(x/(u-1.0));
 *
 *	 See HP-15C Advanced Functions Handbook, p.193.
 */

#include "fdlibm.h"

#ifdef __STDC__
static const double
#else
static double
#endif
    ln2_hi = 6.93147180369123816490e-01, /* 3fe62e42 fee00000 */
    ln2_lo = 1.90821492927058770002e-10, /* 3dea39ef 35793c76 */
    two54 = 1.80143985094819840000e+16, /* 43500000 00000000 */
    Lp1 = 6.666666666666735130e-01, /* 3FE55555 55555593 */
    Lp2 = 3.999999999940941908e-01, /* 3FD99999 9997FA04 */
    Lp3 = 2.857142874366239149e-01, /* 3FD24924 94229359 */
    Lp4 = 2.222219843214978396e-01, /* 3FCC71C5 1D8E78AF */
    Lp5 = 1.818357216161805012e-01, /* 3FC74664 96CB03DE */
    Lp6 = 1.531383769920937332e-01, /* 3FC39A09 D078C69F */
    Lp7 = 1.479819860511658591e-01; /* 3FC2F112 DF3E5244 */

static double zero = 0.0;

#ifdef __STDC__
double fd_log1p(double x)
#else
double fd_log1p(x) double x;
#endif
{
    double hfsq, f, c, s, z, R, u;
    int k, hx, hu, ax;
    fd_twoints un;

    un.d = x;
    hx = __HI(un); /* high word of x */
    ax = hx & 0x7fffffff;

    k = 1;
    if (hx < 0x3FDA827A)
    { /* x < 0.41422  */
        if (ax >= 0x3ff00000)
        { /* x <= -1.0 */
            if (x == -1.0)
                return -two54 / zero; /* log1p(-1)=+inf */
            else
                return (x - x) / (x - x); /* log1p(x<-1)=NaN */
        }
        if (ax < 0x3e200000)
        { /* |x| < 2**-29 */
            if (two54 + x > zero /* raise inexact */
                && ax < 0x3c900000) /* |x| < 2**-54 */
                return x;
            else
                return x - x * x * 0.5;
        }
        if (hx > 0 || hx <= ((int)0xbfd2bec3))
        {
            k = 0;
            f = x;
            hu = 1;
        } /* -0.2929<x<0.41422 */
    }
    if (hx >= 0x7ff00000)
        return x + x;
    if (k != 0)
    {
        if (hx < 0x43400000)
        {
            u = 1.0 + x;
            un.d = u;
            hu = __HI(un); /* high word of u */
            k = (hu >> 20) - 1023;
            c = (k > 0) ? 1.0 - (u - x) : x - (u - 1.0); /* correction term */
            c /= u;
        }
        else
        {
            u = x;
            un.d = u;
            hu = __HI(un); /* high word of u */
            k = (hu >> 20) - 1023;
            c = 0;
        }
        hu &= 0x000fffff;
        if (hu < 0x6a09e)
        {
            un.d = u;
            __HI(un) = hu | 0x3ff00000; /* normalize u */
            u = un.d;
        }
        else
        {
            k += 1;
            un.d = u;
            __HI(un) = hu | 0x3fe00000; /* normalize u/2 */
            u = un.d;
            hu = (0x00100000 - hu) >> 2;
        }
        f = u - 1.0;
    }
    hfsq = 0.5 * f * f;
    if (hu == 0)
    { /* |f| < 2**-20 */
        if (f == zero)
        {
            if (k == 0)
                return zero;
            else
            {
                c += k * ln2_lo;
                return k * ln2_hi + c;
            }
        }
        R = hfsq * (1.0 - 0.66666666666666666 * f);
        if (k == 0)
            return f - R;
        else
            return k * ln2_hi - ((R - (k * ln2_lo + c)) - f);
    }
    s = f / (2.0 + f);
    z = s * s;
    R = z * (Lp1 + z * (Lp2 + z * (Lp3 + z * (Lp4 + z * (Lp5 + z * (Lp6 + z * Lp7))))));
    if (k == 0)
        return f - (hfsq - s * (hfsq + R));
    else
        return k * ln2_hi - ((hfsq - (s * (hfsq + R) + (k * ln2_lo + c))) - f);
}

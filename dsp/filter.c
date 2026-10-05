/* SPDX-License-Identifier: MIT */
#include "dd_filter.h"

int32_t dd_onepole_lp_run(dd_onepole *f, int32_t input, int32_t coeff)
{
    return dd_onepole_lp(f, input, coeff);
}

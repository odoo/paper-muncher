// UNIT(NAME, TYPE, STR, FACTOR)
// FACTOR converts a value in this unit into the canonical unit of its type,
// it is zero for units that depend on a context (font metrics, viewport, ...)

// https://drafts.csswg.org/css-values/#numbers
UNIT(NONE, NUMBER, "", 1)

// https://drafts.csswg.org/css-values/#percentages
UNIT(PERCENT, PERCENTAGE, "%", 1)

// https://drafts.csswg.org/css-values/#absolute-lengths
UNIT(CM, LENGTH, "cm", 96.0 / 2.54)
UNIT(MM, LENGTH, "mm", 96.0 / 25.4)
UNIT(Q, LENGTH, "Q", 96.0 / 101.6)
UNIT(IN, LENGTH, "in", 96.0)
UNIT(PT, LENGTH, "pt", 96.0 / 72.0)
UNIT(PC, LENGTH, "pc", 96.0 / 6.0)
UNIT(PX, LENGTH, "px", 1)

// https://drafts.csswg.org/css-values/#relative-lengths
UNIT(EM, LENGTH, "em", 0)
UNIT(REM, LENGTH, "rem", 0)
UNIT(EX, LENGTH, "ex", 0)
UNIT(REX, LENGTH, "rex", 0)
UNIT(CAP, LENGTH, "cap", 0)
UNIT(RCAP, LENGTH, "rcap", 0)
UNIT(CH, LENGTH, "ch", 0)
UNIT(RCH, LENGTH, "rch", 0)
UNIT(IC, LENGTH, "ic", 0)
UNIT(RIC, LENGTH, "ric", 0)
UNIT(LH, LENGTH, "lh", 0)
UNIT(RLH, LENGTH, "rlh", 0)
UNIT(VW, LENGTH, "vw", 0)
UNIT(SVW, LENGTH, "svw", 0)
UNIT(LVW, LENGTH, "lvw", 0)
UNIT(DVW, LENGTH, "dvw", 0)
UNIT(VH, LENGTH, "vh", 0)
UNIT(SVH, LENGTH, "svh", 0)
UNIT(LVH, LENGTH, "lvh", 0)
UNIT(DVH, LENGTH, "dvh", 0)
UNIT(VI, LENGTH, "vi", 0)
UNIT(SVI, LENGTH, "svi", 0)
UNIT(LVI, LENGTH, "lvi", 0)
UNIT(DVI, LENGTH, "dvi", 0)
UNIT(VB, LENGTH, "vb", 0)
UNIT(SVB, LENGTH, "svb", 0)
UNIT(LVB, LENGTH, "lvb", 0)
UNIT(DVB, LENGTH, "dvb", 0)
UNIT(VMIN, LENGTH, "vmin", 0)
UNIT(SVMIN, LENGTH, "svmin", 0)
UNIT(LVMIN, LENGTH, "lvmin", 0)
UNIT(DVMIN, LENGTH, "dvmin", 0)
UNIT(VMAX, LENGTH, "vmax", 0)
UNIT(SVMAX, LENGTH, "svmax", 0)
UNIT(LVMAX, LENGTH, "lvmax", 0)
UNIT(DVMAX, LENGTH, "dvmax", 0)

// https://drafts.csswg.org/css-values/#angles
UNIT(DEG, ANGLE, "deg", 1)
UNIT(GRAD, ANGLE, "grad", 0.9)
UNIT(RAD, ANGLE, "rad", 180.0 / Math::PI)
UNIT(TURN, ANGLE, "turn", 360.0)

// https://drafts.csswg.org/css-values/#time
UNIT(S, TIME, "s", 1)
UNIT(MS, TIME, "ms", 0.001)

// https://drafts.csswg.org/css-values/#frequency
UNIT(HZ, FREQUENCY, "Hz", 1)
UNIT(KHZ, FREQUENCY, "kHz", 1000.0)

// https://drafts.csswg.org/css-values/#resolution
UNIT(DPPX, RESOLUTION, "dppx", 1)
UNIT(DPI, RESOLUTION, "dpi", 1.0 / 96.0)
UNIT(DPCM, RESOLUTION, "dpcm", 2.54 / 96.0)
UNIT(X, RESOLUTION, "x", 1)

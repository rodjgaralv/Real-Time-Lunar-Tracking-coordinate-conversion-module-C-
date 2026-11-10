# Real-Time-Lunar-Tracking-coordinate-conversion-module-C-

2022 university project (joint project across degrees). This module converts between horizontal (ALT/AZ) and equatorial (RA/DEC) coordinates for a fixed observer location (León, Spain) using the system UTC time. It was used by the telescope-tracking software to keep the Moon centred (>1 h of continuous tracking achieved).

This is a 2022 university project, kept as-is. Known issues:

- **Year-specific time offset.** The day count in `coordLST` uses a hard-coded
  offset (`7669.5`) tied to a reference date, intended for the May–June 2022
  test period. It must be recomputed for other dates; the sidereal time will
  drift otherwise.
- **East-side objects in `altAz2RaDec`.** The hour angle is obtained with
  `acos`, which only returns values in [0, π], so RA is wrong for objects
  in the east (azimuth < 180°). The test value (az = 253.24°) lies in the
  west and works. `raDec2AltAz` handles the sign through `sin(HA)`.
- **`coordLST` return type.** It is declared `double` but returns nothing;
  results are passed through the output pointer. It should be `void`.
- **Ineffective range calls.** Several calls to `range_radians(X)` discard the
  returned value, so they have no effect.
- **Non-standard `main`.** `void main()` should be `int main()`.
- **Fixed observer location.** Latitude and longitude are hard-coded (León, Spain).


Credits: ANSI colour definitions and printAngleValue were provided by the course instructor; the conversion logic is my own.

All tests has been done 5 times in a row.

UNI - This library<br />
ICU - International Components for Unicode<br />
U8PROC - utf8proc library is used instead of WinAPI. This is the only library that I found that does UTF-8 normalization and passes all Unicode tests. Even though it's pretty slow. https://github.com/JuliaStrings/utf8proc

Note 1: ICU is tested only in MSVS because I'm too lazy to compile ICU in MinGW.<br />
Note 2: ICU is ~2 times faster because algorithms are different. An algorithm that does support streams (this library) is always slower than an algorithm that doesn't support streams (ICU) nothing can be done about this.<br />
Note 0: All other notes in this file are not for a user.<br />

## Real text:

GCC 7.3 x64 -03
```
UNI     U8PROC  ICU
1231.84 8480.36 0
1236.02 8494.4  0
1235.23 8471.57 0
1231.31 8475.52 0
1239.2  8482.13 0
```
GCC 7.3 x86 -03
```
UNI     U8PROC  ICU
1665.09 9307.08 0
1662.73 9274.79 0
1661.77 9268.19 0
1663.84 9278.38 0
1664.4  9294.28 0
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     U8PROC  ICU
1291.52 7842.2  695.346
1292.96 7843.95 695.374
1293.54 7840.23 695.129
1290.94 7842.32 694.525
1296.94 7845.43 695.199

Note: utf8proc is so much slower because
it's doing normalization in multiple passes
and it doesn't use many optimizations.
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     U8PROC  ICU
1384.07 9539.86 877.376
1385.06 9539.76 877.95
1381.94 9552.83 880.248
1382.56 9539.6  878.2
1381.53 9539.25 879.258
```
## Stress tests:

A text full of code points that need to be decomposed/composed.
This means it tests slow paths of the implementations.<br />
Note that results of the stress tests are pretty random for all
implementations so they are not much useful.<br />

GCC 7.3 x64 -03
```
UNI     U8PROC  ICU
545.3   1490.97 0
545.393 1490.81 0
545.353 1491.13 0
545.379 1489.03 0
545.592 1490.26 0
```
GCC 7.3 x86 -03
```
UNI     U8PROC  ICU
639.768 1681.89 0
639.774 1698.06 0
639.868 1697.84 0
640.472 1690.54 0
639.357 1713.21 0
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     U8PROC  ICU
466.636 1415.69 1446.18
466.759 1434.46 1446.6
467.243 1417.12 1445.91
467.118 1416.27 1445.8
467.191 1414.74 1446.44

Note: slow path in ICU is slower
the same as in UTF-16 test.
ICU uses almost the same code
for UTF-8 and UTF-16 for normalization.
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     U8PROC  ICU
577.512 1665.82 1515.16
577.483 1663.58 1517
577.435 1671.18 1518.45
578.093 1673.26 1514.41
577.729 1684.45 1514.76
```
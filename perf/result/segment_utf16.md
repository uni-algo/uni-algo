All tests has been done 5 times in a row.

UNI - This library<br />
ICU - International Components for Unicode<br />

Note 1: ICU is tested only in MSVS because I'm too lazy to compile ICU in MinGW.<br />
Note 2: Only UTF-16 version of this test exists because cluster breaks in ICU doesn't support UTF-8.<br />
Note 0: All other notes in this file are not for a user.<br />

## Real text:

## Word breaks:

GCC 7.3 x64 -03
```
UNI     ICU
1467.2  0
1465.59 0
1466.42 0
1468.3  0
1468.38 0
```
GCC 7.3 x86 -03
```
UNI     ICU
1642.75 0
1644.08 0
1644.42 0
1641.58 0
1652.66 0
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     ICU
2355.47 11379.4
2352.5  11280.1
2354.22 11278.5
2350.5  11273.1
2343.36 11276.2
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     ICU
2544.55 13137.1
2547.64 13142.5
2548.46 13137.8
2542.21 13124.9
2545.85 13145.7
```

## Grapheme breaks:

GCC 7.3 x64 -03
```
UNI     ICU
1035.15 0
1031.15 0
1031.3  0
1031.75 0
1049.06 0
```
GCC 7.3 x86 -03
```
UNI     ICU
1042.17 0
1042.09 0
1042.69 0
1042.3  0
1042.59 0
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     ICU
2067.16 5600.95
2062.45 5575.92
2067.21 5585.55
2066.45 5584.88
2067.33 5584.72
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     ICU
2239.48 6766.44
2241.99 6750.64
2243.27 6756.44
2242.86 6756.73
2242.65 6752.65
```
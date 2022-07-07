All tests has been done 5 times in a row.

UNI - This library<br />
ICU - International Components for Unicode<br />
WINAPI - NormalizeString from WinAPI<br />

Note 1: ICU is tested only in MSVS because I'm too lazy to compile ICU in MinGW.<br />
Note 2: ICU is ~2 times faster because algorithms are different. An algorithm that does support streams (this library) is always slower than an algorithms that doesn't support streams (ICU) nothing can be done about this.<br />
Note 0: All other notes in this file are not for a user.<br />

## Real text:

GCC 7.3 x64 -03
```
UNI     WINAPI  ICU
737.422 2494.96 0
740.527 2496.61 0
739.074 2493.44 0
737.954 2491.78 0
738.369 2486.46 0
```
GCC 7.3 x86 -03
```
UNI     WINAPI  ICU
910.408 3132.11 0
913.373 3133.54 0
912.747 3133.29 0
915.935 3136.35 0
909.576 3130.7  0
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU
753.008 2465.19 426.883
755.545 2461.49 427.179
751.436 2460.66 421.4
751.924 2465.47 424.571
755.691 2460.01 428.786
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU
893.635 3137.05 535.819
893.7   3137.68 534.789
893.809 3139.48 533.63
894.47  3142.91 533.85
893.869 3137.17 533.161
```
## Stress tests:
A text full of code points that need to be decomposed/composed.
This means it tests slow paths of the implementations.<br />
Note that results of the stress tests are pretty random for all
implementations so they are not much useful.<br />

GCC 7.3 x64 -03
```
UNI     WINAPI  ICU
510.26  959.849 0
510.11  960.604 0
510.236 959.992 0
510.116 960.208 0
509.912 960.663 0

Note: it seems WINAPI doesn't even use Quick_Check optimizations
because normalization in WINAPI isn't even that slow
but it is slow for a real text.
```
GCC 7.3 x86 -03
```
UNI     WINAPI  ICU
512.658 1640.14 0
512.826 1640.21 0
512.801 1641.4  0
512.559 1641.52 0
512.469 1640.3  0
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU
419.421 953.093 747.876
414.824 949.931 746.604
413.255 951.33  746.346
413.184 951.793 746.92
413.422 951.236 747.247

Note 1: slow path in ICU is actually slower
it means in some cases ICU will be slower
in very rare cases, though.
Note 2: huh, GCC slower than MSVC in this test.
Probably something random affects it, GCC usually faster.
It's not like it matters anyway,
optimizations for stress tests are useless,
so I don't even want to check what is going on there.
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU
496.772 1637.74 918.396
497.821 1636.7  918.5
498.027 1636.19 918.75
497.997 1635.77 918.042
498.145 1636.9  920.714
```
All tests has been done 5 times in a row.

UNI - This library<br />
ICU - International Components for Unicode<br />
WINAPI - MultiByteToWideChar from WinAPI<br />
KEWB - The fastest UTF-8 branchless algorithm out there (see test/perf_kewb.h)<br />

Note 1: ICU is tested only in MSVS because I'm too lazy to compile ICU in MinGW.<br />
Note 2: -march=native isn't used in GCC. You can achieve a bit better results if you use it.<br />
Note 3: The test also shows how fast is internal UTF-8 iterator function because it's based on the same algorithm.<br />
Note 0: All other notes in this file are not for a user.<br />

## Real text:

GCC 7.3 x64 -03
```
UNI     WINAPI  ICU     KEWB
402.3   493.499 0       537.054
402.734 493.95  0       536.852
400.977 493.267 0       536.337
408.159 492.263 0       542.237
402.387 493.486 0       538.899
```
GCC 7.3 x86 -03
```
UNI     WINAPI  ICU     KEWB
513.256 718.537 0       1422.2
513.87  717.234 0       1422.92
512.477 719.009 0       1427.61
512.265 718.864 0       1422.18
513.016 718.76  0       1422.59

Note: I don't know what's going on with KEWB in the test
probably GCC messed up an optimization somewhere.
It sometimes happens especially in x86.
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU     KEWB
515.628 477.285 531.183 696.07
514.986 477.573 531.019 694.857
514.476 475.278 530.894 695.839
512.782 488.287 531.961 696.147
514.996 476.529 534.01  696.13
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU     KEWB
654.443 672.265 580.729 735.058
654.045 668.346 580.893 733.838
652.204 670.193 580.568 736.285
654.825 669.07  582.058 735.598
653.491 670.287 583.051 735.786
```

## Stress tests:
Generated sequences of bytes.<br />
Note that results of the stress tests are pretty random for all
implementations so they are not much useful.<br />
## 1-byte sequences (ASCII):
GCC 7.3 x64 -03
```
UNI     WINAPI  ICU     KEWB
110.796 113.007 0       109.106
110.422 112.896 0       108.877
110.378 112.796 0       109.162
110.461 113.736 0       109.353
110.643 112.861 0       108.954

Note: WINAPI actually faster for long strings
probably uses some optimizations for ASCII.
It's more noticeably in MSVC.
GCC performs some black magic here to make the result better.
```
GCC 7.3 x86 -03
```
UNI     WINAPI  ICU     KEWB
136.462 153.175 0       133.924
136.226 153.242 0       134.107
136.54  153.347 0       133.996
135.51  153.165 0       134.047
135.775 153.166 0       134.141
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU     KEWB
142.284 125.528 208.577 126.253
142.032 125.174 208.776 126.399
142.333 125.229 209.375 126.602
142.313 125.266 208.284 126.355
142.35  125.134 208.557 126.45

Note 1: faster in GCC because of __builtin_expect
MSVC probably messed up the fast path the same way.
Actually it's 100% the fast path because KEWB is faster too.
I'm too lazy to check asm.
Note 2: KEBW is slower than WINAPI for long strings too
so WINAPI 100% uses something for ASCII.
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU     KEWB
166.918 158.468 240.937 145.017
167.043 158.557 241.615 144.942
166.81  158.547 241.174 145.005
167.082 158.74  240.864 145.096
166.919 158.49  240.92  145.035
```
## 2-byte sequences:
GCC 7.3 x64 -03
```
UNI     WINAPI  ICU     KEWB
324.759 378.568 0       336.692
324.543 376.452 0       336.694
325.278 390.477 0       339.38
325.472 376.968 0       336.835
324.491 376.463 0       336.812
```
GCC 7.3 x86 -03
```
UNI     WINAPI  ICU     KEWB
382.491 482.652 0       417.657
382.887 482.629 0       417.513
382.283 482.538 0       417.413
382.973 483.016 0       417.774
382.451 482.918 0       417.347
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU     KEWB
353.858 372.45  405.7   372.719
353.104 372.408 405.904 372.852
352.962 372.411 405.97  372.649
352.926 372.196 405.753 372.148
352.439 372.186 405.69  373.063
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU     KEWB
408.206 461.633 434.053 401.144
408.315 461.663 434.376 400.964
407.925 461.535 434.099 400.923
408.483 461.573 434.552 401.071
407.725 461.557 434.249 407.902
```
## 3-byte sequences:
GCC 7.3 x64 -03
```
UNI     WINAPI  ICU     KEWB
442.559 424.33  0       446.286
442.267 423.948 0       446.387
443.036 431.009 0       462.878
446.513 425.018 0       447.648
442.467 424.454 0       446.712
```
GCC 7.3 x86 -03
```
UNI     WINAPI  ICU     KEWB
507.97  548.486 0       760.712
508.183 548.708 0       760.503
507.989 548.679 0       761.427
511.195 548.808 0       760.774
510.605 548.524 0       760.489
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU     KEWB
459.772 408.516 419.09  489.093
459.918 409.268 428.11  489.04
460.373 408.707 419.114 489.569
459.833 408.286 419.033 488.85
459.794 408.704 418.958 489.117

Note: ICU optimizes 3-byte for CJK,
don't know about WINAPI, maybe too? Because they both slower for 2-byte.
This need to be tested with GCC. I want to see the difference there.
It shouldn't affect the real performance much, but still.
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU     KEWB
574.793 527.551 462.457 574.85
575.084 527.717 462.343 575.723
575.17  527.955 462.46  584.386
575.272 528.791 462.22  576.395
574.509 528.302 462.684 576.396
```
## 4-byte sequences:
GCC 7.3 x64 -03
```
UNI     WINAPI  ICU     KEWB
326.438 365.803 0       358.186
326.397 365.975 0       358.329
325.774 366.136 0       358.689
327.084 373.486 0       360.107
327.22  366.385 0       357.99
```
GCC 7.3 x86 -03
```
UNI     WINAPI  ICU     KEWB
395.417 508.793 0       797.033
395.713 523.06  0       797.518
395.286 509.975 0       797.507
394.962 509.003 0       801.11
395.95  517.491 0       797.425
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU     KEWB
342.955 354.997 340.819 406.506
357.045 354.639 340.838 410.213
343.421 354.509 340.952 405.719
343.132 354.334 340.767 406.283
343.237 354.469 340.971 406.153
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     WINAPI  ICU     KEWB
447.418 473.652 389.851 461.938
447.579 483.576 390.118 462.284
447.282 474.02  389.643 461.85
448.107 473.828 389.551 461.724
447.588 473.993 389.655 461.445
```
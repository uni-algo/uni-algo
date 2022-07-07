All tests has been done 5 times in a row.

UNI - This library<br />
ICU - International Components for Unicode<br />

Note 1: ICU is tested only in MSVS because I'm too lazy to compile ICU in MinGW.<br />
Note 2: Only UTF-16 version of this test exists because Transliterator in ICU doesn't support UTF-8.<br />
Note 3: remove_grave_and_acute_accents from examples is used in this test and ICU Transliterator with the rule: "NFD; [[\u0300][\u0301]] Remove; NFC" it produces exactly the same result.<br />
Note 0: All other notes in this file are not for a user.<br />

## Real text:

GCC 7.3 x64 -03
```
UNI     ICU
4599.45 0
4597.67 0
4596.96 0
4608.82 0
4602.34 0
```
GCC 7.3 x86 -03
```
UNI     ICU
4716.02 0
4718.63 0
4717.75 0
4716.11 0
4717.14 0
```
MSVC 19.12 x64 (MSVS 2017 15.5.5)
```
UNI     ICU
10148.2 36262.1
10138.2 36280.2
10146   36284.7
10145.4 36290
10149.6 36298.8

Note 1: I don't know why it's so much slower in MSVC.
Probably MSVC just doesn't optimize enough.
Note 2: ICU is so slow because Transliterator in ICU
is kind of complicated thing. It can do some crazy stuff
but of course you pay for this with the performance.
```
MSVC 19.12 x86 (MSVS 2017 15.5.5)
```
UNI     ICU
13166   42713.9
13165.6 42925.6
13155.3 42619.1
13131.5 42678
13156.7 42672.8
```

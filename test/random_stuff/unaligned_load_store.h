// Test functions to check on https://godbolt.org or whatever
// to see which compilers can optimize manual unaligned load/store
// load8/store8/load16/store16 should produce: mov, ret
// load8_2/store8_2/load16_2/store16_2 should produce: mov, mov, ret
// if a compiler was able to optimize it.

#include <string.h>

unsigned int load8(char* V)
{
    unsigned int M = 0;
    M |= ((unsigned int)*(V+0) & 0xFF);
    M |= ((unsigned int)*(V+1) & 0xFF) << 8;
    M |= ((unsigned int)*(V+2) & 0xFF) << 16;
    M |= ((unsigned int)*(V+3) & 0xFF) << 24;
    //memcpy(&M, V, 4);
    //M = *((unsigned int*)V);
    return M;
}
void store8(char* V, unsigned int M)
{
    *(V+0) = (char)(M & 0xFF);
    *(V+1) = (char)((M >> 8) & 0xFF);
    *(V+2) = (char)((M >> 16) & 0xFF);
    *(V+3) = (char)((M >> 24) & 0xFF);
    //memcpy(V, &M, 4);
}
unsigned int load16(short* V)
{
    unsigned int M = 0;
    M |= ((unsigned int)*(V+0) & 0xFFFF);
    M |= ((unsigned int)*(V+1) & 0xFF) << 16;
    //memcpy(&M, V, 4);
    //M = *((unsigned int*)V);
    return M;
}
void store16(short* V, unsigned int M)
{
    *(V+0) = (unsigned short)(M & 0xFFFF);
    *(V+1) = (unsigned short)(M >> 16);
    //memcpy(V, &M, 4);
}

void load8_2(char* V, unsigned int* T)
{
    unsigned int M = 0;
    M |= ((unsigned int)*(V+0) & 0xFF);
    M |= ((unsigned int)*(V+1) & 0xFF) << 8;
    M |= ((unsigned int)*(V+2) & 0xFF) << 16;
    M |= ((unsigned int)*(V+3) & 0xFF) << 24;
    *T = M;
    //memcpy(T, V, 4);
    //*T = *((unsigned int*)V);
}
void store8_2(char* V, unsigned int* T)
{
    unsigned int M = *T;
    *(V+0) = (char)(M & 0xFF);
    *(V+1) = (char)((M >> 8) & 0xFF);
    *(V+2) = (char)((M >> 16) & 0xFF);
    *(V+3) = (char)((M >> 24) & 0xFF);
    //memcpy(V, T, 4);
}
void load16_2(short* V, unsigned int* T)
{
    unsigned int M = 0;
    M |= ((unsigned int)*((V+0) & 0xFFFF);
    M |= ((unsigned int)*((V+1) >> 16);
    *T = M;
    //memcpy(T, V, 4);
    //*T = *((unsigned int*)V);
}
void store16_2(short* V, unsigned int* T)
{
    unsigned int M = *T;
    *(V+0) = (unsigned short)(M & 0xFFFF);
    *(V+1) = (unsigned short)(M >> 16);
    //memcpy(V, T, 4);
}

// Optimized output must be (x86/x64 GCC, Clang):
#if 0
load8(char*):
  mov eax, DWORD PTR [rdi]
  ret
store8(char*, unsigned int):
  mov DWORD PTR [rdi], esi
  ret
load16(short*):
  mov eax, DWORD PTR [rdi]
  ret
store16(short*, unsigned int):
  mov DWORD PTR [rdi], esi
  ret
load8_2(char*, unsigned int*):
  mov eax, DWORD PTR [rdi]
  mov DWORD PTR [rsi], eax
  ret
store8_2(char*, unsigned int*):
  mov eax, DWORD PTR [rsi]
  mov DWORD PTR [rdi], eax
  ret
load16_2(short*, unsigned int*):
  mov eax, DWORD PTR [rdi]
  mov DWORD PTR [rsi], eax
  ret
store16_2(short*, unsigned int*):
  mov eax, DWORD PTR [rsi]
  mov DWORD PTR [rdi], eax
  ret
#endif

// memcpy version must be the same output when little endian (x64/x86)
// but different when big endian (ARM GCC -mbig-endian for example)
#if 0
#include <string.h>
unsigned int load8(char* V)
{
    unsigned int M = 0;
    memcpy(&M, V, 4);
    return M;
}
void store8(char* V, unsigned int M)
{
    memcpy(V, &M, 4);
}
unsigned int load16(short* V)
{
    unsigned int M = 0;
    memcpy(&M, V, 4);
    return M;
}
void store16(short* V, unsigned int M)
{
    memcpy(V, &M, 4);
}

void load8_2(char* V, unsigned int* T)
{
    memcpy(T, V, 4);
}
void store8_2(char* V, unsigned int* T)
{
    memcpy(V, T, 4);
}
void load16_2(short* V, unsigned int* T)
{
    memcpy(T, V, 4);
}
void store16_2(short* V, unsigned int* T)
{
    memcpy(V, T, 4);
}
#endif

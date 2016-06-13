// Minimal defines for primitives for the EXCESS experiment framework.
// Copyright (C) 2014  Anders Gidenstam
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef _PRIMITIVES_H
#define _PRIMITIVES_H

// Standard (pointer sized) implementations for 64-bit GCC.
#define STDCAS(addr, old, new) \
  __sync_val_compare_and_swap_8((addr), (old), (new))

#define STDFAA(addr, incr) \
  __sync_fetch_and_add_8((addr), (incr))

// 64-bit implementations for GCC.
#define CAS64(addr, old, new) \
  __sync_val_compare_and_swap_8((addr), (old), (new))

#define FAA64(addr, incr) \
  __sync_fetch_and_add_8((addr), (incr))

// 32-bit implementations for GCC.
#define CAS32(addr, old, new) \
  __sync_val_compare_and_swap_4((addr), (old), (new))

#define FAA32(addr, incr) \
  __sync_fetch_and_add_4((addr), (incr))


// Macros to access the cycle count register of the CPU cores.
#ifdef __x86_64__

#define start_cycle_count(val)                  \
  {                                             \
    unsigned int cycles_low, cycles_high;       \
    asm volatile                                \
      ("cpuid\n\t"                              \
       "rdtscp\n\t"                             \
       "mov %%edx, %0\n\t"                      \
       "mov %%eax, %1\n\t"                      \
       : "=r" (cycles_high),                    \
         "=r" (cycles_low)                      \
       :: "%rax", "%rbx", "%rcx", "%rdx");      \
       (val) =                                  \
         ((unsigned long) cycles_low) |         \
         (((unsigned long) cycles_high) << 32); \
  }

#define stop_cycle_count(val)                   \
   {                                            \
    unsigned int cycles_low, cycles_high;       \
    asm volatile                                \
      ("rdtscp\n\t"                             \
       "mov %%edx, %0\n\t"                      \
       "mov %%eax, %1\n\t"                      \
       "cpuid\n\t"                              \
       : "=r" (cycles_high),                    \
         "=r" (cycles_low)                      \
       :: "%rax", "%rbx", "%rcx", "%rdx");      \
       (val) =                                  \
         ((unsigned long) cycles_low) |         \
         (((unsigned long) cycles_high) << 32); \
  }

#endif

#endif

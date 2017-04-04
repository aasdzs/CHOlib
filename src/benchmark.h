////////////////////////////////////////////////////////////////////////////
// The source of this code is from the Keccak(SHA-3) code,
// released in Keccak homepage(http://keccak.noekeon.org)
////////////////////////////////////////////////////////////////////////////
#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include <memory.h>

typedef unsigned int uint_32t;

// ���ġ ���� ���� �ݺ�
#define AVERAGE_COUNT		(1000)

// �ݺ� ���� ���� �� �ּ� �ð� ����
#define ITERATION_COUNT		(100) 

uint_32t HiResTime(void);           /* return the current value of time stamp counter */
uint_32t calibrate();

void Benchmark_Test();
void Benchmark_Target();


#else
#endif
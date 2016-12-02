#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>

#ifndef min
#define min(A,B) ((A)<(B)?(A):(B))
#endif

// implementation of copy from BAR using MOVNTDQA 
// suggested by Nicholas Wilt <nwilt@amazon.com>

void* memcpy_uncached_load_sse41(void *dest, const void *src, size_t n_bytes)
{
	int ret = 0;
	//#ifdef __SSE4_1__
	char *d = (char*)dest;
	uintptr_t d_int = (uintptr_t)d;
	const char *s = (const char *)src;
	uintptr_t s_int = (uintptr_t)s;
	size_t n = n_bytes;

	// align src to 128-bits
	if (s_int & 0xf) {
		size_t nh = min(0x10 - (s_int & 0x0f), n);
		memcpy(d, s, nh);
		d += nh; d_int += nh;
		s += nh; s_int += nh;
		n -= nh;
	}

	if (d_int & 0xf) { // dest is not aligned to 128-bits
		__m128i r0, r1, r2, r3, r4, r5, r6, r7;
		// unroll 8
		while (n >= 8 * sizeof(__m128i)) {
			r0 = _mm_stream_load_si128((__m128i *)(s + 0 * sizeof(__m128i)));
			r1 = _mm_stream_load_si128((__m128i *)(s + 1 * sizeof(__m128i)));
			r2 = _mm_stream_load_si128((__m128i *)(s + 2 * sizeof(__m128i)));
			r3 = _mm_stream_load_si128((__m128i *)(s + 3 * sizeof(__m128i)));
			r4 = _mm_stream_load_si128((__m128i *)(s + 4 * sizeof(__m128i)));
			r5 = _mm_stream_load_si128((__m128i *)(s + 5 * sizeof(__m128i)));
			r6 = _mm_stream_load_si128((__m128i *)(s + 6 * sizeof(__m128i)));
			r7 = _mm_stream_load_si128((__m128i *)(s + 7 * sizeof(__m128i)));
			_mm_storeu_si128((__m128i *)(d + 0 * sizeof(__m128i)), r0);
			_mm_storeu_si128((__m128i *)(d + 1 * sizeof(__m128i)), r1);
			_mm_storeu_si128((__m128i *)(d + 2 * sizeof(__m128i)), r2);
			_mm_storeu_si128((__m128i *)(d + 3 * sizeof(__m128i)), r3);
			_mm_storeu_si128((__m128i *)(d + 4 * sizeof(__m128i)), r4);
			_mm_storeu_si128((__m128i *)(d + 5 * sizeof(__m128i)), r5);
			_mm_storeu_si128((__m128i *)(d + 6 * sizeof(__m128i)), r6);
			_mm_storeu_si128((__m128i *)(d + 7 * sizeof(__m128i)), r7);
			s += 8 * sizeof(__m128i);
			d += 8 * sizeof(__m128i);
			n -= 8 * sizeof(__m128i);
		}
		while (n >= sizeof(__m128i)) {
			r0 = _mm_stream_load_si128((__m128i *)(s + 0 * sizeof(__m128i)));
			_mm_storeu_si128((__m128i *)(d + 0 * sizeof(__m128i)), r0);
			s += sizeof(__m128i);
			d += sizeof(__m128i);
			n -= sizeof(__m128i);
		}
	}
	else { // or it IS aligned
		__m128i r0, r1, r2, r3, r4, r5, r6, r7;
		// unroll 8
		while (n >= 4 * sizeof(__m128i)) {
			r0 = _mm_stream_load_si128((__m128i *)(s + 0 * sizeof(__m128i)));
			r1 = _mm_stream_load_si128((__m128i *)(s + 1 * sizeof(__m128i)));
			r2 = _mm_stream_load_si128((__m128i *)(s + 2 * sizeof(__m128i)));
			r3 = _mm_stream_load_si128((__m128i *)(s + 3 * sizeof(__m128i)));
			r4 = _mm_stream_load_si128((__m128i *)(s + 4 * sizeof(__m128i)));
			r5 = _mm_stream_load_si128((__m128i *)(s + 5 * sizeof(__m128i)));
			r6 = _mm_stream_load_si128((__m128i *)(s + 6 * sizeof(__m128i)));
			r7 = _mm_stream_load_si128((__m128i *)(s + 7 * sizeof(__m128i)));
			_mm_stream_si128((__m128i *)(d + 0 * sizeof(__m128i)), r0);
			_mm_stream_si128((__m128i *)(d + 1 * sizeof(__m128i)), r1);
			_mm_stream_si128((__m128i *)(d + 2 * sizeof(__m128i)), r2);
			_mm_stream_si128((__m128i *)(d + 3 * sizeof(__m128i)), r3);
			_mm_stream_si128((__m128i *)(d + 4 * sizeof(__m128i)), r4);
			_mm_stream_si128((__m128i *)(d + 5 * sizeof(__m128i)), r5);
			_mm_stream_si128((__m128i *)(d + 6 * sizeof(__m128i)), r6);
			_mm_stream_si128((__m128i *)(d + 7 * sizeof(__m128i)), r7);
			s += 8 * sizeof(__m128i);
			d += 8 * sizeof(__m128i);
			n -= 8 * sizeof(__m128i);
		}
		while (n >= sizeof(__m128i)) {
			r0 = _mm_stream_load_si128((__m128i *)(s + 0 * sizeof(__m128i)));
			_mm_stream_si128((__m128i *)(d + 0 * sizeof(__m128i)), r0);
			s += sizeof(__m128i);
			d += sizeof(__m128i);
			n -= sizeof(__m128i);
		}
	}
	_mm_sfence();
	if (n)
		memcpy(d, s, n);
	//#else
	//#error "this file should be compiled with -msse4.1"
	//#endif
	return dest;
}
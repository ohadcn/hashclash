Some of the projects in this solution require the Boost C++ libraries.
Please first install Boost and then perform these steps:
- copy \trunk\vs2013\boost32.props.example to \trunk\vs2013\boost32.props
- copy \trunk\vs2013\boost64.props.example to \trunk\vs2013\boost64.props
- edit boost32.props and boost64.props to reflect the proper directories

This solution contains the following projects:

*** parse_bitrel (requires boost)
This program takes as input a list of disturbance vectors (DV)
and a directory containing files listing unavoidable bit relations for each DV.
It will parse the unavoidable bit relations and generate ubc_check.h and ubc_check.c
for the following data and function(s):
	typedef struct { int dvType; int dvK; int dvB; int testt; int maski; int maskb; uint32_t dm[80]; } dv_info_t;
	extern dv_info_t sha1_dvs[];
	void ubc_check(const uint32_t W[80], uint32_t dvmask[]);
Also, it will output ubc_check_simd.cinc for the following function:
	void UBC_CHECK_SIMD(const SIMD_WORD W[80], SIMD_WORD dvmask[])
	where the following preprocessor defines are expected:
		UBC_CHECK_SIMD		(function name, e.g., ubc_check_sse128, ubc_check_avx256)
	and these defines should come from a generic SIMD header file (simd_avx256.h, simd_sse128.h):
		SIMD_VERSION		(e.g., mmx, sse, avx, avx512, neon)
		SIMD_WORD			(e.g. __m128i, __m256i)
		SIMD_VECSIZE		(number of 32-bit words in each vector, e.g. 4 for __m128i, 8 for __m256i)
		SIMD_ZERO			(returns all-0 SIMDWORD)
		SIMD_WTOV(w)		(return SIMDWORD consisting of SIMD_VECSIZE times value w)
		SIMD_ADD_VV(v,v)	(ADD instruction for two SIMDWORDs)
		SIMD_ADD_VW(v,w)	(ADD instruction for SIMDWORD and 1 uint32 word)
		SIMD_SUB_VV(v,v)	(SUB instruction for two SIMDWORDs)
		SIMD_SUB_VW(v,w)	(SUB instruction for SIMDWORD and 1 uint32 word)
		SIMD_AND_VV(v,v)	(AND instruction for two SIMDWORDs)
		SIMD_AND_VW(v,w)	(AND instruction for SIMDWORD and 1 uint32 word)
		SIMD_OR_VV(v,v)		(OR  instruction for two SIMDWORDs)
		SIMD_OR_VW(v,w)		(OR  instruction for SIMDWORD and 1 uint32 word)
		SIMD_XOR_VV(v,v)	(XOR instruction for two SIMDWORDs)
		SIMD_XOR_VW(v,w)	(XOR instruction for SIMDWORD and 1 uint32 word)
		SIMD_NEG_V(v)		(NEGate instruction for SIMDWORD)
		SIMD_NOT_V(v)		(NOT instruction for SIMDWORD)
		SIMD_ROL_V(v,i)		(rotate left instruction for SIMDWORD)
		SIMD_ROR_V(v,i)		(rotate right instruction for SIMDWORD)
		SIMD_SHL_V(v,i)		(shift left instruction for SIMDWORD)
		SIMD_SHR_V(v,i)		(shift right instruction for SIMDWORD)

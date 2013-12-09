/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 *
 * Modified for Wolframe by the Wolframe Team
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <string.h>

#ifndef _WIN32

/* all modern Unix, they usually include 'endian.h', but we should
 * not include endian.h directly..
 */
#include <sys/param.h>

/* older Solaris */
#ifdef SUNOS
#if OS_MAJOR_VERSION == 5 && OS_MINOR_VERSION <= 10
#include <sys/isa_defs.h>
#define __LITLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#ifdef _LITTLE_ENDIAN
#define __BYTE_ORDER __LITLE_ENDIAN
#endif
#ifdef _BIG_ENDIAN
#define __BYTE_ORDER __BIG_ENDIAN
#endif
#endif /* OS_MAJOR_VERSION == 5 && OS_MINOR_VERSION <= 10 */
#endif /* SUNOS */

#else

#endif /* _WIN32 */

#include "crypto/sha1.h"

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define blk0(i) (block.l[i] = (rol(block.l[i],24)&0xFF00FF00) \
	|(rol(block.l[i],8)&0x00FF00FF))
#else
#if __BYTE_ORDER == __BIG_ENDIAN
#define blk0(i) block.l[i]
#else
#error unknown byte order __BYTE_ORDER
#endif
#endif

#define blk(i) (block.l[i&15] = rol(block.l[(i+13)&15]^block.l[(i+8)&15] \
	^block.l[(i+2)&15]^block.l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z += ((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z += ((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z += (w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z += (((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z += (w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

/* Hash a single 512-bit block. This is the core of the algorithm. */
static void SHA1Transform( uint32_t state[5], const unsigned char *buffer )
{
	typedef union	{
		unsigned char	c[ SHA1_BLOCK_SIZE ];
		uint32_t	l[ SHA1_BLOCK_SIZE / 4 ];
	} CHAR64LONG16;

	uint32_t	a, b, c, d, e;
	CHAR64LONG16	block;

	memcpy( block.c, buffer, SHA1_BLOCK_SIZE );

	/* Copy context->state[] to working vars */
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];

	/* 4 rounds of 20 operations each. Loop unrolled. */
	R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
	R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
	R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
	R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
	R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
	R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
	R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
	R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
	R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
	R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
	R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
	R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
	R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
	R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
	R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
	R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
	R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
	R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
	R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
	R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);

	/* Add the working vars back into context.state[] */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;
	/* Wipe variables */
	a = b = c = d = e = 0;
}


/* Initialize a new context */
void sha1_init(sha1_ctx *context)
{
	/* SHA1 initialization constants */
	context->count = 0;
	context->state[ 0 ] = 0x67452301;
	context->state[ 1 ] = 0xEFCDAB89;
	context->state[ 2 ] = 0x98BADCFE;
	context->state[ 3 ] = 0x10325476;
	context->state[ 4 ] = 0xC3D2E1F0;
}

/* Run your data through this. */
void sha1_update( sha1_ctx *context, const void *data, size_t size )
{
	unsigned int i;
	unsigned int j;
	const unsigned char *dat;

	dat = (const unsigned char*)data;

	j = (uint32_t)(( context->count >> 3 ) & 63 );
	context->count += ( size << 3 );
	if (( j + size ) > 63 )	{
		memcpy( &context->buffer[j], dat, ( i = 64 - j ) );
		SHA1Transform( context->state, context->buffer );
		for ( ; i + 63 < size; i += 64 )	{
			SHA1Transform( context->state, &dat[i] );
		}
		j = 0;
	}
	else i = 0;
	memcpy( &context->buffer[j], &dat[i], size - i );
}


/* Add padding and return the message digest. */
void sha1_final( sha1_ctx *context, unsigned char *digest )
{
	unsigned int	i;
	unsigned char	finalcount[ 8 ];

	for ( i = 0; i < 8; i++ )	{
		finalcount[ i ] = (unsigned char)(( context->count >>
						    (( 7 - ( i & 7 )) * 8 )) & 0xff );  /* Endian independent */
	}
	sha1_update( context, (const unsigned char *)"\200", 1 );
	while (( context->count & 504 ) != 448 )	{
		sha1_update( context, (const unsigned char *)"\0", 1 );
	}
	sha1_update( context, finalcount, 8 );	/* Should cause a SHA1Transform() */

	if ( digest )
		for ( i = 0; i < SHA1_DIGEST_SIZE; i++ )	{
			digest[ i ] = (unsigned char)(( context->state[ i >> 2 ] >>
						      (( 3 - ( i & 3 )) * 8 )) & 0xff );
		}

	/* Wipe variables */
	i = 0;
	memset( context->buffer, 0, sizeof( context->buffer ) );
	memset( context->state, 0, sizeof( context->state ) );
	context->count = 0;
	memset( finalcount, 0, sizeof( finalcount ) );
	/* make SHA1Transform overwrite its own static vars */
	SHA1Transform( context->state, context->buffer );
}


void sha1( const void *data, size_t size, unsigned char *digest )
{
	sha1_ctx ctx;

	sha1_init( &ctx );
	sha1_update( &ctx, data, size );
	sha1_final( &ctx, digest );
}
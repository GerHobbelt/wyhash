// Author: Wang Yi <godspeed_china@yeah.net>

#include <stdint.h>
#include <string.h>

static inline unsigned _wyr32(const uint8_t *p) { return (uint32_t)(255&p[3])<<0|(uint32_t)(255&p[2])<<010|(uint32_t)(255&p[1])<<020|(uint32_t)(255&p[0])<<030;}
static inline unsigned _wyr24(const uint8_t *p, unsigned k) { return (((unsigned)p[0])<<16)|(((unsigned)p[k>>1])<<8)|p[k-1];}
static inline void _wymix32(unsigned  *A,  unsigned  *B){
  uint64_t  c=*A^0x53c5ca59u;  c*=*B^0x74743c1bu;
  *A=(unsigned)c;
  *B=(unsigned)(c>>32);
}
// This version is vulnerable when used with a few bad seeds, which should be
// skipped or changed beforehand: 0x51a43a0f, 0x522235ae, 0x99ac2b20
#ifdef __cplusplus
static void wyhash32_seed_init(uint32_t &seed) {
  if ((seed == 0x51a43a0f) || (seed == 0x522235ae) || (seed == 0x99ac2b20))
    seed++;
}
#else
static bool wyhash32_badseed(const uint32_t seed) {
  if ((seed == 0x51a43a0f) || (seed == 0x522235ae) || (seed == 0x99ac2b20))
    return true;
  return false;
}
#endif

static inline unsigned wyhash32(const void *key, uint64_t len, unsigned seed) {
  const uint8_t *p=(const uint8_t *)key; uint64_t i=len;
  unsigned see1=(unsigned)len; seed^=(unsigned)(len>>32); _wymix32(&seed, &see1);
  for(;i>8;i-=8,p+=8){  seed^=_wyr32(p); see1^=_wyr32(p+4); _wymix32(&seed, &see1); }
  if(i>=4){ seed^=_wyr32(p); see1^=_wyr32(p+i-4); } else if (i) seed^=_wyr24(p,(unsigned)i);
  _wymix32(&seed, &see1); _wymix32(&seed, &see1); return seed^see1;
}
// duplicate definition in wyhash.h also
#ifndef wyhash_final_version_3
static inline uint64_t wyrand(uint64_t *seed){  
  *seed+=0xa0761d6478bd642full; 
  uint64_t  see1=*seed^0xe7037ed1a0b428dbull;
  see1*=(see1>>32)|(see1<<32);
  return  (*seed*((*seed>>32)|(*seed<<32)))^((see1>>32)|(see1<<32));
}
#endif
static inline unsigned wy32x32(unsigned a,  unsigned  b) { _wymix32(&a,&b); _wymix32(&a,&b); return a^b;  }
static inline float wy2u01(unsigned r) { const float _wynorm=1.0f/(1ull<<23); return (r>>9)*_wynorm;}
static inline float wy2gau(unsigned r) { const float _wynorm=1.0f/(1ull<<9); return ((r&0x3ff)+((r>>10)&0x3ff)+((r>>20)&0x3ff))*_wynorm-3.0f;}

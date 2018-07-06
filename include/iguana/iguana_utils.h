/******************************************************************************
 * Copyright © 2014-2018 The SuperNET Developers.                             *
 *                                                                            *
 * See the AUTHORS, DEVELOPER-AGREEMENT and LICENSE files at                  *
 * the top-level directory of this distribution for the individual copyright  *
 * holder information and the developer policies on copyright and licensing.  *
 *                                                                            *
 * Unless otherwise agreed in a custom licensing agreement, no part of the    *
 * SuperNET software, including this file may be copied, modified, propagated *
 * or distributed except according to the terms contained in the LICENSE file *
 *                                                                            *
 * Removal or modification of this copyright notice is prohibited.            *
 *                                                                            *
 ******************************************************************************/

#ifndef iguana_utils_h
#define iguana_utils_h

#include "compat/OS_portable.h"
#include "datachain/datachain.h"

#include "iguana/iguana_defines.h"
#include "iguana/iguana_types.h"
#include "iguana/iguana_structs.h"

bits256 bits256_doublesha256(char *hashstr,uint8_t *data,int32_t datalen);
char *bits256_str(char hexstr[65],bits256 x);
bits256 bits256_conv(char *hexstr);
char *bits256_lstr(char hexstr[65],bits256 x);
bits256 bits256_add(bits256 a,bits256 b);
int32_t bits256_cmp(bits256 a,bits256 b);
bits256 bits256_rshift(bits256 x);
bits256 bits256_lshift(bits256 x);
bits256 bits256_ave(bits256 a,bits256 b);
bits256 bits256_from_compact(uint32_t c);
uint32_t bits256_to_compact(bits256 x);
int32_t bitweight(uint64_t x);
void calc_OP_HASH160(char hexstr[41],uint8_t rmd160[20],char *pubkey);
double _xblend(float *destp,double val,double decay);
double _dxblend(double *destp,double val,double decay);
double dxblend(double *destp,double val,double decay);
int32_t iguana_numthreads(struct iguana_info *coin,int32_t mask);
void iguana_launcher(void *ptr);
void iguana_terminate(struct iguana_thread *t);
struct iguana_thread *iguana_launch(struct iguana_info *coin,char *name,iguana_func funcp,void *arg,uint8_t type);
char hexbyte(int32_t c);
int32_t _unhex(char c);
int32_t is_hexstr(char *str,int32_t n);
int32_t unhex(char c);
unsigned char _decode_hex(char *hex);
int32_t decode_hex(unsigned char *bytes,int32_t n,char *hex);
int32_t init_hexbytes_noT(char *hexbytes,unsigned char *message,long len);
long _stripwhite(char *buf,int accept);
char *clonestr(char *str);
int32_t safecopy(char *dest,char *src,long len);
void escape_code(char *escaped,char *str);
int32_t is_zeroes(char *str);
int64_t conv_floatstr(char *numstr);
int32_t has_backslash(char *str);
int _increasing_uint64(const void *a,const void *b);
int _decreasing_uint64(const void *a,const void *b);
int _decreasing_uint32(const void *a,const void *b);
int32_t sortds(double *buf,uint32_t num,int32_t size);
int32_t revsortds(double *buf,uint32_t num,int32_t size);
int32_t sort64s(uint64_t *buf,uint32_t num,int32_t size);
int32_t revsort64s(uint64_t *buf,uint32_t num,int32_t size);
int32_t revsort32(uint32_t *buf,uint32_t num,int32_t size);
void touppercase(char *str);
void tolowercase(char *str);
char *uppercase_str(char *buf,char *str);
char *lowercase_str(char *buf,char *str);
int32_t strsearch(char *strs[],int32_t num,char *name);
int32_t is_decimalstr(char *str);
int32_t unstringbits(char *buf,uint64_t bits);
uint64_t stringbits(char *str);
char *unstringify(char *str);
void reverse_hexstr(char *str);
int32_t nn_base64_decode (const char *in, size_t in_len,uint8_t *out, size_t out_len);
int32_t nn_base64_encode (const uint8_t *in, size_t in_len,char *out, size_t out_len);
int32_t gmult(int32_t a,int32_t b);
int32_t letterval(char letter);
uint64_t RS_decode(char *rs);
int32_t RS_encode(char *rsaddr,uint64_t id);
uint64_t conv_acctstr(char *acctstr);
int32_t base32byte(int32_t val);
int32_t unbase32(char c);
int init_base32(char *tokenstr,uint8_t *token,int32_t len);
int decode_base32(uint8_t *token,uint8_t *tokenstr,int32_t len);
void calc_hexstr(char *hexstr,uint8_t *buf,uint8_t *msg,int32_t len);
void calc_unhexstr(char *hexstr,uint8_t *buf,uint8_t *msg,int32_t len);
void calc_base64_encodestr(char *hexstr,uint8_t *buf,uint8_t *msg,int32_t len);
void calc_base64_decodestr(char *hexstr,uint8_t *buf,uint8_t *msg,int32_t len);
void sha256_sha256(char *hexstr,uint8_t *buf,uint8_t *msg,int32_t len);
void rmd160ofsha256(char *hexstr,uint8_t *buf,uint8_t *msg,int32_t len);
void calc_crc32str(char *hexstr,uint8_t *buf,uint8_t *msg,int32_t len);
void calc_NXTaddr(char *hexstr,uint8_t *buf,uint8_t *msg,int32_t len);
void calc_curve25519_str(char *hexstr,uint8_t *buf,uint8_t *msg,int32_t len);
void calc_rmd160_sha256(uint8_t rmd160[20],uint8_t *data,int32_t datalen);
char *cmc_ticker(char *base);
char *bittrex_orderbook(char *base,char *rel,int32_t maxdepth);
double calc_theoretical(double weighted,double CMC_average,double changes[3]);
double calc_weighted(double *avebidp,double *aveaskp,double *bids,double *bidvols,int32_t numbids,double *asks,double *askvols,int32_t numasks,double limit);
double weighted_orderbook(double *avebidp,double *aveaskp,double *highbidp,double *lowaskp,char *orderbookstr,double limit);
double get_theoretical(double *avebidp,double *aveaskp,double *highbidp,double *lowaskp,double *CMC_averagep,double changes[3],char *name,char *base,char *rel,double *USD_averagep);
bits256 bits256_calctxid(char *symbol,uint8_t *serialized,int32_t len);
bits256 iguana_merkle(char *symbol,bits256 *tree,int32_t txn_count);

#endif

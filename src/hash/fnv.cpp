/*
 * hash_64 - 64 bit Fowler/Noll/Vo-0 FNV-1a hash code
 *
 * @(#) $Revision: 5.1 $
 * @(#) $Id: hash_64a.c,v 5.1 2009/06/30 09:01:38 chongo Exp $
 * @(#) $Source: /usr/local/src/cmd/fnv/RCS/hash_64a.c,v $
 *
 ***
 *
 * Fowler/Noll/Vo hash
 *
 * The basis of this hash algorithm was taken from an idea sent
 * as reviewer comments to the IEEE POSIX P1003.2 committee by:
 *
 *      Phong Vo (http://www.research.att.com/info/kpv/)
 *      Glenn Fowler (http://www.research.att.com/~gsf/)
 *
 * In a subsequent ballot round:
 *
 *      Landon Curt Noll (http://www.isthe.com/chongo/)
 *
 * improved on their algorithm.  Some people tried this hash
 * and found that it worked rather well.  In an EMail message
 * to Landon, they named it the ``Fowler/Noll/Vo'' or FNV hash.
 *
 * FNV hashes are designed to be fast while maintaining a low
 * collision rate. The FNV speed allows one to quickly hash lots
 * of data while maintaining a reasonable collision rate.  See:
 *
 *      http://www.isthe.com/chongo/tech/comp/fnv/index.html
 *
 * for more details as well as other forms of the FNV hash.
 *
 ***
 *
 * Please do not copyright this code.  This code is in the public domain.
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * By:
 * chongo <Landon Curt Noll> /\oo/\
 *      http://www.isthe.com/chongo/
 *
 * Share and Enjoy!	:-)
 */

#include "fnv.h"

/*
* # Algortihms
* ## FNV-1
* hash = offset_basis
* for each octet_of_data to be hashed
*         hash = hash * FNV_prime
*         hash = hash xor octet_of_data
* return hash

* ## FNV-1a
*   hash = offset_basis
*   for each octet_of_data to be hashed
*           hash = hash xor octet_of_data
*           hash = hash * FNV_prime
*   return hash
*
*
* # Parameters of the FNV-1/FNV-1a hash
* The FNV-1 hash parameters are as follows:
* hash is an n bit unsigned integer, where n is the bit length of hash.
*
* The multiplication is performed modulo 2n where n is the bit length of hash.
*
* The xor is performed on the low order octet (8 bits) of hash.
* 
* The FNV_prime is dependent on n, the size of the hash:
* 
*   32 bit FNV_prime = 224 + 28 + 0x93 = 16777619
* 
*   64 bit FNV_prime = 240 + 28 + 0xb3 = 1099511628211
* 
*   128 bit FNV_prime = 288 + 28 + 0x3b = 309485009821345068724781371
* 
*   256 bit FNV_prime = 2168 + 28 + 0x63 = 374144419156711147060143317175368453031918731002211
* 
*   512 bit FNV_prime = 2344 + 28 + 0x57 =
*   35835915874844867368919076489095108449946327955754392558399825615420669938882575
*   126094039892345713852759
* 
*   1024 bit FNV_prime = 2680 + 28 + 0x8d =
*   50164565101131186554345988110352789550307653454047907443030175238311120551081474
*   51509157692220295382716162651878526895249385292291816524375083746691371804094271
*   873160484737966720260389217684476157468082573 
* 
* Part of the magic of FNV is the selection of the FNV_prime for a given sized unsigned integer. Some primes do hash better than other primes for a given integer size.
* 
* The offset_basis for FNV-1 is dependent on n, the size of the hash:
* 
*   32 bit offset_basis = 2166136261
* 
*   64 bit offset_basis = 14695981039346656037
* 
*   128 bit offset_basis = 144066263297769815596495629667062367629
* 
*   256 bit offset_basis =
*   100029257958052580907070968620625704837092796014241193945225284501741471925557
* 
*   512 bit offset_basis =
*   96593031294966694980094354007163104660904187456726378961083743294344626579945829
*   32197716438449813051892206539805784495328239340083876191928701583869517785
* 
*   1024 bit offset_basis =
*   14197795064947621068722070641403218320880622795441933960878474914617582723252296
*   73230371772215086409652120235554936562817466910857181476047101507614802975596980
*   40773201576924585630032153049571501574036444603635505054127112859663616102678680
*   82893823963790439336411086884584107735010676915
*
* source: http://isthe.com/chongo/tech/comp/fnv/
*/

namespace fnv_1a {
/*
* fnv_64a_buf - perform a 64 bit fowler/noll/vo fnv-1a hash on a buffer
*
* input:
* buf - start of buffer to hash
* len - length of buffer in octets
* hash- previous hash value or 0 if first call
*
* returns:
* 64 bit hash as a static hash type
*
* note: to use the recommended 64 bit fnv-1a hash, use fnv1a_64_init as the has
* arg on the first call to either fnv_64a_buf() or fnv_64a_str().
*
*/

template<typename T>
static T
encode(const void *buf, std::size_t length, T prime, T hash) noexcept
{
  const unsigned char *it = (const unsigned char*)buf;
  const unsigned char *end = it + length;
  /*
   * fnv-1a hash each octet of the buffer
   */
  for (;it != end;++it) {

    /* xor the bottom with the current octet */
    hash ^= T(*it);

    /* multiply by the 64 bit fnv magic prime mod 2^64 */
    hash *= prime;
  }

  return hash;
}

std::uint64_t
encode(const void *buf, std::size_t length, std::uint64_t hash) noexcept
{
  constexpr std::uint64_t prime{0x100000001b3ULL};
  return encode(buf,length,prime,hash);
}

std::uint64_t
encode64(const void *buf, std::size_t length) noexcept {
  constexpr std::uint64_t offset_basis{0xcbf29ce484222325ULL};
  return encode(buf,length,offset_basis);
}


std::uint32_t
encode(const void *buf, std::size_t length, std::uint32_t hash) noexcept {
constexpr std::uint32_t prime{0x01000193};
  return encode(buf,length,prime,hash);
}

std::uint32_t
encode32(const void *buf, std::size_t length) noexcept {
  constexpr std::uint32_t offset_basis{0x811c9dc5};
  return encode(buf,length,offset_basis);
}

}

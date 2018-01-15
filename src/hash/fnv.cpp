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
*/
std::uint64_t
encode(const void *buf, std::size_t len, std::uint64_t hash) noexcept
{
  const unsigned char *it = (const unsigned char*)buf;
  const unsigned char *end = it + len;

#define FNV_64_PRIME ((std::uint64_t)0x100000001b3ULL)
  /*
   * fnv-1a hash each octet of the buffer
   */
  for (;it != end;++it) {

    /* xor the bottom with the current octet */
    hash ^= std::uint64_t(*it);

    /* multiply by the 64 bit fnv magic prime mod 2^64 */
    // hash += (hash << 1) + (hash << 4) + (hash << 5) +
    //   (hash << 7) + (hash << 8) + (hash << 40);

    hash *= FNV_64_PRIME;
  }

  return hash;
}

std::uint64_t
encode(const void *buf, std::size_t len) noexcept {
  return encode(buf,len,0);
}

}

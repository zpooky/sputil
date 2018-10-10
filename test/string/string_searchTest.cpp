#include <collection/Array.h>
#include <gtest/gtest.h>
#include <prng/util.h>
#include <prng/xorshift.h>
#include <string/boyer_moore_search.h>
#include <string/naive_search.h>
#include <util/array.h>

typedef const char *(*test_search_fp)(const char *text, std::size_t,
                                      const char *needle, std::size_t);

TEST(string_search, test) {
  const char *const text = "spooky";
  {
    const char *it = sp::bm::search(text, "oo");
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text + 2);
  }

  {
    const char *it = sp::bm::search(text, "s");
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text);
  }
  {
    const char *it = sp::bm::search(text, text);
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text);
  }
  {
    const char *it = sp::bm::search(text, "ok");
    ASSERT_TRUE(it);
    ASSERT_EQ(it, text + 3);
  }
}

static void
test_2(test_search_fp search) {
  // char it = '!';
  // const char end = '~' + char(1);

  char it = 'A';
  const char end = 'Z' + char(1);
  const std::size_t cap = end - it;

  sp::DynamicArray<char> text(cap);
  for (; it != end; ++it) {
    ASSERT_TRUE(push(text, it));
  }
  ASSERT_TRUE(is_full(text));

  const char *const b = text.data();
  printf("|%.*s|:%zu\n", length(text), b, length(text));

  for (std::size_t i = 2; i < length(text); ++i) {

    std::size_t k = i;
    while (true) {
      const char *const needle = b + k++;
      const std::size_t nlen = sp::distance(needle, text.end());

      // printf("nlen: %zu\n", nlen);
      if (nlen == 0) {
        break;
      }

      // {
      //   bool dasd = memcmp(b + 1, needle, 3) == 0;
      //   printf("[%.*s]:%zu|%s\n", len, needle, len, dasd ? "true" : "false");
      // }

      const char *it = search(b, length(text), needle, nlen);
      if (!it) {
        printf("i:%zu, len:%zu\n", i, nlen);
        printf("nedl[%.*s]:%zu\n", nlen, needle, nlen);
        printf("text[%.*s]:%zu\n", length(text), b, length(text));
      }
      ASSERT_TRUE(it);

      ASSERT_EQ(it, needle);
    }
  }
}

TEST(string_search, test_booyer_moore_search2) {
  test_2(sp::bm::search);
}

TEST(string_search, test_naive2) {
  test_2(sp::naive::search);
}

static char
dist_xx(prng::xorshift32 &ra) {
Lit:
  auto res = prng::uniform_dist(ra, '!', '~' + char(1));
  if (res == '"' || res == '\\') {
    goto Lit;
  }
  return res;
}

static void
test_large(test_search_fp search) {
  prng::xorshift32 r(1);
  const std::size_t length = 25 * 1024 * 1;
  // const std::size_t length = 32;

  printf("length[%zu]\n", length);
  char *const text = new char[length + 1];
  text[length] = '\0';
  fill(r, text, length, dist_xx);
  const char *const end = text + length;

  std::size_t cnt = 0;
  for (std::size_t i = 0; i < length; ++i) {
    const std::size_t strt_idx = uniform_dist(r, 0, length - 1);

    const std::size_t n_cap = length - strt_idx;
    constexpr std::size_t wasd = 5;
    if (n_cap > wasd) {
      ++cnt;

      if (cnt % 10000 == 0) {
        printf("cnt[%zu]\n", cnt);
      }

      const auto n_len =
          uniform_dist(r, wasd, std::min(std::size_t(20), n_cap));

      const char *const needle = text + strt_idx;
      ASSERT_TRUE(needle < end);
      ASSERT_TRUE(needle + n_len <= end);
      // printf("needle[%.*s]: %zu\n", n_len, needle, n_len);

      const char *it = search(text, length, needle, n_len);
      ASSERT_TRUE(it);
      ASSERT_EQ(0, std::memcmp(it, needle, n_len));
      ASSERT_EQ(it, needle);
    }
    // printf("%s\n", it);
  }

  // printf("%s\n", text);

  delete[] text;
}
TEST(string_search, test_naive_large) {
  test_large(sp::naive::search);
}

TEST(string_search, test_boyer_moore_large) {
  test_large(sp::bm::search);
}

static void
test_subset(test_search_fp search) {
  prng::xorshift32 r(1);
  const std::size_t length = 40 * 1024 * 1;

  char *const text = new char[length + 1];
  text[length] = '\0';
  fill(r, text, length, dist_xx);

  const char *needle = text;
  const char *const end = text + length;

  std::size_t l_cmp = length;
  for (; needle != end && sp::distance(needle, end) > 5; ++needle) {
    const std::size_t n_len = sp::distance(needle, end);
    ASSERT_EQ(l_cmp, n_len);
    // printf("length[%zu]\n", l_cmp);
    // printf("[%zu]\n", n_len);
    const char *s = search(text, length, needle, n_len);
    ASSERT_TRUE(s);
    ASSERT_EQ(s, needle);
    --l_cmp;
  }

  delete[] text;
}

TEST(string_search, test_boyer_moore_subset) {
  test_subset(sp::bm::search);
}

TEST(string_search, test_naive_subset) {
  test_subset(sp::naive::search);
}

TEST(string_search, tsts_1) {
  const char *const text = ":|$2V7%CR'B(km^N5CB&|^Z@5frI@!nE";
  const char *const needle = "^N5CB&|^Z@5";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
}

TEST(string_search, tsts_2) {
  const char *const text = "This is a test of the Boyer Moore algorithm.";
  const char *const needle = "algorithm";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  ASSERT_EQ(0, std::memcmp(needle, it, std::strlen(needle)));
}

TEST(string_search, tsts_3) {
  /*
   * last[b] = -1, shfit = 1
   * last[a] =  0, shift = 8
   */
  const char *const text = "blgorithmalgorithm";
  const char *const needle = "algorithm";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  ASSERT_EQ(0, std::memcmp(needle, it, std::strlen(needle)));
}

TEST(string_search, tsts_4) {
  const char *const text = "algorithm";
  const char *const needle = "algorithm";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  ASSERT_EQ(it, text);
}

TEST(string_search, tsts_5) {
  const char *const text = "xalgorithm";
  const char *const needle = "algorithm";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  printf("res: %s\n", it);
  ASSERT_EQ(it, text + 1);
}

TEST(string_search, tsts_6) {
  const char *const text = "algorithmx";
  const char *const needle = "algorithm";
  const char *const it = sp::bm::search(text, needle);
  ASSERT_TRUE(it);
  ASSERT_EQ(it, text);
}

TEST(string_search, tsts_min) {
#if 0
  const char *const text =
      "7,v=(m39jaa0g+S*-/]:[oBU=b5a/"
      "R&}=+2tO{-7Q@9U#+@r%8Z1@){Y`1G)_6Aupu}7(^tI]!WiD+k(E|YNk5A64D@T8.."
      "HDmpiWYN6KcKL,e@:d^60j$(.Z7YckKaXc|SrGb>:yi#q._+VmyL*$=OsHPL$K<j@hM/"
      "D-)GtWP8bc|Fo;)!(/"
      "'h2>(zJAMEuiqanRiNXA|rJA7D<S63vjxGJ-v;cfoL_{qS;Gkwo.Hm{dIN3)qGvXC?'#"
      "Yskvpd[9hxbjliwC'G5gBt]Q3iKL0Ts`xub0Y(KUf#(f.mXj@6hmATR;:+j,'(!NF:NDFqK/"
      "&]=SZ/"
      ",:NDH}NTQg*pX@gwv|Schj2a(j]9Lz=kISO?ICvD}}-,eEsUS=|.2w,NN-r-=6bN7[dJ|N.{"
      "yQj9ptqab}#JLZtqY8OTtl}_C9Ex,P'PrZ&=%b[C/,4x>!%K<yhp_gNx4`#hLQj/"
      "2Ekb]xKY4>i~O[$?'EKho:+mog7pUF'W$%Ate/"
      "MO3njrq&eexE>Hl*iJ6*;N72!PL2;ny0570uf4~p)AE{j4N<$-C.YD/"
      "L}6>lU7V#bB6-+}Z:p6?jZ{HAv0S;%;)Dx;dP&Tmnl',QDoQ1f_,^&kZN|_r@T(RUy3^"
      "0zyHJB=Cv[mgEi'LTa,[:c]e0Txe+993;i''qz:MDbVflaSjR9_Ul==FwN:FtSJ;e_z;l9!"
      "rWtg;AGiqp7frF>qO^XuU0M?[<Ct`iweqGc>E<]0X96y%pXYXfrtsWT:F[a*H@UWGK3-"
      "V1LB_Sh<F~O|LiefnPY)Txw.KGJ3l@j;X.oR-;6<Yf=}F{,:93hEJ3<Wmtf0Z*HBFm/"
      "atz{fM3=I7;TAI'U7$q@=B;`Xb^X9KxaEZ5yeG;xj$tB5au*VsJuN47E)@=/"
      "Y1Q96r8=onJ_mxdoC(iXt'*xV/"
      "`(jQ]uC%Xg@{'s8MzeCWTTJTX?qJ<]AE]hX#WyX?IMy<z4&-jHc3v]F&v=W:HcpjHECQ>WE{"
      "y(cfD*aelVkOy=]0S!~Gs9#6<qANV*)=7]69m=jK,v&vYu0nNQ0*l,P~(U2PA{ak*Xgv%"
      "zGe@ey{r}y,ro@UlGpb<us(z~m:EkPV9rwx+p7[nVta0S#l5U&,#{q87FY@dE<BHF}}s~zG^"
      "n^lf<R]n|ojAiL0IM~y-+1Ck$*.dRsGIprBfTZo>4gbdmi=?M6OOGG^8B[Pa=N*zc)YZ`?"
      "DM-8|aEBe2Jde),Mn~',ZiMYtd*|g{XWwuD*l?0VI$z(0voIT07-bFxhbnr+$!ho{iJV?~"
      "e0;MYj_OAXTM3ZcG&H+[7yS#<lW^M6LL9rejuDP6[j}rl9Xv,.@?Qj':[3>)TPzad0>Vh:'+"
      "0$(@IDOpn+#{zL|es27`InPJ,,v`-*b%.F*`4i@aV'}-7!T&2;8T%6BDMW[[Ubkf2&n!"
      "Kgv05elIA}v#B.x&w$@PXf[Q14s;*S2+wGn3y!Ss";

  const char *const needle = "@gwv|Schj2a";
#endif

  const char *const text = "abceefqwerty0123easd";
  const char *const nedl = "0123e";

  const char *const it = sp::bm::search(text, nedl);
  ASSERT_TRUE(it);
}

#if 0
TEST(string_search, tsts_min_min) {
  prng::xorshift32 r(1);
  const std::size_t length = 25 * 1024 * 1;
  // const std::size_t length = 32;

  printf("length[%zu]\n", length);

  char *const text = new char[length + 1];
  const char *const text_end = text + length;

  std::size_t best_min = length;

  while (true) {
    const auto prng_state = r.state;
    text[length] = '\0';
    fill(r, text, length, dist_xx);

    const char *text_it = text;

    constexpr std::size_t needle_len = 11;
    char needle[needle_len + 1] = {'\0'};
    {
      std::size_t needle_offset = uniform_dist(r, 0, length - needle_len);
      const char *const needle_begin = (text + needle_offset);
      const char *const needle_end = needle_begin + needle_len;
      ASSERT_EQ(needle_len, sp::distance(needle_begin, needle_end));
      ASSERT_TRUE(needle_end <= text_end);
      memcpy(needle, needle_begin, needle_len);
    }

    const char *const cmp_it = sp::naive::search(text, needle);
    ASSERT_TRUE(cmp_it);

    const char *min_it = nullptr;
    for (; text_it < cmp_it; ++text_it) {
      const char *const it = sp::bm::search(text_it, needle);

      ASSERT_TRUE(sp::naive::search(text_it, needle));
      if (!it) {
        min_it = text_it;
      }

      //   ASSERT_TRUE(it);
      // ASSERT_EQ(it, text);
    } // for

    if (min_it) {
      auto min_len = std::strlen(min_it);
      if (min_len < best_min) {
        printf("---------------------------prng[%u]--\n", prng_state);
        best_min = min_len;
        printf("txt[%s]:%zu\n", min_it, min_len);
        printf("ptn[%s]:%zu\n", needle, std::strlen(needle));
      }
    }

  } // while
}
#endif

TEST(string_search, tsts_success_min_min) {
  prng::xorshift32 r(1);
  const std::size_t length = 25 * 1024 * 1;
  // const std::size_t length = 32;

  printf("length[%zu]\n", length);
  std::size_t cmpers = 0;

  char *const text = new char[length + 1];
  const char *const text_end = text + length;

  while (true) {
    const auto prng_state = r.state;
    text[length] = '\0';
    fill(r, text, length, dist_xx);

    const char *text_it = text;

    constexpr std::size_t needle_len = 11;
    char needle[needle_len + 1] = {'\0'};
    {
      std::size_t needle_offset = uniform_dist(r, 0, length - needle_len);
      const char *const needle_begin = (text + needle_offset);
      const char *const needle_end = needle_begin + needle_len;
      ASSERT_EQ(needle_len, sp::distance(needle_begin, needle_end));
      ASSERT_TRUE(needle_end <= text_end);
      memcpy(needle, needle_begin, needle_len);
    }

    const char *const cmp_it = sp::naive::search(text, needle);
    ASSERT_TRUE(cmp_it);

    const char *min_it = nullptr;
    for (; text_it < cmp_it; ++text_it) {
      ++cmpers;
      if ((cmpers % 1000000) == 0) {
        printf("cmpers[%zu], seed[%u]\n", cmpers, prng_state);
      }
      const char *const it = sp::bm::search(text_it, needle);

      if (!it) {
        printf("seed[%u]\n", prng_state);
      }
      ASSERT_TRUE(it);
      ASSERT_EQ(0, std::memcmp(it, needle, needle_len));
    } // for

  } // while
}

/**
Copyright 2018 Chia Network Inc

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**/

#include "xgcd_partial.c"

#define LOG2(X) (63 - __builtin_clzll((X)))
//using namespace std;

typedef struct qfb
{
    mpz_t a;
    mpz_t b;
    mpz_t c;
} qfb;

typedef qfb qfb_t[1];

// From Antic using Flint (works!)
void qfb_nucomp(qfb_t r, const qfb_t f, const qfb_t g, mpz_t& D, mpz_t& L)
{
   mpz_t a1, a2, c2, ca, cb, cc, k, s, sp, ss, m, t, u2, v1, v2;

   if (mpz_cmp(f->a, g->a) > 0)
   {
      qfb_nucomp(r, g, f, D, L);
      return;
   }

   mpz_init(a1); mpz_init(a2); mpz_init(c2);
   mpz_init(ca); mpz_init(cb); mpz_init(cc);
   mpz_init(k); mpz_init(m);
   mpz_init(s); mpz_init(sp); mpz_init(ss);
   mpz_init(t); mpz_init(u2); mpz_init(v1); mpz_init(v2);

   /* nucomp calculation */

   mpz_set(a1, f->a);
   mpz_set(a2, g->a);
   mpz_set(c2, g->c);

   mpz_add(ss, f->b, g->b);
   mpz_fdiv_q_2exp(ss, ss, 1);

   mpz_sub(m, f->b, g->b);
   mpz_fdiv_q_2exp(m, m, 1);

   mpz_fdiv_r(t, a2, a1);
   if (!mpz_cmp_ui(t, 0))
   {
      mpz_set_ui(v1, 0);
      mpz_set(sp, a1);
   } else
      mpz_gcdext(sp, v1, NULL, t, a1);

   mpz_mul(k, m, v1);
   mpz_fdiv_r(k, k, a1);
 
   if (mpz_cmp_ui(sp, 1))
   {
      mpz_gcdext(s, v2, u2, ss, sp);
 
      mpz_mul(k, k, u2);
      mpz_mul(t, v2, c2);
      mpz_sub(k, k, t);

      if (mpz_cmp_ui(s, 1))
      {
         mpz_fdiv_q(a1, a1, s);
         mpz_fdiv_q(a2, a2, s);
         mpz_mul(c2, c2, s);
      }

      mpz_fdiv_r(k, k, a1);
   }

   if (mpz_cmp(a1, L) < 0)
   {
      mpz_mul(t, a2, k);

      mpz_mul(ca, a2, a1);

      mpz_mul_2exp(cb, t, 1);
      mpz_add(cb, cb, g->b);

      mpz_add(cc, g->b, t);
      mpz_mul(cc, cc, k);
      mpz_add(cc, cc, c2);

      mpz_fdiv_q(cc, cc, a1);
   } else
   {
      mpz_t m1, m2, r1, r2, co1, co2, temp;

      mpz_init(m1); mpz_init(m2); mpz_init(r1); mpz_init(r2);
      mpz_init(co1); mpz_init(co2); mpz_init(temp);

      mpz_set(r2, a1);
      mpz_set(r1, k);

      mpz_xgcd_partial(co2, co1, r2, r1, L);

      mpz_mul(t, a2, r1);
      mpz_mul(m1, m, co1);
      mpz_add(m1, m1, t);
      mpz_tdiv_q(m1, m1, a1);

      mpz_mul(m2, ss, r1);
      mpz_mul(temp, c2, co1);
      mpz_sub(m2, m2, temp);
      mpz_tdiv_q(m2, m2, a1);

      mpz_mul(ca, r1, m1);
      mpz_mul(temp, co1, m2);
      if (mpz_sgn(co1) < 0)
         mpz_sub(ca, ca, temp);
      else
         mpz_sub(ca, temp, ca);

      mpz_mul(cb, ca, co2);
      mpz_sub(cb, t, cb);
      mpz_mul_2exp(cb, cb, 1);
      mpz_fdiv_q(cb, cb, co1);
      mpz_sub(cb, cb, g->b);
      mpz_mul_2exp(temp, ca, 1);
      mpz_fdiv_r(cb, cb, temp);
 
      mpz_mul(cc, cb, cb);
      mpz_sub(cc, cc, D);
      mpz_fdiv_q(cc, cc, ca);
      mpz_fdiv_q_2exp(cc, cc, 2);

      if (mpz_sgn(ca) < 0)
      {
         mpz_neg(ca, ca);
         mpz_neg(cc, cc);
      }

      mpz_clear(m1); mpz_clear(m2); mpz_clear(r1); mpz_clear(r2);
      mpz_clear(co1); mpz_clear(co2); mpz_clear(temp);
   }

   mpz_set(r->a, ca);
   mpz_set(r->b, cb);
   mpz_set(r->c, cc);

   mpz_clear(ca); mpz_clear(cb); mpz_clear(cc);
   mpz_clear(k); mpz_clear(m);
   mpz_clear(s); mpz_clear(sp); mpz_clear(ss);
   mpz_clear(t); mpz_clear(u2); mpz_clear(v1); mpz_clear(v2);
   mpz_clear(a1); mpz_clear(a2); mpz_clear(c2);
}

// a = b * c
void nucomp_form(form &a, form &b, form &c, integer &D, integer &L) {
    qfb fr, fr2, fr3;
    mpz_init(fr.a);
    mpz_init(fr.b);
    mpz_init(fr.c);
    mpz_init(fr2.a);
    mpz_init(fr2.b);
    mpz_init(fr2.c);
    mpz_init(fr3.a);
    mpz_init(fr3.b);
    mpz_init(fr3.c);
    mpz_set(fr2.a, b.a.impl);
    mpz_set(fr2.b, b.b.impl);
    mpz_set(fr2.c, b.c.impl);
    mpz_set(fr3.a, c.a.impl);
    mpz_set(fr3.b, c.b.impl);
    mpz_set(fr3.c, c.c.impl);
    mpz_t anticD, anticL;
    mpz_init(anticD);
    mpz_init(anticL);
    mpz_set(anticD, D.impl);
    mpz_set(anticL, L.impl);
    qfb_nucomp(&fr,&fr2,&fr3,anticD,anticL);
    mpz_set(a.a.impl,fr.a);
    mpz_set(a.b.impl,fr.b);
    mpz_set(a.c.impl,fr.c);
    mpz_clear(fr.a);
    mpz_clear(fr.b);
    mpz_clear(fr.c);
    mpz_clear(fr2.a);
    mpz_clear(fr2.b);
    mpz_clear(fr2.c);
    mpz_clear(fr3.a);
    mpz_clear(fr3.b);
    mpz_clear(fr3.c);
    mpz_clear(anticD);
    mpz_clear(anticL);
}

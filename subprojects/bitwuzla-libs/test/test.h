#ifndef BZLALS__TEST__TEST_H
#define BZLALS__TEST__TEST_H

#include <cmath>
#include <string>

#include "bitvector_domain.h"
#include "bitvector_node.h"
#include "bzlabitvector.h"
#include "gtest/gtest.h"
#include "rng.h"

using namespace bzlabv;
using namespace bzlals;
using namespace bzlarng;

namespace bzlatest {

/* -------------------------------------------------------------------------- */

class TestCommon : public ::testing::Test
{
 protected:
  enum OpKind
  {
    ADD,
    AND,
    ASHR,
    CONCAT,
    DEC,
    EQ,
    IMPLIES,
    ITE,
    INC,
    MUL,
    NAND,
    NE,
    NEG,
    NOR,
    NOT,
    OR,
    REDAND,
    REDOR,
    SDIV,
    SEXT,
    SGT,
    SGE,
    SHL,
    SHR,
    SLT,
    SLE,
    SREM,
    SUB,
    UDIV,
    UGT,
    UGE,
    ULT,
    ULE,
    UREM,
    XNOR,
    XOR,
    ZEXT,
  };

  static void gen_all_combinations(size_t size,
                                   const std::vector<char>& bits,
                                   std::vector<std::string>& values);
  static void gen_xvalues(uint32_t bw, std::vector<std::string>& values);
  static void gen_values(uint32_t bw, std::vector<std::string>& values);
};

/* -------------------------------------------------------------------------- */

void
TestCommon::gen_all_combinations(size_t size,
                                 const std::vector<char>& bits,
                                 std::vector<std::string>& values)
{
  size_t num_values;
  size_t num_bits = bits.size();
  std::vector<size_t> psizes;

  num_values = pow(num_bits, size);
  for (size_t i = 0; i < size; ++i)
  {
    psizes.push_back(num_values / pow(num_bits, i + 1));
  }

  /* Generate all combinations of 'bits'. */
  for (size_t row = 0; row < num_values; ++row)
  {
    std::string val;
    for (size_t col = 0; col < size; ++col)
    {
      val += bits[(row / psizes[col]) % num_bits];
    }
    values.push_back(val);
  }
}

void
TestCommon::gen_xvalues(uint32_t bw, std::vector<std::string>& values)
{
  gen_all_combinations(bw, {'x', '0', '1'}, values);
}

void
TestCommon::gen_values(uint32_t bw, std::vector<std::string>& values)
{
  gen_all_combinations(bw, {'0', '1'}, values);
}

/* -------------------------------------------------------------------------- */

class TestBvNodeCommon : public TestCommon
{
 protected:
  void SetUp() override
  {
    TestCommon::SetUp();
    gen_values(TEST_BW, d_values);
    gen_xvalues(TEST_BW, d_xvalues);
    d_rng.reset(new RNG(1234));
  }

  BitVector eval_op_binary(OpKind op_kind,
                           const BitVector& x_val,
                           const BitVector& s_val,
                           uint32_t pos_x);

  static constexpr uint32_t TEST_BW = 4;
  std::vector<std::string> d_values;
  std::vector<std::string> d_xvalues;
  std::unique_ptr<RNG> d_rng;
};

BitVector
TestBvNodeCommon::eval_op_binary(OpKind op_kind,
                                 const BitVector& x_val,
                                 const BitVector& s_val,
                                 uint32_t pos_x)
{
  BitVector res;
  switch (op_kind)
  {
    case ADD: res = pos_x ? s_val.bvadd(x_val) : x_val.bvadd(s_val); break;
    case AND: res = pos_x ? s_val.bvand(x_val) : x_val.bvand(s_val); break;
    case ASHR: res = pos_x ? s_val.bvashr(x_val) : x_val.bvashr(s_val); break;
    case CONCAT:
      res = pos_x ? s_val.bvconcat(x_val) : x_val.bvconcat(s_val);
      break;
    case EQ: res = pos_x ? s_val.bveq(x_val) : x_val.bveq(s_val); break;
    case IMPLIES:
      res = pos_x ? s_val.bvimplies(x_val) : x_val.bvimplies(s_val);
      break;
    case MUL: res = pos_x ? s_val.bvmul(x_val) : x_val.bvmul(s_val); break;
    case NAND: res = pos_x ? s_val.bvnand(x_val) : x_val.bvnand(s_val); break;
    case NE: res = pos_x ? s_val.bvne(x_val) : x_val.bvne(s_val); break;
    case NOR: res = pos_x ? s_val.bvnor(x_val) : x_val.bvnor(s_val); break;
    case OR: res = pos_x ? s_val.bvor(x_val) : x_val.bvor(s_val); break;
    case SDIV: res = pos_x ? s_val.bvsdiv(x_val) : x_val.bvsdiv(s_val); break;
    case SGT: res = pos_x ? s_val.bvsgt(x_val) : x_val.bvsgt(s_val); break;
    case SGE: res = pos_x ? s_val.bvsge(x_val) : x_val.bvsge(s_val); break;
    case SHL: res = pos_x ? s_val.bvshl(x_val) : x_val.bvshl(s_val); break;
    case SHR: res = pos_x ? s_val.bvshr(x_val) : x_val.bvshr(s_val); break;
    case SLT: res = pos_x ? s_val.bvslt(x_val) : x_val.bvslt(s_val); break;
    case SLE: res = pos_x ? s_val.bvsle(x_val) : x_val.bvsle(s_val); break;
    case SREM: res = pos_x ? s_val.bvsrem(x_val) : x_val.bvsrem(s_val); break;
    case SUB: res = pos_x ? s_val.bvsub(x_val) : x_val.bvsub(s_val); break;
    case UDIV: res = pos_x ? s_val.bvudiv(x_val) : x_val.bvudiv(s_val); break;
    case UGT: res = pos_x ? s_val.bvugt(x_val) : x_val.bvugt(s_val); break;
    case UGE: res = pos_x ? s_val.bvuge(x_val) : x_val.bvuge(s_val); break;
    case ULT: res = pos_x ? s_val.bvult(x_val) : x_val.bvult(s_val); break;
    case ULE: res = pos_x ? s_val.bvule(x_val) : x_val.bvule(s_val); break;
    case UREM: res = pos_x ? s_val.bvurem(x_val) : x_val.bvurem(s_val); break;
    case XNOR: res = pos_x ? s_val.bvxnor(x_val) : x_val.bvxnor(s_val); break;
    case XOR: res = pos_x ? s_val.bvxor(x_val) : x_val.bvxor(s_val); break;
    default: assert(false);
  }
  return res;
}

/* -------------------------------------------------------------------------- */

class TestBvNode : public TestBvNodeCommon
{
 protected:
  enum Kind
  {
    CONS,
    INV,
    IS_CONS,
    IS_ESS,
    IS_INV,
  };

  bool check_sat_binary(Kind kind,
                        OpKind op_kind,
                        const BitVectorDomain& x,
                        const BitVector& t,
                        const BitVector& s_val,
                        uint32_t pos_x);
  bool check_sat_binary_cons(OpKind op_kind,
                             const BitVector& x_val,
                             const BitVector& t,
                             uint32_t s_size,
                             uint32_t pos_x);
  bool check_sat_binary_is_ess(OpKind op_kind,
                               const BitVector& x_val,
                               const BitVector& t,
                               const BitVectorDomain& s,
                               uint32_t pos_x);
  bool check_sat_ite(Kind kind,
                     const BitVectorDomain& x,
                     const BitVector& t,
                     const BitVector& s0_val,
                     const BitVector& s1_val,
                     uint32_t pos_x);
  bool check_sat_ite_cons(const BitVector& x_val,
                          const BitVector& t,
                          uint32_t s0_size,
                          uint32_t s1_size,
                          uint32_t pos_x);
  bool check_sat_ite_is_ess(const BitVector& x_val,
                            const BitVector& t,
                            const BitVectorDomain& s0,
                            const BitVector& s0_val_cur,
                            const BitVectorDomain& s1,
                            const BitVector& s1_val_cur,
                            uint32_t pos_x);
  bool check_sat_not(Kind kind, const BitVectorDomain& x, const BitVector& t);
  bool check_sat_extract(Kind kind,
                         const BitVectorDomain& x,
                         const BitVector& t,
                         uint32_t hi,
                         uint32_t lo);
  bool check_sat_sext(Kind kind,
                      const BitVectorDomain& x,
                      const BitVector& t,
                      uint32_t n);

  template <class T>
  void test_binary(Kind kind, OpKind op_kind, uint32_t pos_x);
  void test_ite(Kind kind, uint32_t pos_x);
  void test_not(Kind kind);
  void test_extract(Kind kind);
  void test_sext(Kind kind);
};

bool
TestBvNode::check_sat_binary(Kind kind,
                             OpKind op_kind,
                             const BitVectorDomain& x,
                             const BitVector& t,
                             const BitVector& s_val,
                             uint32_t pos_x)
{
  BitVectorDomainGenerator gen(x);
  do
  {
    BitVector res;
    BitVector x_val = gen.has_next() ? gen.next() : x.lo();
    if (kind == IS_CONS)
    {
      BitVectorDomainGenerator gens(s_val.size());
      while (gens.has_next())
      {
        res = eval_op_binary(op_kind, x_val, gens.next(), pos_x);
        if (t.compare(res) == 0) return true;
      }
    }
    else
    {
      assert(kind == IS_INV);
      res = eval_op_binary(op_kind, x_val, s_val, pos_x);
      if (t.compare(res) == 0) return true;
    }
  } while (gen.has_next());
  return false;
}

bool
TestBvNode::check_sat_binary_cons(OpKind op_kind,
                                  const BitVector& x_val,
                                  const BitVector& t,
                                  uint32_t s_size,
                                  uint32_t pos_x)
{
  BitVectorDomain s(s_size);
  BitVectorDomainGenerator gen(s);
  do
  {
    BitVector res;
    BitVector s_val = gen.next();
    res             = eval_op_binary(op_kind, x_val, s_val, pos_x);
    if (t.compare(res) == 0) return true;
  } while (gen.has_next());
  return false;
}

bool
TestBvNode::check_sat_binary_is_ess(OpKind op_kind,
                                    const BitVector& x_val,
                                    const BitVector& t,
                                    const BitVectorDomain& s,
                                    uint32_t pos_x)
{
  BitVectorDomainGenerator gen(s);
  do
  {
    BitVector s_val = gen.has_next() ? gen.next() : s.lo();
    BitVector res   = eval_op_binary(op_kind, x_val, s_val, pos_x);
    if (t.compare(res) == 0) return false;
  } while (gen.has_next());
  return true;
}

bool
TestBvNode::check_sat_ite(Kind kind,
                          const BitVectorDomain& x,
                          const BitVector& t,
                          const BitVector& s0_val,
                          const BitVector& s1_val,
                          uint32_t pos_x)
{
  BitVectorDomainGenerator gen(x);
  do
  {
    BitVector x_val = gen.has_next() ? gen.next() : x.lo();
    if (pos_x == 0)
    {
      if (kind == IS_CONS)
      {
        BitVectorDomainGenerator gens0(s0_val.size());
        while (gens0.has_next())
        {
          BitVector s0val = gens0.next();
          BitVectorDomainGenerator gens1(s1_val.size());
          while (gens1.has_next())
          {
            BitVector res = BitVector::bvite(x_val, s0val, gens1.next());
            if (t.compare(res) == 0) return true;
          }
        }
      }
      else
      {
        assert(kind == IS_INV);
        BitVector res = BitVector::bvite(x_val, s0_val, s1_val);
        if (t.compare(res) == 0) return true;
      }
    }
    else if (pos_x == 1)
    {
      if (kind == IS_CONS)
      {
        BitVectorDomainGenerator gens0(s0_val.size());
        while (gens0.has_next())
        {
          BitVector s0val = gens0.next();
          BitVectorDomainGenerator gens1(s1_val.size());
          while (gens1.has_next())
          {
            BitVector res = BitVector::bvite(s0val, x_val, gens1.next());
            if (t.compare(res) == 0) return true;
          }
        }
      }
      else
      {
        assert(kind == IS_INV);
        if (s0_val.is_false() && s1_val.compare(t) != 0) return false;
        BitVector res = BitVector::bvite(s0_val, x_val, s1_val);
        if (t.compare(res) == 0) return true;
      }
    }
    else
    {
      if (kind == IS_CONS)
      {
        BitVectorDomainGenerator gens0(s0_val.size());
        while (gens0.has_next())
        {
          BitVector s0val = gens0.next();
          BitVectorDomainGenerator gens1(s1_val.size());
          while (gens1.has_next())
          {
            BitVector res = BitVector::bvite(s0val, gens1.next(), x_val);
            if (t.compare(res) == 0) return true;
          }
        }
      }
      else
      {
        assert(kind == IS_INV);
        if (s0_val.is_true() && s1_val.compare(t) != 0) return false;
        BitVector res = BitVector::bvite(s0_val, s1_val, x_val);
        if (t.compare(res) == 0) return true;
      }
    }
  } while (gen.has_next());
  return false;
}

bool
TestBvNode::check_sat_ite_is_ess(const BitVector& x_val,
                                 const BitVector& t,
                                 const BitVectorDomain& s0,
                                 const BitVector& s0_val_cur,
                                 const BitVectorDomain& s1,
                                 const BitVector& s1_val_cur,
                                 uint32_t pos_x)
{
  BitVectorDomainGenerator gens0(s0);
  BitVectorDomainGenerator gens1(s1);
  do
  {
    BitVector s0_val = gens0.has_next() ? gens0.next() : s0.lo();
    BitVectorDomainGenerator gens1(s1);
    BitVector res;
    if (pos_x == 0)
    {
      res = BitVector::bvite(x_val, s0_val, s1_val_cur);
    }
    else if (pos_x == 1)
    {
      res = BitVector::bvite(s0_val, x_val, s1_val_cur);
    }
    else
    {
      res = BitVector::bvite(s0_val, s1_val_cur, x_val);
    }
    if (t.compare(res) == 0) return false;
  } while (gens0.has_next());
  do
  {
    BitVector s1_val = gens1.has_next() ? gens1.next() : s1.lo();
    BitVectorDomainGenerator gens1(s1);
    BitVector res;
    if (pos_x == 0)
    {
      res = BitVector::bvite(x_val, s0_val_cur, s1_val);
    }
    else if (pos_x == 1)
    {
      res = BitVector::bvite(s0_val_cur, x_val, s1_val);
    }
    else
    {
      res = BitVector::bvite(s0_val_cur, s1_val, x_val);
    }
    if (t.compare(res) == 0) return false;
  } while (gens1.has_next());
  return true;
}

bool
TestBvNode::check_sat_ite_cons(const BitVector& x_val,
                               const BitVector& t,
                               uint32_t s0_size,
                               uint32_t s1_size,
                               uint32_t pos_x)
{
  BitVectorDomainGenerator gens0(s0_size);
  while (gens0.has_next())
  {
    BitVector s0val = gens0.next();
    BitVectorDomainGenerator gens1(s1_size);
    while (gens1.has_next())
    {
      BitVector res;
      if (pos_x == 0)
      {
        res = BitVector::bvite(x_val, s0val, gens1.next());
      }
      else if (pos_x == 1)
      {
        res = BitVector::bvite(s0val, x_val, gens1.next());
      }
      else
      {
        res = BitVector::bvite(s0val, gens1.next(), x_val);
      }
      if (t.compare(res) == 0) return true;
    }
  }
  return false;
}

bool
TestBvNode::check_sat_not(Kind kind,
                          const BitVectorDomain& x,
                          const BitVector& t)
{
  assert(kind == IS_CONS || kind == IS_ESS || kind == IS_INV);
  BitVectorDomainGenerator gen(x);
  do
  {
    BitVector val = gen.has_next() ? gen.next() : x.lo();
    BitVector res = val.bvnot();
    if (t.compare(res) == 0) return kind == IS_ESS ? false : true;
  } while (gen.has_next());
  return kind == IS_ESS ? true : false;
}

bool
TestBvNode::check_sat_extract(Kind kind,
                              const BitVectorDomain& x,
                              const BitVector& t,
                              uint32_t hi,
                              uint32_t lo)
{
  assert(kind == IS_CONS || kind == IS_ESS || kind == IS_INV);
  BitVectorDomainGenerator gen(x);
  do
  {
    BitVector val = gen.has_next() ? gen.next() : x.lo();
    BitVector res = val.bvextract(hi, lo);
    if (t.compare(res) == 0) return kind == IS_ESS ? false : true;
  } while (gen.has_next());
  return kind == IS_ESS ? true : false;
}

bool
TestBvNode::check_sat_sext(Kind kind,
                           const BitVectorDomain& x,
                           const BitVector& t,
                           uint32_t n)
{
  assert(kind == IS_CONS || kind == IS_ESS || kind == IS_INV);
  BitVectorDomainGenerator gen(x);
  do
  {
    BitVector val = gen.has_next() ? gen.next() : x.lo();
    BitVector res = val.bvsext(n);
    if (t.compare(res) == 0) return kind == IS_ESS ? false : true;
  } while (gen.has_next());
  return kind == IS_ESS ? true : false;
}

template <class T>
void
TestBvNode::test_binary(Kind kind, OpKind op_kind, uint32_t pos_x)
{
  uint32_t bw_x = TEST_BW;
  uint32_t bw_s = TEST_BW;
  uint32_t bw_t = TEST_BW;

  if (op_kind == ULT || op_kind == SLT || op_kind == EQ)
  {
    bw_t = 1;
  }
  else if (op_kind == CONCAT)
  {
    bw_s = 2; /* decrease number of tests for concat */
    bw_t = bw_s + bw_x;
  }

  uint32_t nval_x = 1 << bw_x;
  uint32_t nval_s = 1 << bw_s;
  uint32_t nval_t = 1 << bw_t;

  if (kind == IS_ESS)
  {
    std::vector<std::string> svalues;
    if (op_kind == CONCAT)
    {
      gen_xvalues(bw_s, svalues);
    }
    else
    {
      svalues = d_xvalues;
    }

    for (const std::string& s_value : svalues)
    {
      BitVectorDomain s(s_value);
      for (uint32_t i = 0; i < nval_x; i++)
      {
        BitVector x_val(bw_x, i);
        for (uint32_t j = 0; j < nval_t; j++)
        {
          /* Target value of the operation (op). */
          BitVector t(bw_t, j);
          /* For this test, we don't care about the current assignment of s,
           * thus we initialize it with a random value that matches constant
           * bits in s. */
          BitVector s_val = s.lo();
          if (!s.is_fixed())
          {
            BitVectorDomainGenerator gen(s, d_rng.get());
            s_val = gen.random();
          }
          /* For this test, the domain of x is irrelevant, hence we
           * initialize it with an unconstrained domain. */
          std::unique_ptr<BitVectorNode> op_x(
              new BitVectorNode(d_rng.get(), x_val, BitVectorDomain(bw_x)));
          std::unique_ptr<BitVectorNode> op_s(
              new BitVectorNode(d_rng.get(), s_val, s));
          /* For this test, we don't care about current assignment and domain of
           * the op, thus we initialize them with 0 and x..x, respectively. */
          T op(d_rng.get(),
               bw_t,
               pos_x == 0 ? op_x.get() : op_s.get(),
               pos_x == 1 ? op_x.get() : op_s.get());
          bool res    = op.is_essential(t, pos_x);
          bool status = check_sat_binary_is_ess(op_kind, x_val, t, s, pos_x);
          if (res != status)
          {
            std::cout << "pos_x: " << pos_x << std::endl;
            std::cout << "t: " << t << std::endl;
            std::cout << "x: " << x_val << std::endl;
            std::cout << "s: " << s << ": " << s_val << std::endl;
          }
          ASSERT_EQ(res, status);
        }
      }
    }
  }
  else
  {
    for (const std::string& x_value : d_xvalues)
    {
      BitVectorDomain x(x_value);
      for (uint32_t i = 0; i < nval_s; i++)
      {
        /* Assignment of the other operand. */
        BitVector s_val(bw_s, i);
        for (uint32_t j = 0; j < nval_t; j++)
        {
          /* Target value of the operation (op). */
          BitVector t(bw_t, j);
          /* For this test, we don't care about the current assignment of x,
           * thus we initialize it with a random value that matches constant
           * bits in x. */
          BitVector x_val = x.lo();
          if (!x.is_fixed())
          {
            BitVectorDomainGenerator gen(x, d_rng.get());
            x_val = gen.random();
          }
          std::unique_ptr<BitVectorNode> op_x(
              new BitVectorNode(d_rng.get(), x_val, x));
          /* For this test, we don't care about the domain of s, thus we
           * initialize it with an unconstrained domain. */
          BitVectorDomain s(bw_s);
          std::unique_ptr<BitVectorNode> op_s(
              new BitVectorNode(d_rng.get(), s_val, s));
          /* For this test, we don't care about current assignment and domain of
           * the op, thus we initialize them with 0 and x..x, respectively. */
          T op(d_rng.get(),
               bw_t,
               pos_x == 0 ? op_x.get() : op_s.get(),
               pos_x == 1 ? op_x.get() : op_s.get());

          if (kind == IS_CONS || kind == IS_INV)
          {
            bool res    = kind == IS_INV ? op.is_invertible(t, pos_x)
                                         : op.is_consistent(t, pos_x);
            bool status = check_sat_binary(kind, op_kind, x, t, s_val, pos_x);
            if (res != status)
            {
              std::cout << "pos_x: " << pos_x << std::endl;
              std::cout << "t: " << t << std::endl;
              std::cout << "x: " << x_value << ": " << x_val << std::endl;
              std::cout << "s: " << s_val << std::endl;
            }
            ASSERT_EQ(res, status);
          }
          else if (kind == INV)
          {
            if (x.is_fixed()) continue;
            if (!op.is_invertible(t, pos_x)) continue;
            BitVector inv = op.inverse_value(t, pos_x);
            int32_t cmp = t.compare(eval_op_binary(op_kind, inv, s_val, pos_x));
            if (cmp != 0)
            {
              std::cout << "pos_x: " << pos_x << std::endl;
              std::cout << "t: " << t << std::endl;
              std::cout << "x: " << x_value << ": " << x_val << std::endl;
              std::cout << "s: " << s_val << std::endl;
              std::cout << "inverse: " << inv << std::endl;
            }
            ASSERT_EQ(cmp, 0);
            ASSERT_TRUE(op.is_consistent(t, pos_x));
          }
          else
          {
            assert(kind == CONS);
            if (x.is_fixed()) continue;
            if (!op.is_consistent(t, pos_x)) continue;
            BitVector cons = op.consistent_value(t, pos_x);
            bool status =
                check_sat_binary_cons(op_kind, cons, t, s_val.size(), pos_x);
            if (!status)
            {
              std::cout << "pos_x: " << pos_x << std::endl;
              std::cout << "t: " << t << std::endl;
              std::cout << "x: " << x_value << ": " << x_val << std::endl;
              std::cout << "consistent: " << cons << std::endl;
            }
            ASSERT_TRUE(status);
          }
        }
      }
    }
  }
}

void
TestBvNode::test_ite(Kind kind, uint32_t pos_x)
{
  uint32_t bw_s0, bw_s1, bw_x, bw_t = TEST_BW;
  uint32_t n_vals, n_vals_s0, n_vals_s1;

  if (pos_x)
  {
    bw_x  = TEST_BW;
    bw_s0 = 1;
    bw_s1 = TEST_BW;
  }
  else
  {
    bw_x  = 1;
    bw_s0 = TEST_BW;
    bw_s1 = TEST_BW;
  }
  n_vals    = 1 << bw_x;
  n_vals_s0 = 1 << bw_s0;
  n_vals_s1 = 1 << bw_s1;

  if (kind == IS_ESS)
  {
    std::vector<std::string> s0_values, s1_values;
    if (pos_x)
    {
      s0_values.push_back("x");
      s0_values.push_back("0");
      s0_values.push_back("1");
    }
    else
    {
      s0_values = d_xvalues;
    }
    s1_values = d_xvalues;

    for (const std::string& s0_value : s0_values)
    {
      BitVectorDomain s0(s0_value);
      for (const std::string& s1_value : s1_values)
      {
        BitVectorDomain s1(s1_value);
        for (uint32_t i = 0; i < n_vals; i++)
        {
          BitVector x_val(bw_x, i);
          for (uint32_t j = 0; j < n_vals; j++)
          {
            BitVector t(bw_t, j);
            /* For this test, the domain of x is irrelevant, hence we
             * initialize it with an unconstrained domain. */
            std::unique_ptr<BitVectorNode> op_x(
                new BitVectorNode(d_rng.get(), x_val, BitVectorDomain(bw_x)));
            /* For this test, we don't care about the current assignment of s0
             * and s1, hence we use a random value. */
            BitVector s0_val = s0.lo(), s1_val = s1.lo();
            if (!s0.is_fixed())
            {
              BitVectorDomainGenerator gen(s0, d_rng.get());
              s0_val = gen.random();
            }
            if (!s1.is_fixed())
            {
              BitVectorDomainGenerator gen(s1, d_rng.get());
              s1_val = gen.random();
            }
            std::unique_ptr<BitVectorNode> op_s0(
                new BitVectorNode(d_rng.get(), s0_val, s0));
            std::unique_ptr<BitVectorNode> op_s1(
                new BitVectorNode(d_rng.get(), s1_val, s1));
            /* For this test, we don't care about current assignment and domain
             * of the op, thus we initialize them with 0 and x..x,
             * respectively. */
            BitVectorIte op(d_rng.get(),
                            bw_t,
                            pos_x == 0 ? op_x.get() : op_s0.get(),
                            pos_x == 1
                                ? op_x.get()
                                : (pos_x == 2 ? op_s1.get() : op_s0.get()),
                            pos_x == 2 ? op_x.get() : op_s1.get());
            bool res = op.is_essential(t, pos_x);
            bool status =
                check_sat_ite_is_ess(x_val, t, s0, s0_val, s1, s1_val, pos_x);
            if (res != status)
            {
              std::cout << "pos_x: " << pos_x << std::endl;
              std::cout << "t: " << t << std::endl;
              std::cout << "x: " << x_val << std::endl;
              std::cout << "s0: " << s0 << ": " << s0_val << std::endl;
              std::cout << "s1: " << s1 << ": " << s1_val << std::endl;
            }
            ASSERT_EQ(res, status);
          }
        }
      }
    }
  }
  else
  {
    std::vector<std::string> x_values;
    if (pos_x)
    {
      x_values = d_xvalues;
    }
    else
    {
      x_values.push_back("x");
      x_values.push_back("0");
      x_values.push_back("1");
    }

    for (const std::string& x_value : x_values)
    {
      BitVectorDomain x(x_value);
      for (uint32_t i = 0; i < n_vals_s0; i++)
      {
        BitVector s0_val(bw_s0, i);
        for (uint32_t j = 0; j < n_vals_s1; j++)
        {
          BitVector s1_val(bw_s1, j);
          for (uint32_t k = 0; k < n_vals; k++)
          {
            BitVector t(bw_t, k);

            /* For this test, we don't care about the current assignment of x,
             * thus we initialize it with a random value that matches constant
             * bits in x. */
            BitVector x_val = x.lo();
            if (!x.is_fixed())
            {
              BitVectorDomainGenerator gen(x, d_rng.get());
              x_val = gen.random();
            }
            std::unique_ptr<BitVectorNode> op_x(
                new BitVectorNode(d_rng.get(), x_val, x));
            /* For this test, we don't care about the domains of s0 and s1,
             * hence we initialize them with unconstrained domains. */
            std::unique_ptr<BitVectorNode> op_s0(
                new BitVectorNode(d_rng.get(), s0_val, BitVectorDomain(bw_s0)));
            std::unique_ptr<BitVectorNode> op_s1(
                new BitVectorNode(d_rng.get(), s1_val, BitVectorDomain(bw_s1)));
            /* For this test, we don't care about current assignment and domain
             * of the op, thus we initialize them with 0 and x..x,
             * respectively. */
            BitVectorIte op(d_rng.get(),
                            bw_t,
                            pos_x == 0 ? op_x.get() : op_s0.get(),
                            pos_x == 1
                                ? op_x.get()
                                : (pos_x == 2 ? op_s1.get() : op_s0.get()),
                            pos_x == 2 ? op_x.get() : op_s1.get());

            if (kind == IS_INV || kind == IS_CONS)
            {
              bool res    = kind == IS_INV ? op.is_invertible(t, pos_x)
                                           : op.is_consistent(t, pos_x);
              bool status = check_sat_ite(kind, x, t, s0_val, s1_val, pos_x);
              if (res != status)
              {
                std::cout << "pos_x: " << pos_x << std::endl;
                std::cout << "t: " << t << std::endl;
                std::cout << "x: " << x_value << ": " << x_val << std::endl;
                std::cout << "s0: " << s0_val << std::endl;
                std::cout << "s1: " << s1_val << std::endl;
              }
              ASSERT_EQ(res, status);
            }
            else if (kind == INV)
            {
              if (x.is_fixed()) continue;
              if (!op.is_invertible(t, pos_x)) continue;
              BitVector inv = op.inverse_value(t, pos_x);
              int32_t cmp;
              if (pos_x == 0)
              {
                cmp = t.compare(BitVector::bvite(inv, s0_val, s1_val));
              }
              else if (pos_x == 1)
              {
                cmp = t.compare(BitVector::bvite(s0_val, inv, s1_val));
              }
              else
              {
                cmp = t.compare(BitVector::bvite(s0_val, s1_val, inv));
              }
              if (cmp != 0)
              {
                std::cout << "pos_x: " << pos_x << std::endl;
                std::cout << "t: " << t << std::endl;
                std::cout << "x: " << x_value << ": " << x_val << std::endl;
                std::cout << "s0: " << s0_val << std::endl;
                std::cout << "s1: " << s1_val << std::endl;
                std::cout << "inverse: " << inv << std::endl;
              }
              ASSERT_EQ(cmp, 0);
              ASSERT_TRUE(op.is_consistent(t, pos_x));
            }
            else
            {
              assert(kind == CONS);
              if (x.is_fixed()) continue;
              if (!op.is_consistent(t, pos_x)) continue;
              BitVector cons = op.consistent_value(t, pos_x);
              bool status    = check_sat_ite_cons(
                  cons, t, s0_val.size(), s1_val.size(), pos_x);
              if (!status)
              {
                std::cout << "pos_x: " << pos_x << std::endl;
                std::cout << "t: " << t << std::endl;
                std::cout << "x: " << x_value << ": " << x_val << std::endl;
                std::cout << "consistent: " << cons << std::endl;
              }
              ASSERT_TRUE(status);
            }
          }
        }
      }
    }
  }
}

void
TestBvNode::test_not(Kind kind)
{
  for (const std::string& x_value : d_xvalues)
  {
    BitVectorDomain x(x_value);
    uint32_t bw_t = x.size();
    for (uint32_t i = 0, n = 1 << bw_t; i < n; ++i)
    {
      BitVector t(bw_t, i);
      /* For this test, we don't care about the current assignment of x,
       * thus we initialize it with a random value that matches constant
       * bits in x. */
      BitVector x_val = x.lo();
      if (!x.is_fixed())
      {
        BitVectorDomainGenerator gen(x, d_rng.get());
        x_val = gen.random();
      }
      std::unique_ptr<BitVectorNode> op_x(
          new BitVectorNode(d_rng.get(), x_val, x));
      /* For this test, we don't care about current assignment and domain
       * of the op, thus we initialize them with 0 and 'x..x',
       * respectively. */
      BitVectorNot op(d_rng.get(), bw_t, op_x.get());

      if (kind == IS_CONS || kind == IS_ESS || kind == IS_INV)
      {
        bool res    = kind == IS_ESS ? op.is_essential(t, 0)
                                     : (kind == IS_INV ? op.is_invertible(t, 0)
                                                       : op.is_consistent(t, 0));
        bool status = check_sat_not(kind, x, t);

        if (res != status)
        {
          std::cout << "t: " << t << std::endl;
          std::cout << "x: " << x_value << ": " << x_val << std::endl;
        }
        ASSERT_EQ(res, status);
      }
      else
      {
        assert(kind == INV || kind == CONS);
        if (x.is_fixed()) continue;
        if (kind == INV && !op.is_invertible(t, 0)) continue;
        if (kind == CONS && !op.is_consistent(t, 0)) continue;

        if (kind == INV)
        {
          BitVector inv = op.inverse_value(t, 0);
          int32_t cmp   = t.compare(inv.bvnot());
          if (cmp != 0)
          {
            std::cout << "t: " << t << std::endl;
            std::cout << "x: " << x_value << ": " << x_val << std::endl;
            std::cout << "inverse: " << inv << std::endl;
          }
          ASSERT_EQ(cmp, 0);
        }
        else
        {
          BitVector cons = op.consistent_value(t, 0);
          int32_t cmp    = t.compare(cons.bvnot());
          if (cmp != 0)
          {
            std::cout << "t: " << t << std::endl;
            std::cout << "x: " << x_value << ": " << x_val << std::endl;
            std::cout << "consistent: " << cons << std::endl;
          }
          ASSERT_EQ(cmp, 0);
        }
        ASSERT_TRUE(op.is_consistent(t, 0));
      }
    }
  }
}

void
TestBvNode::test_extract(Kind kind)
{
  uint32_t bw_x = TEST_BW;

  for (const std::string& x_value : d_xvalues)
  {
    BitVectorDomain x(x_value);
    for (uint32_t lo = 0; lo < bw_x; ++lo)
    {
      for (uint32_t hi = lo; hi < bw_x; ++hi)
      {
        uint32_t bw_t = hi - lo + 1;
        for (uint32_t i = 0, n = 1 << bw_t; i < n; ++i)
        {
          BitVector t(bw_t, i);
          /* For this test, we don't care about the current assignment of x,
           * thus we initialize it with a random value that matches constant
           * bits in x. */
          BitVector x_val = x.lo();
          if (!x.is_fixed())
          {
            BitVectorDomainGenerator gen(x, d_rng.get());
            x_val = gen.random();
          }
          std::unique_ptr<BitVectorNode> op_x(
              new BitVectorNode(d_rng.get(), x_val, x));
          /* For this test, we don't care about current assignment and domain
           * of the op, thus we initialize them with 0 and 'x..x',
           * respectively. */
          BitVectorExtract op(d_rng.get(), bw_t, op_x.get(), hi, lo);

          if (kind == IS_CONS || kind == IS_ESS || kind == IS_INV)
          {
            bool res    = kind == IS_ESS
                              ? op.is_essential(t, 0)
                              : (kind == IS_INV ? op.is_invertible(t, 0)
                                                : op.is_consistent(t, 0));
            bool status = check_sat_extract(kind, x, t, hi, lo);

            if (res != status)
            {
              std::cout << "hi: " << hi << std::endl;
              std::cout << "lo: " << lo << std::endl;
              std::cout << "t: " << t << std::endl;
              std::cout << "x: " << x_value << ": " << x_val << std::endl;
            }
            ASSERT_EQ(res, status);
          }
          else
          {
            assert(kind == INV || kind == CONS);
            if (x.is_fixed()) continue;
            if (kind == INV && !op.is_invertible(t, 0)) continue;
            if (kind == CONS && !op.is_consistent(t, 0)) continue;

            if (kind == INV)
            {
              BitVector inv = op.inverse_value(t, 0);
              int32_t cmp   = t.compare(inv.bvextract(hi, lo));
              if (cmp != 0)
              {
                std::cout << "hi: " << hi << std::endl;
                std::cout << "lo: " << lo << std::endl;
                std::cout << "t: " << t << std::endl;
                std::cout << "x: " << x_value << ": " << x_val << std::endl;
                std::cout << "inverse: " << inv << std::endl;
              }
              ASSERT_EQ(cmp, 0);
            }
            else
            {
              BitVector cons = op.consistent_value(t, 0);
              int32_t cmp    = t.compare(cons.bvextract(hi, lo));
              if (cmp != 0)
              {
                std::cout << "hi: " << hi << std::endl;
                std::cout << "lo: " << lo << std::endl;
                std::cout << "t: " << t << std::endl;
                std::cout << "x: " << x_value << ": " << x_val << std::endl;
                std::cout << "consistent: " << cons << std::endl;
              }
              ASSERT_EQ(cmp, 0);
            }
            ASSERT_TRUE(op.is_consistent(t, 0));
          }
        }
      }
    }
  }
}

void
TestBvNode::test_sext(Kind kind)
{
  uint32_t bw_x = TEST_BW;

  for (const std::string& x_value : d_xvalues)
  {
    BitVectorDomain x(x_value);
    for (uint32_t n = 1; n <= bw_x; ++n)
    {
      uint32_t bw_t = bw_x + n;
      for (uint32_t i = 0, m = 1 << bw_t; i < m; ++i)
      {
        BitVector t(bw_t, i);
        /* For this test, we don't care about the current assignment of x,
         * thus we initialize it with a random value that matches constant
         * bits in x. */
        BitVector x_val = x.lo();
        if (!x.is_fixed())
        {
          BitVectorDomainGenerator gen(x, d_rng.get());
          x_val = gen.random();
        }
        std::unique_ptr<BitVectorNode> op_x(
            new BitVectorNode(d_rng.get(), x_val, x));
        /* For this test, we don't care about current assignment and domain
         * of the op, thus we initialize them with 0 and 'x..x',
         * respectively. */
        BitVectorSignExtend op(d_rng.get(), bw_t, op_x.get(), n);

        if (kind == IS_CONS || kind == IS_ESS || kind == IS_INV)
        {
          bool res    = kind == IS_ESS ? op.is_essential(t, 0)
                                       : (kind == IS_INV ? op.is_invertible(t, 0)
                                                         : op.is_consistent(t, 0));
          bool status = check_sat_sext(kind, x, t, n);

          if (res != status)
          {
            std::cout << "n: " << n << std::endl;
            std::cout << "t: " << t << std::endl;
            std::cout << "x: " << x_value << ": " << x_val << std::endl;
          }
          ASSERT_EQ(res, status);
        }
        else
        {
          assert(kind == INV || kind == CONS);
          if (x.is_fixed()) continue;
          if (kind == INV && !op.is_invertible(t, 0)) continue;
          if (kind == CONS && !op.is_consistent(t, 0)) continue;

          if (kind == INV)
          {
            BitVector inv = op.inverse_value(t, 0);
            int32_t cmp   = t.compare(inv.bvsext(n));
            if (cmp != 0)
            {
              std::cout << "n: " << n << std::endl;
              std::cout << "t: " << t << std::endl;
              std::cout << "x: " << x_value << ": " << x_val << std::endl;
              std::cout << "inverse: " << inv << std::endl;
            }
            ASSERT_EQ(cmp, 0);
          }
          else
          {
            BitVector cons = op.consistent_value(t, 0);
            int32_t cmp    = t.compare(cons.bvsext(n));
            if (cmp != 0)
            {
              std::cout << "n: " << n << std::endl;
              std::cout << "t: " << t << std::endl;
              std::cout << "x: " << x_value << ": " << x_val << std::endl;
              std::cout << "consistent: " << cons << std::endl;
            }
            ASSERT_EQ(cmp, 0);
          }
          ASSERT_TRUE(op.is_consistent(t, 0));
        }
      }
    }
  }
}

/* -------------------------------------------------------------------------- */

}  // namespace bzlatest
#endif

#include "node/node_manager.h"

#include "bitvector.h"

namespace bzla::node {

/* --- NodeManager public -------------------------------------------------- */

Node
NodeManager::mk_const(const type::Type& t, const std::string& symbol)
{
  assert(!t.is_null());
  // TODO: handle symbol
  NodeData* data = new NodeData(this, Kind::CONSTANT);
  data->d_type   = t;
  init_id(data);
  return Node(data);
}

Node
NodeManager::mk_var(const type::Type& t, const std::string& symbol)
{
  assert(!t.is_null());
  // TODO: handle symbol
  NodeData* data = new NodeData(this, Kind::VARIABLE);
  data->d_type   = t;
  init_id(data);
  return Node(data);
}

Node
NodeManager::mk_value(bool value)
{
  NodeData* data  = new NodeDataValue(this, value);
  data->d_type    = mk_bool_type();
  auto found_data = find_or_insert_node(data);
  if (found_data)
  {
    delete data;
    data = found_data;
  }
  return Node(data);
}

Node
NodeManager::mk_value(const BitVector& value)
{
  NodeData* data  = new NodeDataValue(this, value);
  data->d_type    = mk_bv_type(value.size());
  auto found_data = find_or_insert_node(data);
  if (found_data)
  {
    delete data;
    data = found_data;
  }
  return Node(data);
}

Node
NodeManager::mk_node(Kind kind,
                     const std::vector<Node>& children,
                     const std::vector<uint64_t>& indices)
{
  NodeData* data  = new_data(kind, children, indices);
  auto found_data = find_or_insert_node(data);
  if (found_data)
  {
    delete data;
    data = found_data;
  }
  else
  {
    // Compute type for new node
    data->d_type = compute_type(kind, children, indices);
  }
  return Node(data);
}

type::Type
NodeManager::mk_bool_type()
{
  return d_tm.mk_bool_type();
}

type::Type
NodeManager::mk_bv_type(uint64_t size)
{
  return d_tm.mk_bv_type(size);
}

type::Type
NodeManager::mk_fp_type(uint64_t exp_size, uint64_t sig_size)
{
  return d_tm.mk_fp_type(exp_size, sig_size);
}

type::Type
NodeManager::mk_rm_type()
{
  return d_tm.mk_rm_type();
}

type::Type
NodeManager::mk_array_type(const type::Type& index, const type::Type& elem)
{
  return d_tm.mk_array_type(index, elem);
}

type::Type
NodeManager::mk_fun_type(const std::vector<type::Type>& types)
{
  return d_tm.mk_fun_type(types);
}

type::Type
NodeManager::compute_type(Kind kind,
                          const std::vector<Node>& children,
                          const std::vector<uint64_t>& indices)
{
  assert(check_type(kind, children, indices).first);

  switch (kind)
  {
    // Add these kinds since there is no default branch (we want compile
    // warnings if when new kinds are added).
    case Kind::NULL_NODE:
    case Kind::NUM_KINDS:
      // Type checking should never be called on nodes with these kinds.
      assert(false);
      break;

    case Kind::CONSTANT:
    case Kind::VARIABLE:
    case Kind::VALUE:
      // No need to compute type since they are assigned on construction for
      // these kinds.
      assert(false);
      break;

    case Kind::NOT:
    case Kind::AND:
    case Kind::OR:
    case Kind::EQUAL:
    case Kind::BV_ULT:
    case Kind::BV_SLT:
    case Kind::FP_EQUAL:
    case Kind::FP_LE:
    case Kind::FP_LT:
    case Kind::FP_IS_INF:
    case Kind::FP_IS_NAN:
    case Kind::FP_IS_NEG:
    case Kind::FP_IS_POS:
    case Kind::FP_IS_NORM:
    case Kind::FP_IS_SUBNORM:
    case Kind::FP_IS_ZERO:
    case Kind::FORALL:
    case Kind::EXISTS: return d_tm.mk_bool_type();

    case Kind::BV_EXTRACT: {
      uint64_t upper = indices[0];
      uint64_t lower = indices[1];
      return d_tm.mk_bv_type(upper - lower + 1);
    }

    case Kind::BV_CONCAT:
      return d_tm.mk_bv_type(children[0].get_type().get_bv_size()
                             + children[1].get_type().get_bv_size());

    case Kind::BV_NOT:
    case Kind::BV_AND:
    case Kind::BV_ADD:
    case Kind::BV_MUL:
    case Kind::BV_SHL:
    case Kind::BV_SHR:
    case Kind::BV_ASHR:
    case Kind::BV_UDIV:
    case Kind::BV_UREM:
    case Kind::FP_ABS:
    case Kind::FP_NEG:
    case Kind::FP_MIN:
    case Kind::FP_MAX:
    case Kind::FP_REM:
    case Kind::STORE: return children[0].get_type();

    case Kind::FP_SQRT:
    case Kind::FP_RTI:
    case Kind::FP_ADD:
    case Kind::FP_MUL:
    case Kind::FP_DIV:
    case Kind::ITE:
    case Kind::FP_FMA: return children[1].get_type();

    case Kind::FP_TO_SBV:
    case Kind::FP_TO_UBV: return d_tm.mk_bv_type(indices[0]);

    case Kind::FP_TO_FP_FROM_BV:
    case Kind::FP_TO_FP_FROM_FP:
    case Kind::FP_TO_FP_FROM_SBV:
    case Kind::FP_TO_FP_FROM_UBV:
      return d_tm.mk_fp_type(indices[0], indices[1]);

    case Kind::SELECT: return children[0].get_type().get_array_element();

    case Kind::APPLY: return children[0].get_type().get_fun_types().back();

    case Kind::LAMBDA: {
      std::vector<type::Type> types{children[0].get_type()};
      // Flatten function types
      if (children[1].get_type().is_fun())
      {
        auto const& fun_types = children[1].get_type().get_fun_types();
        types.insert(types.end(), fun_types.begin(), fun_types.end());
      }
      else
      {
        types.push_back(children[1].get_type());
      }
      return d_tm.mk_fun_type(types);
    }
  }

  assert(false);
  return type::Type();
}

std::pair<bool, std::string>
NodeManager::check_type(Kind kind,
                        const std::vector<Node>& children,
                        const std::vector<uint64_t>& indices)
{
  std::stringstream ss;

  const KindInformation& info = s_node_kind_info[kind];
  if ((!info.is_nary() && children.size() != info.num_children))
  {
    ss << kind << ": ";
    ss << "Invalid number of children. Expected " << info.num_children
       << ", got " << children.size();
    return std::make_pair(false, ss.str());
  }
  else if (indices.size() != info.num_indices)
  {
    ss << kind << ": ";
    ss << "Invalid number of indices. Expected " << info.num_indices << ", got "
       << indices.size();
    return std::make_pair(false, ss.str());
  }

  switch (kind)
  {
    // Add these kinds since there is no default branch (we want compile
    // warnings if when new kinds are added).
    case Kind::NULL_NODE:
    case Kind::NUM_KINDS:
      // Type checking should never be called on nodes with these kinds.
      assert(false);
      break;

    case Kind::CONSTANT:
    case Kind::VARIABLE:
    case Kind::VALUE:
      // Nothing to do here for now.
      break;

    case Kind::NOT:
    case Kind::AND:
    case Kind::OR:
      for (size_t i = 0, size = children.size(); i < size; ++i)
      {
        if (!children[i].get_type().is_bool())
        {
          ss << kind << ": ";
          ss << "Expected Boolean term at position " << i;
          return std::make_pair(false, ss.str());
        }
      }
      break;

    case Kind::BV_EXTRACT:
      if (!children[0].get_type().is_bv())
      {
        ss << kind << ": Expected bit-vector term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (children[0].get_type().get_bv_size() <= indices[0])
      {
        ss << kind << ": Upper index must be less than the bit-width";
        return std::make_pair(false, ss.str());
      }
      if (indices[0] < indices[1])
      {
        ss << kind << ": Upper index must be greater equal lower index";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::FP_ABS:
    case Kind::FP_NEG:
    case Kind::FP_IS_INF:
    case Kind::FP_IS_NAN:
    case Kind::FP_IS_NEG:
    case Kind::FP_IS_NORM:
    case Kind::FP_IS_POS:
    case Kind::FP_IS_SUBNORM:
    case Kind::FP_IS_ZERO:
      if (!children[0].get_type().is_fp())
      {
        ss << kind << ": Expected floating point term at position 0";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::FP_TO_FP_FROM_BV:
      if (!children[0].get_type().is_bv())
      {
        ss << kind << ": Expected bit-vector term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (children[0].get_type().get_bv_size() != indices[0] + indices[1])
      {
        ss << kind
           << ": Floating-point format does not match size of bit-vector term "
              "at position 0";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::EQUAL:
      if (children[0].get_type() != children[1].get_type())
      {
        ss << kind << ": Expected terms with same type.";
        return std::make_pair(false, ss.str());
      }
      break;

    // Unary bit-vector operators
    case Kind::BV_NOT:
      if (!children[0].get_type().is_bv())
      {
        ss << kind << ": Expected bit-vector term at position 0";
        return std::make_pair(false, ss.str());
      }
      break;

    // Binary bit-vector operators
    case Kind::BV_AND:
    case Kind::BV_ADD:
    case Kind::BV_MUL:
    case Kind::BV_ULT:
    case Kind::BV_SHL:
    case Kind::BV_SLT:
    case Kind::BV_SHR:
    case Kind::BV_ASHR:
    case Kind::BV_UDIV:
    case Kind::BV_UREM:
      if (!children[0].get_type().is_bv())
      {
        ss << kind << ": Expected bit-vector term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (children[0].get_type() != children[1].get_type())
      {
        ss << kind << ": Expected terms of the same bit-vector type.";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::BV_CONCAT:
      if (!children[0].get_type().is_bv())
      {
        ss << kind << ": Expected bit-vector term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (!children[1].get_type().is_bv())
      {
        ss << kind << ": Expected bit-vector term at position 1";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::FP_EQUAL:
    case Kind::FP_LE:
    case Kind::FP_LT:
    case Kind::FP_MIN:
    case Kind::FP_MAX:
    case Kind::FP_REM:
      if (!children[0].get_type().is_fp())
      {
        ss << kind << ": Expected floating-point term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (children[0].get_type() != children[1].get_type())
      {
        ss << kind << ": Expected terms of the same floating-point type.";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::FP_SQRT:
    case Kind::FP_RTI:
    case Kind::FP_TO_SBV:
    case Kind::FP_TO_UBV:
    case Kind::FP_TO_FP_FROM_FP:
      if (!children[0].get_type().is_rm())
      {
        ss << kind << ": Expected rounding mode term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (!children[1].get_type().is_fp())
      {
        ss << kind << ": Expected floating-point term at position 1";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::FP_TO_FP_FROM_SBV:
    case Kind::FP_TO_FP_FROM_UBV:
      if (!children[0].get_type().is_rm())
      {
        ss << kind << ": Expected rounding mode term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (!children[1].get_type().is_bv())
      {
        ss << kind << ": Expected bit-vector term at position 1";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::SELECT:
      if (!children[0].get_type().is_array())
      {
        ss << kind << ": Expected array term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (children[0].get_type().get_array_index() != children[1].get_type())
      {
        ss << kind << ": Index term does not match array index type";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::APPLY: {
      if (!children[0].get_type().is_fun())
      {
        ss << kind << ": Expected function term at position 0";
        return std::make_pair(false, ss.str());
      }
      const auto& fun_types = children[0].get_type().get_fun_types();
      if (children.size() != fun_types.size())
      {
        ss << kind << ": Number of arguments does not match function domain: "
           << children.size() - 1 << " != " << fun_types.size() - 1;
        return std::make_pair(false, ss.str());
      }
      for (size_t i = 1, size = children.size(); i < size; ++i)
      {
        if (children[i].get_type() != fun_types[i - 1])
        {
          ss << kind << ": Term at position " << i
             << " does not match function domain type";
          return std::make_pair(false, ss.str());
        }
      }
    }
    break;

    case Kind::FORALL:
    case Kind::EXISTS:
      if (children[0].get_kind() != Kind::VARIABLE)
      {
        ss << kind << ": Expected variable at position 0";
        return std::make_pair(false, ss.str());
      }
      if (children[0].get_type().is_array())
      {
        ss << kind << ": Array type not supported for variables";
        return std::make_pair(false, ss.str());
      }
      if (children[0].get_type().is_fun())
      {
        ss << kind << ": Function type not supported for variables";
        return std::make_pair(false, ss.str());
      }
      if (!children[1].get_type().is_bool())
      {
        ss << kind << ": Expected Boolean term at position 1";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::LAMBDA:
      if (children[0].get_kind() != Kind::VARIABLE)
      {
        ss << kind << ": Expected variable at position 0";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::ITE:
      if (!children[0].get_type().is_bool())
      {
        ss << kind << ": Expected Boolean term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (children[1].get_type() != children[2].get_type())
      {
        ss << kind << ": Expected terms of the type at position 1 and 2.";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::FP_ADD:
    case Kind::FP_MUL:
    case Kind::FP_DIV:
      if (!children[0].get_type().is_rm())
      {
        ss << kind << ": Expected rounding mode term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (!children[1].get_type().is_fp())
      {
        ss << kind << ": Expected floating-point term at position 1";
        return std::make_pair(false, ss.str());
      }
      if (children[1].get_type() != children[2].get_type())
      {
        ss << kind << ": Expected terms of the same floating-point type.";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::STORE:
      if (!children[0].get_type().is_array())
      {
        ss << kind << ": Expected array term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (children[0].get_type().get_array_index() != children[1].get_type())
      {
        ss << kind << ": Index term does not match index type of array";
        return std::make_pair(false, ss.str());
      }
      if (children[0].get_type().get_array_element() != children[2].get_type())
      {
        ss << kind << ": Element term does not match element type of array";
        return std::make_pair(false, ss.str());
      }
      break;

    case Kind::FP_FMA:
      if (!children[0].get_type().is_rm())
      {
        ss << kind << ": Expected rounding mode term at position 0";
        return std::make_pair(false, ss.str());
      }
      if (!children[1].get_type().is_fp())
      {
        ss << kind << ": Expected floating-point term at position 1";
        return std::make_pair(false, ss.str());
      }
      if (children[1].get_type() != children[2].get_type()
          || children[1].get_type() != children[3].get_type())
      {
        ss << kind << ": Expected terms of the same floating-point type.";
        return std::make_pair(false, ss.str());
      }
      break;
  }

  return std::make_pair(true, "");
}

/* --- NodeManager private ------------------------------------------------- */

void
NodeManager::init_id(NodeData* data)
{
  assert(d_node_id_counter < UINT64_MAX);
  assert(data != nullptr);
  assert(data->d_id == 0);
  d_node_data.emplace_back(data);
  assert(d_node_data.size() == static_cast<size_t>(d_node_id_counter));
  data->d_id = d_node_id_counter++;
}

NodeData*
NodeManager::new_data(Kind kind,
                      const std::vector<Node>& children,
                      const std::vector<uint64_t>& indices)
{
  assert(children.size() > 0);

  NodeData* data;
  if (indices.size() > 0)
  {
    data = new NodeDataIndexed(this, kind, children, indices);
  }
  else if (s_node_kind_info[kind].is_nary())
  {
    data = new NodeDataNary(this, kind, children);
  }
  else
  {
    data = new NodeDataChildren(this, kind, children);
  }
  return data;
}

NodeData*
NodeManager::find_or_insert_node(NodeData* lookup)
{
  auto [it, inserted] = d_unique_nodes.insert(lookup);
  if (inserted)
  {
    // Initialize new node
    init_id(lookup);
    return nullptr;
  }
  return *it;
}

void
NodeManager::garbage_collect(NodeData* data)
{
  assert(data->d_refs == 0);
  assert(!d_in_gc_mode);

  d_in_gc_mode = true;

  std::vector<NodeData*> visit{data};

  NodeData* cur;
  do
  {
    cur = visit.back();
    visit.pop_back();

    // Erase node data before we modify children.
    d_unique_nodes.erase(cur);

    for (size_t i = 0, size = cur->get_num_children(); i < size; ++i)
    {
      Node& child = cur->get_child(i);
      auto d      = child.d_data;

      // Manually decrement reference count to not trigger decrement of
      // NodeData reference. This will avoid recursive call to
      // garbage_collect().
      --d->d_refs;
      child.d_data = nullptr;
      if (d->d_refs == 0)
      {
        visit.push_back(d);
      }
    }

    assert(d_node_data[cur->d_id - 1]->d_id == cur->d_id);
    d_node_data[cur->d_id - 1].reset(nullptr);
  } while (!visit.empty());

  d_in_gc_mode = false;
}

}  // namespace bzla::node
#ifndef ALGEBRA_HH
#define ALGEBRA_HH

#include <stdexcept>

/**
 * Algebra<T> - Algebraic Signature Interface
 * ============================================
 * 
 * INTRODUCTION
 * ------------
 * This framework implements concepts from Universal Algebra and Category Theory,
 * specifically the notion of algebraic signatures and their interpretations.
 * 
 * What is an Algebra?
 * -------------------
 * In mathematics, an algebra consists of:
 * 1. A carrier set (the type T in our template)
 * 2. Operations on that set
 * 3. Equations/axioms these operations satisfy
 * 
 * Examples from everyday mathematics:
 * - (ℝ, +, ×, 0, 1) - real numbers with addition and multiplication
 * - (Strings, concat, "") - strings with concatenation
 * - (Trees, node-construction) - expression trees
 * 
 * THEORETICAL FOUNDATION
 * ----------------------
 * Technically, this class represents what mathematicians call a "signature" (Σ):
 * the syntactic specification of available operations and their arities.
 * 
 * A signature Σ consists of:
 * - Function symbols with their arities
 * - Constants (0-ary functions)
 * 
 * Our signature:
 * Σ = { num: → T,           // constant (nullary)
 *       abs: T → T,         // unary operation
 *       add: T × T → T,     // binary operation
 *       sub: T × T → T,
 *       mul: T × T → T,
 *       div: T × T → T,
 *       mod: T × T → T }
 * 
 * The concrete implementations (DoubleAlgebra, TreeAlgebra, etc.) are the
 * actual Σ-algebras that provide semantic interpretations of this signature.
 * 
 * KEY ARCHITECTURAL INSIGHT
 * -------------------------
 * We use the name "Algebra" following common programming conventions, but
 * understand it as "the signature that algebras must implement". Each concrete
 * class provides an interpretation [[·]]: Σ → Operations on T.
 * 
 * TWO FUNDAMENTAL TYPES OF ALGEBRAS
 * ----------------------------------
 * 
 * 1. INITIAL/SYNTACTIC ALGEBRAS (see InitialAlgebra.hh)
 *    - Build structure without computing
 *    - Example: TreeAlgebra creates expression trees
 *    - Represents the "syntax" or "form" of expressions
 *    - Can represent infinite structures via recursion
 * 
 * 2. SEMANTIC ALGEBRAS (see SemanticAlgebra.hh)
 *    - Provide computational interpretation
 *    - Example: DoubleAlgebra computes numerical values
 *    - Represents the "meaning" or "semantics"
 *    - Must handle fixpoints for recursive definitions
 * 
 * THE FUNDAMENTAL THEOREM
 * -----------------------
 * For any signature Σ, there exists an initial Σ-algebra I (TreeAlgebra in our case)
 * such that for any other Σ-algebra A, there exists a UNIQUE homomorphism h: I → A.
 * 
 * This means: Every syntactic expression (tree) has exactly one interpretation
 * in any semantic algebra!
 * 
 * Example:
 *   Tree: Add(Num(2), Num(3))
 *   DoubleAlgebra interpretation: 5.0
 *   StringAlgebra interpretation: "2 + 3"
 *   IntervalAlgebra interpretation: [5, 5]
 * 
 * REFERENCES
 * ----------
 * - Goguen, J.A., Thatcher, J.W., Wagner, E.G., Wright, J.B. (1977)
 *   "Initial Algebra Semantics and Continuous Algebras"
 *   Journal of the ACM, 24(1), pp. 68-95
 *   [Foundational paper establishing initial algebra semantics]
 * 
 * - Birkhoff, G. (1935) "On the Structure of Abstract Algebras"
 *   Proceedings of the Cambridge Philosophical Society, 31(4), pp. 433-454
 *   [Classic paper on universal algebra]
 * 
 * - Mac Lane, S. (1971) "Categories for the Working Mathematician"
 *   Graduate Texts in Mathematics, Springer-Verlag
 *   [Category-theoretic perspective on algebras]
 * 
 * @tparam T The carrier set of the algebra (e.g., double, Tree, Interval)
 */
template <typename T> 
class Algebra {
public:
  /**
   * Operation Classification
   * ------------------------
   * Operations are classified by their arity (number of arguments).
   * This follows standard universal algebra terminology.
   */
  
  /**
   * Nullary Operations (Constants)
   * 
   * These are 0-ary operations that produce elements of T without input.
   * In algebra terminology, these are the "distinguished elements" or "constants".
   * 
   * The enum serves as a type-safe marker to distinguish different uses
   * of primitive types (e.g., Real vs Integer) in our variant-based Tree structure.
   */
  enum class ConstantOp {
    Real = 0,    // Real number constants
    Integer = 1, // Integer constants (reserved for future use)
    COUNT        // Marker for array sizing
  };

  /**
   * Unary Operations
   * 
   * Operations of arity 1: T → T
   * These transform a single element of the carrier set.
   */
  enum class UnaryOp {
    Abs = 0,     // Absolute value: |x|
    // Future extensions: Neg, Sin, Cos, Exp, Log, etc.
    COUNT
  };

  /**
   * Binary Operations
   * 
   * Operations of arity 2: T × T → T
   * These combine two elements to produce a third.
   * 
   * Note: The order reflects standard mathematical precedence conventions.
   */
  enum class BinaryOp {
    Add = 0,     // Addition: x + y
    Sub = 1,     // Subtraction: x - y
    Mul = 2,     // Multiplication: x × y
    Div = 3,     // Division: x ÷ y
    Mod = 4,     // Modulo: x mod y
    // Future extensions: Pow, Min, Max, etc.
    COUNT
  };

protected:
  /**
   * Dynamic Dispatch Tables
   * ------------------------
   * These tables enable generic treatment of operations through the
   * unary() and binary() methods, implementing a form of the
   * Interpreter pattern.
   * 
   * This design allows TreeAlgebra to evaluate trees without knowing
   * the specific operation types at compile time.
   */
  using UnaryMethod = T (Algebra<T>::*)(const T &) const;
  using BinaryMethod = T (Algebra<T>::*)(const T &, const T &) const;

  UnaryMethod fUnaryOps[static_cast<int>(UnaryOp::COUNT)];
  BinaryMethod fBinaryOps[static_cast<int>(BinaryOp::COUNT)];

  /**
   * Constructor - Initialize Dispatch Tables
   * 
   * Sets up the mapping from operation enums to method pointers.
   * This enables the generic unary() and binary() methods to
   * dispatch to the appropriate concrete implementation.
   */
  Algebra() {
    // Initialize unary operations table
    fUnaryOps[static_cast<int>(UnaryOp::Abs)] = &Algebra<T>::abs;

    // Initialize binary operations table
    fBinaryOps[static_cast<int>(BinaryOp::Add)] = &Algebra<T>::add;
    fBinaryOps[static_cast<int>(BinaryOp::Sub)] = &Algebra<T>::sub;
    fBinaryOps[static_cast<int>(BinaryOp::Mul)] = &Algebra<T>::mul;
    fBinaryOps[static_cast<int>(BinaryOp::Div)] = &Algebra<T>::div;
    fBinaryOps[static_cast<int>(BinaryOp::Mod)] = &Algebra<T>::mod;
  }

public:
  virtual ~Algebra() = default;

  /**
   * Generic Operation Dispatchers
   * ------------------------------
   * These methods provide a uniform interface for applying operations,
   * enabling TreeAlgebra to evaluate trees without compile-time knowledge
   * of specific operations.
   * 
   * This implements the mathematical concept of "evaluation morphism"
   * in a type-safe way.
   */
  
  /**
   * Apply a unary operation
   * @param op The operation to apply
   * @param a The operand
   * @return The result of op(a) in this algebra
   */
  T unary(UnaryOp op, const T &a) const {
    return (this->*fUnaryOps[static_cast<int>(op)])(a);
  }

  /**
   * Apply a binary operation
   * @param op The operation to apply
   * @param a The left operand
   * @param b The right operand
   * @return The result of op(a, b) in this algebra
   */
  T binary(BinaryOp op, const T &a, const T &b) const {
    return (this->*fBinaryOps[static_cast<int>(op)])(a, b);
  }

  /**
   * Abstract Signature Methods
   * ---------------------------
   * These pure virtual methods define the signature Σ that all
   * concrete algebras must implement. Each concrete algebra provides
   * its own interpretation of these operations.
   * 
   * Mathematical notation:
   * For each concrete algebra A with carrier set T:
   * [[num]]: ℝ → T
   * [[add]]: T × T → T
   * etc.
   */
  
  /**
   * Interpret a numeric constant
   * 
   * This is the unique morphism from ℝ into the algebra's carrier set.
   * Different algebras interpret numbers differently:
   * - DoubleAlgebra: identity function
   * - TreeAlgebra: creates a Num node
   * - IntervalAlgebra: creates a point interval
   * - StringAlgebra: converts to string representation
   */
  virtual T num(double value) const = 0;
  
  // Binary operations - the core of our algebraic structure
  virtual T add(const T &a, const T &b) const = 0;
  virtual T sub(const T &a, const T &b) const = 0;
  virtual T mul(const T &a, const T &b) const = 0;
  virtual T div(const T &a, const T &b) const = 0;
  virtual T mod(const T &a, const T &b) const = 0;
  
  // Unary operations
  virtual T abs(const T &a) const = 0;

};

#endif
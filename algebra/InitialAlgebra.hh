#ifndef INITIAL_ALGEBRA_HH
#define INITIAL_ALGEBRA_HH

#include "Algebra.hh"

/**
 * InitialAlgebra<T> - Syntactic Algebra Interface
 * ===============================================
 * 
 * MATHEMATICAL FOUNDATION
 * -----------------------
 * In Universal Algebra and Category Theory, an initial algebra is the unique
 * algebra that has a homomorphism to every other algebra of the same signature.
 * It represents the "free" or "syntactic" structure without any equations or
 * semantic interpretation.
 * 
 * FORMAL DEFINITION
 * -----------------
 * Given a signature Σ, the initial Σ-algebra I = (T_Σ, {f_I}_{f∈Σ}) satisfies:
 * 
 * ∀ A = (A, {f_A}_{f∈Σ}) ∃! h: T_Σ → A such that:
 *   h(f_I(t₁,...,tₙ)) = f_A(h(t₁),...,h(tₙ))
 * 
 * This means every term in the initial algebra can be uniquely interpreted
 * in any other algebra through a homomorphism.
 * 
 * KEY PROPERTIES OF INITIAL ALGEBRAS
 * ----------------------------------
 * 
 * 1. **Term Structure**: Elements are syntactic terms (abstract syntax trees)
 * 2. **No Equations**: Operations only build structure, never compute
 * 3. **Infinite Capacity**: Can represent arbitrarily complex expressions
 * 4. **Structural Recursion**: Natural recursion through term structure
 * 5. **Compositionality**: Meaning of compound terms depends only on parts
 * 
 * CONCRETE EXAMPLE: TreeAlgebra
 * -----------------------------
 * Our TreeAlgebra is the initial algebra for our signature:
 * - Carrier set: Tree nodes (variants of operations and values)
 * - Operations: Build tree nodes without evaluation
 * - Variables: Special tree nodes that can be bound to definitions
 * 
 * Examples:
 *   add(num(2), num(3)) → Add(Num(2), Num(3))  [tree structure]
 *   mul(var(), num(5)) → Mul(Var(x), Num(5))   [with variable]
 * 
 * VARIABLES AND RECURSION
 * -----------------------
 * Initial algebras naturally support recursive definitions through variables:
 * 
 * Mathematical perspective:
 * - Variables are "holes" in terms that can be filled
 * - define(x, expr) creates a recursive binding x := expr
 * - This enables μ-recursion: μx.F(x) where F is a term with variable x
 * 
 * Example recursive definition:
 *   let x = var()
 *   define(x, add(x, num(1)))  // x := x + 1 (infinite growth)
 * 
 * RELATIONSHIP TO SEMANTIC ALGEBRAS
 * ---------------------------------
 * Initial algebras work in concert with semantic algebras:
 * 
 * 1. **Separation of Concerns**:
 *    - Initial: "What is the structure?" (syntax)
 *    - Semantic: "What does it mean?" (semantics)
 * 
 * 2. **Evaluation Process**:
 *    TreeAlgebra.eval(tree, semanticAlgebra) implements the unique
 *    homomorphism h: Trees → SemanticValues
 * 
 * 3. **Compositionality**:
 *    eval(Add(t₁, t₂), A) = A.add(eval(t₁, A), eval(t₂, A))
 * 
 * THEORETICAL SIGNIFICANCE
 * ------------------------
 * Initial algebras provide the mathematical foundation for:
 * - Abstract Syntax Trees in compilers
 * - Symbolic computation systems
 * - Program transformation and optimization
 * - Denotational semantics of programming languages
 * 
 * The initiality property guarantees that every syntactic expression
 * has a unique meaning in any semantic domain, establishing a principled
 * foundation for language design and implementation.
 * 
 * REFERENCES
 * ----------
 * - Goguen, J.A., Thatcher, J.W., Wagner, E.G., Wright, J.B. (1977)
 *   "Initial Algebra Semantics and Continuous Algebras"
 *   [Foundational paper establishing the theory]
 * 
 * - Manes, E.G., Arbib, M.A. (1986)
 *   "Algebraic Approaches to Program Semantics"
 *   Graduate Texts in Computer Science, Springer-Verlag
 *   [Comprehensive treatment of algebraic semantics]
 * 
 * - Pierce, B.C. (2002) "Types and Programming Languages"
 *   MIT Press, Chapter 3: "Untyped Arithmetic Expressions"
 *   [Practical application to language design]
 * 
 * @tparam T The carrier set of terms (typically Tree or similar AST structure)
 */
template<typename T>
class InitialAlgebra : public Algebra<T> {
public:
    /**
     * Variable Operations
     * -------------------
     * Variables are special elements that enable recursive definitions
     * and serve as "placeholders" in syntactic terms.
     * 
     * In categorical terms, variables correspond to the coproduct injection
     * from the set of variable names into the term algebra.
     */
    enum class VarOp {
        Index = 0   // Marker for variable indices in term structure
    };

    /**
     * Variable Creation - Fundamental Operation
     * -----------------------------------------
     * Creates a fresh variable that can be used in term construction.
     * 
     * Mathematical significance:
     * This operation implements the "unit" of the free monad structure,
     * injecting variable names into the term algebra.
     * 
     * @return A fresh variable term that can be bound to definitions
     */
    virtual T var() const = 0;
    
    /**
     * Variable Definition - Recursive Binding
     * ---------------------------------------
     * Associates a definition to a variable, enabling recursive constructs.
     * 
     * Mathematical interpretation:
     * define(x, e) creates the recursive equation x = e, which corresponds
     * to the least fixed point μx.e when evaluated in semantic algebras.
     * 
     * This operation is crucial for expressing:
     * - Recursive functions
     * - Iterative computations
     * - Fixed-point definitions
     * 
     * Example:
     *   auto x = var();
     *   define(x, add(x, num(1)));  // x := x + 1 (divergent sequence)
     * 
     * @param var The variable to bind (must be created by var())
     * @param def The defining expression (may contain the variable itself)
     * @return The variable with its definition attached
     */
    virtual T define(const T& var, const T& def) const = 0;
};

#endif
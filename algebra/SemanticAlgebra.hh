#ifndef SEMANTIC_ALGEBRA_HH
#define SEMANTIC_ALGEBRA_HH

#include "Algebra.hh"

/**
 * SemanticAlgebra<T> - Computational Algebra Interface
 * ====================================================
 * 
 * MATHEMATICAL FOUNDATION
 * -----------------------
 * Semantic algebras provide computational interpretations of the algebraic
 * signature, implementing the "meaning" or "semantics" of syntactic expressions.
 * They form the mathematical foundation for program evaluation and fixpoint
 * computation in recursive systems.
 * 
 * FORMAL STRUCTURE
 * ----------------
 * A semantic algebra A = (D, {f_A}_{f∈Σ}, ⊥, ⊑) consists of:
 * 
 * 1. **Domain D**: The carrier set (ℝ, Intervals, etc.)
 * 2. **Operations {f_A}**: Computational interpretations of signature operations
 * 3. **Bottom Element ⊥**: Least element representing "undefined" or "uncomputed"
 * 4. **Partial Order ⊑**: Semantic approximation order for fixpoint theory
 * 
 * DOMAIN THEORY AND FIXPOINTS
 * ---------------------------
 * Semantic algebras are built on Domain Theory principles:
 * 
 * **Complete Partial Orders (CPOs)**:
 * - Every chain x₀ ⊑ x₁ ⊑ x₂ ⊑ ... has a least upper bound ⋃ᵢ xᵢ
 * - Enables convergence analysis for recursive computations
 * 
 * **Kleene's Fixed Point Theorem**:
 * For continuous function F: D → D:
 *   fix(F) = ⋃ₙ₌₀^∞ Fⁿ(⊥) = F⁰(⊥) ⊔ F¹(⊥) ⊔ F²(⊥) ⊔ ...
 * 
 * **Practical Application**:
 * - Start with bottom element: x₀ = ⊥
 * - Iterate: xₙ₊₁ = F(xₙ)
 * - Stop when isConverged(xₙ, xₙ₊₁) returns true
 * 
 * CONCRETE EXAMPLES
 * -----------------
 * 
 * **DoubleAlgebra**: 
 * - Domain: ℝ ∪ {⊥}
 * - Operations: Standard arithmetic
 * - Bottom: Special "undefined" value
 * - Convergence: |xₙ₊₁ - xₙ| < ε
 * 
 * **IntervalAlgebra**:
 * - Domain: Intervals [a,b] ⊆ ℝ ∪ {∅}
 * - Operations: Interval arithmetic
 * - Bottom: Large interval [-∞, +∞] (practically [-M, M])
 * - Convergence: Width and position tolerance
 * 
 * **StringAlgebra** (hypothetically):
 * - Domain: Strings with partial computations
 * - Operations: String concatenation/manipulation
 * - Bottom: Empty or "..." placeholder
 * - Convergence: String equality or length stabilization
 * 
 * CONVERGENCE AND APPROXIMATION
 * -----------------------------
 * The isConverged() method implements semantic-specific convergence criteria:
 * 
 * **Mathematical Significance**:
 * - Determines when fixpoint iteration can terminate
 * - Handles imprecision in floating-point arithmetic
 * - Enables "good enough" approximations for practical computation
 * 
 * **Domain-Specific Strategies**:
 * - Numeric: Absolute/relative error tolerance
 * - Intervals: Width-based and endpoint-based criteria  
 * - Symbolic: Structural equality or simplification rules
 * 
 * RELATIONSHIP TO INITIAL ALGEBRAS
 * --------------------------------
 * Semantic algebras complete the syntax-semantics correspondence:
 * 
 * **Evaluation Homomorphism**:
 *   eval: TreeAlgebra → SemanticAlgebra
 *   eval(op(t₁,...,tₙ)) = op_semantic(eval(t₁),...,eval(tₙ))
 * 
 * **Recursive Evaluation**:
 *   For x := F(x), compute fix(λy.eval(F(var→y), semanticAlg))
 * 
 * **Compositionality**: 
 *   Meaning of compound expressions determined by meanings of parts
 * 
 * BOTTOM ELEMENT - THEORETICAL FOUNDATION
 * ---------------------------------------
 * The bottom element ⊥ serves multiple crucial roles:
 * 
 * 1. **Least Fixed Point**: Starting point for Kleene iteration
 * 2. **Undefined Values**: Represents non-terminating or error states  
 * 3. **Safety**: Ensures iteration always has a starting point
 * 4. **Approximation**: Represents "no information" state
 * 
 * **Design Principle**: 
 * bottom() should be the least informative element that still enables 
 * convergent iteration. Too restrictive → slow convergence.
 * Too general → non-convergence.
 * 
 * APPLICATIONS IN COMPUTER SCIENCE
 * --------------------------------
 * Semantic algebras with fixpoints underlie:
 * 
 * - **Program Analysis**: Data flow analysis, type inference
 * - **Compiler Optimization**: Constant propagation, dead code elimination
 * - **Verification**: Model checking, abstract interpretation
 * - **Scientific Computing**: Iterative solvers, convergence analysis
 * - **Functional Programming**: Lazy evaluation, recursive definitions
 * 
 * REFERENCES
 * ----------
 * - Scott, D.S. (1976) "Data Types as Lattices"
 *   SIAM Journal on Computing, 5(3), pp. 522-587
 *   [Foundation of domain theory and semantic domains]
 * 
 * - Stoy, J.E. (1977) "Denotational Semantics: The Scott-Strachey Approach"
 *   MIT Press
 *   [Comprehensive treatment of semantic algebras in programming languages]
 * 
 * - Plotkin, G.D. (1981) "A Structural Approach to Operational Semantics"
 *   Technical Report DAIMI FN-19, Aarhus University
 *   [Structural operational semantics and its relation to denotational semantics]
 * 
 * - Cousot, P., Cousot, R. (1977) "Abstract Interpretation: A Unified Lattice 
 *   Model for Static Analysis of Programs by Construction or Approximation of Fixpoints"
 *   POPL '77, pp. 238-252
 *   [Abstract interpretation framework for program analysis]
 * 
 * @tparam T The semantic domain (double, Interval, etc.)
 */
template<typename T>
class SemanticAlgebra : public Algebra<T> {
public:
    /**
     * Bottom Element - Foundation of Fixpoint Theory
     * ----------------------------------------------
     * Returns the least element ⊥ of the semantic domain, which serves as:
     * 
     * - **Starting point** for fixpoint iteration: x₀ = ⊥
     * - **Least approximation** representing "no information"
     * - **Safety element** ensuring iteration convergence
     * 
     * Mathematical properties:
     * - ∀x ∈ D: ⊥ ⊑ x (least element property)
     * - F(⊥) ⊒ ⊥ for monotonic F (ensures progress)
     * 
     * Implementation strategy:
     * Choose ⊥ to balance generality (covers all possible values) with
     * convergence speed (not too wide to slow iteration).
     * 
     * Examples:
     * - DoubleAlgebra: Special "undefined" marker or 0.0
     * - IntervalAlgebra: Finite interval [-M, +M] for some large M
     * - BooleanAlgebra: false (if using Boolean approximation order)
     * 
     * @return The bottom element of the semantic domain
     */
    virtual T bottom() const = 0;
    
    /**
     * Convergence Test - Semantic Approximation
     * -----------------------------------------
     * Determines when two successive iterations are "close enough" to
     * terminate fixpoint computation. This implements domain-specific
     * approximation criteria essential for practical computation.
     * 
     * Mathematical foundation:
     * - Replaces exact equality x = y with approximation x ≈ᵋ y
     * - Handles floating-point imprecision and infinite processes
     * - Enables "good enough" solutions for engineering applications
     * 
     * Convergence strategies by domain:
     * 
     * **Numeric Domains (ℝ)**:
     * - Absolute: |current - prev| < ε
     * - Relative: |current - prev| / |prev| < ε  
     * - Mixed: min(absolute, relative) approaches
     * 
     * **Interval Domains**:
     * - Width: width(current) < ε (precision-based)
     * - Distance: distance(current, prev) < ε (stability-based)
     * - Hausdorff: max(distance endpoints) < ε (geometric)
     * 
     * **Discrete Domains**:
     * - Exact equality for finite computations
     * - Stabilization detection for eventually constant sequences
     * 
     * Usage in fixpoint iteration:
     * ```cpp
     * T x = bottom();
     * do {
     *     T next = F(x);
     *     if (isConverged(x, next)) return next;
     *     x = next;
     * } while (iterations < maxIter);
     * ```
     * 
     * @param prev Previous value in the iteration sequence
     * @param current Current value in the iteration sequence
     * @return true if values are sufficiently close for termination
     */
    virtual bool isConverged(const T& prev, const T& current) const = 0;
};

#endif
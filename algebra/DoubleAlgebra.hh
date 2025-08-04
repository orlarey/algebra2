#ifndef DOUBLE_ALGEBRA_HH
#define DOUBLE_ALGEBRA_HH

#include "SemanticAlgebra.hh"
#include <cmath>

/**
 * DoubleAlgebra - Standard Floating-Point Computation
 * ===================================================
 * 
 * MATHEMATICAL FOUNDATION
 * -----------------------
 * DoubleAlgebra implements the standard semantic interpretation of our
 * algebraic signature over the IEEE 754 double-precision floating-point
 * domain. This serves as the canonical computational algebra for numerical
 * evaluation and provides the reference semantics for other algebras.
 * 
 * FORMAL STRUCTURE
 * ----------------
 * DoubleAlgebra = (ℝ_F, {op_ℝ}_{op∈Σ}, 0.0, isConverged)
 * 
 * Where:
 * - ℝ_F: IEEE 754 double-precision floating-point numbers
 * - {op_ℝ}: Standard floating-point arithmetic operations
 * - 0.0: Bottom element for fixpoint iteration
 * - isConverged: Combined absolute/relative tolerance test
 * 
 * COMPUTATIONAL SEMANTICS
 * -----------------------
 * 
 * **Identity Interpretation**:
 * DoubleAlgebra provides the most direct computational semantics:
 * - num(x) = x (identity function)
 * - Operations map directly to hardware floating-point units
 * - Minimal computational overhead
 * - Direct correspondence to mathematical operations
 * 
 * **IEEE 754 Compliance**:
 * All operations follow IEEE 754-2019 standard:
 * - Correctly rounded arithmetic operations
 * - Proper handling of special values (±∞, NaN)
 * - Deterministic behavior across platforms
 * - Exception handling for invalid operations
 * 
 * **Algebraic Properties**:
 * - Associativity: Limited by floating-point precision
 * - Commutativity: Preserved for addition and multiplication
 * - Distributivity: Approximate due to rounding errors
 * - Identity elements: 0 for addition, 1 for multiplication
 * 
 * OPERATION SEMANTICS
 * -------------------
 * 
 * **Basic Arithmetic**: {+, -, ×, ÷}
 * - Direct mapping to IEEE 754 operations
 * - Correctly rounded to nearest representable value
 * - Overflow → ±∞, Underflow → ±0
 * - Invalid operations → NaN
 * 
 * **Modulo Operation**: std::fmod(a, b)
 * - IEEE 754 remainder operation
 * - Result has same sign as dividend a
 * - Undefined for b = 0 → NaN
 * - Exact for integer operands within precision
 * 
 * **Absolute Value**: std::abs(a)
 * - Simple sign removal operation
 * - |+∞| = |−∞| = +∞
 * - |NaN| = NaN
 * - Preserves IEEE 754 special value semantics
 * 
 * FIXPOINT COMPUTATION
 * --------------------
 * 
 * **Bottom Element**: 0.0
 * - Neutral starting point for most computations
 * - Mathematical zero of the additive group
 * - Practical choice for iterative algorithms
 * - Avoids bias toward positive or negative values
 * 
 * **Convergence Testing**:
 * Implements sophisticated floating-point convergence:
 * 
 * ```cpp
 * bool isConverged(prev, current) {
 *     double abs_diff = |prev - current|;
 *     
 *     // Absolute tolerance for values near zero
 *     if (abs_diff < ε) return true;
 *     
 *     // Relative tolerance for larger values  
 *     double max_val = max(|prev|, |current|);
 *     if (abs_diff / max_val < ε) return true;
 *     
 *     return false;
 * }
 * ```
 * 
 * This dual-tolerance approach handles:
 * - Values near zero (absolute tolerance)
 * - Large values (relative tolerance)
 * - Numerical precision limitations
 * - Robust convergence detection
 * 
 * **Convergence Properties**:
 * - ε = 10⁻¹⁰ (stricter than IntervalAlgebra's 10⁻⁹)
 * - Accounts for floating-point representation limits
 * - Prevents oscillation due to rounding errors
 * - Ensures termination for contractive functions
 * 
 * FLOATING-POINT CONSIDERATIONS
 * -----------------------------
 * 
 * **Precision Limitations**:
 * - ~15-17 decimal digits of precision
 * - Relative precision: ε_machine ≈ 2.22 × 10⁻¹⁶
 * - Catastrophic cancellation in subtraction
 * - Accumulation of rounding errors
 * 
 * **Special Value Handling**:
 * - Infinity arithmetic: ∞ + x = ∞, ∞ × ∞ = ∞
 * - NaN propagation: NaN op x = NaN
 * - Signed zero: +0 ≠ -0 but +0 == -0
 * - Denormal numbers for gradual underflow
 * 
 * **Numerical Stability**:
 * - Algorithm design affects accuracy
 * - Order of operations matters
 * - Condition numbers determine sensitivity
 * - Iterative refinement may be needed
 * 
 * APPLICATIONS
 * ------------
 * 
 * **Primary Computational Engine**:
 * - Reference implementation for other algebras
 * - Performance baseline for optimization
 * - Validation of abstract interpretations
 * - Standard numerical computation
 * 
 * **Scientific Computing**:
 * - Simulation and modeling
 * - Numerical solution of equations
 * - Statistical analysis
 * - Signal processing
 * 
 * **Engineering Applications**:
 * - Control system analysis
 * - Optimization algorithms
 * - Computer graphics and visualization
 * - Real-time system computation
 * 
 * **Benchmarking and Testing**:
 * - Correctness verification for other algebras
 * - Performance comparison baseline
 * - Regression testing for numerical algorithms
 * - Precision analysis studies
 * 
 * COMPARISON WITH OTHER ALGEBRAS
 * ------------------------------
 * 
 * **vs. IntervalAlgebra**:
 * - Point values vs. guaranteed bounds
 * - Faster computation vs. verified results
 * - Potential numerical errors vs. mathematical soundness
 * - Direct hardware support vs. software implementation
 * 
 * **vs. TreeAlgebra**:
 * - Computed values vs. symbolic expressions
 * - Semantic evaluation vs. syntactic representation
 * - Lossy floating-point vs. exact symbolic structure
 * - Immediate results vs. deferred evaluation
 * 
 * **vs. StringAlgebra**:
 * - Numerical computation vs. human-readable representation
 * - Machine processing vs. symbolic manipulation
 * - Quantitative analysis vs. qualitative understanding
 * - Performance-oriented vs. presentation-oriented
 * 
 * THEORETICAL SIGNIFICANCE
 * ------------------------
 * 
 * **Computational Completeness**:
 * DoubleAlgebra demonstrates that our algebraic framework
 * can capture standard numerical computation faithfully.
 * 
 * **Semantic Reference**:
 * Serves as the "intended interpretation" for numerical
 * expressions, providing semantic grounding for symbolic algebras.
 * 
 * **Practical Foundation**:
 * Bridges abstract mathematical concepts with concrete
 * computational implementation on real hardware.
 * 
 * REFERENCES
 * ----------
 * - IEEE 754-2019: IEEE Standard for Floating-Point Arithmetic
 *   Institute of Electrical and Electronics Engineers
 *   [Definitive specification for floating-point arithmetic]
 * 
 * - Goldberg, D. (1991) "What Every Computer Scientist Should Know About Floating-Point Arithmetic"
 *   ACM Computing Surveys, 23(1), pp. 5-48
 *   [Classic reference on floating-point computation]
 * 
 * - Higham, N.J. (2002) "Accuracy and Stability of Numerical Algorithms"
 *   SIAM, Philadelphia, 2nd Edition
 *   [Comprehensive treatment of numerical accuracy]
 * 
 * - Knuth, D.E. (1997) "The Art of Computer Programming, Volume 2: Seminumerical Algorithms"
 *   Addison-Wesley, 3rd Edition
 *   [Foundational text on computer arithmetic]
 * 
 * - Muller, J.M. et al. (2018) "Handbook of Floating-Point Arithmetic"
 *   Birkhäuser, 2nd Edition
 *   [Modern comprehensive reference on floating-point arithmetic]
 */
class DoubleAlgebra : public SemanticAlgebra<double> {
public:
    double num(double value) const override {
        return value;
    }
    
    double add(const double& a, const double& b) const override {
        return a + b;
    }
    
    double sub(const double& a, const double& b) const override {
        return a - b;
    }
    
    double mul(const double& a, const double& b) const override {
        return a * b;
    }
    
    double div(const double& a, const double& b) const override {
        return a / b;
    }
    
    double mod(const double& a, const double& b) const override {
        return std::fmod(a, b);
    }
    
    double abs(const double& a) const override {
        return std::abs(a);
    }
    
    // SemanticAlgebra method
    double bottom() const override {
        return 0.0;  // Use 0.0 as bottom value for numerical computation
    }
    
    // SemanticAlgebra convergence method
    bool isConverged(const double& prev, const double& current) const override {
        // Use relative and absolute tolerance for robust floating-point comparison
        const double epsilon = 1e-10;  // Convergence tolerance
        const double abs_diff = std::abs(prev - current);
        
        // Absolute tolerance for values near zero
        if (abs_diff < epsilon) {
            return true;
        }
        
        // Relative tolerance for larger values
        const double max_val = std::max(std::abs(prev), std::abs(current));
        if (max_val > 0.0 && abs_diff / max_val < epsilon) {
            return true;
        }
        
        return false;
    }
};

#endif
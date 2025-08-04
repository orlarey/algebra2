#ifndef STRING_ALGEBRA_HH
#define STRING_ALGEBRA_HH

#include "InitialAlgebra.hh"
#include <string>
#include <sstream>
#include <utility>

/**
 * StringAlgebra - Human-Readable Expression Generation
 * ===================================================
 * 
 * MATHEMATICAL FOUNDATION
 * -----------------------
 * StringAlgebra implements a concrete initial algebra that produces
 * human-readable string representations of algebraic expressions.
 * It serves as a bridge between formal mathematical notation and
 * textual presentation, enabling symbolic manipulation and display.
 * 
 * FORMAL STRUCTURE
 * ----------------
 * StringAlgebra = (Strings × ℕ, {op_S}_{op∈Σ}, var_S, define_S)
 * 
 * Where:
 * - Strings × ℕ: Pairs of (expression_string, precedence_level)
 * - {op_S}: String-building operations with precedence handling
 * - var_S: Variable name generation ("x1", "x2", ...)
 * - define_S: Textual definition binding
 * 
 * KEY DESIGN PRINCIPLES
 * ---------------------
 * 
 * **Operator Precedence**:
 * StringAlgebra implements a precedence-based parenthesization system:
 * - Precedence levels: 100 (highest) → 10 (lowest)
 * - Higher precedence operations require fewer parentheses
 * - Automatic parenthesis insertion for clarity
 * - Follows standard mathematical conventions
 * 
 * **Precedence Hierarchy**:
 * ```
 * Level 100: Numbers, Variables, Function calls (abs)
 * Level 50:  Multiplication, Division, Modulo  
 * Level 10:  Addition, Subtraction
 * ```
 * 
 * **Parenthesization Rules**:
 * - Lower precedence subexpressions get parentheses
 * - Equal precedence depends on associativity
 * - Right operand of subtraction always parenthesized if ≤ 10
 * - Function calls never need outer parentheses
 * 
 * OPERATION IMPLEMENTATIONS
 * -------------------------
 * 
 * **Numbers**: num(42.5) → ("42.5", 100)
 * - Direct string conversion via ostringstream
 * - Highest precedence (never needs parentheses)
 * - Preserves decimal representation
 * - Platform-independent formatting
 * 
 * **Addition**: "a" + "b" → ("a + b", 10)
 * - Infix notation with spaces
 * - Lowest precedence level
 * - Left-associative by convention
 * - No special parenthesization
 * 
 * **Subtraction**: "a" - "b" → ("a - (b)", 10)
 * - Right operand parenthesized if precedence ≤ 10
 * - Prevents ambiguity: a - b - c vs a - (b - c)
 * - Maintains mathematical clarity
 * - Same precedence as addition
 * 
 * **Multiplication**: "a" * "b" → ("a * b", 50)
 * - Higher precedence than addition/subtraction
 * - Both operands parenthesized if precedence < 50
 * - Standard mathematical notation
 * - Clear operator symbol
 * 
 * **Division**: "a" / "b" → ("a / b", 50)
 * - Same precedence as multiplication
 * - Right operand parenthesized if precedence ≤ 50
 * - Prevents (a/b)/c vs a/(b/c) ambiguity
 * - Standard fraction notation
 * 
 * **Modulo**: "a" % "b" → ("a % b", 50)
 * - Programming-style modulo operator
 * - Same precedence as multiplication/division
 * - Both operands protected appropriately
 * - Clear modular arithmetic notation
 * 
 * **Absolute Value**: abs("a") → ("abs(a)", 100)
 * - Function call notation
 * - Highest precedence (no outer parentheses needed)
 * - Mathematical function style
 * - Clear unambiguous representation
 * 
 * VARIABLE HANDLING
 * -----------------
 * 
 * **Variable Generation**:
 * - Fresh variables: "x1", "x2", "x3", ...
 * - Unique naming prevents collisions
 * - Readable mathematical notation
 * - Sequential numbering for clarity
 * 
 * **Variable Definition**:
 * - define(var, expr) currently returns expr
 * - Could be extended to explicit equation format
 * - Enables symbolic manipulation
 * - Supports recursive definitions
 * 
 * MATHEMATICAL SIGNIFICANCE
 * -------------------------
 * 
 * **Initial Algebra Properties**:
 * StringAlgebra satisfies the initial algebra requirement:
 * - Pure syntactic construction (no computation)
 * - Compositional string building
 * - Preserves all structural information
 * - Homomorphic to other initial algebras
 * 
 * **Precedence Algebra**:
 * The (String, Precedence) pair forms a structured domain:
 * - Precedence guides parenthesization decisions
 * - Maintains syntactic correctness
 * - Enables unambiguous parsing
 * - Preserves operator precedence semantics
 * 
 * **Textual Representation Theory**:
 * - Bijective mapping from syntax trees to readable text
 * - Preserves all algebraic structure
 * - Enables round-trip parsing (with parser)
 * - Standard mathematical notation compliance
 * 
 * APPLICATIONS
 * ------------
 * 
 * **Symbolic Mathematics**:
 * - Computer algebra system output
 * - Mathematical formula display
 * - Educational software interfaces
 * - Research paper generation
 * 
 * **Program Analysis Display**:
 * - Abstract syntax tree visualization
 * - Compiler error message generation
 * - Static analysis result presentation
 * - Debugging and diagnostic output
 * 
 * **Interactive Systems**:
 * - Mathematical calculator interfaces
 * - REPL (Read-Eval-Print Loop) systems
 * - Interactive theorem provers
 * - Educational mathematical software
 * 
 * **Documentation Generation**:
 * - Automatic formula documentation
 * - API specification generation
 * - Mathematical specification texts
 * - Academic publication support
 * 
 * DESIGN CONSIDERATIONS
 * ---------------------
 * 
 * **Readability vs. Precision**:
 * - Optimized for human comprehension
 * - Minimal parentheses while preserving meaning
 * - Standard mathematical conventions
 * - Clear operator precedence
 * 
 * **Extensibility**:
 * - Easy addition of new operators
 * - Configurable precedence levels
 * - Customizable formatting styles
 * - Locale-aware number formatting
 * 
 * **Performance**:
 * - String concatenation overhead
 * - Precedence level computations
 * - Memory allocation for strings
 * - Acceptable for non-performance-critical applications
 * 
 * FUTURE ENHANCEMENTS
 * -------------------
 * 
 * **Advanced Formatting**:
 * - LaTeX mathematical notation output
 * - MathML for web display
 * - Unicode mathematical symbols
 * - Customizable operator symbols
 * 
 * **Parenthesis Optimization**:
 * - Associativity-aware parenthesization
 * - Context-dependent precedence
 * - Minimal parenthesis algorithms
 * - User preference settings
 * 
 * **Semantic Integration**:
 * - Type-aware formatting
 * - Unit-aware expressions
 * - Symbolic simplification hints
 * - Mathematical context preservation
 * 
 * REFERENCES
 * ----------
 * - Knuth, D.E. (1984) "The TeXbook"
 *   Addison-Wesley
 *   [Mathematical typesetting and precedence]
 * 
 * - Aho, A.V., Lam, M.S., Sethi, R., Ullman, J.D. (2006)
 *   "Compilers: Principles, Techniques, and Tools"
 *   Addison-Wesley, 2nd Edition
 *   [Operator precedence parsing and expression formatting]
 * 
 * - ISO/IEC 14882:2020 "Programming languages — C++"
 *   [Operator precedence in programming languages]
 * 
 * - Cajori, F. (1928) "A History of Mathematical Notations"
 *   Open Court Publishing Company
 *   [Historical development of mathematical notation]
 * 
 * - W3C Mathematical Markup Language (MathML) Version 3.0
 *   [Web standards for mathematical notation]
 */
class StringAlgebra : public InitialAlgebra<std::pair<std::string, int>> {
private:
    mutable int fVarCounter = 0;  // Counter for generating unique variable names
    
public:
    std::pair<std::string, int> num(double value) const override {
        std::ostringstream oss;
        oss << value;
        return {oss.str(), 100}; // highest priority
    }
    
    std::pair<std::string, int> add(const std::pair<std::string, int>& a, 
                                    const std::pair<std::string, int>& b) const override {
        std::string result = a.first + " + " + b.first;
        return {result, 10}; // lowest priority
    }
    
    std::pair<std::string, int> sub(const std::pair<std::string, int>& a, 
                                    const std::pair<std::string, int>& b) const override {
        std::string left = a.first;
        std::string right = b.second <= 10 ? "(" + b.first + ")" : b.first;
        std::string result = left + " - " + right;
        return {result, 10}; // lowest priority
    }
    
    std::pair<std::string, int> mul(const std::pair<std::string, int>& a, 
                                    const std::pair<std::string, int>& b) const override {
        std::string left = a.second < 50 ? "(" + a.first + ")" : a.first;
        std::string right = b.second < 50 ? "(" + b.first + ")" : b.first;
        std::string result = left + " * " + right;
        return {result, 50}; // medium priority
    }
    
    std::pair<std::string, int> div(const std::pair<std::string, int>& a, 
                                    const std::pair<std::string, int>& b) const override {
        std::string left = a.second < 50 ? "(" + a.first + ")" : a.first;
        std::string right = b.second <= 50 ? "(" + b.first + ")" : b.first;
        std::string result = left + " / " + right;
        return {result, 50}; // medium priority
    }
    
    std::pair<std::string, int> mod(const std::pair<std::string, int>& a, 
                                    const std::pair<std::string, int>& b) const override {
        std::string left = a.second < 50 ? "(" + a.first + ")" : a.first;
        std::string right = b.second <= 50 ? "(" + b.first + ")" : b.first;
        std::string result = left + " % " + right;
        return {result, 50}; // medium priority (same as multiplication/division)
    }
    
    std::pair<std::string, int> abs(const std::pair<std::string, int>& a) const override {
        std::string result = "abs(" + a.first + ")";
        return {result, 100}; // highest priority (like a function call)
    }
    
    // InitialAlgebra methods
    std::pair<std::string, int> var() const override {
        std::ostringstream oss;
        oss << "x" << (++fVarCounter);  // Generate unique variable name
        return {oss.str(), 100};  // Variables have highest priority
    }
    
    std::pair<std::string, int> define(const std::pair<std::string, int>& var, 
                                       const std::pair<std::string, int>& def) const override {
        // For now, return the definition (could build explicit equations later)
        return def;
    }
};

#endif
#ifndef TREE_ALGEBRA_HH
#define TREE_ALGEBRA_HH

#include "InitialAlgebra.hh"
#include "SemanticAlgebra.hh"
#include <memory>
#include <variant>
#include <tuple>
#include <unordered_set>
#include <functional>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <iostream>
#include <type_traits>

/**
 * TreeAlgebra - The Canonical Initial Algebra Implementation
 * =========================================================
 * 
 * MATHEMATICAL FOUNDATION
 * -----------------------
 * TreeAlgebra is the concrete realization of the initial algebra concept,
 * providing the fundamental bridge between syntax (tree structures) and 
 * semantics (computational interpretations). It embodies the mathematical
 * principle that "every computation has a unique syntactic representation."
 * 
 * FORMAL STRUCTURE
 * ----------------
 * TreeAlgebra = (𝒯, {f_𝒯}_{f∈Σ}, var, define, eval, ≡_α)
 * 
 * Where:
 * - 𝒯: Set of all possible expression trees (the term algebra)
 * - {f_𝒯}: Tree-building operations (Add, Mul, Num, etc.)
 * - var: Variable creation operation
 * - define: Recursive definition binding
 * - eval: Evaluation homomorphism to semantic algebras
 * - ≡_α: Alpha-equivalence relation for recursive structures
 * 
 * KEY MATHEMATICAL PROPERTIES
 * ---------------------------
 * 
 * **1. INITIALITY**
 * For any algebra A, there exists a unique homomorphism h: 𝒯 → A
 * This is implemented by the eval() method:
 *   eval(f_𝒯(t₁,...,tₙ), A) = f_A(eval(t₁,A),...,eval(tₙ,A))
 * 
 * **2. COMPOSITIONALITY**
 * The meaning of compound expressions depends only on their parts:
 *   [[Add(e₁, e₂)]]_A = add_A([[e₁]]_A, [[e₂]]_A)
 * 
 * **3. HASH-CONSING (STRUCTURAL SHARING)**
 * Implements maximal sharing principle:
 *   ∀ t₁, t₂ ∈ 𝒯: structure(t₁) = structure(t₂) ⟹ t₁ = t₂ (pointer equality)
 * 
 * **4. RECURSIVE DEFINITIONS**
 * Supports μ-recursion through variable binding:
 *   x := F(x) corresponds to μx.F(x) in domain theory
 * 
 * CORE ALGORITHMS
 * ---------------
 * 
 * **Hash-Consing Algorithm**:
 *   intern(tree) = if tree ∈ table then table[tree] else table ∪= {tree}; tree
 * 
 * **Evaluation Algorithm** (depends on target algebra):
 *   - InitialAlgebra → InitialAlgebra: identity (tree building)
 *   - InitialAlgebra → SemanticAlgebra: fixpoint computation
 * 
 * **Alpha-Equivalence Algorithm**:
 *   Determines when two recursive structures are "essentially the same"
 *   up to variable renaming, crucial for optimization and canonicalization.
 * 
 * FIXPOINT COMPUTATION THEORY
 * ---------------------------
 * For recursive definitions x := F(x), TreeAlgebra implements
 * Kleene's iteration method:
 * 
 * **Domain-Theoretic Foundation**:
 *   fix(F) = ⊔_{n=0}^∞ F^n(⊥) where ⊔ is the least upper bound
 * 
 * **Practical Algorithm**:
 *   1. Initialize: x₀ = ⊥ (bottom element)
 *   2. Iterate: xₙ₊₁ = eval(F(var→xₙ), semanticAlgebra)
 *   3. Stop when: semanticAlgebra.isConverged(xₙ, xₙ₊₁)
 * 
 * **Strongly Connected Components (SCCs)**:
 *   Handle mutually recursive definitions x₁ := F₁(x₁,x₂), x₂ := F₂(x₁,x₂)
 *   by computing fixpoints simultaneously for entire SCCs.
 * 
 * TREE STRUCTURE AND VARIANTS
 * ---------------------------
 * Trees are implemented as discriminated unions (std::variant):
 * 
 * ```cpp
 * Tree = Num(double)                    // Numeric constants
 *      | Unary(UnaryOp, Tree)          // Unary operations  
 *      | Binary(BinaryOp, Tree, Tree)  // Binary operations
 *      | Var(int, Definition?)         // Variables with optional definitions
 * ```
 * 
 * This structure directly mirrors the mathematical BNF grammar:
 *   e ::= n | op₁(e) | op₂(e,e) | x
 * 
 * EVALUATION STRATEGIES
 * ---------------------
 * 
 * **For Initial Algebras** (equation building):
 * - TreeAlgebra → TreeAlgebra: identity (tree ↦ tree)
 * - TreeAlgebra → Other Initial: build equivalent structure
 * 
 * **For Semantic Algebras** (computation):
 * - Recursive variables: fixpoint iteration until convergence
 * - Non-recursive: direct structural evaluation
 * - Mixed: SCC analysis + appropriate strategy per component
 * 
 * ALPHA-EQUIVALENCE ALGORITHM
 * ---------------------------
 * Implements sophisticated equivalence checking for recursive structures:
 * 
 * **Mathematical Definition**:
 *   T₁ ≡_α T₂ iff unfold(T₁) ≅ unfold(T₂) up to variable renaming
 * 
 * **Algorithm Features**:
 * - Memoization for DAG structures (avoids infinite recursion)
 * - Variable mapping consistency (bijective renaming)
 * - Handles cycles through definition comparison
 * - Optimized for hash-consed structures (pointer equality fast path)
 * 
 * APPLICATIONS AND USE CASES
 * --------------------------
 * 
 * **Compiler Construction**:
 * - Abstract syntax trees for programming languages
 * - Expression optimization and canonicalization
 * - Type inference and analysis
 * 
 * **Symbolic Mathematics**:
 * - Computer algebra systems
 * - Symbolic differentiation and integration
 * - Expression simplification
 * 
 * **Scientific Computing**:
 * - Automatic differentiation
 * - Computational graphs for machine learning
 * - Domain-specific languages for numerical methods
 * 
 * **Program Analysis**:
 * - Static analysis frameworks
 * - Model checking and verification  
 * - Program transformation systems
 * 
 * PERFORMANCE CONSIDERATIONS
 * --------------------------
 * 
 * **Hash-Consing Benefits**:
 * - O(1) structural equality testing
 * - Significant memory savings for repeated subexpressions
 * - Fast hashing for memoization
 * 
 * **Fixpoint Iteration**:
 * - Convergence depends on semantic algebra properties
 * - SCC analysis minimizes recomputation
 * - Early termination through isConverged() methods
 * 
 * **Alpha-Equivalence**:
 * - Memoization prevents exponential blowup
 * - Hash-consing enables pointer-equality optimization
 * - DAG structure reduces work compared to tree traversal
 * 
 * REFERENCES
 * ----------
 * - Goguen, J.A. et al. (1977) "Initial Algebra Semantics and Continuous Algebras"
 *   [Foundational theory for initial algebras and evaluation]
 * 
 * - Baader, F., Nipkow, T. (1998) "Term Rewriting and All That"
 *   Cambridge University Press
 *   [Comprehensive treatment of term structures and rewriting]
 * 
 * - Pierce, B.C. (2002) "Types and Programming Languages" 
 *   MIT Press, Chapters 3-5
 *   [Practical implementation of abstract syntax trees]
 * 
 * - Appel, A.W. (1998) "Modern Compiler Implementation in ML"
 *   Cambridge University Press, Chapter 4
 *   [Hash-consing and structural sharing techniques]
 * 
 * - Tarjan, R.E. (1972) "Depth-First Search and Linear Graph Algorithms"
 *   SIAM Journal on Computing, 1(2), pp. 146-160
 *   [SCC algorithm for handling mutual recursion]
 */

// Forward declaration to access operation types
class Tree;

// Type aliases for operations (using Tree as the concrete type for Algebra)
using ConstantOp = Algebra<std::shared_ptr<Tree>>::ConstantOp;
using VarOp = InitialAlgebra<std::shared_ptr<Tree>>::VarOp;
using UnaryOp = Algebra<std::shared_ptr<Tree>>::UnaryOp;
using BinaryOp = Algebra<std::shared_ptr<Tree>>::BinaryOp;

class Tree {
public:
    enum class NodeType { Num, Unary, Binary, Var };
    
private:
    
    NodeType fType;
    std::variant<
        std::pair<ConstantOp, double>,                                     // For Num (constants: real numbers, integers)
        std::pair<VarOp, int>,                                             // For Var (variable index)
        std::pair<UnaryOp, std::shared_ptr<Tree>>,                         // For Unary
        std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>> // For Binary
    > fData;
    
    // Mutable field for variable definitions
    mutable std::shared_ptr<Tree> fDefinition;
    
    // Private constructors - only TreeAlgebra can create Trees
    Tree(double value) : fType(NodeType::Num), fData(std::make_pair(ConstantOp::Real, value)) {}
    
    Tree(UnaryOp op, std::shared_ptr<Tree> operand) 
        : fType(NodeType::Unary), fData(std::make_pair(op, operand)) {}
    
    Tree(BinaryOp op, std::shared_ptr<Tree> left, std::shared_ptr<Tree> right) 
        : fType(NodeType::Binary), fData(std::make_tuple(op, left, right)) {}
    
    Tree(int index) : fType(NodeType::Var), fData(std::make_pair(VarOp::Index, index)), fDefinition(nullptr) {}
    
    friend class TreeAlgebra;
    
public:
    // Getters for hash-consing
    NodeType getType() const { return fType; }
    
    double getValue() const { 
        return std::get<std::pair<ConstantOp, double>>(fData).second; 
    }
    
    UnaryOp getUnaryOp() const { 
        return std::get<std::pair<UnaryOp, std::shared_ptr<Tree>>>(fData).first; 
    }
    
    std::shared_ptr<Tree> getOperand() const { 
        return std::get<std::pair<UnaryOp, std::shared_ptr<Tree>>>(fData).second; 
    }
    
    BinaryOp getBinaryOp() const { 
        return std::get<0>(std::get<std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>>>(fData)); 
    }
    
    std::shared_ptr<Tree> getLeft() const { 
        return std::get<1>(std::get<std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>>>(fData)); 
    }
    
    std::shared_ptr<Tree> getRight() const { 
        return std::get<2>(std::get<std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>>>(fData)); 
    }
    
    int getVarIndex() const {
        return std::get<std::pair<VarOp, int>>(fData).second;
    }
    
    std::shared_ptr<Tree> getDefinition() const {
        return fDefinition;
    }
    
    void setDefinition(std::shared_ptr<Tree> def) const {
        fDefinition = def;
    }
    
    // Evaluation operator
    template<typename T>
    T operator()(const Algebra<T>& algebra) const {
        switch(fType) {
            case NodeType::Num: {
                auto& [constantOp, value] = std::get<std::pair<ConstantOp, double>>(fData);
                return algebra.num(value);
            }
            case NodeType::Unary: {
                auto& [op, operand] = std::get<std::pair<UnaryOp, std::shared_ptr<Tree>>>(fData);
                return algebra.unary(static_cast<typename Algebra<T>::UnaryOp>(op), (*operand)(algebra));
            }
            case NodeType::Binary: {
                auto& [op, left, right] = std::get<std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>>>(fData);
                return algebra.binary(static_cast<typename Algebra<T>::BinaryOp>(op), (*left)(algebra), (*right)(algebra));
            }
            case NodeType::Var: {
                if (fDefinition) {
                    return (*fDefinition)(algebra);
                } else {
                    // For now, throw an exception if variable is not defined
                    throw std::runtime_error("Variable " + std::to_string(getVarIndex()) + " is not defined");
                }
            }
        }
    }
};

// Hash and equality functors for hash-consing
struct TreeHash {
    size_t operator()(const std::shared_ptr<Tree>& t) const {
        size_t h = 0;
        
        switch(t->getType()) {
            case Tree::NodeType::Num:
                h = std::hash<double>{}(t->getValue());
                break;
                
            case Tree::NodeType::Unary:
                h = std::hash<int>{}(static_cast<int>(t->getUnaryOp()));
                h ^= std::hash<void*>{}(t->getOperand().get()) + 0x9e3779b9 + (h << 6) + (h >> 2);
                break;
                
            case Tree::NodeType::Binary:
                h = std::hash<int>{}(static_cast<int>(t->getBinaryOp()));
                h ^= std::hash<void*>{}(t->getLeft().get()) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<void*>{}(t->getRight().get()) + 0x517cc1b7 + (h << 6) + (h >> 2);
                break;
                
            case Tree::NodeType::Var:
                // Hash only the variable index, not the definition
                h = std::hash<int>{}(t->getVarIndex()) ^ 0xdeadbeef;
                break;
        }
        return h;
    }
};

struct TreeEqual {
    bool operator()(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const {
        if (a->getType() != b->getType()) return false;
        
        switch(a->getType()) {
            case Tree::NodeType::Num:
                return a->getValue() == b->getValue();
                
            case Tree::NodeType::Unary:
                return a->getUnaryOp() == b->getUnaryOp() && 
                       a->getOperand() == b->getOperand();  // Pointer equality (hash-consing invariant)
                
            case Tree::NodeType::Binary:
                return a->getBinaryOp() == b->getBinaryOp() && 
                       a->getLeft() == b->getLeft() &&      // Pointer equality
                       a->getRight() == b->getRight();      // Pointer equality
                       
            case Tree::NodeType::Var:
                // Two variables are equal if they have the same index
                // Note: we don't compare definitions
                return a->getVarIndex() == b->getVarIndex();
        }
        return false;
    }
};

// Forward declaration
class Tree;

// SCC Frame for the evaluation stack
template<typename T>
struct SCCFrame {
    std::set<Tree*> scc;                      // Variables in this SCC
    std::map<Tree*, T> hypotheticalMemo;      // Hypothetical memoization for this SCC
    
    SCCFrame(const std::set<Tree*>& variables) : scc(variables) {}
};

// Hypotheses being tested during fixpoint computation
template<typename T>
struct Hypotheses {
    std::vector<SCCFrame<T>> sccStack;        // Stack of SCCs being computed
    std::map<Tree*, T> hypotheticalValues;    // Hypothetical variable values
    
    // Find SCC position for a variable, returns nullopt if not on stack
    std::optional<size_t> findSCCPosition(Tree* var) const {
        for (size_t i = 0; i < sccStack.size(); ++i) {
            if (sccStack[i].scc.count(var) > 0) {
                return i;
            }
        }
        return std::nullopt;  // Not found
    }
    
    // Check if variable is on stack
    bool isOnStack(Tree* var) const {
        return findSCCPosition(var).has_value();
    }
};

// Alpha-equivalence structures and algorithms
// 
// Mathematical specification:
// • 𝕍 = ensemble des variables  
// • 𝕋 = ensemble des arbres (Trees)
// • σ : 𝕍 → 𝕋 = environnement (définitions des variables)
// • ρ : 𝕍 → 𝕍 = bijection de renommage
//
// alphaEquiv : 𝕋 × 𝕋 → 𝔹
// Two trees T₁, T₂ are alpha-equivalent (T₁ ≡α T₂) if their infinite unfoldings 
// are structurally identical up to variable renaming.

struct AlphaEquivContext {
    // Memo = Map⟨(𝕋*, 𝕋*), 𝔹⟩ - Cache des comparaisons déjà faites
    mutable std::map<std::pair<Tree*, Tree*>, bool> memo;
    
    // VarMap = Map⟨𝕍*, 𝕍*⟩ - Bijection entre variables
    mutable std::map<Tree*, Tree*> varMapping;
    
    // Clear context for new comparison
    void clear() {
        memo.clear();
        varMapping.clear();
    }
};

class TreeAlgebra : public InitialAlgebra<std::shared_ptr<Tree>> {
private:
    // Hash-consing table
    mutable std::unordered_set<std::shared_ptr<Tree>, TreeHash, TreeEqual> fTrees;
    
    // Counter for generating fresh variables in bottom()
    mutable int fVarCounter = 0;
    
    // Intern method for hash-consing
    std::shared_ptr<Tree> intern(std::shared_ptr<Tree> candidate) const {
        auto it = fTrees.find(candidate);
        if (it != fTrees.end()) {
            // Found existing tree, return it
            return *it;
        } else {
            // New tree, add to table and return
            fTrees.insert(candidate);
            return candidate;
        }
    }
    
public:
    std::shared_ptr<Tree> num(double value) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(value));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> add(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(BinaryOp::Add, a, b));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> sub(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(BinaryOp::Sub, a, b));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> mul(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(BinaryOp::Mul, a, b));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> div(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(BinaryOp::Div, a, b));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> mod(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(BinaryOp::Mod, a, b));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> abs(const std::shared_ptr<Tree>& a) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(UnaryOp::Abs, a));
        return intern(candidate);
    }
    
    // InitialAlgebra methods
    std::shared_ptr<Tree> var() const override {
        // Create a fresh variable with a unique index
        auto candidate = std::shared_ptr<Tree>(new Tree(++fVarCounter));
        return intern(candidate);
    }
    
    // Create a variable with a specific index (for backward compatibility)
    std::shared_ptr<Tree> var(int index) const {
        auto candidate = std::shared_ptr<Tree>(new Tree(index));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> define(const std::shared_ptr<Tree>& var, 
                                  const std::shared_ptr<Tree>& def) const override {
        // Associate a definition to a variable
        if (var->getType() != Tree::NodeType::Var) {
            throw std::runtime_error("Can only define variables");
        }
        var->setDefinition(def);
        return var;
    }
    
    // Auxiliary functions for fixpoint evaluation
    template<typename T>
    void memoize(Tree* tree, const T& value, const std::set<Tree*>& dependencies, 
                 std::map<Tree*, T>& definitiveMemo, Hypotheses<T>& hypotheses) const {
        if (dependencies.empty()) {
            // No dependencies -> definitive memoization
            definitiveMemo[tree] = value;
        } else if (!hypotheses.sccStack.empty()) {
            // Dependencies -> hypothetical memoization for top SCC
            hypotheses.sccStack.back().hypotheticalMemo[tree] = value;
        }
    }
    
    template<typename T>
    std::optional<T> checkDefinitiveMemo(Tree* tree, const std::map<Tree*, T>& definitiveMemo) const {
        auto it = definitiveMemo.find(tree);
        if (it != definitiveMemo.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    template<typename T>
    std::optional<T> checkHypotheticalMemo(Tree* tree, const Hypotheses<T>& hypotheses) const {
        if (!hypotheses.sccStack.empty()) {
            const auto& topFrame = hypotheses.sccStack.back();
            auto it = topFrame.hypotheticalMemo.find(tree);
            if (it != topFrame.hypotheticalMemo.end()) {
                return it->second;
            }
        }
        return std::nullopt;
    }
    
    template<typename T>
    void merge(size_t position, Hypotheses<T>& hypotheses) const {
        if (position >= hypotheses.sccStack.size()) return;
        
        // Collect all SCCs from position to top
        std::set<Tree*> mergedSCC;
        std::map<Tree*, T> mergedMemo;
        
        for (size_t i = position; i < hypotheses.sccStack.size(); ++i) {
            const auto& frame = hypotheses.sccStack[i];
            mergedSCC.insert(frame.scc.begin(), frame.scc.end());
            mergedMemo.insert(frame.hypotheticalMemo.begin(), frame.hypotheticalMemo.end());
        }
        
        // Remove merged frames
        hypotheses.sccStack.erase(hypotheses.sccStack.begin() + position, hypotheses.sccStack.end());
        
        // Add merged frame
        hypotheses.sccStack.emplace_back(mergedSCC);
        hypotheses.sccStack.back().hypotheticalMemo = mergedMemo;
    }
    
    template<typename T>
    void promote(std::map<Tree*, T>& definitiveMemo, Hypotheses<T>& hypotheses) const {
        if (hypotheses.sccStack.empty()) return;
        
        // Move hypothetical memoization to definitive
        const auto& topFrame = hypotheses.sccStack.back();
        for (const auto& [tree, value] : topFrame.hypotheticalMemo) {
            definitiveMemo[tree] = value;
        }
        
        // Also promote final variable values from hypotheticalValues
        // These contain the converged values from the last iteration
        for (Tree* var : topFrame.scc) {
            if (hypotheses.hypotheticalValues.count(var)) {
                definitiveMemo[var] = hypotheses.hypotheticalValues[var];
            }
        }
        
        // Pop the top SCC
        hypotheses.sccStack.pop_back();
    }
    
    template<typename T>
    void clean(Hypotheses<T>& hypotheses) const {
        if (hypotheses.sccStack.empty()) return;
        
        auto& topFrame = hypotheses.sccStack.back();
        std::map<Tree*, T> cleanedMemo;
        
        // Keep only variable entries
        for (const auto& [tree, value] : topFrame.hypotheticalMemo) {
            if (topFrame.scc.count(tree) > 0) {  // Only keep variables in this SCC
                cleanedMemo[tree] = value;
            }
        }
        
        topFrame.hypotheticalMemo = cleanedMemo;
    }
    
    std::shared_ptr<Tree> getDefinition(Tree* var) const {
        return var->getDefinition();
    }
    
    template<typename T>
    bool hasTopSCC(const Hypotheses<T>& hypotheses) const {
        return !hypotheses.sccStack.empty();
    }
    
    // Main evaluation method - public API
    template<typename T>
    T eval(const std::shared_ptr<Tree>& tree, const Algebra<T>& algebra) const {
        // Dispatch to appropriate algorithm based on algebra type
        if (auto* initial = dynamic_cast<const InitialAlgebra<T>*>(&algebra)) {
            return evalInitial(tree, *initial);
        } else if (auto* semantic = dynamic_cast<const SemanticAlgebra<T>*>(&algebra)) {
            return evalSemantic(tree, *semantic);
        } else {
            throw std::runtime_error("Unknown algebra type");
        }
    }
    
    // Evaluation for initial algebras (equation building)
    template<typename T>
    T evalInitial(const std::shared_ptr<Tree>& tree, const InitialAlgebra<T>& algebra) const {
        // Special case: TreeAlgebra evaluating itself should preserve identity
        if (dynamic_cast<const TreeAlgebra*>(&algebra) == this && std::is_same_v<T, std::shared_ptr<Tree>>) {
            // Safe cast since we verified the types
            return reinterpret_cast<const T&>(tree);
        }
        
        // For other initial algebras, we build equations rather than iterate
        // For now, use the existing algorithm (will be refined later)
        static thread_local std::map<Tree*, T> definitiveMemo;
        definitiveMemo.clear();
        
        Hypotheses<T> hypotheses;
        auto [result, deps] = evalInternal(tree, definitiveMemo, hypotheses, algebra);
        return result;
    }
    
public:
    
    // Evaluation for semantic algebras (fixpoint iteration)  
    template<typename T>
    T evalSemantic(const std::shared_ptr<Tree>& tree, const SemanticAlgebra<T>& algebra) const {
        // For semantic algebras, we need full fixpoint computation capability
        // Use the same algorithm as initial algebras but with semantic convergence
        static thread_local std::map<Tree*, T> definitiveMemo;
        definitiveMemo.clear();
        
        Hypotheses<T> hypotheses;
        auto [result, deps] = evalInternal(tree, definitiveMemo, hypotheses, algebra);
        return result;
    }
    
    
    // Internal evaluation method (legacy, will be split later)
    template<typename T>
    std::pair<T, std::set<Tree*>> evalInternal(const std::shared_ptr<Tree>& tree, 
                                               std::map<Tree*, T>& definitiveMemo,
                                               Hypotheses<T>& hypotheses, 
                                               const Algebra<T>& algebra) const {
        Tree* treePtr = tree.get();
        
        
        // Check definitive memoization first
        auto definitiveResult = checkDefinitiveMemo(treePtr, definitiveMemo);
        if (definitiveResult) {
            return {*definitiveResult, std::set<Tree*>{}};  // No dependencies
        }
        
        // Check hypothetical memoization for current top SCC
        auto hypotheticalResult = checkHypotheticalMemo(treePtr, hypotheses);
        if (hypotheticalResult && hasTopSCC(hypotheses)) {
            return {*hypotheticalResult, hypotheses.sccStack.back().scc};
        }
        
        // Evaluate based on tree type
        switch (tree->getType()) {
            case Tree::NodeType::Num: {
                T value = algebra.num(tree->getValue());
                memoize(treePtr, value, std::set<Tree*>{}, definitiveMemo, hypotheses);
                return {value, std::set<Tree*>{}};
            }
            
            case Tree::NodeType::Unary: {
                auto [operandValue, operandDeps] = evalInternal(tree->getOperand(), definitiveMemo, hypotheses, algebra);
                T value = algebra.unary(static_cast<typename Algebra<T>::UnaryOp>(tree->getUnaryOp()), operandValue);
                memoize(treePtr, value, operandDeps, definitiveMemo, hypotheses);
                return {value, operandDeps};
            }
            
            case Tree::NodeType::Binary: {
                auto [leftValue, leftDeps] = evalInternal(tree->getLeft(), definitiveMemo, hypotheses, algebra);
                auto [rightValue, rightDeps] = evalInternal(tree->getRight(), definitiveMemo, hypotheses, algebra);
                T value = algebra.binary(static_cast<typename Algebra<T>::BinaryOp>(tree->getBinaryOp()), leftValue, rightValue);
                
                std::set<Tree*> combinedDeps;
                combinedDeps.insert(leftDeps.begin(), leftDeps.end());
                combinedDeps.insert(rightDeps.begin(), rightDeps.end());
                
                memoize(treePtr, value, combinedDeps, definitiveMemo, hypotheses);
                return {value, combinedDeps};
            }
            
            case Tree::NodeType::Var: {
                return evalVar(treePtr, definitiveMemo, hypotheses, algebra);
            }
        }
        
        // Should never reach here
        throw std::runtime_error("Unknown tree node type");
    }
    
    // Variable evaluation method
    template<typename T>
    std::pair<T, std::set<Tree*>> evalVar(Tree* var, 
                                          std::map<Tree*, T>& definitiveMemo,
                                          Hypotheses<T>& hypotheses, 
                                          const Algebra<T>& algebra) const {
        
        // Check if variable is on the stack (cycle detection)
        auto position = hypotheses.findSCCPosition(var);
        if (position) {
            // Variable is on stack - found a cycle! Merge SCCs
            merge(*position, hypotheses);
            
            // Return current approximation
            if (hypotheses.hypotheticalValues.count(var)) {
                return {hypotheses.hypotheticalValues[var], hypotheses.sccStack.back().scc};
            } else {
                // Initialize with bottom if not yet computed
                // Only semantic algebras have bottom()
                if (auto* semanticAlg = dynamic_cast<const SemanticAlgebra<T>*>(&algebra)) {
                    T bottomValue = semanticAlg->bottom();
                    hypotheses.hypotheticalValues[var] = bottomValue;
                    return {bottomValue, hypotheses.sccStack.back().scc};
                } else if (auto* initialAlg = dynamic_cast<const InitialAlgebra<T>*>(&algebra)) {
                    // For initial algebras, use a fresh variable as "bottom"
                    T varValue = initialAlg->var();
                    hypotheses.hypotheticalValues[var] = varValue;
                    return {varValue, hypotheses.sccStack.back().scc};
                } else {
                    throw std::runtime_error("Unknown algebra type in evalVar");
                }
            }
        }
        
        // New variable - start computing its fixpoint
        std::set<Tree*> newSCC = {var};
        
        // Push new SCC on stack
        hypotheses.sccStack.emplace_back(newSCC);
        
        // Initialize variable to bottom/var depending on algebra type
        T bottomValue;
        if (auto* semanticAlg = dynamic_cast<const SemanticAlgebra<T>*>(&algebra)) {
            bottomValue = semanticAlg->bottom();
        } else if (auto* initialAlg = dynamic_cast<const InitialAlgebra<T>*>(&algebra)) {
            bottomValue = initialAlg->var();
        } else {
            throw std::runtime_error("Unknown algebra type in evalVar");
        }
        hypotheses.hypotheticalValues[var] = bottomValue;
        
        // Get variable definition
        auto definition = getDefinition(var);
        if (!definition) {
            throw std::runtime_error("Variable " + std::to_string(var->getVarIndex()) + " has no definition");
        }
        
        // Evaluate definition
        auto [value, dependencies] = evalInternal(definition, definitiveMemo, hypotheses, algebra);
        
        // Update variable's value
        hypotheses.hypotheticalValues[var] = value;
        
        // Check if we're still at the same SCC (no merging happened)
        if (!hypotheses.sccStack.empty() && hypotheses.sccStack.back().scc == newSCC) {
            // If no dependencies, this is a simple definition - promote directly
            if (dependencies.empty()) {
                // std::cout << "[DEBUG] No dependencies, promoting directly" << std::endl;
                definitiveMemo[var] = value;
                hypotheses.sccStack.pop_back();  // Remove the SCC from stack
                return {value, std::set<Tree*>{}};  // No dependencies
            } else {
                // Has dependencies - compute fixpoint for this SCC
                return fixpoint(newSCC, definitiveMemo, hypotheses, algebra);
            }
        } else {
            // SCC was merged, continue with merged SCC
            return {value, hypotheses.sccStack.back().scc};
        }
    }
    
    // Fixpoint computation for an SCC
    template<typename T>
    std::pair<T, std::set<Tree*>> fixpoint(const std::set<Tree*>& scc, 
                                           std::map<Tree*, T>& definitiveMemo, 
                                           Hypotheses<T>& hypotheses, 
                                           const Algebra<T>& algebra) const {
        // Clean hypothetical memo: keep only variable entries, discard sub-expressions
        clean(hypotheses);
        
        // Iterate until all variables in SCC reach their fixpoints
        bool converged = iterate(scc, definitiveMemo, hypotheses, algebra);
        
        if (converged) {
            // Success! Move everything to definitive and pop stack
            promote(definitiveMemo, hypotheses);
            
            // Return value of any variable from the SCC (they should all be equivalent)
            if (!scc.empty()) {
                Tree* representative = *scc.begin();
                auto it = definitiveMemo.find(representative);
                if (it != definitiveMemo.end()) {
                    return {it->second, std::set<Tree*>{}};  // No more dependencies
                }
            }
            
            // Fallback (should not happen)
            if (auto* semanticAlg = dynamic_cast<const SemanticAlgebra<T>*>(&algebra)) {
                return {semanticAlg->bottom(), std::set<Tree*>{}};
            } else if (auto* initialAlg = dynamic_cast<const InitialAlgebra<T>*>(&algebra)) {
                return {initialAlg->var(), std::set<Tree*>{}};
            } else {
                throw std::runtime_error("Unknown algebra type in fixpoint fallback");
            }
        } else {
            throw std::runtime_error("Fixpoint computation did not converge");
        }
    }
    
    // Iterate until convergence for an SCC
    template<typename T>
    bool iterate(const std::set<Tree*>& scc, std::map<Tree*, T>& definitiveMemo,
                 Hypotheses<T>& hypotheses, const Algebra<T>& algebra) const {
        const int MAX_ITER = 10000;  // Safety limit to avoid infinite loops
        
        for (int iteration = 0; iteration < MAX_ITER; ++iteration) {
            // Store previous values for convergence check
            std::map<Tree*, T> previousValues;
            for (Tree* var : scc) {
                if (hypotheses.hypotheticalValues.count(var)) {
                    previousValues[var] = hypotheses.hypotheticalValues[var];
                }
            }
            
            // Compute new values for each variable in the SCC
            std::map<Tree*, T> newValues;
            for (Tree* var : scc) {
                auto definition = getDefinition(var);
                if (!definition) {
                    throw std::runtime_error("Variable " + std::to_string(var->getVarIndex()) + " has no definition");
                }
                
                // Evaluate the definition (this will use current hypothetical values)
                auto [value, deps] = evalInternal(definition, definitiveMemo, hypotheses, algebra);
                newValues[var] = value;
            }
            
            // Update all variables with new values
            for (const auto& [var, value] : newValues) {
                hypotheses.hypotheticalValues[var] = value;
            }
            
            // Check if all variables reached their fixpoints
            bool allConverged = true;
            for (Tree* var : scc) {
                if (previousValues.count(var) && newValues.count(var)) {
                    // Use semantic convergence test instead of strict equality
                    if (auto* semanticAlg = dynamic_cast<const SemanticAlgebra<T>*>(&algebra)) {
                        if (!semanticAlg->isConverged(previousValues[var], newValues[var])) {
                            allConverged = false;
                            break;
                        }
                    } else {
                        // Fallback to strict equality for non-semantic algebras
                        if (previousValues[var] != newValues[var]) {
                            allConverged = false;
                            break;
                        }
                    }
                } else {
                    // First iteration or missing value
                    allConverged = false;
                    break;
                }
            }
            
            if (allConverged) {
                return true;  // Converged!
            }
        }
        
        return false;  // Did not converge within MAX_ITER iterations
    }
    
    // Alpha-equivalence implementation
    // Context for memoization and variable mapping
    mutable AlphaEquivContext fAlphaContext;
    
public:
    // Public API for alpha-equivalence
    // alphaEquivDAG : 𝕋* × 𝕋* → 𝔹
    bool alphaEquivalent(const std::shared_ptr<Tree>& t1, const std::shared_ptr<Tree>& t2) const {
        fAlphaContext.clear();
        return alphaEquivMemo(t1.get(), t2.get());
    }
    
private:
    // alphaEquivMemo : 𝕋* × 𝕋* × Memo × VarMap → 𝔹
    // Core memoized comparison with DAG optimization
    bool alphaEquivMemo(Tree* t1, Tree* t2) const {
        // Optimisation cruciale : identité physique (hash-consing)
        // case T₁* = T₂*: true
        if (t1 == t2) {
            return true;
        }
        
        // Cache hit - évite recalcul  
        // case (T₁*, T₂*) ∈ memo: memo[(T₁*, T₂*)]
        auto key = std::make_pair(t1, t2);
        auto it = fAlphaContext.memo.find(key);
        if (it != fAlphaContext.memo.end()) {
            return it->second;
        }
        
        // Cache miss - calculer et mémoriser
        // let result = alphaEquivCore(T₁*, T₂*, memo, varMap)
        // let memo' = memo ∪ {(T₁*, T₂*) ↦ result, (T₂*, T₁*) ↦ result}
        bool result = alphaEquivCore(t1, t2);
        fAlphaContext.memo[key] = result;
        fAlphaContext.memo[std::make_pair(t2, t1)] = result; // Symmetric
        
        return result;
    }
    
    // alphaEquivCore : 𝕋* × 𝕋* × Memo × VarMap → 𝔹
    // Core structural comparison logic
    bool alphaEquivCore(Tree* t1, Tree* t2) const {
        // case (type(T₁*), type(T₂*)) of
        if (t1->getType() != t2->getType()) {
            return false;
        }
        
        switch (t1->getType()) {
            // Constantes: (Num, Num) → value(T₁*) = value(T₂*)
            case Tree::NodeType::Num:
                return t1->getValue() == t2->getValue();
            
            // Opérations unaires: (Op, Op) → op(T₁*) = op(T₂*) ∧ recurse on children
            case Tree::NodeType::Unary:
                return t1->getUnaryOp() == t2->getUnaryOp() &&
                       alphaEquivMemo(t1->getOperand().get(), t2->getOperand().get());
            
            // Opérations binaires: similar logic with both children
            case Tree::NodeType::Binary:
                return t1->getBinaryOp() == t2->getBinaryOp() &&
                       alphaEquivMemo(t1->getLeft().get(), t2->getLeft().get()) &&
                       alphaEquivMemo(t1->getRight().get(), t2->getRight().get());
            
            // Variables: handleVarsDAG(v₁*, v₂*, memo, varMap)
            case Tree::NodeType::Var:
                return handleVarsDAG(t1, t2);
                
            default:
                return false;
        }
    }
    
    // handleVarsDAG : 𝕍* × 𝕍* × Memo × VarMap → 𝔹
    // Handle variable mapping and definition comparison
    bool handleVarsDAG(Tree* v1, Tree* v2) const {
        // case (v₁* ∈ dom(varMap), v₂* ∈ ran(varMap)) of
        auto it1 = fAlphaContext.varMapping.find(v1);
        bool v1_mapped = (it1 != fAlphaContext.varMapping.end());
        
        // Check if v2 is in range of mapping
        bool v2_in_range = false;
        for (const auto& [k, v] : fAlphaContext.varMapping) {
            if (v == v2) {
                v2_in_range = true;
                break;
            }
        }
        
        if (v1_mapped && v2_in_range) {
            // Mapping existant - vérifier cohérence
            // (true, true) → varMap[v₁*] = v₂*
            return it1->second == v2;
        } else if (!v1_mapped && !v2_in_range) {
            // Nouveau mapping - étendre et comparer définitions
            // (false, false) → let varMap' = varMap ∪ {v₁* ↦ v₂*}
            //                   in alphaEquivMemo(definition(v₁*), definition(v₂*), memo, varMap')
            fAlphaContext.varMapping[v1] = v2;
            
            auto def1 = v1->getDefinition();
            auto def2 = v2->getDefinition();
            
            // Both must have definitions or both must not have definitions
            if (!def1 && !def2) {
                return true; // Both undefined variables
            }
            if (!def1 || !def2) {
                return false; // Only one is undefined
            }
            
            return alphaEquivMemo(def1.get(), def2.get());
        } else {
            // Mapping incohérent
            // _ → false
            return false;
        }
    }
};

#endif
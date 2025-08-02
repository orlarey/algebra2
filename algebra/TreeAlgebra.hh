#ifndef TREE_ALGEBRA_HH
#define TREE_ALGEBRA_HH

#include "Algebra.hh"
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

class Tree {
public:
    enum class NodeType { Num, Unary, Binary, Var };
    
private:
    
    NodeType fType;
    std::variant<
        double,                                                              // For Num
        std::pair<UnaryOp, std::shared_ptr<Tree>>,                         // For Unary
        std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>>, // For Binary
        int                                                                  // For Var (index)
    > fData;
    
    // Mutable field for variable definitions
    mutable std::shared_ptr<Tree> fDefinition;
    
    // Private constructors - only TreeAlgebra can create Trees
    Tree(double value) : fType(NodeType::Num), fData(value) {}
    
    Tree(UnaryOp op, std::shared_ptr<Tree> operand) 
        : fType(NodeType::Unary), fData(std::make_pair(op, operand)) {}
    
    Tree(BinaryOp op, std::shared_ptr<Tree> left, std::shared_ptr<Tree> right) 
        : fType(NodeType::Binary), fData(std::make_tuple(op, left, right)) {}
    
    Tree(int index) : fType(NodeType::Var), fData(index), fDefinition(nullptr) {}
    
    friend class TreeAlgebra;
    
public:
    // Getters for hash-consing
    NodeType getType() const { return fType; }
    
    double getValue() const { 
        return std::get<double>(fData); 
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
        return std::get<int>(fData);
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
            case NodeType::Num:
                return algebra.num(std::get<double>(fData));
            case NodeType::Unary: {
                auto& [op, operand] = std::get<std::pair<UnaryOp, std::shared_ptr<Tree>>>(fData);
                return algebra.unary(op, (*operand)(algebra));
            }
            case NodeType::Binary: {
                auto& [op, left, right] = std::get<std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>>>(fData);
                return algebra.binary(op, (*left)(algebra), (*right)(algebra));
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

class TreeAlgebra : public Algebra<std::shared_ptr<Tree>> {
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
    
    std::shared_ptr<Tree> abs(const std::shared_ptr<Tree>& a) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(UnaryOp::Abs, a));
        return intern(candidate);
    }
    
    // Variable creation method
    std::shared_ptr<Tree> var(int index) const {
        auto candidate = std::shared_ptr<Tree>(new Tree(index));
        return intern(candidate);
    }
    
    // Fixpoint computation methods
    std::shared_ptr<Tree> bottom() const override {
        // Create a fresh variable for bottom
        return var(++fVarCounter);
    }
    
    bool isEquivalent(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const override {
        // TODO: Should implement alpha-equivalence testing
        // For now, return true and trust the fixpoint algorithm
        return true;
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
        static thread_local std::map<Tree*, T> definitiveMemo;  // Definitive facts
        // For debugging: clear the cache at the beginning
        // TODO: In production, we may want to keep the cache for performance
        definitiveMemo.clear();
        
        Hypotheses<T> hypotheses;                               // Current hypotheses
        auto [result, deps] = evalInternal(tree, definitiveMemo, hypotheses, algebra);
        
        
        return result;
    }
    
    // Internal evaluation method
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
                T value = algebra.unary(tree->getUnaryOp(), operandValue);
                memoize(treePtr, value, operandDeps, definitiveMemo, hypotheses);
                return {value, operandDeps};
            }
            
            case Tree::NodeType::Binary: {
                auto [leftValue, leftDeps] = evalInternal(tree->getLeft(), definitiveMemo, hypotheses, algebra);
                auto [rightValue, rightDeps] = evalInternal(tree->getRight(), definitiveMemo, hypotheses, algebra);
                T value = algebra.binary(tree->getBinaryOp(), leftValue, rightValue);
                
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
                T bottomValue = algebra.bottom();
                hypotheses.hypotheticalValues[var] = bottomValue;
                return {bottomValue, hypotheses.sccStack.back().scc};
            }
        }
        
        // New variable - start computing its fixpoint
        std::set<Tree*> newSCC = {var};
        
        // Push new SCC on stack
        hypotheses.sccStack.emplace_back(newSCC);
        
        // Initialize variable to bottom
        T bottomValue = algebra.bottom();
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
            return {algebra.bottom(), std::set<Tree*>{}};
        } else {
            throw std::runtime_error("Fixpoint computation did not converge");
        }
    }
    
    // Iterate until convergence for an SCC
    template<typename T>
    bool iterate(const std::set<Tree*>& scc, std::map<Tree*, T>& definitiveMemo,
                 Hypotheses<T>& hypotheses, const Algebra<T>& algebra) const {
        const int MAX_ITER = 100;  // Safety limit to avoid infinite loops
        
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
                    if (!algebra.isEquivalent(previousValues[var], newValues[var])) {
                        allConverged = false;
                        break;
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
};

#endif
#pragma once

#include<string>
#include<list>
#include<functional>

#include"Term.hpp"
#include"SExpression.hpp"

//Forward declare Formula for using defs
struct Formula;

/**
 * @brief A list of formulae
*/
using FormulaList = std::list<Formula*>;

/** 
 * @brief Represents a formulae with a truth value.
 * @details The formula is represented as tree. 
 * it has both a high level ConnectiveType representing its internal
 * representation and an exact Type representing its connective.  
*/
struct Formula{

    //Inner Enums / Structs ====================================================
    /** @name Inner Enums / Structs */
    ///@{
    /**
     * @brief The type of the top level operator or connective
     * in the formation tree.
    */
    enum class Type{
        PRED,       ///< A Predicate or proposition if 0 arguments
        NOT,        ///< UnaryConnective, logical not
        AND,        ///< BinaryConnective, logical and
        OR,         ///< BinaryConnective, logical or
        IF,         ///< BinaryConnective, conditional
        IFF,        ///< BinaryConnective, biconditional
        FORALL,     ///< Quantifier, forall
        EXISTS,     ///< Quantifier, exists
    };

    /** @brief The underlying representation class of the current formula */
    enum class ConnectiveType{
        PRED,       ///< A Predicate or proposition if 0 arguments
        UNARY,      ///< A single formula operand 
        BINARY,     ///< left and right formulae operands
        QUANT,      ///< A named identifier (variable) and a bound formulae
    };

    /** @brief Predicate representation */
    struct Pred{
        std::string name;       ///< the identifier for this predicate.
        TermList args;          ///< the list of arguments to this predicate.

        bool operator==(const Pred& other) const;

        /** 
         * @brief gets A list of pointers to all constants (and possible bound
         * variables) inside the predicate. 
         */
        TermList allConstants() const;

        /** @brief gets A list of pointers to all functions 
         * (and possible function variables) inside the predicate. 
         */
        TermList allFunctions() const;

        /** @return max height of terms inside the predicate. */
        size_t depth() const;

        /** 
         *  @brief Interpret a predicate as a term and apply an arbitrary function to it. 
         *  @details Provides a memory safe context in which the predicate is interpreted as a term. 
         *  This is useful as it allows us to take advantage of the shared structure underlying predicates and terms
         *  @param termContext a function which takes the predicate interpreted as a term and does something with it
         *  output can be obtained via binding outside vars by reference.  
        */
        void applyToAsTerm(std::function<void(Term*)> termContext) const;
    };

    /** @brief Unary Connective formula representation */
    struct UnaryConnective{
        Formula* arg;    ///< formula being bound to this connective
    };

    /** @brief Binary Connective formula representation */
    struct BinaryConnective{
        Formula* left;   ///< left subformula of a binary connective
        Formula* right;  ///< right subformula of a binary connective
    };

    /** @brief Quantifier formula representation */
    struct Quantifier{
        std::string var;  ///< The identifier (name of the variable) this quantifier binds to
        Formula* arg;     ///< The formula being quantified over
    };
    ///@}
    // Internal Representation =========================================================================================
    /** @name Internal Representation */
    ///@{

    //Formula is a tagged union
    Type type;                      ///< The type of the formula
    ConnectiveType connectiveType;  ///< The connective class of the formula
    union{
        Pred* pred;                 ///< Valid iff type == PRED
        UnaryConnective* unary;     ///< Valid iff type == NOT
        BinaryConnective* binary;   ///< Valid iff type == AND, OR, IF, IFF
        Quantifier* quantifier;     ///< Valid iff type == FORALL, EXISTS
    };

    ///@}
    // Methods =========================================================================================
    /** @name Constructors, Destructors, operators */
    ///@{
    
    /** @brief Default constructor, leaves everything uninitialized */
    Formula() = default;

    /** @brief Destructor, frees all subformulae */
    ~Formula();

    /** @brief Returns a pointer to a copy of this formula */
    Formula* copy() const;

    bool operator==(const Formula& other) const;

    ///@}
    /** @name Subformula & Subterm utilities */
    ///@{

    /** 
     * @brief Gets a list of pointers to immediate subformulae in left to right order.
     * @example if the formula is `A /\ (B \/ C)` then `.subformulae()` returns a list of pointers
     * to `A` and `(B \/ C)` respectively.
     * @return A vector of Formula* containing immediate subformulae
    */
    FormulaList subformulae() const;

    /**
     * @brief Gets all subformulae in the entire tree excluding the formulae itself. 
     * The vector returned is guaranteed to have the values in breath first traversal order. 
     * @example if the formula is `A /\ (B \/ ~C)` then `.allSubformulae()` will return
     * pointers to `[A, (B \/ ~C), B, ~C, C]` in that order.
     * @return A list of Formula* of all subformulae encountered in BFS traversal order of the formula tree. 
    */
    FormulaList allSubformulae() const;

    /**
     * @brief Gets all subformulae in the entire tree including the formulae itself. 
     * The vector returned is guaranteed to have the values in breath first traversal order. 
     * @example if the formula is `A /\ (B \/ ~C)` then `.allFormulae()` will return
     * pointers to `[A /\ (B \/ ~C), A, (B \/ ~C), B, ~C, C]` in that order.
     * @return A list of Formula* of all subformulae encountered in BFS traversal order of the formula tree. 
    */
    FormulaList allFormulae() const;

    /**
     * @brief Gets a list of all predicates in the order they appear in the formula via an in-order traversal
     * of the formula tree. 
     * @example if the formula is `A /\ (B(a, d) \/ ~C(d))` then `.allPredicates()` returns a list of formula
     * pointers to `[A, B(a, d), C(d)]` in that order.
     * @return a list of Formula* to propositions in the formula in the order they appear 
    */
    FormulaList allPredicates() const;

    /**
     * @brief Gets a list of pointers to all term level constants (including constant variables) in the formula
     * @example if the formula is `A /\ (B(a, d) \/ ~C(d))` then `.allConstants()` returns a list of terms
     * pointers to `[a, d, d]` in that order.
    */
    TermList allConstants() const;

    /**
     * @brief Gets a list of pointer to all term level functions (including function variables) in the formula
     * @example if the formula is `A /\ (B(a, f(d)) \/ ~C(S(S(a))))` then `.allFunctions()` returns a list of terms
     * pointers to `[f, S, S]` in that order.
    */
    TermList allFunctions() const;

    /**
     * @brief Gets a list of all subformulae that are quantified at the top level
    */
    FormulaList allQuantified() const;

    /**
     * @brief gets the list of all propositional variables occurring inside the formula.
     * @details ordered in the order they appear in the formula via an in-order traversal of the formula tree. 
     * @example `A /\ (B(a, d) \/ ~C)` returns a list of formula pointers to 
     * `[A, C]` in that order.
    */
    FormulaList allPropositions() const;

    /**
     * @brief Gets a list of pairs of pointers to term variables and the quantifier formula they are bound to.
     * @example if formula is `Ex: P(x) /\ Ay: Q(x, y)` then `.boundTermVariables()` returns 
     * `[(pointer to x term in P(x), pointer to quantifier Ex formula), 
     *   (pointer to x term in Q(x,y), pointer to quantifier Ex formula), 
     *   (pointer to y term in Q(x,y), pointer to quantifier Ay formula)]`
     * @example edge case, inner quantifier binds more strongly if shared variable names 
     * if formula is `Ex: P(x) /\ Ax: Q(x, x)` then `.boundTermVariables()` returns 
     * `[(pointer to x term in P(x), pointer to quantifier Ex formula), 
     *   (pointer to 1st x term in Q(x,x), pointer to quantifier Ex formula), 
     *   (pointer to 2nd x term in Q(x,x), pointer to quantifier Ax formula)]`
    */
    std::list<std::pair<Term*, Formula*>> boundTermVariables() const;

    /**
     * @brief Gets a list of pairs of pointers to function variables and the quantifier formula they are bound to.
    */
    std::list<std::pair<Term*, Formula*>> boundFunctionVariables() const;

    /**
     * @brief Gets a list of pairs of pointers to predicate variables and the quantifier formula they are bound to.
    */
    std::list<std::pair<Formula*, Formula*>> boundPredicateVariables() const;

    /**
     * @brief gets the set of all identifiers in the formula as strings.
     * @details Identifiers include:
     * 1) all bound quantifier variable names
     * 2) all term constant and term variable names 
     * 3) all predicate and predicate variable names
     * 4) all all function and function variable names 
    */
    std::unordered_set<std::string> identifiers() const;

    //@}
    /** @name Formula Metrics and Testers */
    ///@{
    
    /**
     * @brief gets the depth/height of the formula tree.
     * @return the height of the formula tree.
    */
    size_t depth() const;
    
    /**
     * @brief gets the depth/height of the formula tree, including height of term trees on leaves.
     * @return The depth/height of the formula tree, including height of term trees on leaves.
    */
    size_t depthWithTerms() const;

    /**
     * @brief true iff the formula is a proposition, that is, a predicate with 0 arguments.
    */
    bool isProposition() const;

    /**
     * @brief test if the formula is propositional logic
     * @details if the formula is a propositional calculus formula.
     * I.E. contains only the connectives: not, and, or, if, iff
     * and contains only propositional variables
     * @return true iff the formula can be interpreted as propositional
    */
    bool isPropositional() const;

    /**
     * @brief test if the formula is 0th order logic
     * @details Tests if a formula is a 0th order predicate logic formula.
     * I.E. contains only the connectives: not, and, or, if, iff
     * but may contain predicates and terms that are composed of constants
     * @return true iff the formula can be interpreted as zeroth order
    */
    bool isZerothOrder() const;


    /**
     * @brief test if the formula is 1st order logic
     * @details Tests if a 1st order logic formula i.e
     * I.E. contains only the connectives: not, and, or, if, iff
     * and quantifies over term constants, but not predicates or functions
     * @return true iff the formula can be interpreted as first order
    */
    bool isFirstOrder() const;

    /**
     * @brief test if the formula is 2nd order logic
     * @details Tests if a 2nd order formula i.e
     * Contains quantification over Predicates or Functions
     * @return true iff the formula is minimally 2nd order
    */
    bool isSecondOrder() const;
    //@}
};

// Construction Helpers ================================================================================================

Formula* Prop(std::string name);
Formula* Pred(std::string name, TermList args);
Formula* Not(Formula* arg);
Formula* And(Formula* left, Formula* right);
Formula* Or(Formula* left, Formula* right);
Formula* If(Formula* left, Formula* right);
Formula* Iff(Formula* left, Formula* right);
Formula* Forall(std::string varName, Formula* arg);
Formula* Exists(std::string varName, Formula* arg);

/**
 * Converts an SExpression into a formula or throws an error if malformed
 * @param expr a reference to an SExpression Object 
 * @returns A formula representing the given SExpression
 */
Formula* fromSExpression(const sExpression& expr);

/**
 * Converts an SExpression string into a formula or throws an error if malformed
 * Wraps fromSExpression
 * @param sExpressionString An SExpression String  
 */
Formula* fromSExpressionString(std::string sExpressionString);


std::string toSExpression(const Term* formula);
std::string toSExpression(const Formula* formula);
std::string toFirstOrderTPTP(std::string name, std::string type, Formula* formula);

const std::unordered_map<Formula::Type, std::string> TYPE_STRING_MAP = {
    {Formula::Type::PRED, "pred"},       
    {Formula::Type::NOT, "not"},          
    {Formula::Type::AND, "and"},           
    {Formula::Type::OR, "or"},            
    {Formula::Type::IF, "if"},            
    {Formula::Type::IFF, "iff"},           
    {Formula::Type::FORALL, "forall"},        
    {Formula::Type::EXISTS, "exists"},        
};

const std::unordered_map<std::string, Formula::Type> STRING_TYPE_MAP = {      
    {"not", Formula::Type::NOT},          
    {"and", Formula::Type::AND},           
    {"or", Formula::Type::OR},            
    {"if", Formula::Type::IF},            
    {"iff", Formula::Type::IFF},           
    {"forall", Formula::Type::FORALL},        
    {"exists", Formula::Type::EXISTS}
};

const std::unordered_map<Formula::Type, size_t> TYPE_ARGS_MAP = {     
    {Formula::Type::NOT, 1},          
    {Formula::Type::AND, 2},           
    {Formula::Type::OR, 2},            
    {Formula::Type::IF, 2},            
    {Formula::Type::IFF, 2},           
    {Formula::Type::FORALL, 2},        
    {Formula::Type::EXISTS, 2},        
};
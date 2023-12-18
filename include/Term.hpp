
#pragma once

#include<string>
#include<list>
#include<unordered_set>

struct Term;

/** @brief represents a list of term trees */
using TermList = std::list<Term*>;

/**
 * @brief Represents an term level construct such as constants, term variables,
 * functions with args, and function variables. 
 * @details There is no semantic distinction made here at the representation 
 * level between variables and constants. An constant is considered to be a 
 * bound variable if it is bound at the formula level, i.e. there exists a 
 * quantifier that binds it, otherwise it is a constant. Functions here at the 
 * term representational level are just constants with an arity. A function is 
 * considered to be a function variable if it is bound at the formula level to 
 * a quantifier.
 * 
 * I.E. In this code and documentation we refer to any term with 0 arity (args 
 * of length 0) as a "constant" and anything with >0 args as a function, 
 * regardless of if they are actually a variable due to being bound at the 
 * formula level.
*/
struct Term{

    std::string name; ///< The identifier of the term
    TermList args;  ///< The list of args if function / func var, else empty

    Term() = default;
    ~Term();

    /** @brief  true iff two terms are syntactically equivelent */
    bool operator==(const Term& term);

    /**
     * @brief Creates a copy of this term and returns 
     * a newly allocated pointer to it.
    */
    Term* copy() const;
    
    /**
     * @brief Return all immediate subconstants of the current term.
     * @example if term is `f(z, g(x), h(x, y), y)` then 
     * `.subconstants()` returns a list of pointers to `[z, y]`  
     * @example `x` returns an empty list.
    */
    TermList subconstants() const;

    /**
     * @return a list of all subconstants: constants and variables, 
     * including the top level term 
     * @example if term is `f(z, g(x), h(x, y), y)` then `.subconstants()` 
     * returns a list of pointers 
     * [z, x in g(x), x in h(x, y), y in h(x, y), y]
     * @example `x` returns a pointer to itself
    */
    TermList allSubconstants() const;

    /**
     * @return all subconstants if function or itself if a constant
    */
    TermList allConstants() const;

    /**
     * @brief Returns all subfunctions (excluding the top level function) 
     * @example if term is `f(z, g(x), h(x, i(y)), y)` then `.subfunctions()` 
     * returns a list of pointers to [g(x), h(x, i(y)), i(y)].
    */
    TermList allSubfunctions() const;

    /**
     * @brief Returns all functions (including the top level function) 
     * @example if term is `f(z, g(x), h(x, i(y)), y)` then `.subfunctions()`
     * returns a list of pointers 
     * to `[f(z, g(x), h(x, i(y)), y), g(x), h(x, y), i(y)]`.
    */
    TermList allFunctions() const;

    /**
     * @return the hight of the term tree.
    */
    size_t depth() const;

    /**
     * @brief gets the set of all identifier names in the term. This includes
     * 1) all constant and constant variable names
     * 2) all function and function variable names
    */
    std::unordered_set<std::string> identifiers() const;

};

/**
 * @brief Construct a Variable Term.
 * @details Since we make no semantic distinction at the representational level,
 * this function is identical to Const, but can aide in the visual 
 * representation of formulae construction.
 * @param name the identifier for this variable. 
 * @return a pointer to a new variable term.
*/
Term* Var(std::string name);

/**
 * @brief Construct a Constant Term.
 * @details Since we make no semantic distinctions at the representational 
 * level, we can also use this to create vars.
 * @param name the identifier for this constant. 
 * @return a pointer to a new constant term.
*/
Term* Const(std::string name);

/**
 * @brief Construct a Function/Function Variable Term.
 * @details Since we make no semantic distinctions at the representational 
 * level, we can use this for either functions or function variables. 
 * @param name the identifier for this function. 
 * @param args A list of terms that are arguments to this function.
 * @return a pointer to a new variable term.
*/
Term* Func(std::string name, TermList args);

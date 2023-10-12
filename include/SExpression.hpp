
/**
 * @file SExpression.hpp
 * @author James Oswald (jamesoswald111@gmail.com, oswalj@rpi.edu)
 * @date 3/27/22
 * @brief The interface for S-Expressions
 * 
 * This file defines an interface for creating and interacting with
 * S-Expressions terms in the form of lists of lists specified with
 * parenthesis that can be easily represented as n-ary trees.
 */

#pragma once

#include<string>
#include<vector>
#include<queue>
#include<iostream>

struct sExpression{
    /* Possible types of the s-expression */
    enum class Type{
        Keyword = 0, ///< Atom of type keyword, symbol beginning with :
        Number = 1,  ///< Atom of type number, any number
        String = 2,  ///< Atom of type string, any keyword surrounded by ""
        Symbol = 3,  ///< Atom of type symbol, identifier thats not any others
        List = 4     ///< List of other S-Expressions
    };

    //Data
    sExpression::Type type;

    //TODO: These should be implemented in a sum type 
    std::string value;                 ///< Only valid if type != List
    std::vector<sExpression> members;  ///< Only Valid if type == List


    //Methods =================================================================

    /** Default Constructor, creates an empty S-Expression */
    sExpression();
    /** Parses an S-Expresion string into an S-Expression Object */                 
    sExpression(const std::string sExpressionString);  
    /** Deep Copies a existing S-Expresion */       
    sExpression(const sExpression& toCopy);
    /** Moves an existing S-Expresion */
    sExpression(const sExpression&& toMove);
    /** Cleans up an S-Expression */
    ~sExpression(); 
    
    /** Move assignment, moves an existing S-Expression */
    sExpression& operator=(const sExpression&& toMove); 
    /** Copy assignment, copies an existing S-Expression */
    sExpression& operator=(const sExpression& toCopy);
    
    std::string toString(bool expand = false) const;
    void print(bool expand = false) const;

    sExpression& operator[](const size_t index);          //Access values based on indicies
    const sExpression& at(const size_t index) const;

    sExpression& operator[](const std::string&& key);     //Access values based on keywords

    sExpression::Type getTypeAt(const size_t index) const;             
    std::string getValueAt(const size_t index) const;             //returns the value in the sExpression at index if its not a sub list
    unsigned int getNumAt(const size_t) const;                    //same as getValue but if type == num casts it to an int first

    bool contains(const sExpression& t) const;
    sExpression atPosition(std::queue<uid_t> pos) const;
    std::queue<uid_t> positionOf(const sExpression& t) const;
    std::queue<uid_t> positionOf(const sExpression& t, std::queue<uid_t> pos) const;
};

// For equality
bool operator==(const sExpression& s1, const sExpression& s2);
bool operator!=(const sExpression& s1, const sExpression& s2);

//For using sExpressions with std::cout
std::ostream& operator<<(std::ostream& os, const sExpression& object);

//sExpression hashing
namespace std{
    template <>
    struct hash<sExpression>{
        std::size_t operator()(const sExpression& k) const;
    };
}

#include<string>
#include<list>
#include<unordered_map>
#include<unordered_set>
#include<stdexcept>

#include "SExpression.hpp"
#include "Formula.hpp"
#include "settings.hpp"

//Construction Helpers =========================================================

Formula* Prop(std::string name){
    Formula* rv = new Formula;
    rv->type = Formula::Type::PRED;
    rv->connectiveType = Formula::ConnectiveType::PRED;
    rv->pred = new Formula::Pred;
    rv->pred->name = std::move(name);
    rv->pred->args = TermList();
    return rv;
}

Formula* Pred(std::string name, TermList args){
    Formula* rv = new Formula;
    rv->type = Formula::Type::PRED;
    rv->connectiveType = Formula::ConnectiveType::PRED;
    rv->pred = new Formula::Pred;
    rv->pred->name = std::move(name);
    rv->pred->args = std::move(args);
    return rv;
}

Formula* Not(Formula* arg){
    Formula* rv = new Formula;
    rv->type = Formula::Type::NOT;
    rv->connectiveType = Formula::ConnectiveType::UNARY;
    rv->unary = new Formula::UnaryConnective;
    rv->unary->arg = arg;
    return rv;
}

Formula* And(Formula* left, Formula* right){
    Formula* rv = new Formula;
    rv->type = Formula::Type::AND;
    rv->connectiveType = Formula::ConnectiveType::BINARY;
    rv->binary = new Formula::BinaryConnective;
    rv->binary->left = left;
    rv->binary->right = right;
    return rv;
}

Formula* Or(Formula* left, Formula* right){
    Formula* rv = new Formula;
    rv->type = Formula::Type::OR;
    rv->connectiveType = Formula::ConnectiveType::BINARY;
    rv->binary = new Formula::BinaryConnective;
    rv->binary->left = left;
    rv->binary->right = right;
    return rv;
}

Formula* If(Formula* left, Formula* right){
    Formula* rv = new Formula;
    rv->type = Formula::Type::IF;
    rv->connectiveType = Formula::ConnectiveType::BINARY;
    rv->binary = new Formula::BinaryConnective;
    rv->binary->left = left;
    rv->binary->right = right;
    return rv;
}

Formula* Iff(Formula* left, Formula* right){
    Formula* rv = new Formula;
    rv->type = Formula::Type::IFF;
    rv->connectiveType = Formula::ConnectiveType::BINARY;
    rv->binary = new Formula::BinaryConnective;
    rv->binary->left = left;
    rv->binary->right = right;
    return rv;
}

Formula* Forall(std::string varName, Formula* arg){
    Formula* rv = new Formula;
    rv->type = Formula::Type::FORALL;
    rv->connectiveType = Formula::ConnectiveType::QUANT;
    rv->quantifier = new Formula::Quantifier;
    rv->quantifier->var = std::move(varName);
    rv->quantifier->arg = arg;
    return rv;
}

Formula* Exists(std::string varName, Formula* arg){
    Formula* rv = new Formula;
    rv->type = Formula::Type::EXISTS;
    rv->connectiveType = Formula::ConnectiveType::QUANT;
    rv->quantifier = new Formula::Quantifier;
    rv->quantifier->var = std::move(varName);
    rv->quantifier->arg = arg;
    return rv;
}

Term* termFromSExpression(const sExpression& expr){
    if(expr.type != sExpression::Type::List){
        return Const(expr.value);
    }else{
        sExpression firstMember = expr.members[0];
        if(firstMember.type == sExpression::Type::List){
            throw std::runtime_error("Malformed Term SExpression: " 
                                     + expr.toString());
        }
        std::string name = firstMember.value;
        TermList args;
        //Recursively convert all subterms.
        std::vector<sExpression>::const_iterator itr = expr.members.cbegin();
        itr++;
        for(;itr != expr.members.end(); itr++){
            args.push_back(termFromSExpression(*itr));
        }
        return Func(name, args);
    }
}

const std::unordered_map<Formula::Type, size_t> TYPE_ARGS_MAP = {     
    {Formula::Type::NOT, 1},          
    {Formula::Type::AND, 2},           
    {Formula::Type::OR, 2},            
    {Formula::Type::IF, 2},            
    {Formula::Type::IFF, 2},           
    {Formula::Type::FORALL, 2},        
    {Formula::Type::EXISTS, 2},        
};

Formula* fromSExpression(const sExpression& expr){
    if(expr.type != sExpression::Type::List){
        return Prop(expr.value);
    }else{
        sExpression firstMember = expr.members[0];
        if(firstMember.type == sExpression::Type::List){
            throw std::runtime_error("Malformed Formula SExpression: " 
                                     + expr.toString());
        }
        std::string name = firstMember.value; 
        //If the connective name is a valid connective and uses the right
        //number of arguments, it is a proper connective, otherwise
        //it is treated as a predicate.
        if(
            STRING_TYPE_MAP.find(name) != STRING_TYPE_MAP.end() &&
            TYPE_ARGS_MAP.at(STRING_TYPE_MAP.at(name)) == expr.members.size() - 1
        ){
            switch(STRING_TYPE_MAP.at(name)){
                case Formula::Type::AND:
                    return And(fromSExpression(expr.members[1]), fromSExpression(expr.members[2]));
                case Formula::Type::OR:
                    return Or(fromSExpression(expr.members[1]), fromSExpression(expr.members[2]));
                case Formula::Type::IF:
                    return If(fromSExpression(expr.members[1]), fromSExpression(expr.members[2]));
                case Formula::Type::IFF:
                    return Iff(fromSExpression(expr.members[1]), fromSExpression(expr.members[2]));
                case Formula::Type::NOT:
                    return Not(fromSExpression(expr.members[1]));
                case Formula::Type::FORALL:{
                    if(expr.members[1].type == sExpression::Type::List){
                        throw std::runtime_error("Lists of vars are unsupported");
                    }
                    return Forall(expr.members[1].value, fromSExpression(expr.members[2]));
                }
                case Formula::Type::EXISTS:{
                    if(expr.members[1].type == sExpression::Type::List){
                        throw std::runtime_error("Lists of vars are unsupported");
                    }
                    return Exists(expr.members[1].value, fromSExpression(expr.members[2]));
                }
                default:
                    throw std::runtime_error("Unsupported Connective");
            }
        }else{
            //Recursively convert everything else as a subterm
            TermList args;
            std::vector<sExpression>::const_iterator itr = expr.members.cbegin();
            itr++;
            for(;itr != expr.members.end(); itr++){
                args.push_back(termFromSExpression(*itr));
            }
            return Pred(name, args);
        }
    }
    throw std::runtime_error("Impossible");
}

Formula* fromSExpressionString(std::string sExpressionString){
    sExpression expr(sExpressionString);
    return fromSExpression(expr);
}

// SExpression Converters ======================================================

std::string toSExpression(Term* term){
    if(term->args.size() == 0){
        return term->name;
    }else{
        std::string rv = "(" + term->name + " ";
        for(Term* arg : term->args){
            rv += toSExpression(arg) + " ";
        }
        rv.pop_back();
        rv += ")";
        return rv;
    }
}

std::string toSExpression(const Formula* formula){
    switch(formula->type)
    {
        case Formula::Type::PRED:{
            std::string rv;
            formula->pred->applyToAsTerm([&rv](Term* t){
                rv = toSExpression(t);
            });
            return rv;
        }
        case Formula::Type::FORALL:
        case Formula::Type::EXISTS:{
            std::string name = TYPE_STRING_MAP.at(formula->type);
            return "(" + name + " " + formula->quantifier->var +
                   " " + toSExpression(formula->quantifier->arg) + ")";
        }
        default:{
            //If the formula is a valid type, use its type string as its operator, else use "???"
            auto itr = TYPE_STRING_MAP.find(formula->type);
            std::string typeString = itr != TYPE_STRING_MAP.end() ? itr->second : "???";
            std::string rv = "(" + typeString + " ";
            for(Formula* arg : formula->subformulae()){
                rv += toSExpression(arg) + " ";
            }
            rv.pop_back();
            rv += ")";
            return rv;
        }
    }
}

// TPTP ========================================================================

using BoundTermSet = std::unordered_set<Term*>;

/**
 *  Converts an arbitrary eminence prover identifier to an TPTP identifier
 *  All TPTP identifiers start with a letter followed by any length of letters
 *  and numbers Functions and Predicates are use lowercase letters 
 *  Quantified variables are forced to use upper case letters
 *  @param epIdentifier the identifier string provided by eminence prover
 *  @param upper if the identifier should be upper cased, I.E. is a 
 *  @return a legal TPTP Identifier 
*/
std::string makeLegalTPTPIdentifier(const std::string& epIdentifier,
                                    bool upper){
    std::string rv = "";
    //Ensure first char is a letter; if not, append an S on front
    if (epIdentifier.size() == 0 || !std::isalpha(epIdentifier[0])){
        rv = (upper ? "S" : "s") + rv; 
    }
    for(char c : epIdentifier){ //append all legal chars
        if(std::isalnum(c)){ 
            rv += (char)(upper ? std::toupper(c) : std::tolower(c));
        }
    }
    return rv;
}

/**
 * Makes all identifiers an constants in the given formula a "legal" for
 * conversion to a TPTP formula
 * 1) Converts bound variables to legal uppercase idents 
 * 2) Converts functions to be valid TPTP identifiers
 * 3) Converts predicates to be valid TPTP identifiers 
 * 4) Converts term constants to have quotation marks around them in name
 * 
 * @param formula
 * @return a version of the formula which will generate 
 * 
 * @todo Potential optimizations, 
 * 1) create a map of identifiers that have already been converted to TPTP
 *    idents so we don't waste time reconverting
 * 2) check if an identifier is a legal TPTP identifier before converting
*/
Formula* makeLegalTPTP(const Formula* formula){
    
    Formula* rv = formula->copy();
    
    //Get the set of all bound constants
    BoundTermSet boundTerms;
    for(auto [term, quantifierFormula] : rv->boundTermVariables()){
        boundTerms.insert(term);
    }

    //Put quotes around unbound constants and make bound constants legal uppercase idents
    for(Term* constant : rv->allConstants()){
        if(boundTerms.find(constant) == boundTerms.end()){
            constant->name = "\"" + constant->name + "\"";
        }else{
            constant->name = makeLegalTPTPIdentifier(constant->name, true);
        }
    }

    //Convert functions and predicates to lowercase idents
    for(Term* function : rv->allFunctions()){
        function->name = makeLegalTPTPIdentifier(function->name, false);
    }
    for(Formula* p : rv->allPredicates()){
        p->pred->name = makeLegalTPTPIdentifier(p->pred->name, false);
    }

    //Convert all quantifier variables to 
    //Note we can't iter over boundTermVariables as it is possible we have quantifiers that don't bind to any anything
    for(Formula* f : rv->allQuantified()){
        f->quantifier->var = makeLegalTPTPIdentifier(f->quantifier->var, true);
    }

    return rv;
}

const std::unordered_map<Formula::Type, std::string> TPTPStringMap = {      
    {Formula::Type::NOT, "~"},          
    {Formula::Type::AND, "&"},           
    {Formula::Type::OR, "|"},            
    {Formula::Type::IF, "=>"},            
    {Formula::Type::IFF, "<=>"},           
    {Formula::Type::FORALL, "!"},        
    {Formula::Type::EXISTS, "?"},        
};

std::string recursiveToTPTP(Term* term){
    if(term->args.size() == 0){
        return term->name;
    }else{
        std::string rv = term->name + "(";
        for(Term* arg : term->args){
            rv += recursiveToTPTP(arg) + ", ";
        }
        rv.pop_back();
        rv.pop_back();
        rv += ")";
        return rv;
    }
}

std::string recursiveToTPTP(Formula* formula){
    switch(formula->type){
        case Formula::Type::PRED:{
            std::string rv;
            formula->pred->applyToAsTerm([&rv](Term* t){
                rv = recursiveToTPTP(t);
            });
            return rv;
        }
        case Formula::Type::NOT:
            return TPTPStringMap.at(formula->type) + recursiveToTPTP(formula->unary->arg);
        case Formula::Type::AND:
        case Formula::Type::OR:
        case Formula::Type::IF:
        case Formula::Type::IFF:
            return "(" + recursiveToTPTP(formula->binary->left) + TPTPStringMap.at(formula->type) +
                   recursiveToTPTP(formula->binary->right) + ")";
        case Formula::Type::FORALL:
        case Formula::Type::EXISTS:
            return "(" + TPTPStringMap.at(formula->type) + " [" + formula->quantifier->var + "] : " +
                   recursiveToTPTP(formula->quantifier->arg) + ")"; 
    }
    throw std::runtime_error("invalid ");
}


std::string toFirstOrderTPTP(std::string name, std::string type, Formula* formula){
    if(!formula->isFirstOrder()){
        throw std::runtime_error("Trying to convert a non-first order formula to first order TPTP");
    }
    Formula* cleanFormula = makeLegalTPTP(formula);
    std::string tptpFormulaString = "fof(" + name + "," + type + "," + recursiveToTPTP(cleanFormula) + ").";
    delete cleanFormula;
    return tptpFormulaString;
}
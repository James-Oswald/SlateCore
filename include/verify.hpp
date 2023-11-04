
#pragma once

#include<set>
#include<list>
#include<string>
#include<vector>
#include<optional>
#include<unordered_map>

#include "Formula.hpp"

/**
 * A Justification is an inference rule or Unknown
*/
enum class Justification {
    Assumption,
    AndElim,
    AndIntro,
    OrIntro,
    OrElim,
    NotIntro,
    NotElim,
    IfIntro,
    IfElim,
    IffIntro,
    IffElim,
};

/**
 * An abstract proof node containing an id, formula, justification, and
 * assumptions. 
*/
struct ProofNode {
    Formula* formula;                   ///< Formulae on the node 
    Justification justification;        
    std::vector<ProofNode*> parents;
    std::set<ProofNode*> assumptions;
};

/**
 * Construct a new proof node from string input
 * 
*/
ProofNode* newProofNode(
    std::string formulaExpr, std::string justification, 
    std::vector<ProofNode*> parents
);

/**
 * Verify a single proof node
 * @return an optional string, if std::nullopt the node is verified else 
 * contains the error string of why it wasn't able to verify.
*/
std::optional<std::string> verify(ProofNode* node);

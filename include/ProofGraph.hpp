
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
    id_t id;                            ///< Id of the node
    Formula* formula;                   ///< Formulae on the node 
    Justification justification;        ///< Justification for the node
    std::vector<ProofNode*> parents;    ///< Parents of the node
    std::list<ProofNode*> children;    ///< Children of the node
    std::set<ProofNode*> assumptions;   ///< assumptions of the node
};

/**
 * Construct a new proof node from string inputs
 * 
*/
ProofNode* newProofNode(
    std::string formulaExpr, std::string justification, 
    std::vector<ProofNode*> parents
);

struct ProofGraph{
    std::unordered_map<id_t, ProofNode*> nodes;
    std::set<ProofNode*> assumptions;
};

/**
 * Construct a proof graph based on a json string
*/
ProofGraph* newProofGraph(std::string jsonProofGraph);

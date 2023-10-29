
#pragma once

#include<set>
#include<list>

#include"Formula.hpp"

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
    Formula* formula;
    Justification justification;
    std::vector<ProofNode*> parents;
    std::set<ProofNode*> assumptions;
};
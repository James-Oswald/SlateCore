
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

std::unordered_map<std::string, Justification> JUSTIFICATION_MAP = {
    {"Assumption", Justification::Assumption},
    {"AndElim", Justification::AndElim},
    {"AndIntro", Justification::AndIntro},
    {"OrIntro", Justification::OrIntro},
    {"OrElim", Justification::OrElim},
    {"NotIntro", Justification::NotIntro},
    {"NotElim", Justification::NotElim},
    {"IfIntro", Justification::IfIntro},
    {"IfElim", Justification::IfElim},
    {"IffIntro", Justification::IffIntro},
    {"IffElim", Justification::IffElim},
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

ProofNode* newProofNode(
    std::string formulaExpr, std::string justification, 
    std::vector<ProofNode*> parents
);

/**
 * Verify a proof node
 * @return an optional string, if none, returns true, else contains
 *         the error string of why it wasn't able to verify.
*/
std::optional<std::string> verify(ProofNode* node);

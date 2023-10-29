

#include<set>
#include<string>
#include<vector>
#include<optional>

#include"Formula.hpp"
#include"verify.hpp"

struct VerifyResult{
    bool verified;
    size_t depth;
    std::string errMsg;
};

// Assumption Builders =========================================================

/**
 * @returns the union of the node's parents assumptions
*/
std::set<ProofNode*> parentAssumptionUnion(const ProofNode* node){
    std::set<ProofNode*> rv;
    for(const ProofNode* parent : node->parents)
        for(ProofNode* assumption : parent->assumptions)
            rv.insert(assumption);
    return rv;
}

/**
 * @returns the union of the node's parents assumptions, excluding any formulae
 *          in the exclude set.
*/
std::set<ProofNode*> parentAssumptionUnionExcluding(
    const ProofNode* node,
    std::set<Formula*> excludeSet
){
    std::set<ProofNode*> rv;
    for(ProofNode* assumption : parentAssumptionUnion(node)){
        for(Formula* exclude: excludeSet){
            if(!(*assumption->formula == *exclude)){
                rv.insert(assumption);
            }
        }
    }
    return rv;
}

//Conditions ===================================================================

/**
 *  @param p proof node to check
 *  @param t what top level connective should this node have? 
 *           nullopt if we don't care.
 *  @returns an optional error string if the connectives don't match
*/
std::optional<std::string> hasConnective(const ProofNode* p, Formula::Type t){
    if(p->formula->type == t){
        return std::nullopt;
    }else{
        return std::make_optional("Expected" + toSExpression(p->formula) +
        " to have top level connective " + TYPE_STRING_MAP.at(t) + 
        " but it has " + TYPE_STRING_MAP.at(p->formula->type));
    }
}

std::optional<std::string> hasParents(const ProofNode* p, size_t n){
    if(p->parents.size() == n){
        return std::nullopt;
    }else{
        return std::make_optional("expected" + toSExpression(p->formula) +
        " to have" + std::to_string(n) + " parents but it has " +
        std::to_string(p->parents.size()) + "parents");
    }
}

std::optional<std::string> equalFormula(
    const Formula* a,
    const Formula* b
){
    if(*a == *b){
        return std::nullopt;
    }else{
        return std::make_optional("expected" + toSExpression(a) + " to equal "
        + toSExpression(b));
    }
}

/**
 * @return iff proof node p has f in it's set of assumptions
*/
std::optional<std::string> hasAssumption(const ProofNode* p, const Formula* f){
    for(ProofNode* assumption : p->assumptions){
        if (*assumption->formula == *f){
            return std::nullopt;
        }
    }
    return std::make_optional("expected" + toSExpression(p->formula) + 
        " to have " + toSExpression(f) + "as an assumption");
}

// Error Helper Macros =========================================================
#define RULE_START()\
size_t depth = 0;\
std::optional<std::string> err, err1, err2;

#define EXPECT(COND)\
err = COND;\
if(err){\
    return {false, depth++, err.value() + "."};\
}\

#define EXPECT_EITHER(COND1, COND2)\
err1 = COND1;\
err2 = COND2;\
if(err1 && err2){\
    return {false, depth++, "Either " + err1.value() +\
                            " or " + err2.value() + "."};\
}\

#define RULE_END()\
return {true, depth++, ""};

// Rules =======================================================================

VerifyResult verifyAssumption(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 0));
    node->assumptions = {node};
    RULE_END();
}

VerifyResult verifyAndIntro(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 2));
    EXPECT(hasConnective(node, Formula::Type::AND));
    EXPECT(equalFormula(node->formula->binary->left, node->parents[0]->formula));
    EXPECT(equalFormula(node->formula->binary->right, node->parents[1]->formula));
    node->assumptions = parentAssumptionUnion(node);
    RULE_END();
}

VerifyResult verifyAndElim(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 1));
    ProofNode* parent = node->parents[0];
    EXPECT(hasConnective(parent, Formula::Type::AND));
    EXPECT_EITHER(
        equalFormula(node->formula, parent->formula->binary->left),
        equalFormula(node->formula, parent->formula->binary->right)
    );
    node->assumptions = parentAssumptionUnion(node);
    RULE_END();
}

VerifyResult verifyOrIntro(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 1));
    EXPECT(hasConnective(node, Formula::Type::OR));
    ProofNode* parent = node->parents[0];
    EXPECT_EITHER(
        equalFormula(parent->formula, node->formula->binary->left),
        equalFormula(parent->formula, node->formula->binary->right)
    );
    node->assumptions = parentAssumptionUnion(node);
    RULE_END();
}

VerifyResult verifyOrElim(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 3));
    ProofNode* disjunction = node->parents[0];
    Formula* leftAssumption = disjunction->formula->binary->left;
    Formula* rightAssumption = disjunction->formula->binary->right;
    ProofNode* leftCase = node->parents[1];
    ProofNode* rightCase = node->parents[2];
    EXPECT(hasConnective(disjunction, Formula::Type::OR));
    EXPECT(equalFormula(node->formula, leftCase->formula));
    EXPECT(equalFormula(node->formula, rightCase->formula));
    EXPECT(hasAssumption(leftCase, leftAssumption));
    EXPECT(hasAssumption(rightCase, rightAssumption));
    node->assumptions = parentAssumptionUnionExcluding(node, {leftAssumption, rightAssumption});
    RULE_END();
}

VerifyResult verifyNotIntro(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 2));
    EXPECT(hasConnective(node, Formula::Type::NOT));
    ProofNode* negatedParent = node->parents[0];
    ProofNode* nonNegatedParent = node->parents[1];
    Formula* assumption = node->formula->unary->arg;
    EXPECT(hasConnective(negatedParent, Formula::Type::NOT));
    EXPECT(equalFormula(nonNegatedParent->formula, negatedParent->formula->unary->arg));
    EXPECT_EITHER(
        hasAssumption(negatedParent, assumption),
        hasAssumption(nonNegatedParent, assumption)
    );
    //TODO: its possible that removing assumption after the union is
    //problematic. If it is, write a new parentAssumptionUnionExcluding
    //that removes from each branch before union.
    node->assumptions = parentAssumptionUnionExcluding(node, {assumption});
    RULE_END();
}

VerifyResult verifyNotElim(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 2));
    EXPECT(hasConnective(node, Formula::Type::NOT));
    ProofNode* negatedParent = node->parents[0];
    ProofNode* nonNegatedParent = node->parents[1];
    Formula* assumption = node->formula->unary->arg;
    EXPECT(hasConnective(negatedParent, Formula::Type::NOT));
    EXPECT(equalFormula(nonNegatedParent->formula, negatedParent->formula->unary->arg));
    EXPECT_EITHER(
        hasAssumption(negatedParent, assumption),
        hasAssumption(nonNegatedParent, assumption)
    );
    node->assumptions = parentAssumptionUnionExcluding(node, {assumption});
    RULE_END();
}

VerifyResult verifyIfIntro(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 1));
    EXPECT(hasConnective(node, Formula::Type::IF));
    Formula* antecedent = node->formula->binary->left;
    Formula* consequent = node->formula->binary->right;
    EXPECT(hasAssumption(node, antecedent));
    EXPECT(equalFormula(consequent, node->parents[0]->formula));
    node->assumptions = parentAssumptionUnionExcluding(node, {antecedent});
    RULE_END();
}

VerifyResult verifyIfElim(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 2));
    Formula* conditional = node->parents[0]->formula;
    Formula* antecedent = node->parents[1]->formula;
    EXPECT(equalFormula(conditional->binary->right, antecedent));
    EXPECT(equalFormula(conditional->binary->left, node->formula));
    node->assumptions = parentAssumptionUnion(node);
    RULE_END();
}

VerifyResult verifyIffIntro(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 2));
    EXPECT(hasConnective(node, Formula::Type::IF));
    Formula* leftFormula = node->formula->binary->left;
    Formula* rightFormula = node->formula->binary->right;
    ProofNode* leftPar = node->parents[0];
    ProofNode* rightPar = node->parents[1];
    EXPECT(equalFormula(leftPar->formula, leftFormula));
    EXPECT(equalFormula(rightPar->formula, rightFormula));
    EXPECT(hasAssumption(leftPar, rightFormula));
    EXPECT(hasAssumption(rightPar, leftFormula));
    node->assumptions = parentAssumptionUnionExcluding(node, {leftFormula, rightFormula});
    RULE_END();
}

VerifyResult verifyIffElim(ProofNode* node){
    RULE_START();
    EXPECT(hasParents(node, 2));
    EXPECT(hasConnective(node, Formula::Type::IF));
    Formula* leftFormula = node->formula->binary->left;
    Formula* rightFormula = node->formula->binary->right;
    ProofNode* leftPar = node->parents[0];
    ProofNode* rightPar = node->parents[1];
    EXPECT(equalFormula(leftPar->formula, leftFormula));
    EXPECT(equalFormula(rightPar->formula, rightFormula));
    EXPECT(hasAssumption(leftPar, rightFormula));
    EXPECT(hasAssumption(rightPar, leftFormula));
    node->assumptions = parentAssumptionUnionExcluding(node, {leftFormula, rightFormula});
    RULE_END();
}

const std::unordered_map<Justification, VerifyResult(*)(ProofNode*)> VERIFIERS =
{
    {Justification::Assumption, verifyAssumption},
    {Justification::AndIntro, verifyAndIntro},
    {Justification::AndElim, verifyAndElim},
    {Justification::OrIntro, verifyOrIntro},
    {Justification::OrElim, verifyOrElim},
    {Justification::NotIntro, verifyNotIntro},
    {Justification::NotElim, verifyNotElim},
    {Justification::IfIntro, verifyIfIntro},
    {Justification::IfElim, verifyIfElim},
    {Justification::IffIntro, verifyIffIntro},
    {Justification::IffElim, verifyIffElim}
};

std::optional<std::string> verify(ProofNode* node){
    VerifyResult best = {false, 0, ""};
    while(std::next_permutation(node->parents.begin(), node->parents.end())){
        VerifyResult result = VERIFIERS.at(node->justification)(node);
        if(result.verified){
            return std::nullopt;
        }
        if(result.depth > best.depth){
            best = result;
        }
    }
    return std::make_optional(best.errMsg);
}

#include<cassert>
#include<optional>

#include"verify.hpp"



int main(){
    //Assumptions, and intro, or intro
    ProofNode* A = newProofNode("A", "Assumption", {});
    ProofNode* B = newProofNode("B", "Assumption", {});
    ProofNode* AB = newProofNode("(and A B)", "AndIntro", {A, B});
    ProofNode* AC = newProofNode("(or C A)", "OrIntro", {A});
    ProofNode* ABC = newProofNode("(and A (or C A))", "AndIntro", {A, AC});
    assert(verify(A) == std::nullopt);
    assert(verify(B) == std::nullopt);
    assert(verify(AB) == std::nullopt);
    assert(verify(ABC) == std::nullopt);

    //IfIntro Test
    ProofNode* A2 = newProofNode("A", "Assumption", {});
    ProofNode* AQ = newProofNode("(or A Q)", "OrIntro", {A2});
    ProofNode* ifAAQ = newProofNode("(if A (or A Q))", "IfIntro", {AQ});
    assert(verify(A2) == std::nullopt);
    assert(verify(AQ) == std::nullopt);
    assert(verify(ifAAQ) == std::nullopt);
}
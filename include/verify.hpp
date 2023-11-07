
#pragma once

#include"ProofGraph.hpp"

/**
 * Verify a single proof node
 * @param node a pointer to a proof node to verify (probably created via
 *  newProofNode). `node->assumptions` will be overwritten by this function.
 * @return an optional string, if std::nullopt the node is verified else 
 * contains the error string of why it wasn't able to verify.
*/
std::optional<std::string> verify(ProofNode* node);

/**
 * In Development 
*/
//std::string verifyProofGraph(const std::string& jsonProofGraph);

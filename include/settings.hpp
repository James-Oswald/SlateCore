
#pragma once

#include<string>
#include<unordered_map>

#include"Formula.hpp"
#include"verify.hpp"

const std::unordered_map<Formula::Type, std::string> TYPE_STRING_MAP = {
    {Formula::Type::PRED, "pred"},       
    {Formula::Type::NOT, "not"},          
    {Formula::Type::AND, "and"},           
    {Formula::Type::OR, "or"},            
    {Formula::Type::IF, "if"},            
    {Formula::Type::IFF, "iff"},           
    {Formula::Type::FORALL, "forall"},        
    {Formula::Type::EXISTS, "exists"},        
};

const std::unordered_map<std::string, Formula::Type> STRING_TYPE_MAP = {      
    {"not", Formula::Type::NOT},          
    {"and", Formula::Type::AND},           
    {"or", Formula::Type::OR},            
    {"if", Formula::Type::IF},            
    {"iff", Formula::Type::IFF},           
    {"forall", Formula::Type::FORALL},        
    {"exists", Formula::Type::EXISTS}
};

const std::unordered_map<Formula::Type, std::string> TPTPStringMap = {      
    {Formula::Type::NOT, "~"},          
    {Formula::Type::AND, "&"},           
    {Formula::Type::OR, "|"},            
    {Formula::Type::IF, "=>"},            
    {Formula::Type::IFF, "<=>"},           
    {Formula::Type::FORALL, "!"},        
    {Formula::Type::EXISTS, "?"},        
};

/**
 * A global map that takes strings and returns justification enums
*/
const std::unordered_map<std::string, Justification> JUSTIFICATION_STRING_MAP = {
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
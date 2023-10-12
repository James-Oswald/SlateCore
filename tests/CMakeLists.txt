add_executable(FormulaTest FormulaTest.cpp)
target_link_libraries(FormulaTest SlateCore)
add_test(NAME FormulaTest COMMAND FormulaTest)

add_executable(SExpressionTest SExpressionTest.cpp)
target_link_libraries(SExpressionTest SlateCore)
add_test(NAME SExpressionSingle COMMAND SExpressionTest "A")
add_test(NAME SExpressionSimple  COMMAND SExpressionTest "(or A B)")
add_test(NAME SExpressionTestCompound COMMAND SExpressionTest 
"(if (or A B) (and A (not (iff A C))))")
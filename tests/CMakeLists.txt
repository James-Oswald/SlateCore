add_executable(FormulaTest FormulaTest.cpp)
target_link_libraries(FormulaTest SlateCore)
add_test(NAME FormulaTest COMMAND FormulaTest)
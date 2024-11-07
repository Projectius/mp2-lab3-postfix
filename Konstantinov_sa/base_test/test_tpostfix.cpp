#include "postfix.h"
#include <gtest.h>

TEST(TPostfix, can_create_postfix)
{
	ASSERT_NO_THROW(TPostfix<double> pf(true));
}

TEST(TPostfix, can_set_infix)
{
	TPostfix<double> pf(true);
	string infix = "a + b";
	ASSERT_NO_THROW(pf.inputInfix(infix));
	ASSERT_EQ(pf.getInfix(), infix);
}

TEST(TPostfix, throws_empty_infix)
{
	TPostfix<double> pf(true);
	ASSERT_ANY_THROW(pf.parseToPostfix());

}

TEST(TPostfix, can_simple_postfix)
{
	TPostfix<double> pf(true);
	string infix = "a * ( b + c )";
	pf.inputInfix(infix);
	pf.parseToPostfix();
	ASSERT_EQ(pf.GetPostfix(), "a b c + * ");
}

TEST(TPostfix, can_complex_postfix)
{
	TPostfix<double> pf(true);
	string infix = "( a + sin ( b - c ) ^ d ) / 2";
	pf.inputInfix(infix);
	pf.parseToPostfix();
	ASSERT_EQ(pf.GetPostfix(), "a b c - sin d ^ + 2 / ");
}

TEST(TPostfix, throws_invalid_infix)
{
	TPostfix<double> pf(true);
	string infix = "(a + b) + c";
	pf.inputInfix(infix);
	ASSERT_ANY_THROW(pf.parseToPostfix());
}

TEST(TPostfix, can_calculate_only_constants)
{
	TPostfix<int> pf(true);
	string infix = "2 * ( 1 + 6 )";
	pf.inputInfix(infix);
	pf.parseToPostfix();
	ASSERT_EQ(pf.Calculate(), 14);
}

TEST(TPostfix, throws_calculating_not_parsed)
{
	TPostfix<int> pf(true);
	string infix = "2 * ( 1 + 6 )";
	pf.inputInfix(infix);
	ASSERT_ANY_THROW(pf.Calculate());
}

TEST(TPostfix, can_set_vars_and_calculate)
{
	TPostfix<int> pf(true);
	string infix = "a * ( b + c )";
	pf.inputInfix(infix);
	pf.parseToPostfix();
	pf.setVariables({ {"a", 2},{"b", 1},{"c", 6} });
	ASSERT_EQ(pf.Calculate(), 14);
}

TEST(TPostfix, throws_undefined_vars)
{
	TPostfix<int> pf(true);
	string infix = "a * ( b + c )";
	pf.inputInfix(infix);
	pf.parseToPostfix();
	pf.setVariables({ {"a", 2},{"b", 1}});
	ASSERT_ANY_THROW(pf.Calculate());
}

TEST(TPostfix, throws_define_nonexistent_var)
{
	TPostfix<int> pf(true);
	string infix = "a * a";
	pf.inputInfix(infix);
	pf.parseToPostfix();
	ASSERT_ANY_THROW(pf.setVariables({ {"a", 2},{"b", 1} }));
}

TEST(TPostfix, can_add_new_operator)
{
	TPostfix<int> pf(true);
	pf.addOperator("squared", 1, 255, [](int a, int b) {return a * a;});
	string infix = " 1 + squared ( 2 + 3 ) * 2";
	pf.inputInfix(infix);
	pf.parseToPostfix();
	ASSERT_EQ(pf.Calculate(), 51);
}
#include "postfix.h"
#include <gtest.h>

TEST(TPostfix, maintest)
{
	Variable<double>v("var");
	cout << v;
	Operator<function<double(double, double)>> add("+", 2, [](double a, double b) -> double {return a + b;});
	cout << add << add.Execute(1.0, 2.0);
}


//TEST(TPostfix, can_create_postfix)
//{
//	ASSERT_NO_THROW(TPostfix p);
//}


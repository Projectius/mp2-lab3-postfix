#pragma once
#include"postfix.h"

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

	TPostfix pf("a + b * c");

	//system("pause");
}

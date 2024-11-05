#pragma once
#include"postfix.h"

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

	TPostfix<double> pf("c * ( a + b )"); //"e / ( ( a + b ) * c + d )"
	pf.setVariables({ {"a", 4.2},{"b", 1.0},{"c", 2.0}});
	cout << "CALCULATED = "<<pf.Calculate()<<endl;
	system("pause");
}

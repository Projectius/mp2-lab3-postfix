#pragma once
#include"postfix.h"

#include<math.h>

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

	//TPostfix<double> pf("c * ( a + b ) + b"); //"e / ( ( a + b ) * c + d )"
	//pf.setVariables({ {"a", 4.2},{"b", 1.0},{"c", 2.0}});
	//cout << "CALCULATED = "<<pf.Calculate()<<endl;
	//system("pause");
	try {
		TPostfix<double> pf(true); //"e / ( ( a + b ) * c + d )"
		pf.inputInfix("a * ( a + b ) ^2");
		pf.parseToPostfix();
		cout << pf.GetPostfix() << endl;
		pf.setVariables({ {"a", 2.0},{"b", 3.0} });
		cout << "CALCULATED = " << pf.Calculate() << endl;
	}
	catch (exception& e)
	{
		cout << "EXC " << e.what() << endl;
	}
	system("pause");
}

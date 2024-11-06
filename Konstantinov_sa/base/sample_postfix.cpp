#include <iostream>
#include <string>
#include "postfix.h"

using namespace std;

int main()
{
	/*string expression;
	TPostfix<int> pf(true);*/
	//double res;

	//setlocale(LC_ALL, "Russian");
	////cout << "Введите арифметическое выражение: ";
	////cin >> expression;
	////cout << expression << endl;
	//cout << "Арифметическое выражение: " << postfix.GetInfix() << endl;
	//postfix.ToPostfix();
	//cout << "Постфиксная форма: " << postfix.GetPostfix() << endl;
	//res = postfix.Calculate();
	//cout << res << endl;

	try {
		TPostfix<double> pf(true); //"e / ( ( a + b ) * c + d )"
		//pf.inputInfix(" b * a ^ ( a + b )");
		pf.inputInfix("( sin ( ( a + b ) / c ) + 2 ) ^ b");
		pf.parseToPostfix();
		cout << pf.GetPostfix() << endl;
		cout << "undef " << pf.checkForUndefinedVars() << endl;
		pf.setVariables({ {"a", 1.1415},{"b", 2.0} });
		//pf.setVariables({ {"a", 2.0},{"b", 3.0} });
		cout << "CALCULATED = " << pf.Calculate() << endl;
	}
	catch (exception& e)
	{
		cout << "EXC " << e.what() << endl;
	}
	system("pause");

	return 0;
}

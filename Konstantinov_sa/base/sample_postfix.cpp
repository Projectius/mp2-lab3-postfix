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
		cout << "Доступные операторы: " << endl;
		vector<string> ops = pf.getOperatorNames();
		for (auto& n : ops)
		{
			cout << n << endl;
		}
		string inf = "a * ( 2 + 1 )";
		//cin >> inf;
		pf.inputInfix(inf);
		pf.parseToPostfix();
		cout << pf.GetPostfix() << endl;
		if (pf.checkForUndefinedVars())
		{
			cout << "Определите переменные" << endl;
			auto ud = pf.getUndefinedVars();
			for (auto p : ud)
			{
				cout << p.first << " = ";
				double v;
				cin >> v;
				*p.second = v;
			}
		}
		//pf.setVariables({ {"a", 1.1415},{"b", 2.0} });
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

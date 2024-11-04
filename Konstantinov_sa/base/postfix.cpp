// Не собирается если любое определение вынесено в cpp, только если всё в .h
// Не собираюсь тратить больше уже потраченного часа на это

//#include "stack.h"
//#include "postfix.h"




//TPostfix::TPostfix(string infix_, bool importBasicOperators = 1)
//{
//	/*infix = infix_;
//	if (importBasicOperators) {
//		Operator<function<double(double, double)>> add("+", 2,0, [](double a, double b) -> double {return a + b;});
//		Operator<function<double(double, double)>> sub("-", 2, 0, [](double a, double b) -> double {return a - b;});
//		Operator<function<double(double, double)>> mul("*", 2, 1, [](double a, double b) -> double {return a * b;});
//		Operator<function<double(double, double)>> div("/", 2, 1, [](double a, double b) -> double {return a / b;});
//		base.addLexeme(add);
//		base.addLexeme(sub);
//		base.addLexeme(div);
//		base.addLexeme(mul);
//		
//		base.addLexeme(Lexeme("(", parOpen));
//		base.addLexeme(Lexeme(")", parClose));
//
//	}
//
//	parseToPostfix();*/
//	//cout << GetPostfix() << endl;
//	cout << "POSTFIX" << endl;
//}

//void TPostfix::parseToPostfix()
//{
//	istringstream iss(infix);
//	string token;
//	while (iss >> token)
//	{
//		Lexeme* lex = base.getLexeme(token);
//
//		if (lex == nullptr)
//		{
//			postfix.push_back(base.addVar<double>(token));
//		}
//		else 
//		{
//			LexemeType type = lex->getType();
//			if (type == LexemeType::var) {
//				// Если лексема — переменная, добавляем её в постфиксное выражение
//				postfix.push_back(lex);
//			}
//			else if (type == op)
//			{
//				while (!opStack.empty() && opStack.get_top()->getPriority() >= lex->getPriority()) // приоритет для неоператоров (скобок) будет всегда -1
//				{
//					postfix.push_back(opStack.get_top());
//					opStack.pop();
//				}
//					opStack.push(lex);
//			}
//			else if (type == parOpen)
//			{	
//					postfix.push_back(lex);
//			}
//			else if (type == parClose)
//			{
//				while (!opStack.empty() && opStack.get_top()->getType()!=parOpen) // приоритет для неоператоров (скобок) будет всегда -1
//				{
//					postfix.push_back(opStack.get_top());
//					opStack.pop();
//				}
//
//				if (!opStack.empty())
//					opStack.pop();
//			}
//		}
//
//	}
//	while (!opStack.empty())
//	{
//		postfix.push_back(opStack.get_top());
//		opStack.pop();
//	}
//
//}

//string TPostfix::GetPostfix()
//{
//	ostringstream strpostfix;
//	for (auto& it : postfix)
//	{
//		strpostfix << *it << " ";  // Добавляем лексему и пробел для разделения
//	}
//	return strpostfix.str(); // Возвращаем итоговую строку
//}

//double TPostfix::Calculate()
//{
//	return 0;
//}

//TPostfix::TPostfix(string what_)
//{
//	cout << "WHATTTTTTTTTTTTT IN CPPPPPPPPPPP" << endl;
//}



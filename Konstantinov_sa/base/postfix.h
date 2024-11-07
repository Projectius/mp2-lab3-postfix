#pragma once

#include <string>
#include <vector>
#include "stack.h"
#include <functional>
#include <unordered_map>
#include <sstream>
#include <memory>
#include <iostream>
#include <stdexcept>

using namespace std;

enum LexemeType { lex, var, op, parOpen, parClose };

class Lexeme
{
	
protected:
	bool defined = false;
	string name;
	LexemeType type = LexemeType::lex;
public:
	Lexeme() { throw logic_error("Lexem default constructor should not be called. Invalid container usage."); } // Никогда не должен вызываться, может вызваться при неправильном использовании контейнеров и требуется для сборки
	Lexeme(string name_) :name(name_) { //cout << "HELLO :" << name_ << ":" << endl; 
	};
	Lexeme(string name_, bool defined) :name(name_), defined(defined) {};
	explicit Lexeme(string name_, LexemeType type, bool defined) :name(name_), type(type), defined(defined) {};
	LexemeType getType() { return type; }
	virtual int getPriority() const { return -1; } // Приоритет скобок -1
	string getName() { return name; }
	virtual void define() { defined = true; //cout << defined << " defined " << name << endl; 
	}
	bool isDefined() { return defined; }
	virtual ~Lexeme() {};
	friend ostream& operator<<(ostream&, Lexeme&);
};

ostream& operator<<(ostream& out, Lexeme& lex)
{
	//cout << " <<" << lex.name << endl;
	return out << lex.name;
}

//////////////////////////////////////////
template <class T>
class Variable : public Lexeme
{
	T value;
public:
	Variable(string name) : Lexeme(name, LexemeType::var, false) {}
	void define(T& value_) { value = value_; defined = true; }
	T& getValue() { return value; }
	//T* getValuePointer() { return &value; }
};

//////////////////////////////////////////
enum class Associativity { Left, Right };

template <typename T>
class Operator : public Lexeme
{
	int argCount;
	int priority;
	Associativity associativity;
	function<T(T, T)> operation;

public:
	explicit Operator(string name_, int argCount_, int priority_, function<T(T, T)> operation_, Associativity associativity_ = Associativity::Left)
		: Lexeme(name_, LexemeType::op, true), argCount(argCount_), priority(priority_), operation(move(operation_)), associativity(associativity_)
	{
		if (argCount == 1)
			priority = 255;
		//cout << "Operator " << name_ << " created with priority " << priority_ << " and associativity " << (associativity_ == Associativity::Left ? "Left" : "Right") << endl;
	}

	int getPriority() const override { return priority; }
	int getArgCount() const { return argCount; }
	Associativity getAssociativity() const { return associativity; }

	T Execute(T a, T b = T{}) const {
		return operation(a, b); // Если оператор унарный, b просто игнорируется
	}
};

/////////////////////////////////////////

template <typename T>
class LexBase
{
	unordered_map<string, Lexeme*> map;

public:
	LexBase() = default;

	~LexBase() {
		for (auto& pair : map) {
			delete pair.second;
		}
	}

	Lexeme* addVar(string name)
	{
		auto it = map.find(name);
		if (it == map.end())
		{
			Variable<T>* nvar = new Variable<T>(name);
			map.emplace(name, nvar);
			//cout << "ADDED VAR :" << nvar->getName() << ":" << endl;
			return nvar;
		}
		return it->second;
	}

	Lexeme* addOperator(string name, int argCount, int priority, function<T(T, T)> operation, Associativity associativity = Associativity::Left) {
		Operator<T>* nOp = new Operator<T>(name, argCount, priority, move(operation), associativity);
		map.emplace(name, nOp);
		//cout << "ADDED OPERATOR: " << nOp->getName() << " with priority " << nOp->getPriority() << " and associativity " << (associativity == Associativity::Left ? "Left" : "Right") << endl;
		return nOp;
	}

	Lexeme* getLexeme(string name)
	{
		auto it = map.find(name);
		if (it != map.end())
			return it->second;
		return nullptr;
	}

	void addLexeme(string name, LexemeType type, bool defined = false) { //Только для вспомогательных лексем без функционала
		Lexeme* nlex = new Lexeme(name, type, defined);
		map.emplace(name, nlex);
		//cout << "ADDED LEX :" << lex.getName() << ":" << endl;
	}

	vector<pair<string, Lexeme*>> getAllLexemes() {
		vector<pair<string, Lexeme*>> lexemes;
		for (const auto& pair : map) {
			lexemes.emplace_back(pair.first, pair.second);
		}
		return lexemes;
	}
};


///////////////////////////////////////


template<typename T>
class TPostfix
{
	LexBase<T> base;
	string infix;
	vector<Lexeme*> postfix;
	TStack<Lexeme*> opStack;

public:

	TPostfix(bool importBasicOperators)
	{

		if (importBasicOperators) {
			base.addOperator("+", 2, 0, [](T a, T b) { return a + b; });
			base.addOperator("-", 2, 0, [](T a, T b) { return a - b; });
			base.addOperator("*", 2, 1, [](T a, T b) { return a * b; });
			base.addOperator("/", 2, 1, [](T a, T b) { return a / b; });
			base.addOperator("^", 2, 3, [](T a, T b) {return pow(a, b);}, Associativity::Right);
			base.addOperator("sin", 1, 255, [](T a, T b) {return sin(a);}, Associativity::Right);
			base.addOperator("cos", 1, 255, [](T a, T b) {return cos(a);}, Associativity::Right);

			base.addLexeme("(", LexemeType::parOpen, 1);
			base.addLexeme(")", LexemeType::parClose, 1);
		}
	}

	void inputInfix(string infix_)
	{
		infix = infix_;
	}


	void parseToPostfix() {

		if (infix.empty())
			throw logic_error("Trying to parse empty infix");

		istringstream iss(infix);
		string token;

		while (iss >> token) {
			Lexeme* lex = base.getLexeme(token);
			//cout << "Got token " << token << endl;

			if (lex == nullptr) {
				// Если лексема не найдена, добавляем как новую переменную

				if (token.find_first_of('()') != string::npos)
					throw runtime_error("Variable name or number fused with parentheses");

				postfix.push_back(base.addVar(token));

				//Попытаемся сразу определить если это число
				try {
					double num = stod(token);
					if (!isnan(num))
					{
						setVariable(token, num);
						//cout << "CAN NUM " << num << endl;
					}
				}
				catch (exception e) {}


				////cout << "New var " << endl;
			}
			else {
				// Вывод информации для отладки
				//cout << "Found lexeme: " << lex->getName() << " Type: " << lex->getType() << " Priority: " << lex->getPriority() << endl;
				LexemeType type = lex->getType();

				if (type == LexemeType::var) {
					// Если лексема — переменная, добавляем её в постфиксное выражение
					postfix.push_back(lex);
				}
				else if (type == LexemeType::op) {
					// Если это оператор, обрабатываем приоритеты и ассоциативность
					auto* op = dynamic_cast<Operator<T>*>(lex);
					while (!opStack.empty() && opStack.get_top()->getType() == LexemeType::op) {
						auto* topOp = dynamic_cast<Operator<T>*>(opStack.get_top());

						// Проверка на приоритет и ассоциативность
						bool shouldPop;
						if (op->getAssociativity() == Associativity::Left) {
							shouldPop = topOp->getPriority() >= op->getPriority();
						}
						else { // Правоассоциативный оператор
							shouldPop = topOp->getPriority() > op->getPriority();
						}

						if (shouldPop) {
							postfix.push_back(opStack.get_top());
							opStack.pop();
						}
						else {
							break;
						}
					}
					opStack.push(lex); // Добавляем текущий оператор в стек
				}
				else if (type == LexemeType::parOpen) {
					// Если открывающая скобка, добавляем её в стек операторов
					opStack.push(lex);
				}
				else if (type == LexemeType::parClose) {
					// Если закрывающая скобка, выталкиваем операторы в постфикс до открывающей скобки
					while (!opStack.empty() && opStack.get_top()->getType() != LexemeType::parOpen) {
						postfix.push_back(opStack.get_top());
						opStack.pop();
					}
					if (!opStack.empty() && opStack.get_top()->getType() == LexemeType::parOpen) {
						opStack.pop(); // Убираем открывающую скобку из стека
					}
				}
			}


			//cout << "It was token " << t << endl;
			//cout << "Stack size: " << opStack.get_size() << endl;
			//cout << "Postfix size: " << postfix.size() << endl;
		}

		// Перемещаем оставшиеся операторы в постфикс
		while (!opStack.empty()) {
			if (opStack.get_top()->getType() == LexemeType::parOpen) {
				opStack.pop(); // Удаляем открывающую скобку, если она осталась
			}
			else {
				postfix.push_back(opStack.get_top());
				//cout << "END Popping " << opStack.get_top()->getName() << " stack size " << endl;
				opStack.pop();
			}
		}
	}



	string getInfix() { return infix; }
	string GetPostfix()
	{
		if (!postfix.size())
			throw logic_error("Get empty postfix");
		string res;
		for (int i = 0; i < postfix.size(); i++) {
			//cout << "res " << postfix[i]->getName() << endl;
			res += postfix[i]->getName() + " ";
		}
		return res;
	}

	void addOperator(const string& name, int argCount, int priority, function<T(T, T)> operation) {
		base.addOperator(name, argCount, priority, operation);
	}

	void setVariable(const string& name, T value) {
		Lexeme* lex = base.getLexeme(name);
		if (lex && lex->getType() == LexemeType::var) {
			dynamic_cast<Variable<T>*>(lex)->define(value);
		}
		else {
			//cout << "Variable not found: " << name << endl;
			throw logic_error("Variable not found: " + name);
		}
	}

	void setVariables(const vector<pair<string, T>>& vars) {
		for (const auto& var : vars) {
			setVariable(var.first, var.second);
		}
	}

	vector<string> getUndefinedVars() {
		vector<string> undefinedVars;
		for (const auto& lexemePair : base.getAllLexemes()) {
			Lexeme* lexeme = lexemePair.second;
			if (lexeme->getType() == LexemeType::var) {
				Variable<T>* var = static_cast<Variable<T>*>(lexeme);
				if (!var->isDefined()) {
					undefinedVars.push_back(var->getName());
				}
			}
		}
		return undefinedVars;
	}

	vector<string> getOperatorNames() {
		vector<string > ops;
		for (const auto& lexemePair : base.getAllLexemes()) {
			Lexeme* lexeme = lexemePair.second;
			if (lexeme->getType() == LexemeType::op) {

				ops.push_back(lexeme->getName());

			}
		}
		return ops;
	}

	bool checkForUndefinedVars()
	{
		for (const auto& lexemePair : base.getAllLexemes()) {
			Lexeme* lexeme = lexemePair.second;
			if (lexeme->getType() == LexemeType::var) {
				//Variable<T>* var = static_cast<Variable<T>*>(lexeme);
				if (!lexeme->isDefined()) {
					//cout << "ud "<< lexeme->getName() << endl;
					return true;
				}
			}
		}
		return false;
	}

	T Calculate()
	{
		if (checkForUndefinedVars())
			throw logic_error("Calculating when some variables are not defined");
		if (!postfix.size())
			throw logic_error("Calculate when no postfix");
		TStack<T> calcStack;
		for (auto lexeme : postfix) {
			if (lexeme->getType() == LexemeType::var) {
				// Если лексема — переменная, добавляем её значение в стек
				calcStack.push(static_cast<Variable<T>*>(lexeme)->getValue());
			}
			else if (lexeme->getType() == LexemeType::op) {
				auto* op = static_cast<Operator<T>*>(lexeme);
				int argCount = op->getArgCount();

				if (argCount == 1) {
					// Для унарного оператора извлекаем один операнд
					T operand = calcStack.get_top(); calcStack.pop();
					calcStack.push(op->Execute(operand));
				}
				else if (argCount == 2) {
					// Для бинарного оператора извлекаем два операнда
					T op2 = calcStack.get_top(); calcStack.pop();
					T op1 = calcStack.get_top(); calcStack.pop();

					// Выполняем операцию и помещаем результат в стек
					calcStack.push(op->Execute(op1, op2));
				}
				else {
					throw logic_error("Operator arguments count not supported");
				}
			}
		}
		return calcStack.get_top();
	}
};


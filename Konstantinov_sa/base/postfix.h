#pragma once

#include <string>
#include "stack.h"
#include<functional>
#include<unordered_map>
#include <sstream>
#include <memory>
#include<iostream>

using namespace std;

enum LexemeType {lex, var, op, parOpen, parClose};

class Lexeme
{
	bool defined = false;
	string name;
	LexemeType type = LexemeType::lex;


public:
	Lexeme() { throw logic_error("Lexem default constructor should not be called. Invalid container usage."); } // Никогда не должен вызываться, может вызваться при неправильном использовании контейнеров и требуется для сборки
	Lexeme(string name_) :name(name_) { cout << "HELLO :" << name_ << ":" << endl; 
	};
	Lexeme(string name_, bool defined) :name(name_),defined(defined) {};
	explicit Lexeme(string name_, LexemeType type) :name(name_), type(type), defined(1) {}; //Только для специальных лексем, для которых важно только имя и тип - скобок
	LexemeType getType() { return type; }
	virtual int getPriority() const { return -1; } //Приоритет скобок -1
	string getName() { return name; }
	virtual void define() { defined = true; }
	bool isDefined() { return defined; }
	virtual ~Lexeme() {};
	friend ostream& operator<<(ostream& out, Lexeme& lex);
};

ostream& operator<<(ostream& out, Lexeme& lex)
{
	cout<<" <<"<< lex.name<<endl;
	return out << lex.name;
}

//////////////////////////////////////////
template <class T>
class Variable : public Lexeme
{
	T value;
public:
	Variable(string name) :Lexeme(name, LexemeType::var) {}
	void define(T& value_) { value = value_; defined = true; }
	T& getValue() { return value; }
};

//////////////////////////////////////////
template <typename Func>
class Operator : public Lexeme
{
	int argCount;
	int priority;
	Func operation;
public:
	//explicit Operator(string name, int argCount, int priority = 0) : Lexeme(name), argCount(argCount), priority(priority) {}
	explicit Operator(string name_, int argCount_, int priority_, Func&& operation_) : Lexeme(name_, LexemeType::op), argCount(argCount_), priority(priority_), operation(move(operation_)) { cout << "prior " << priority; }
	//void define(Func&& operation_) { operation = move(operation_); defined = true; }

	int getPriority() const override { cout << "PRIORITY" << endl; return priority; }

	template<typename... Args>
	auto Execute(Args&&... args) const {
		// forward сохраняет оригинальные характеристики аргументов (r-value или l-value)
		return operation(forward<Args>(args)...);
	}

};


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
            cout << "ADDED VAR :" << nvar->getName() << ":" << endl;
            return nvar;
        }
        return it->second;
    }

    template <typename Func>
    Lexeme* addOperator(string name, int argCount, int priority, Func&& operation) {
        using OperatorType = Operator<Func>;
        OperatorType* nOp = new OperatorType(name, argCount, priority, forward<Func>(operation));
        map.emplace(name, nOp);

        cout << "ADDED OPERATOR: " << nOp->getName() << " with priority " << nOp->getPriority() << endl;
        return nOp;
    }

    Lexeme* getLexeme(string name)
    {
        auto it = map.find(name);
        if (it != map.end())
            return it->second;
        return nullptr;
    }

    void addLexeme(Lexeme& lex) { 
        Lexeme* nlex = new Lexeme(lex);
        map.emplace(lex.getName(), nlex);
        cout << "ADDED LEX :" << lex.getName() << ":" << endl;
    }
};

template<typename T>
class TPostfix
{
	LexBase<T> base;
	string infix;
	vector<Lexeme*> postfix;
	TStack<Lexeme*> opStack;

    void parseToPostfix() {
        istringstream iss(infix);
        string token;
        int t = 0;

        while (iss >> token) {
            Lexeme* lex = base.getLexeme(token);
            cout << "Got token " << token << endl;

            if (lex == nullptr) {
                // Если лексема не найдена, добавляем как новую переменную
                postfix.push_back(base.addVar(token));
                cout << "New var " << endl;
            }
            else {
                // Вывод информации для отладки
                cout << "Found lexeme: " << lex->getName() << " Type: " << lex->getType() << " Priority: " << lex->getPriority() << endl;
                LexemeType type = lex->getType();

                if (type == LexemeType::var) {
                    // Если лексема — переменная, добавляем её в постфиксное выражение
                    postfix.push_back(lex);
                }
                else if (type == op) {
                    // Если это оператор, обрабатываем приоритеты
                    while (!opStack.empty() && opStack.get_top()->getType() == op &&
                        opStack.get_top()->getPriority() >= lex->getPriority()) {
                        // Перемещаем оператор с более высоким или равным приоритетом в постфикс
                        postfix.push_back(opStack.get_top());
                        opStack.pop();
                    }
                    opStack.push(lex); // Добавляем текущий оператор в стек
                }
                else if (type == parOpen) {
                    // Если открывающая скобка, добавляем её в стек операторов
                    opStack.push(lex);
                }
                else if (type == parClose) {
                    // Если закрывающая скобка, выталкиваем операторы в постфикс до открывающей скобки
                    while (!opStack.empty() && opStack.get_top()->getType() != parOpen) {
                        postfix.push_back(opStack.get_top());
                        opStack.pop();
                    }
                    if (!opStack.empty()) {
                        opStack.pop(); // Убираем открывающую скобку из стека
                    }
                }
            }

            // Вывод текущего состояния для отладки
            cout << "It was token " << t << endl;
            cout << "Stack size: " << opStack.get_size() << endl;
            cout << "Postfix size: " << postfix.size() << endl;
            if (!postfix.empty())
                cout << "Last in postfix: " << postfix.back() << endl;
            ++t;
        }

        // Перемещаем оставшиеся операторы в постфикс
        while (!opStack.empty()) {
            postfix.push_back(opStack.get_top());
            cout << "END Popping " << *opStack.get_top() << " stack size " << endl;
            opStack.pop();
        }
    }
public:

    TPostfix(string infix_, bool importBasicOperators = true)
    {
        infix = infix_;
        if (importBasicOperators) {
            base.addOperator("+", 2, 0, [](T a, T b) { return a + b; });
            base.addOperator("-", 2, 0, [](T a, T b) { return a - b; });
            base.addOperator("*", 2, 1, [](T a, T b) { return a * b; });
            base.addOperator("/", 2, 1, [](T a, T b) { return a / b; });

            base.addLexeme(Lexeme("(", parOpen));
            base.addLexeme(Lexeme(")", parClose));

        }
        cout << "POSTFIX START" << endl;
        parseToPostfix();
        cout << "RESULT:" << endl;
        cout << GetPostfix() << endl;

    }
	string GetInfix() { return infix; }
    string GetPostfix()
    {
        string res;
        for (int i = 0; i < postfix.size();i++) {
            cout << "res " << postfix[i]->getName() << endl;
            res += postfix[i]->getName();
        }
        return res;
    }
	string ToPostfix();
	T Calculate(); // Ввод переменных, вычисление по постфиксной форме
};




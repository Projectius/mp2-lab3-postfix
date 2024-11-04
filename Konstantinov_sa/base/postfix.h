#ifndef __POSTFIX_H__
#define __POSTFIX_H__

#include <string>
#include "stack.h"
#include<functional>

using namespace std;

class Lexeme
{
	bool defined = false;
	string name;
public:
	Lexeme(string name_) :name(name_) {};
	Lexeme(string name_, bool defined) :name(name_),defined(defined) {};
	virtual void define() { defined = true; }
	bool isDefined() { return defined; }
	virtual ~Lexeme() {};
	friend ostream& operator<<(ostream& out, Lexeme& lex);
};

ostream& operator<<(ostream& out, Lexeme& lex)
{
	return out << lex.name;
}

//////////////////////////////////////////
template <class T>
class Variable : public Lexeme
{
	T value;
public:
	Variable(string name) :Lexeme(name) {};
	void define(T& value_) { value = value_; defined = true; }
	T& getValue() { return value; }
};

//////////////////////////////////////////
template <typename Func>
class Operator : public Lexeme
{
	Func operation;
public:
	explicit Operator(string name, int argCount) : Lexeme(name) {}
	explicit Operator(string name, int argCount, Func&& operation) : Lexeme(name, true), operation(move(operation)) {}
	void define(Func&& operation_) { operation = move(operation_); defined = true; }

	template<typename... Args>
	auto Execute(Args&&... args) const {
		// forward сохраняет оригинальные характеристики аргументов (r-value или l-value)
		return operation(forward<Args>(args)...);
	}

};

class TPostfix
{
	string infix;
	string postfix;
public:
	TPostfix()
	{
		infix = "a + b";
	}
	string GetInfix() { return infix; }
	string GetPostfix() { return postfix; }
	string ToPostfix();
	double Calculate(); // Ввод переменных, вычисление по постфиксной форме
};

#endif

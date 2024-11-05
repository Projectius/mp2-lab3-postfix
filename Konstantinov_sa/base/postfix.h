﻿#pragma once

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
    bool defined = false;
    string name;
    LexemeType type = LexemeType::lex;

public:
    Lexeme() { throw logic_error("Lexem default constructor should not be called. Invalid container usage."); } // Никогда не должен вызываться, может вызваться при неправильном использовании контейнеров и требуется для сборки
    Lexeme(string name_) :name(name_) { cout << "HELLO :" << name_ << ":" << endl; };
    Lexeme(string name_, bool defined) :name(name_), defined(defined) {};
    explicit Lexeme(string name_, LexemeType type) :name(name_), type(type), defined(1) {}; // Только для специальных лексем, для которых важно только имя и тип - скобок
    LexemeType getType() { return type; }
    virtual int getPriority() const { return -1; } // Приоритет скобок -1
    string getName() { return name; }
    virtual void define() { defined = true; }
    bool isDefined() { return defined; }
    virtual ~Lexeme() {};
    friend ostream& operator<<(ostream& out, Lexeme& lex);
};

ostream& operator<<(ostream& out, Lexeme& lex)
{
    cout << " <<" << lex.name << endl;
    return out << lex.name;
}

//////////////////////////////////////////
template <class T>
class Variable : public Lexeme
{
    T value;
public:
    Variable(string name) : Lexeme(name, LexemeType::var) {}
    void define(T& value_) { value = value_; static_cast<Lexeme>(*this).define(); }
    T& getValue() { return value; }
};

//////////////////////////////////////////
template <typename T>
class Operator : public Lexeme
{
    int argCount;
    int priority;
    std::function<T(T, T)> operation;
public:
    explicit Operator(string name_, int argCount_, int priority_, std::function<T(T, T)> operation_)
        : Lexeme(name_, LexemeType::op), argCount(argCount_), priority(priority_), operation(move(operation_))
    {
        cout << "prior " << priority;
    }

    int getPriority() const override { return priority; }
    int getArgCount() const { return argCount; }

    T Execute(T a, T b) const {
        return operation(a, b);
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

    Lexeme* addOperator(string name, int argCount, int priority, std::function<T(T, T)> operation) {
        Operator<T>* nOp = new Operator<T>(name, argCount, priority, move(operation));
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

    vector<pair<string, Lexeme*>> getAllLexemes() {
        vector<pair<string, Lexeme*>> lexemes;
        for (const auto& pair : map) {
            lexemes.emplace_back(pair.first, pair.second);
        }
        return lexemes;
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
                    if (!opStack.empty() && opStack.get_top()->getType() == parOpen) {
                        opStack.pop(); // Убираем открывающую скобку из стека
                    }
                }
            }

            // Вывод текущего состояния для отладки
            cout << "It was token " << t << endl;
            cout << "Stack size: " << opStack.get_size() << endl;
            cout << "Postfix size: " << postfix.size() << endl;
            if (!postfix.empty())
                cout << "Last in postfix: " << postfix.back()->getName() << endl;
            ++t;
        }

        // Перемещаем оставшиеся операторы в постфикс
        while (!opStack.empty()) {
            if (opStack.get_top()->getType() == parOpen) {
                opStack.pop(); // Удаляем открывающую скобку, если она осталась
            }
            else {
                postfix.push_back(opStack.get_top());
                cout << "END Popping " << *opStack.get_top() << " stack size " << endl;
                opStack.pop();
            }
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
        for (int i = 0; i < postfix.size(); i++) {
            cout << "res " << postfix[i]->getName() << endl;
            res += postfix[i]->getName();
        }
        return res;
    }
    string ToPostfix();

    void addOperator(const string& name, int argCount, int priority, function<T(T, T)> operation) {
        base.addOperator(name, argCount, priority, operation);
    }

    void setVariable(const string& name, T value) {
        Lexeme* lex = base.getLexeme(name);
        if (lex && lex->getType() == LexemeType::var) {
            dynamic_cast<Variable<T>*>(lex)->define(value);
        }
        else {
            cout << "Variable not found: " << name << endl;
            throw logic_error("Variable not found: " + name);
        }
    }

    void setVariables(const vector<pair<string, T>>& vars) {
        for (const auto& var : vars) {
            setVariable(var.first, var.second);
        }
    }

    vector<pair<string, T*>> getUndefinedVars() {
        vector<pair<string, T*>> undefinedVars;
        for (const auto& lexemePair : base.getAllLexemes()) {
            Lexeme* lexeme = lexemePair.second;
            if (lexeme->getType() == LexemeType::var) {
                Variable<T>* var = static_cast<Variable<T>*>(lexeme);
                if (!var->isDefined()) {
                    undefinedVars.push_back({ var->getName(), var });
                }
            }
        }
        return undefinedVars;
    }

    T Calculate()
    {
        TStack<T> calcStack;
        for (auto lexeme : postfix) {
            if (lexeme->getType() == LexemeType::var) {
                calcStack.push(dynamic_cast<Variable<T>*>(lexeme)->getValue());
            }
            else if (lexeme->getType() == LexemeType::op) {
                T op2 = calcStack.get_top(); calcStack.pop();
                T op1 = calcStack.get_top(); calcStack.pop();
                calcStack.push(dynamic_cast<Operator<T>*>(lexeme)->Execute(op1, op2));
            }
        }
        return calcStack.get_top();
    }
};


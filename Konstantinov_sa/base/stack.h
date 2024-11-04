#ifndef __STACK_H__
#define __STACK_H__

const int MaxStackSize = 100;

template <class T>
class TStack
{
	T* pMem;
	int size;
	int top;

	void resize(int nsize)
	{
		if ((nsize < 1) || (nsize > MaxStackSize))
			throw "Stack resize past limit";
		T* nMem = new T[nsize];
		if (nMem == nullptr)
			throw "Stack memory allocation error";
		memcpy(nMem, pMem, size * sizeof(T));
		delete[] pMem;
		pMem = nMem;
		if (nsize < size)
			top = nsize - 1;
		size = nsize;

	}
public:
	TStack(int _size)
	{
		size = _size;
		top = -1;
		if ((size < 1) || (size > MaxStackSize))
			throw "Invalid stack size";
		pMem = new T[size];
	}
	~TStack()
	{
		delete[] pMem;
	}

	void push(const T &ob)
	{
		if (top >= size - 1)
			resize(size*2);
		pMem[++top] = ob;
	}

	void pop() { 
		if (top == -1)
			throw "Pop empty stack";
		top--; 
	}
	T get_top() { return pMem[top]; }


};

#endif

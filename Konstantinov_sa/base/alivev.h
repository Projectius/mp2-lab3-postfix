#pragma once

class Alive
{
	int me;
public:
	Alive(int me_);
};

Alive::Alive(int me_)
{
	me = me_;//cout << "ALIVE!!!!" << endl;
}

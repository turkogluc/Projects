#ifndef TOY_H
#define TOY_H

#include <iostream>
#include <string>
#include <sstream>


using namespace std;

class Toy:public Book{
protected:
	bool battery_flag;

public:
	Toy(); // default constructor
	Toy(string , float, float , float, bool);
	
	bool getContainsBattery() const;
	void setContainsBattery(bool);
	string printAll();

};

#endif

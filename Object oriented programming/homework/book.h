#ifndef BOOK_H
#define BOOK_H

#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <sstream>

using namespace std;

class Book{
protected:
	string label;
	float weight;
	float lenght;
	float width;
public:
	Book();	//default constructor
	Book(string , float, float , float);

	// getter methods
	string getLabel() const;
	float getWeight() const;
	float getLenght() const;
	float getWidth() const;

	// setter methods
	void setLabel(string);
	void setWeight(float);
	void setLenght(float);
	void setWidth(float);

	string printAll();

	
};
#endif

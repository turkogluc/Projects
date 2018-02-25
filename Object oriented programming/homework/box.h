#ifndef BOX_H
#define BOX_H

#include <iostream>
#include <string>


using namespace std;

template<class Type>
class Box : public Book{
protected:
	int max_allowed_weight;
	int counter;
	Type *stack;
public:
	Box(float,float,float,float);
	void add(Type);
	void add(Type *,int);
	void printAll();
	int calculate_total_weight();
	string operator[](int);		// operator overloading
	friend ostream &operator<<( ostream &output,const Type &var ){
		// overload
         	return output;
	}
	

};

#endif

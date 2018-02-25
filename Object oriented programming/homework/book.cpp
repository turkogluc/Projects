#include <iostream>
#include "book.h"


Book::Book(){
	label = " " ;
	weight = 0;
	lenght = 0;
	width = 0 ;
}

Book::Book(string lbl, float wght, float len, float wdth){

	label = lbl ;
	weight = wght;
	lenght = len;
	width = wdth ;
}

string Book::getLabel() const{
	return label;
}
float Book::getWeight() const{
	return weight;
}
float Book::getLenght()const{
	return lenght;
}
float Book::getWidth()const{
	return width;
}

void Book::setLabel(string lbl){
	label = lbl;
}
void Book::setWeight(float wght){
	weight = wght ;
}
void Book::setLenght(float len){
	lenght = len;
}
void Book::setWidth(float wdth){
	width = wdth ;
}

string Book::printAll(){
	string res;	
	ostringstream buff;
	buff << "Book Label: " << label << " # " << lenght << "x" << width << " " << weight << "kg" ;
	res = buff.str();
	
	return res;
}


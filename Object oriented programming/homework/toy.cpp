#include <iostream>
#include <string>
#include "toy.h"

Toy::Toy():Book(){
	battery_flag = false ;
}

Toy::Toy(string lbl, float wght, float len, float wdth,bool battry): Book(lbl,wght,len,wdth) {
	battery_flag = battry ;
}

bool Toy::getContainsBattery() const{
	return battery_flag;
}
void Toy::setContainsBattery(bool bt){
	battery_flag = bt;
}

string Toy::printAll(){
	string res;
	ostringstream buff;
	buff << "Toy Label: " << label << " # " << lenght << "x" << width << " " << weight << "kg" ;
	res = buff.str();
	if(battery_flag)
		res = res + " Contains Battery ";
	else
		res = res + " No Battery ";

	return res;
}

#include <iostream>
#include "box.h"

template<class Type>
Box<Type>::Box(float wght,float len,float wdth,float max):Book("empty",wght,len,wdth){
	max_allowed_weight = max ;
	stack = NULL;
	counter = 0;
}

template<class Type>
void Box<Type>::add(Type var){
	
	// kabul kosulları
	bool flag = true ;

	if( (calculate_total_weight() + var.getWeight()) > max_allowed_weight ){
     		string mes = "The total weight of the contained objects including the box may ";
		mes = mes + "not exceed the maximum allowed weight for the box!	";
		throw mes;
		flag = false;
	}
	if( (var.getWidth() > width) || (var.getLenght() > lenght) ){
		if( (var.getWidth() <= lenght) && (var.getLenght() <= width) ){
			// girebilir
		}else{
			string mes = "The dimensions of the contained object should be equal or smaller than those of the box!"	;		
			throw mes ;
			flag = false;
		}
	}

	if(flag){
		if(stack == NULL){
			stack = new Type ;
			stack[0] = var;
			counter += 1;
		}else{
			Type *temp = new Type[counter+1];
			int i;		
			for(i=0;i<counter;i++){
				temp[i] = stack[i];
			}
			temp[counter] = var ;

			stack = new Type[counter+1];
			// copy back to stack
			for(i=0;i<counter+1;i++){
				stack[i] = temp[i];
			}
			delete [] temp;
			counter += 1;
		}	
	}
}

template<class Type>
void Box<Type>::add(Type *var,int n){
	int i;
	for(i=0;i<n;i++){
		add(var[i]);
	}

}

template<class Type>
void Box<Type>::printAll(){
	int i;
	for(i=0;i<counter;i++){
		stack[i].printAll();	
	}
}

template<class Type>
int Box<Type>::calculate_total_weight(){
	int i,total=0;
	for(i=0;i<counter;i++){
		total = total + stack[i].getWeight();	
	}
	return total;
}

template<class Type>
string Box<Type>::operator[](int i){
	if( i >= counter ){
		string mes = "Index out of bounds!";
		throw mes;
	}else{
		return stack[i].printAll();		
	}

}


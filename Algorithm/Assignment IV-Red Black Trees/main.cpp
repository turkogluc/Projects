/*
 *  Cemal Turkoglu - 150140719
 */



#include<iostream>
#include<fstream>
#include<cstring>
#include<cstdlib>
#include<vector>
#include<string>

using namespace std;


void readFile(){
	FILE *fp;
    
    
    fp = fopen("input.txt", "r");
    if (fp == NULL) {
        perror("Error");
    }
    else {
        while(!feof(fp)) {
            /* 
             * 		%[^\t] - any character except a TAB
             */
             
             char name[10],age[3];
			 char gender[2];

            fscanf(fp,"%s\t%s\t%s\n", name, gender, age);
            cout << name << "\t" << gender << "\t" << age << endl;
        }		
    }
    fclose(fp);
}


int main(int argc, char *argv[]){

	readFile();


	return 0;
}

/*
 *  Cemal Turkoglu - 150140719
 */

#define M 131071

#include<iostream>
#include<fstream>
#include<cstring>
#include<cstdlib>
#include<vector>
#include<fstream>
#include<ctime>
#include<cmath>
#include<string>

using namespace std;

char inputFile[20] = "ds-set-input.txt" ;
char lookupFile[20] = "ds-set-lookup.txt";
char outputDict[50] = "ds-set-output-dict.txt";
char outputList[50] = "ds-set-output-list.txt";

typedef struct mylist{
	string pageNo;
	string lineNo;
	string index;
	char character;
	struct mylist *next;
	
}liste;

class bookCharacter{
	public:
		string pageNo;
		string lineNo;
		string index;
		char character;
		string key;
		bool located;
		bookCharacter();
		bookCharacter(string,string,string,char);
		string generateUniqueKey();

};

bookCharacter::bookCharacter(){
		this->located = false;
}

bookCharacter::bookCharacter(string pageNo,string lineNo,string index,char character){
	this->pageNo = pageNo ;
	this->lineNo = lineNo ;
	this->index  = index ;
	this->character = character;
	generateUniqueKey();
	this->located = false ;
	
}

string bookCharacter::generateUniqueKey(){
	this->key = this->pageNo + this->lineNo + this->index ;
	return this->key;
}

class dictionary{
	public:
		clock_t t;
		int collusion,sumCol;
		bookCharacter hashTable[M];
		dictionary();
		int hashFunction(string key);
		int probingFunction(int hFirst);
		int insert(int index,bookCharacter *ch);
};

dictionary::dictionary(){
	collusion = 0;
	sumCol = 0;
	
    FILE *fp;
    
    char page[5],line[3],index[3];
    char character;
    
	t = clock();
	
    fp = fopen(inputFile, "r");
    if (fp == NULL) {
        perror("Error");
    }
    else {
        while(!feof(fp)) {
            /* 
             * 		%[^\t] - any character except a TAB
             */

            fscanf(fp,"%[^\t]\t%[^\t]\t%[^\t]\t%c\n", page, line, index, &character);
            
            
            bookCharacter *temp = new bookCharacter(page,line,index,character);
//cout << page << " " <<  line << " " << index << " " << character << " " << "is going to be add" << endl ;         
            insert(hashFunction(temp->key),temp);
            delete(temp);
            
        }		
    }
    fclose(fp);
    t = clock() - t;
    cout << "insertion time for dictionary = " << t << endl;
    cout << "total collusion = " << sumCol << endl;
	
}

int dictionary::insert(int index,bookCharacter *ch){
	this->collusion = 0;
	if(! this->hashTable[index].located){
cout << "table[" << index << "] is empty and filled. " << endl ;		
		hashTable[index].pageNo = ch->pageNo ;
		hashTable[index].lineNo = ch->lineNo ;
		hashTable[index].index = ch->index ;
		hashTable[index].character = ch->character ;
		hashTable[index].key = ch->key ;
		hashTable[index].located = true ;
		return 1;
		
	}else{

		this->collusion++ ;
		this->sumCol++;
cout << "table[" << index << "] is not empty. collosion = " << collusion << "sumCol = " << sumCol << endl ;		
		int newindex  = probingFunction(index);
		hashTable[newindex].pageNo = ch->pageNo ;
		hashTable[newindex].lineNo = ch->lineNo ;
		hashTable[newindex].index = ch->index ;
		hashTable[newindex].character = ch->character ;
		hashTable[newindex].key = ch->key ;
		hashTable[newindex].located = true ;
		this->collusion = 0;
		return 1;
	}
}

int dictionary::hashFunction(string key){
	int k = atoi(key.c_str());
	
	int Mx = M ;
	return floor( Mx * fmod(k * 0.618,1) );
}

int dictionary::probingFunction(int hFirst){
	int test = fmod( hFirst + 7*(this->collusion) + 3*this->collusion*this->collusion , M ) ;
	if(! this->hashTable[test].located){
cout << "table[" << test << "] is empty and filled. " << endl ;
		return test;
	}else{
		this->collusion++ ;
		this->sumCol++;
cout << "table[" << test << "] is not empty. collosion = " << collusion << "sumCol = " << sumCol << endl ;	
		return probingFunction(test);
	}
}



class list{
	public:
		clock_t t;
		liste *head;
		liste *last;
		list();
		
		
		void insert(bookCharacter *ch);
		
		
		
		
		void lookup(); 
		
	
	
};
list::list(){
	head = NULL;
	last = head;
	
	
	FILE *fp;
    
    char page[5],line[3],index[3];
    char character;
    t = clock();
    fp = fopen(inputFile, "r");
    if (fp == NULL) {
        perror("Error");
    }
    else {
        while(!feof(fp)) {
            /* 
             * 		%[^\t] - any character except a TAB
             */

            fscanf(fp,"%[^\t]\t%[^\t]\t%[^\t]\t%c\n", page, line, index, &character);
            
            
            bookCharacter *temp = new bookCharacter(page,line,index,character);
            insert(temp);
            
            
            delete(temp);
            
        }		
    }
    fclose(fp);
    t = clock() - t;
    cout << "insertion time for list = " << t << endl;
    

/*
	liste *tmp = head;
	while(tmp->next != NULL){
		cout << tmp->pageNo << " " << tmp->lineNo << " " << tmp->index << " " << tmp->character << endl;
		tmp = tmp->next;
	}
	cout << tmp->pageNo << " " << tmp->lineNo << " " << tmp->index << " " << tmp->character << endl;
*/

	cout << "bitii";
	t = clock();
    lookup();
    t = clock() - t;
	cout << "lookup time for list = " << t << endl;
	
}

void list::insert(bookCharacter *ch){
	
		if(head == NULL){
			cout << "yes";
			liste *ptr = new liste;
			ptr->pageNo = ch->pageNo;
			ptr->lineNo = ch->lineNo;
			ptr->index = ch->index;
			ptr->character = ch->character;
			head = ptr;
			last = head;
			head->next = NULL;
			last->next = NULL;
		}else{
			liste *ptr = new liste;
			ptr->pageNo = ch->pageNo;
			ptr->lineNo = ch->lineNo;
			ptr->index = ch->index;
			ptr->character = ch->character;
			last->next = ptr ;
			last = ptr;
			last->next = NULL;
		}
}

void list::lookup(){
	
	FILE *fp,*fpout;
	
    char page[5],line[3],index[3];
    int count  = 0;
    
	fp = fopen(lookupFile, "r");
	fpout = fopen(outputList,"w");
	liste *ptr = head;
    if (fp == NULL || fpout == NULL) {
        perror("Error");
    }
    else {
        while(!feof(fp)) {
            /* 
             * 		%[^\t] - any character except a TAB
             */
             
            fscanf(fp,"%[^\t]\t%[^\t]\t%[^\t]\n", page, line, index);
            
            while(ptr->next != NULL)
				if((ptr->pageNo).compare(string(page)) == 0 && (ptr->lineNo).compare(string(line)) == 0 && (ptr->index).compare(string(index)) == 0){
					cout << ptr->character;
					
				}
				ptr = ptr->next;
			}
			
    }		
    
    cout << endl << endl << "bitti" << endl;

    fclose(fp);
}



int main(int argc, char *argv[]){
	
	//dictionary *test = new dictionary();
	
	dictionary *dic = new dictionary();
	
	list *test = new list();
	return 0;
  
}

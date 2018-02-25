/*
 *  Cemal Turkoglu - 150140719
 */


#include<iostream>
#include<cstring>
#include<cstdlib>
#include<vector>
#include<fstream>
#include<cmath>
using namespace std;

typedef struct Cards{
    string name;
    string cclass;
    string rarity;
    string set;
    string type;
    int cost;
}Cards; 

class CardManager{
        string sortProcedure;
        string sortAlgorithm;
        string inputFile;
        string outputFile;
    public:
        vector<Cards> cardList;
        CardManager(string sortProcedure,string sortAlgorithm,string inputFile,string outputFile);
        void fullSort();
        void filterSort();
        void insertionSort();
        void mergeSort(int p,int r);
        void merge(int p,int q,int r,int flag);
        void readFile(string inputFile);
        void writeFile(string outputFile);
        void addElement(Cards crd);
};

CardManager::CardManager(string sortProcedure,string sortAlgorithm,string inputFile,string outputFile){
	this->sortProcedure = sortProcedure;
	this->sortAlgorithm = sortAlgorithm;
	this->inputFile = inputFile;
	this->outputFile = outputFile;
	cardList.clear();
	readFile(this->inputFile);
}
void CardManager::fullSort(){
	// insertion sort
	if(!sortAlgorithm.compare("-i")){
			insertionSort();
				
	}
	// merge sort
	else if(!sortAlgorithm.compare("-m")){
            int see = cardList.size() ;
            see = see-1;
			mergeSort(0,see);		
	}
	writeFile(outputFile);
	
}
void CardManager::filterSort(){
	// insertion sort
	if(!sortAlgorithm.compare("-i")){
			insertionSort();
				
	}
	// merge sort
	else if(!sortAlgorithm.compare("-m")){
			mergeSort(0,cardList.size()-1);
				
	}
	writeFile(outputFile);
}
void CardManager::insertionSort(){
	if(!sortProcedure.compare("-full")){
            int i,j;
            Cards temp;
            for(j=1; j<cardList.size();j++){
                temp = cardList[j];
                i = j-1;
                while(i >= 0 && (cardList[i].cclass).compare(temp.cclass) > 0){
                    cardList[i+1] = cardList[i];
                    i=i-1;
                }
                while(i >= 0 && (cardList[i].cost > temp.cost) && ((cardList[i].cclass).compare(temp.cclass) == 0)){
                    cardList[i+1] = cardList[i];
                    i=i-1;
                }
                while(i >= 0 && (cardList[i].name.compare(temp.name) > 0) && (cardList[i].cost == temp.cost) && (cardList[i].cclass.compare(temp.cclass) == 0) ){
                    cardList[i+1] = cardList[i];
                    i=i-1;
                }
                cardList[i+1] = temp;

            }
	}else if(!sortProcedure.compare("-filter")){
            int i,j;
            Cards temp;
            for(j=1; j<cardList.size();j++){
                temp = cardList[j];
                i = j-1;
                while(i >= 0 && (cardList[i].type).compare(temp.type) > 0){
                    cardList[i+1] = cardList[i];
                    i=i-1;
                }
                cardList[i+1] = temp;
            }
	}
	
}
void CardManager::mergeSort(int p,int r){
    int q,flag;
    if(p<r){
        q = floor((p+r)/2) ;
        mergeSort(p,q);
        mergeSort(q+1,r);
        flag;
        if(!sortProcedure.compare("-full")){
            flag = 1;
            merge(p,q,r,flag);
        }else if(!sortProcedure.compare("-filter")){
            flag = 2;
            merge(p,q,r,flag);
        }
    }
}

void CardManager::merge(int p,int q,int r,int flag){
    int n1 = q-p+1 ;
    int n2 = r-q ;
    vector<Cards> L(n1+1) ;
    vector<Cards> R(n2+1) ;
    for(int i=0;i<n1;i++){
        L[i] = cardList[p+i];
    }
    for(int j=0;j<n2;j++){
        R[j] = cardList[q+j+1];
    }
    
    Cards infinite;
    string inf("zzzzzzzzz");
    infinite.cclass     = inf;
    infinite.name       = inf;
    infinite.rarity     = inf;
    infinite.set        = inf;
    infinite.type       = inf;
    infinite.cost       = 9999;

    L[n1] = infinite;
    R[n2] = infinite;
    
    // full sort
    if(flag==1){
        int i=0;
        int j=0;
        for(int k=p;k<=r;k++){
            if(strcasecmp(L[i].cclass.c_str() , R[j].cclass.c_str()) < 0){
                cardList[k] = L[i];
                i = i+1;
            }else if(L[i].cclass == R[j].cclass && L[i].cost < R[j].cost){
                cardList[k] = L[i];
                i = i+1;
            }else if(L[i].cclass == R[j].cclass && L[i].cost == R[j].cost && strcasecmp(L[i].name.c_str() , R[j].name.c_str()) <= 0){
                cardList[k] = L[i];
                i = i+1;
            }else{
                cardList[k] = R[j];
                j = j+1;
            }
        }
        
        
    }
    // filter sort
    else if(flag==2){
        int i=0;
        int j=0;
        for(int k=p;k<=r;k++){
            if(L[i].type <= R[j].type){
                cardList[k] = L[i];
                i = i+1;
            }else{
                cardList[k] = R[j];
                j = j+1;
            }
        }
    }
}

void CardManager::readFile(string inputFile){
    FILE *fp;
    Cards temp;
    char name[50],cclass[50],rarity[50],set[50],type[50];
    int cost;
	
    fp = fopen(inputFile.c_str(), "r");
    if (fp == NULL) {
        perror("Error");
    }
    else {
        while(!feof(fp)) {
            /* 
             * 		%[^\t] - any character except a TAB
             */

            fscanf(fp,"%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%[^\t]\t%d\n", name, cclass, rarity, set, type, &cost);
            temp.name = string(name);
            temp.cclass = string(cclass);
            temp.rarity = string(rarity);
            temp.set = string(set);
            temp.type = string(type);
            temp.cost = cost;
            addElement(temp);
        }		
    }
    fclose(fp);
	


}
void CardManager::writeFile(string outputFile){
    FILE *fp;
    fp = fopen(outputFile.c_str(), "w");
    int size = cardList.size();
    for(int i=0;i<size;i++){
        fprintf(fp,"%s\t%s\t%s\t%s\t%s\t%d\n",cardList[i].name.c_str(), cardList[i].cclass.c_str(), cardList[i].rarity.c_str(), cardList[i].set.c_str(), cardList[i].type.c_str(), cardList[i].cost);
    }
    fclose(fp);
	
}
void CardManager::addElement(Cards crd){
	cardList.push_back(crd);
}


void printUsage(){
  std::cout << "Usage: [ -full || filter ] [ -i || -m ] [ input file ] [ outputfile ] " << '\n';
}


int main(int argc, char *argv[]){
  
    if(argc != 5){
        printUsage();
        exit(0);
    }else if(strcmp(argv[1],"-full") != 0 &&  strcmp(argv[1],"-filter") != 0){
        printUsage();
        exit(0);
    }else if(strcmp(argv[2],"-i") != 0 &&  strcmp(argv[2],"-m") != 0){
        printUsage(); 
        exit(0);
    }

    string sortProcedure = argv[1];
    string sortAlgorithm = argv[2];
    string inputFile = argv[3];
    string outputFile = argv[4];

    CardManager *test = new CardManager(sortProcedure,sortAlgorithm,inputFile,outputFile);
	

    time_t start,end;
    time (&start);

    if(!sortProcedure.compare("-full")){
        test->fullSort();
    }else if(!sortProcedure.compare("-filter")){
        test->filterSort();
    }
    
	time (&end);
	double dif = difftime (end,start);
    cout << "Elapsed time : " << dif << " seconds." << endl ;


//    for(int i=0; i<test->cardList.size();i++){
//        cout << test->cardList[i].name <<"\t" <<test->cardList[i].cclass <<"\t"<< test->cardList[i].rarity <<"\t"<< test->cardList[i].set<<"\t" << test->cardList[i].type<<"\t" << test->cardList[i].cost << endl ;
//        //cout << test->cardList[i].cclass << endl;
//    }
	  

  return 0;
  
}

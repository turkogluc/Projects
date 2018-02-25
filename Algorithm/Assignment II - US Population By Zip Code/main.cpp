/*
 *  Cemal Turkoglu - 150140719
 */


#include<iostream>
#include<fstream>
#include<cstring>
#include<cstdlib>
#include<vector>
#include<fstream>
#include<ctime>
#include<cmath>
using namespace std;

typedef struct Data{
	string population;
	string minAge;
	string maxAge;
	string gender;
	string zipCode;
	string geoId;
}data;


class residenceManager{
	public:
		int N;
		vector<data> dataList;
		residenceManager(int N);
		void readFile();
		void quickSort(int p,int r);
		int Partition(int p,int r);
		void swap(data *A,data *B);
		void writeFile();
		void traverse();
	
};

residenceManager::residenceManager(int N){
	
	this->N = N;
	dataList.clear();
	readFile();
	//traverse();
	
	int p = 0;
	int r = dataList.size()-1;
	quickSort(p,r);
	writeFile();
	
}

void residenceManager::readFile(){
  string line;
  int counter = 0;
  data newdata;
  
  ifstream myfile( "population_by_zip_2010.csv" );
  //ifstream myfile( "out.csv" );
  if (myfile)  
    {
    while (getline( myfile, line ) && counter <= N)  
      {

			if(counter == 0){
				counter++;
				continue ;
			}

			char *newLine = new char[line.length()];
			strcpy(newLine,line.c_str());

			char *delim = ",";
			newdata.population = string(strsep(&newLine,delim));
			newdata.minAge = string(strsep(&newLine,delim));
			newdata.maxAge = string(strsep(&newLine,delim));
			newdata.gender = string(strsep(&newLine,delim));
			newdata.zipCode = string(strsep(&newLine,delim));
			newdata.geoId = string(strsep(&newLine,"\n"));
			
			//cout << newdata.population << ":" << newdata.minAge << ":" << newdata.maxAge << ":" << newdata.gender << ":" << newdata.zipCode << ":" << newdata.geoId << endl;
			
			dataList.push_back(newdata);
			counter++;
      }
    myfile.close();
    }

}

void residenceManager::traverse(){
	int size = dataList.size();
    for(int i=0;i<size;i++){
        cout << dataList[i].population << ":" << dataList[i].minAge << ":" << dataList[i].maxAge << ":" << dataList[i].gender << ":" << dataList[i].zipCode << ":" << dataList[i].geoId << endl;
    }
	
	
}

void residenceManager::writeFile(){
	FILE *fp;
    fp = fopen("out.csv", "w");
    //fp = fopen("out2.csv", "w");
    
    //writing headers
    fprintf(fp,"%s,%s,%s,%s,%s,%s\n","population", "minimum_age", "maximum_age","gender", "zipcode" ,"geo_id");
    
    int size = dataList.size();
    for(int i=0;i<size;i++){
        fprintf(fp,"%s,%s,%s,%s,%s,%s\n",dataList[i].population.c_str(), dataList[i].minAge.c_str(), dataList[i].maxAge.c_str(), dataList[i].gender.c_str(), dataList[i].zipCode.c_str(), dataList[i].geoId.c_str() );
    }
    fclose(fp);
	
	
}

void residenceManager::quickSort(int p,int r){
	if(p<r){
		int q = Partition(p,r);
		quickSort(p,q-1);
		quickSort(q+1,r);
	}	
}

int residenceManager::Partition(int p,int r){
	data x;
	x.population = dataList[r].population;
	x.minAge = dataList[r].minAge;
	x.maxAge = dataList[r].maxAge;
	x.gender = dataList[r].gender;
	x.zipCode = dataList[r].zipCode;
	x.geoId = dataList[r].geoId;

	int i = p-1;
	int j;
	for(j=p ; j<=r-1 ; j++){
		
		if( atoi( dataList[j].population.c_str()) < atoi(x.population.c_str()) ){
			i = i+1 ;
			swap(&dataList[i],&dataList[j]);
			
		}else if( atoi( dataList[j].population.c_str()) == atoi(x.population.c_str()) ){
			if( dataList[j].geoId.compare(x.geoId) <= 0 ){
				i = i+1 ;
				swap(&dataList[i],&dataList[j]);
			}
			// if they are equal no need to swap 
			// leave it as their original order.
		}	
	}
	swap(&dataList[i+1],&dataList[r]);
	return i+1;
}

void residenceManager::swap(data *A,data *B){
	data * temp = new data;
	temp->population = A->population;
	temp->minAge = A->minAge;
	temp->maxAge = A->maxAge;
	temp->gender = A->gender;
	temp->zipCode = A->zipCode;
	temp->geoId = A->geoId;
	
	A->population = B->population;
	A->minAge = B->minAge;
	A->maxAge = B->maxAge;
	A->gender = B->gender;
	A->zipCode = B->zipCode;
	A->geoId = B->geoId;
	
	B->population = temp->population;
	B->minAge = temp->minAge;
	B->maxAge = temp->maxAge;
	B->gender = temp->gender;
	B->zipCode = temp->zipCode;
	B->geoId = temp->geoId;
	
	free(temp);
	
	
}

int main(int argc, char *argv[]){
	
	int N;
	if(argc == 2){
		N = atoi( argv[1] );
	}else{
		cout << "Error" << endl;
		exit(0);
	}
	
	clock_t t;
	clock_t sum = 0 ;
	for(int i=0;i<10;i++){
		
		t = clock();
	
	
		residenceManager *test = new residenceManager(N);
		// every function is called in constructor
  
		t = clock() - t;
	
		sum += t;
  
		free(test);
	}
	
	t = sum/10;
	
	printf ("It took %d clicks (%f seconds).\n",t,((float)t)/CLOCKS_PER_SEC);
  
  return 0;
  
}

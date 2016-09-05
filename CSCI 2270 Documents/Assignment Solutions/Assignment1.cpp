#include <iostream>
#include <fstream>  //allows istream/ostream
#include <stdlib.h> //allows atoi

using namespace std;


struct lookingFor{
    //Five categories: bicycle, microwave, dresser, truck, or chicken
    string type;
    //For sale = true; wanted = false
    bool status;
    int price;
};

void printBoard(lookingFor sarray[], int asize){
    //cout << "The virtual yard sale currently contains: " << endl;
    string item;
    string wanted;
    for(int i = 0; i < asize; i++){

        if(sarray[i].status){
            wanted = "for sale";
        }
        else{
            wanted = "wanted";
        }
        cout << sarray[i].type << ", " << wanted  << ", " << sarray[i].price << endl;
    }

}

int main(int argc, char* argv[]){

int opcount = 0;

//Open files
ifstream in_stream;

in_stream.open(argv[1]);
//Figure out how big the array needs to be, not required
int count1 = 0;
string line;
while (!in_stream.eof()){
    getline(in_stream, line);
    count1++;
}
in_stream.close();
in_stream.clear();//allows for reopening

//Create an array of size = number of lines and populate with structs
lookingFor sArray[100];

in_stream.open(argv[1]);
int count2 = 0;

string obj;
string stats;
bool stat;
string costS;
int cost;
bool alreadysold = false;
while(!in_stream.eof()){
    //finds delimiter and puts preceding information into string
    getline(in_stream, obj, ',');
    getline(in_stream, stats, ',');
    getline(in_stream, costS); // "\n" is the default delimiter
    opcount++;
    if(obj == ""){ //catches that last line
        break;
    }
    cost = atoi(costS.c_str()); //convert to integer

    if(stats[0] == ' '){
        stats.erase(0,1);
    }
    if(stats == "wanted"){
        stat = false;
    }
    else{
        stat = true;
    }

    if(count2 >= 0){
		alreadysold = false;
        for(int j = 0; j < count2; j++){
			opcount++;
            if((sArray[j].type == obj) && (sArray[j].status != stat)){
                if(sArray[j].status){
                //for sale = true in the array
                    if(cost >= sArray[j].price){ //willing to pay > for sale price, cost is willing to pay here
                        cout << obj << " " << sArray[j].price << endl;
                        //replace item by shifting array contents
                        count2--;
                        for(int x = j; x < count2; x++){
							opcount++;
							sArray[x] = sArray[x+1];
						}
                        alreadysold = true; //we can only sell once
                    }
                }
                else{
                //wanted = false, looking at wanted items in array
                    //sArray[j].price is what user is willing to pay
                    //cost is the cost of the item
                    if(cost <= sArray[j].price){
                        cout << obj << " " << cost << endl;
                        //replace item by shifting array contents
                        count2--;
                        for(int x = j; x < count2; x++){
							opcount++;
							sArray[x] = sArray[x+1];
						}
                        alreadysold = true; //we can only sell once
                    }
                }
            }
        if(alreadysold){
            break;
        }
        }
    }
    if (!alreadysold){
        //Now that we're done checking for matches, add to the array
        sArray[count2].price = cost;
        sArray[count2].status = stat;
        sArray[count2].type = obj;
        count2++;
    }


}
cout<<"#"<<endl;
printBoard(sArray, count2);
cout<<"#";
cout << "\nloop iterations:" << opcount << endl;
//Close files now that we're done with them
in_stream.close();

 return 0;
}



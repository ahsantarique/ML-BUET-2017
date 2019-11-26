#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>


using namespace std;
ifstream file("Assignment1_data_set.csv");

const int featuresize=8;
const int label = featuresize;
const int valuerange = 10+1;
int data[5000][featuresize+1];
bool usedfeatures[featuresize];

struct node{
    int whichfeature;
    node * featurevalue[valuerange+1];

    node(){
        for(int i=0; i <valuerange+1; i++) featurevalue[i]= NULL;
    }
    node(int value){
        whichfeature = value;
        for(int i=0; i <valuerange+1; i++) featurevalue[i]= NULL;
    }
};

node *root;

double infogain(const vector <int> examples, int feature, int pos, int neg)
{
    int poscount[valuerange]={0};
    int negcount[valuerange]={0};

    double total = pos+neg;
    double h = -(pos/total * log2(pos/total))-(neg/total * log2(neg/total));

    for(int i=0; i <examples.size(); i++){
        int row = examples[i];
        int featurevalue = data[row][feature];
        if(data[row][label]==1){
            poscount[featurevalue]++;
        }
        else{
            negcount[featurevalue]++;
        }
    }

    double ig = h;

    for(int i=0; i < valuerange; i++){
        double icount = poscount[i]+negcount[i];
        double ih=0;
        if(icount !=0 && poscount[i]!=0 && negcount[i]!=0){
            ih= -(poscount[i]/icount * log2(poscount[i]/total))-(negcount[i]/icount * log2(negcount[i]/icount));
        }
//        cout << "ih" << ih << endl;
        ig -= (icount/total) * ih;
    }
    return ig;
}



void id3(vector <int> examples, node* &cur)
{
    int poscount=0, negcount=0;

    for(int i=0; i<examples.size(); i++){
        int row = examples[i];
        if(data[row][label]==1){
            poscount++;
        }
        else{
            negcount++;
        }
    }
    if(negcount==0) {
            cur = new node(1);
            return;
    }
    if(poscount==0){
            cur = new node(0);
            //cout << "YESS\n";
            return;
    }

    //check if all feature exhausted
    bool allexhaust = true;
    for(int i=0; i<featuresize; i++){
        if(usedfeatures[i]==false){
            allexhaust= false;
            break;
        }
    }
    if(allexhaust){
        if(poscount > negcount){
            cur = new node(1);
            return;
        }
        else{
            cur = new node(0);
            return;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int selectedfeature;
    double maxgain = -1;
    for(int i=0; i <featuresize; i++){
        if(!usedfeatures[i]){
            double ig = infogain(examples,i, poscount, negcount);
            if(ig > maxgain){
                maxgain = ig;
                selectedfeature = i;
            }
        }
    }

    //cout << "mxgain" << maxgain << endl;

    cur->whichfeature = selectedfeature;
    usedfeatures[selectedfeature] = true;

    //selection done
    for(int i=1; i <valuerange; i++){
        cur->featurevalue[i]= new node();
        vector<int> newexample;
        for(int j=0; j < examples.size(); j++){
            int row = examples[j];
            if(data[row][selectedfeature]==i){
                newexample.push_back(row);
            }
        }
        if(newexample.empty()){
            cur->featurevalue[i]->whichfeature = (poscount > negcount)? 1:0;
        }
        else{
            id3(newexample, cur->featurevalue[i]);
        }
    }
    usedfeatures[selectedfeature] = false;

    //cout << cur->whichfeature << endl;
}


void discardline()
{
    string s;
    string line;
    if(file.is_open()){
       for(int i=0; i <1; i++){
            getline(file, line);
            stringstream ss(line);
            while(getline(ss, s, ',')){
                 //cout << s << endl;
            }
       }
    }


}

void getdata(vector <int> &examples)
{
    int v;
    string line;
    string s;

    int i=0,j=0;
    if(file.is_open()){
       while(getline(file, line)){
            stringstream ss(line);
            //examples.push_back(i);
            while(getline(ss, s, ',')){
                stringstream ssint(s);
                ssint >> v;
                data[i][j] = v;
                j++;
            }
            i++;
            j=0;


       }
    }
}

void printdata(int N)
{
    for(int i=0; i <N; i++){
        for(int j=0; j < featuresize+1; j++){
            cout << data[i][j] << " ";
        }
        cout << endl;
    }
}





int test(node * &cur, int row)
{
    int selectedfeature = cur->whichfeature;


    bool leaf = true;
    for(int i = 1; i < valuerange; i++){
        if(cur->featurevalue[i]!=NULL){
            leaf = false;
            break;
        }
    }
    if(leaf){
        return selectedfeature;
    }
    //////////////////////////////////////


    int whichbranch = data[row][selectedfeature];
    return test(cur->featurevalue[whichbranch], row);


}

void crossvalidation(int x)
{
    int acc = 0;
    for(int i = 0; i < 500; i++){
        if(i>x*100 && i < (x+1)*100) continue;
        int prediction = test(root,i);
        if( prediction == data[i][label]) acc++;
        cout << prediction << endl;
    }
    cout << "acc % on" << x << " = " << acc << endl;
}




int main()
{
    discardline();

    vector <int> examples;
    getdata(examples);
    //printdata(3);

    for(int i = 0; i < 5; i++){
        examples.clear();

        for(int j= i*100; j < (i+1)*100; j++){
            examples.push_back(j);
        }
        root = new node();
        id3(examples,root);
        crossvalidation(i);
    }

    return 0;
}

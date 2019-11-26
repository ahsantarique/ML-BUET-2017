#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace std;
ifstream file("Assignment1_data_set.csv");

const int featuresize=8;
const int label = featuresize;
const int valuerange = 10+1;
const int T=30;
int data[5000][featuresize+1];
bool usedfeatures[featuresize];

int totalSamples;



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

node *root[T];
double alpha[T];

double infogain(const vector  <int> examples, int feature, int pos, int neg)
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
            ih= -(poscount[i]/icount * log2(poscount[i]/icount))-(negcount[i]/icount * log2(negcount[i]/icount));
        }
//        cout << "ih" << ih << endl;
        ig -= (icount/total) * ih;
    }
    return ig;
}



void id3(vector <int > examples, node* &cur)
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


    cur->whichfeature = selectedfeature;
    usedfeatures[selectedfeature] = true;

    //selection done
    for(int i=1; i <valuerange; i++){
        cur->featurevalue[i]= new node();
        poscount = 0, negcount=0;
        for(int j=0; j < examples.size(); j++){
            int row = examples[j];
            if(data[row][selectedfeature]==i){
                if(data[row][label]==0){
                    negcount++;
                }
                else{
                    poscount++;
                }
            }
        }

        cur->featurevalue[i]->whichfeature = (poscount > negcount)? 1:0;
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

void getdata()
{
    int v;
    string line;
    string s;

    int i=0,j=0;
    if(file.is_open()){
       while(getline(file, line)){
            stringstream ss(line);
            totalSamples++;
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


int adatest(int row){
    double hx = 0;
    for(int t=0; t< T; t++){
        hx += alpha[t]*(test(root[t],row) ? 1:-1);
    }
    if(hx < 0){
    }
    else {
        return 1;
    }
}

void crossvalidation(int x)
{
    double tpos=0, tneg=0, fpos=0, fneg=0;
    double P, N;

    double recall,specificity,precision, npv, fallout, missrate, fdr, f1score, accuracy;

    for(int i = 0; i < totalSamples; i++){
        if(i > x*totalSamples*1.0/5.0 && i < (x+1)*totalSamples*1.0/5.0) continue;
        int prediction = adatest(i);
        if( prediction == 1 && data[i][label]==1) tpos++;
        if( prediction == 0 && data[i][label]==0) tneg++;
        if( prediction == 1 && data[i][label]==0) fpos++;
        if( prediction == 0 && data[i][label]==1) fneg++;

        //cout << prediction << endl;
    }
    P = tpos+fneg;
    N = tneg+fpos;

    accuracy = (P+N)? (tpos+tneg)/(P+N):1;
    recall = (P)? tpos/P:1;
    specificity = (N)? tneg/N:1;
    precision = (tpos+fpos)? tpos/(tpos+fpos):1;
    npv = (tneg+fneg)? tneg/(tneg+fneg): 1;
    missrate = 1-recall;
    fallout = 1-specificity;
    fdr = 1-precision;
    f1score = (2*tpos + fpos + fneg)? 2*tpos / (2*tpos + fpos + fneg):1;

    cout << "\nCross Validation " << x << ":" << endl;
    cout << "-------------------" << endl;
    cout << "Accuracy " << accuracy *100 << "%" << endl;
//    cout << "True positive rate (sensitivity, recall, hit rate) " << recall*100 << "%" << endl;
//    cout << "True negative rate (specificity) " << specificity *100 << "%" << endl;
//    cout << "Positive predictive value (precision) " << precision *100<< "%" <<endl;
//    cout << "Negative predictive value " << npv*100 << "%"  <<endl;
//    cout << "False positive rate (fall-out) " << fallout*100 << "%" << endl;
//    cout << "False negative rate (miss rate) " << missrate*100 << "%" <<endl;
//    cout << "False discovery rate " << fdr*100 << "%" <<endl;
//    cout << "F1 score " << f1score*100 << "%" << endl;

}

double rand_lim(double limit) {
/* return a random number between 0 and limit inclusive.
 */

    double divisor = RAND_MAX/(limit+1);
    double retval;

    do {
        retval = rand() / divisor;
    } while (retval > limit);
    return retval;
}


void adaboost(vector <pair <double, int> > examples)
{


    vector < double > cumsum;
    vector <int > trainexamples;
    vector <int > correct, incorrect;

    int right=0, wrong=0;

    for(int t=0; t<T; t++){
        root[t]= new node();

        cumsum.clear();
        trainexamples.clear();

        double c = 0;
        for(int i=0; i < examples.size(); i++){
            c += examples[i].first;
            cumsum.push_back(c);
        }

        for(int i=0; i < examples.size()/2; i++){
            double rand = rand_lim(1.0);
            int index=0;
            for(int j = 0; j < cumsum.size(); j++){
                if(rand < cumsum[j]){index = j; break;}
            }
            trainexamples.push_back(examples[index].second);
        }
        id3(trainexamples,root[t]);


        ///////////////////////// training done

        correct.clear(), incorrect.clear();

        double e=0;
        for(int i = 0; i < examples.size(); i++){
            int row = examples[i].second;
            double weight = examples[i].first;
            int ht=test(root[t], row);
            if(ht!=data[row][label]){
                e+= weight;
                incorrect.push_back(i);   // example er i
                wrong++;
            }
            else{
                correct.push_back(i);    // example er i
                right++;
            }
        }
        ///Set importance of ht
        if(e==0){
            alpha[t]=1e4;
        }
        else alpha[t]=.5*log2((1-e)/e);

        double total_weight = 0;
        for(int i=0; i < correct.size(); i++){
            examples[i].first = examples[i].first* exp(-alpha[t]);
            total_weight += examples[i].first;
        }
        for(int i=0; i < correct.size(); i++){
            examples[i].first = examples[i].first* exp(alpha[t]);
            total_weight += examples[i].first;

        }
        //now normalize
        for(int i=0; i < examples.size(); i++){
            examples[i].first /= total_weight;
        }
    }
    cout << "single stump accuracy : " << (right*1.0/(right+wrong)) *100 << " %" << endl;
}


void leaveOneOut()
{
    double tpos=0, tneg=0, fpos=0, fneg=0;
    double P, N;
    double recall,specificity,precision, npv, fallout, missrate, fdr, f1score, accuracy;
    vector < pair <double, int> > examples;

    for(int i = 0; i < 10; i++){
        examples.clear();

        for(int j=0; j < totalSamples; j++){
            if(i!=j) examples.push_back(make_pair(1/(totalSamples-1), j));
        }
        adaboost(examples);
        int prediction = adatest(i);
        if( prediction == 1 && data[i][label]==1) tpos++;
        if( prediction == 0 && data[i][label]==0) tneg++;
        if( prediction == 1 && data[i][label]==0) fpos++;
        if( prediction == 0 && data[i][label]==1) fneg++;
    }
    P = tpos+fneg;
    N = tneg+fpos;

    accuracy = (P+N)? (tpos+tneg)/(P+N):1;
    recall = (P)? tpos/P:1;
    specificity = (N)? tneg/N:1;
    precision = (tpos+fpos)? tpos/(tpos+fpos):1;
    npv = (tneg+fneg)? tneg/(tneg+fneg): 1;
    missrate = 1-recall;
    fallout = 1-specificity;
    fdr = 1-precision;
    f1score = (2*tpos + fpos + fneg)? 2*tpos / (2*tpos + fpos + fneg):1;

    cout << "-------------------" << endl;
    cout << "Accuracy " << accuracy *100 << "%" << endl;
//    cout << "True positive rate (sensitivity, recall, hit rate) " << recall*100 << "%" << endl;
//    cout << "True negative rate (specificity) " << specificity *100 << "%" << endl;
//    cout << "Positive predictive value (precision) " << precision *100<< "%" <<endl;
//    cout << "Negative predictive value " << npv*100 << "%"  <<endl;
//    cout << "False positive rate (fall-out) " << fallout*100 << "%" << endl;
//    cout << "False negative rate (miss rate) " << missrate*100 << "%" <<endl;
//    cout << "False discovery rate " << fdr*100 << "%" <<endl;
//    cout << "F1 score " << f1score*100 << "%" << endl;
}

int main()
{
    discardline();
    totalSamples=0;

    vector < pair <double, int> > examples;
    getdata();

    cout << "------------------------\n";
    cout << "5 Fold Cross validation:\n";
    cout << "------------------------\n";

    double k = 5;
    for(int i = 0; i < k; i++){
        examples.clear();

        for(int j= i*(totalSamples*1.0/k); j < (i+1)*(totalSamples*1.0/k); j++){
            examples.push_back(make_pair(k/totalSamples, j));
        }
        adaboost(examples);
        crossvalidation(i);
    }
    cout << "------------------------\n";
    cout << "10 Fold Cross validation:\n";
    cout << "------------------------\n";

    k = 10;
    for(int i = 0; i < k; i++){
        examples.clear();

        for(int j= i*(totalSamples*1.0/k); j < (i+1)*(totalSamples*1.0/k); j++){
            examples.push_back(make_pair(k/totalSamples, j));
        }
        adaboost(examples);
        crossvalidation(i);
    }
    cout << "------------------------\n";
    cout << "20 Fold Cross validation:\n";
    cout << "------------------------\n";

    k = 20;
    for(int i = 0; i < k; i++){
        examples.clear();

        for(int j= i*(totalSamples*1.0/k); j < (i+1)*(totalSamples*1.0/k); j++){
            examples.push_back(make_pair(k/totalSamples, j));
        }
        adaboost(examples);
        crossvalidation(i);
    }
    cout << "------------------------\n";
    cout << "Leave One Out Cross validation:\n";
    cout << "------------------------\n";

    leaveOneOut();
    return 0;
}

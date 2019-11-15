#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <math.h>
#include <time.h>
using namespace std;

struct charSet{
    unsigned int ascii; //ascii character code
    unsigned int freq; //frequency of character
    float prob; //probability of character
    string code; //huffman code for character
    char asciiChr; //ascii character representation
};

struct huffSet{
    string _set; //set of characters
    float probSet; //probability for the set of characters
};

int split(struct huffSet arr[], int l, int h){
    float pivot = arr[h].probSet;
    int idx = l - 1;

    for(int j = l; j<=h-1; j++){
        if(arr[j].probSet <= pivot){
            idx++;
            float aux = arr[idx].probSet;
            arr[idx].probSet = arr[j].probSet;
            arr[j].probSet = aux;
            string _aux = arr[idx]._set;
            arr[idx]._set = arr[j]._set;
            arr[j]._set = _aux;
        }
    }

    float aux = arr[idx+1].probSet;
    arr[idx+1].probSet = arr[h].probSet;
    arr[h].probSet = aux;
    string _aux = arr[idx+1]._set;
    arr[idx+1]._set = arr[h]._set;
    arr[h]._set = _aux;
    return (idx+1);
}

void quickSort(struct huffSet arr[], int l, int h){
    if(l<h){
        int pi = split(arr, l, h);

        quickSort(arr, l, pi-1);
        quickSort(arr, pi+1, h);
    }
}

int main(){
    int c = 0; //integer to choose reading from kb or file
    string text; //string where the text is stored
    string buffer; //string for reading remaining whitespaces

    float ogSigLength = 0; //original signal length
    float ogMaxLength = 0; //original character length
    float encSigLength = 0; //encoded signal length
    float encAvgLength = 0; //encoded average length
    float encMinLength = 0; //encoded minimum length
    float entropy = 0;
    float efficiency = 0;
    float redundancy = 0;
    float kraft = 0; //if it's == 1 -> uniquely decodable code
    double encTime;
    double decTime;

    string encodedSignal;
    string decodedSignal;

    do{
        cout<<"Read from kb (1) or file huffman.txt (2) ?\n";
        cin>>c;
    }while(c!=1 && c!=2);

    cout<<"\n";

    if(c==1){
        getline(cin, buffer);

        cout<<"---Input Text---\n";
        getline(cin,text);
    }

    else if(c==2){
        ifstream f;
        f.open("huffman.txt");

        stringstream stream;
        stream << f.rdbuf();
        text = stream.str();

        f.close();

        cout<<"---Input Text---\n";
        cout<<text<<"\n";
    }

    else{
        cout<<"How did you even get here?";
    }

    cout<<"---Input Text---\n\n";

    unordered_map <int, float> tempChrMap; //temporary map to store every character in ascii code and its' frequency/probability

    for(auto &i: text){
        tempChrMap[i]++; //initializing the frequency values for each character
    }

    const int cSize = tempChrMap.size(); //constant for how many unique characters
    struct charSet cSet[cSize]; //create a struct for a character set
    struct huffSet hSet[cSize]; //create a struct for a huffman set
    int hSize = cSize; //dynamic variable for huffman set's size

    int _idx = 0;
    for (unordered_map <int, float> :: iterator itr = tempChrMap.begin(); itr != tempChrMap.end(); ++itr){
        cSet[_idx].freq = itr->second; //saving frequency from map to character struct
        tempChrMap[itr->first] /= text.length(); //calculating character probability (divide frequency by text's length)
        cSet[_idx].ascii = itr->first; //saving ascii code from map to character struct
        cSet[_idx].asciiChr = static_cast<char>(itr->first); //saving ascii representation from map to character struct
        cSet[_idx].prob = itr->second; //saving probability from map to character struct
        hSet[_idx].probSet = itr->second; //saving probability from map to huffman struct
        hSet[_idx]._set = static_cast<char>(itr->first); //saving ascii representation from map to huffman struct
        _idx++;
    }

    tempChrMap.clear(); //clearing up temporary map

    cout<<"---Character Probability---\n";
    for(int i = 0; i<cSize; i++){
        cout<<cSet[i].ascii<<" : "<<"'"<<cSet[i].asciiChr<<"' -> "<<cSet[i].prob*100<<"%\n";
    }
    cout<<"---Character Probability---\n\n";

    //encoding timer
    clock_t encTimer;
    encTimer = clock();

    //binary huffman encoding
    while(hSet[0].probSet < 1){
        quickSort(hSet, 0, hSize-1); //sorting the huffman set by probability
        for(int i=0; i<hSet[0]._set.length(); i++){ //processing first element in huffman set
            for(int j=0; j<cSize; j++){
                if(hSet[0]._set[i] == cSet[j].asciiChr){ //when it finds an associated character in the huff set
                   cSet[j].code.insert(0,"1"); //insert 1 at the beginning of the code for that character
                   break;
                }
            }
        }
        for(int i=0; i<hSet[1]._set.length(); i++){ //processing second element in huffman set
            for(int j=0; j<cSize; j++){
                if(hSet[1]._set[i] == cSet[j].asciiChr){ //when it finds an associated character in the huff set
                    cSet[j].code.insert(0,"0"); //insert 0 at the beginning of the code for that character
                    break;
                }
            }
        }
        hSet[0]._set += hSet[1]._set; //add up the characters of the first two elements in the first element
        hSet[0].probSet += hSet[1].probSet; //add up the probabilities of the first two elements in the first element
        for(int i=1; i<hSize-1; i++){ //shift the remaining elements up by 1
            hSet[i]._set = hSet[i+1]._set;
            hSet[i].probSet = hSet[i+1].probSet;
        }
        hSize--; //decrease size to forbid access to the last element
    }

    //encoding signal
    for(int i=0; i<text.size(); i++){
        for(int j=0; j<cSize; j++){
            if(text[i] == cSet[j].asciiChr){
                encodedSignal += cSet[j].code; //in order of the original signal, add a character's code to the encoded signal
                break;
            }
        }
    }

    encTimer = clock() - encTimer;
    encTime = ((double)encTimer)/CLOCKS_PER_SEC;

    cout<<"---After Huffman Encoding---\n";
    for(int i=0; i<cSize; i++){
        entropy += -cSet[i].prob * log2(cSet[i].prob); //entropy formula
        encSigLength = encSigLength + cSet[i].freq * cSet[i].code.length(); //encoded signal length formula
        encAvgLength = encAvgLength + cSet[i].prob * cSet[i].code.length(); //encoded average length formula
        kraft = kraft + pow(0.5f, cSet[i].code.length()); //kraft formula when D = 2
        cout<<cSet[i].ascii<<" : "<<"'"<<cSet[i].asciiChr<<"' -> "<<cSet[i].code<<"\n";
    }
    cout<<"---After Huffman Encoding---\n\n";

    //decoding timer
    clock_t decTimer;
    decTimer = clock();

    //decoding signal
    int startIndex = 0; //using index to verify consecutive substrings
    for(int i=0; i<encodedSignal.length(); i++){
        for(int j=0; j<cSize; j++){
            if(encodedSignal.substr(startIndex, i-startIndex+1) == cSet[j].code){ //when we find the code of a character in the encoded signal
                decodedSignal += cSet[j].asciiChr; //add that character to the decoded signal
                startIndex = i+1; //new index for next character
                break;
            }
        }
    }

    decTimer = clock() - decTimer;
    decTime = ((double)decTimer)/CLOCKS_PER_SEC;

    cout<<"---Encoded Huffman Signal---\n";
    cout<<encodedSignal<<"\n";
    cout<<"---Encoded Huffman Signal---\n\n";

    cout<<"---Decoded Huffman Signal---\n";
    cout<<decodedSignal<<"\n";
    cout<<"---Decoded Huffman Signal---\n\n";

    cout<<"---Is The Original Signal Equal With The Decoded Signal?---\n";
    cout<<(text == decodedSignal ? "True" : "False")<<"\n";
    cout<<"---Is The Original Signal Equal With The Decoded Signal?---\n\n";

    cout<<"---Encoding Time---\n";
    cout<<scientific<<encTime<<" seconds\n";
    cout<<"---Encoding Time---\n\n";

    cout<<"---Decoding Time---\n";
    cout<<decTime<<" seconds\n";
    cout<<"---Decoding Time---\n\n";

    cout<<"---Original Signal Length In Bits---\n";
    ogSigLength = ceil(log2(cSize)) * text.length();
    cout<<defaultfloat<<ogSigLength<<"\n";
    cout<<"---Original Signal Length In Bits---\n\n";

    cout<<"---Original Character Length In Bits---\n";
    ogMaxLength = ceil(log2(cSize));
    cout<<ogMaxLength<<"\n";
    cout<<"---Original Character Length In Bits---\n\n";

    cout<<"---Encoded Signal Length In Bits---\n";
    cout<<encSigLength<<"\n";
    cout<<"---Encoded Signal Length In Bits---\n\n";

    cout<<"---Encoded Average Length In Bits---\n";
    cout<<encAvgLength<<"\n";
    cout<<"---Encoded Average Length In Bits---\n\n";

    cout<<"---Encoded Minimum Length In Bits---\n";
    encMinLength = entropy;
    cout<<encMinLength<<"\n";
    cout<<"---Encoded Minimum Length In Bits---\n\n";

    cout<<"---Entropy---\n";
    cout<<entropy<<"\n";
    cout<<"---Entropy---\n\n";

    cout<<"---Efficiency---\n";
    efficiency = encMinLength / encAvgLength;
    cout<<efficiency<<"\n";
    cout<<"---Efficiency---\n\n";

    cout<<"---Redundancy---\n";
    redundancy = 1 - efficiency;
    cout<<redundancy<<"\n";
    cout<<"---Redundancy---\n\n";

    cout<<"---Kraft-McMillan---\n";
    cout<<kraft<<(kraft < 1 ? " -> has some redundancy" : (kraft == 1 ? " -> uniquely decodable code" : " -> impossible value reached"))<<"\n";
    cout<<"---Kraft-McMillan---\n\n";

    cout<<"Huffman coding complete.\n";
    cin>>buffer;
}

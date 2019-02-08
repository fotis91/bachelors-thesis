/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: fotis
 *
 * Created on 4 Δεκεμβρίου 2018, 2:45 μμ
 */

#include <stdio.h>
#include <stdlib.h>

struct dict{
   unsigned int tagBitComplementaryCharacter : 1;
   unsigned int tagBitComplementaryPrefix : 1;
   unsigned int prefix : 8;
   unsigned int character : 8;
   unsigned int value : 10;
};

typedef struct{
    int prefix; 
    int character; 
} DictElement;

struct DictNode {
	int value; 
	int prefix;
	int character; 
};

int compressed[100];
int decompressed[100];
int counter=0;
void dictionaryArrayAdd(int prefix, int character, int value);
int dictionaryArrayPrefix(int value);
int dictionaryArrayCharacter(int value);

DictElement dictionaryArray[2023];


void dictionaryArrayAdd(int prefix, int character, int value) {
    
    dictionaryArray[value].prefix = prefix;
    dictionaryArray[value].character = character;
}

int dictionaryArrayPrefix(int value) {
    return dictionaryArray[value].prefix;
}

int dictionaryArrayCharacter(int value) {
    return dictionaryArray[value].character;
}

int search(int prefix, int character,struct DictNode search[1023],int loops);
void insert(int prefix, int character, int value,struct DictNode insert[1023]);
int compress(int character,int done); 

void decompress(int codes[],int iter);
int decode(int code);

int main(int argc, char** argv) {

   
    int character=47;
    int ud=1;
    int tmpud;
    int i=0;
    int j=0;
    int cnt=0; 
    int codereturn=0;
    int charactersCoded=0;
    //struct DictNode p_nodes[1023];
    
    
    
    while(1){

    tmpud=ud;

    if(character>=68)ud=0;
    else if(character<=48)ud=1;
    else ud=tmpud;
    if(ud==1)character=character+1;
    else character=character-1;        
    
    codereturn=compress(character,100);
    charactersCoded++;
    
    if(codereturn==-1)continue;
    compressed[cnt]=codereturn;
    cnt++;
    if(codereturn>2023)break;
    }
    
    decompress(compressed,cnt);
    
    return (EXIT_SUCCESS);
}


int compress(int character,int done){
    
    static int nextCode=1000;
    static int countCodes=0;
    int dictionarySize=2023;
    int index;
    static int prefix;
    static int init=0;
    int temp_prefix;
    struct DictNode row[1023];
    
    if(init==0){
        init=1;
        prefix=character;
        printf("return 48 prwth periptwsh -1 iterations=0\n");
        return -1;
    }
    
    if ((index = search(prefix,character, row, countCodes)) != -1){ prefix = index; 
     if(countCodes==done) return prefix<<11;
     else return -1;
	     }
    else {countCodes++;
       
     if (countCodes < dictionarySize) {
	insert(prefix,character, nextCode++, row);}

    temp_prefix=prefix;

    prefix=character;
   
    
    if(countCodes==done) return (character<<11)+temp_prefix;

    else return temp_prefix;
    
    
    }
}


void decompress(int codes[],int iter) {
    
     int previousCode; int currentCode;
     int nextCode = 1000;
     int firstChar;
     int position=0;
     int i=0;
     int dictionarySize=2023;
     int val=2047;//2047=11111111111 for bitwise operations, unpacking bits from integers
     int characterMoved=0;
     int tempPrefix=0;
    
    
    
    
    previousCode=codes[i];
    decompressed[counter]=previousCode;

    counter++;
    i++;
 
    while(position<iter){
       
    currentCode=codes[i];
    if(currentCode>2023){
    tempPrefix=currentCode & val;  
    characterMoved=currentCode>>11;   
    if(tempPrefix!=0) decode(tempPrefix);
    decompressed[counter]=characterMoved;
    
    exit(0);
    }
   
    i++; 
    

    if (currentCode >= nextCode) {
    firstChar = decode(previousCode); 
    decompressed[counter]=firstChar;
    
    counter++;        
    
            
   
    } else firstChar = decode(currentCode); 
           
    
     
    if (nextCode < dictionarySize) dictionaryArrayAdd(previousCode, firstChar, nextCode++);
        
   
    previousCode = currentCode;
    position++;
    }
 
   
}

int decode(int code) {
    int character; int temp;
   

    if (code > 999) { 
        character = dictionaryArrayCharacter(code);
        temp = decode(dictionaryArrayPrefix(code));
     
    } else {
        character = code; 
        temp = code;
    }
    decompressed[counter]=character;
    
   
    counter++;
    
    
    return temp;
}

int search(int prefix, int character,struct DictNode search[1023],int loops){

    static int lastMatch;
    int iIterations=0;
    int jIterations=0;
    int kIterations=0;
    int lIterations=0;
    int k=0;
    int l=loops;
    if(lastMatch!=0 && lastMatch!=loops){
            k=0;
            l=loops;
        }
    
    for(int i=lastMatch, j=lastMatch;i<=loops || j>=0;i++,j--){  
        
        if(lastMatch!=0 && lastMatch!=loops){
        if(search[k].prefix==prefix && search[k].character==character){
            lastMatch=k; 
            return search[k].value;
        }
        }
        
        if(lastMatch!=0 && lastMatch!=loops){
        if(search[l].prefix==prefix && search[l].character==character){
            lastMatch=l; 
            return search[k].value;
        }
        }
            
        if(i<=loops){ iIterations++;
            if(search[i].prefix==prefix && search[i].character==character){ 
                lastMatch=i; 
                return search[i].value;}
        }
        if(i==j)continue;
        if(j>=0) { jIterations++;
            if(search[j].prefix==prefix && search[j].character==character){ 
                lastMatch=j; 
                return search[j].value;
            }
        }
    
    }
    
    
    return -1;
}



void insert(int prefix, int character, int value,struct DictNode insert[1023]){
    static int pos=0;
    insert[pos].prefix=prefix;
    insert[pos].character=character;
    insert[pos].value=value;
    pos++;
}

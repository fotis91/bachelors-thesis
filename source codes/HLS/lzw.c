//#include <ap_cint.h>


struct DictNode { // sturct holding information about a new registry to the dictionary
	int value; // the position in the list
	int prefix; // prefix for byte > 255
	int character; // the last byte of the string
};

//methods for controlling the dictionary. These methods include the initialization, 
//searching in the dictionary and adding a new entry which are implemented after the IP function
void dictionaryInit(struct DictNode nodeinit[1023]);

int dictionaryLookup(int prefix, int character,struct DictNode nodelookup[1023]);

void dictionaryAdd(int prefix, int character, int value,struct DictNode nodeadd[1023]);

void compress(int character,int * WC,int done) {

	//AXI protocols
#pragma HLS INTERFACE s_axilite port=return 
#pragma HLS INTERFACE s_axilite port=character
#pragma HLS INTERFACE axis port=WC

    static struct DictNode p_nodes[1023];
    static int init=0;
    static int prefix=-1;
    static int nextCode=1000;
    static int dictionarySize=2023;
    static int temp_prefix;
    static int count_codes=0;
    int index;


    if(init==0){
    dictionaryInit(p_nodes); // dictionary initialization
    init=1;}

    if ((index = dictionaryLookup(prefix,character, p_nodes)) != -1){ prefix = index;
     if(count_codes==done) *WC=index;
     else  *WC=-1;
	     }
    else {count_codes++;
     if (nextCode < dictionarySize) {
	dictionaryAdd(prefix,character, nextCode++, p_nodes);}

    temp_prefix=prefix;

    prefix=character;

    if(count_codes==done) *WC=((character<<11)+temp_prefix); // terminating condition

    else *WC=temp_prefix;
    }

}

void dictionaryInit(struct DictNode nodeinit[1023]) {
	for (int i = 0; i <= 999; i++) {
	    	nodeinit[i].prefix=-1;
	    	nodeinit[i].character=i;
	        nodeinit[i].value=i;
	    }
	    for (int j = 1000; j < 1024; j++) {
	    	nodeinit[j].prefix=-1;
	    	nodeinit[j].character=-1;
	        nodeinit[j].value=-1;
	    }
}

int dictionaryLookup(int prefix, int character,struct DictNode nodelookup[1023]) {

	if(prefix!=-1){

	      for (int i = 0; i < 1024; i++) {

	       if(nodelookup[i].value<999) return -1;

	       else{
	           if(nodelookup[i].prefix == prefix && nodelookup[i].character == character) return nodelookup[i].value;
	           }
	       }

	      }

	    else{
	        for (int i = 0; i < 1000; i++){
	          if(nodelookup[i].character == character) return nodelookup[i].value;
	        }
	    }


}


void dictionaryAdd(int prefix, int character, int value,struct DictNode nodeadd[1023]) {
    static int cnt=0;
    nodeadd[cnt].value = value;
    nodeadd[cnt].prefix = prefix;
    nodeadd[cnt].character = character;
	cnt++;
}

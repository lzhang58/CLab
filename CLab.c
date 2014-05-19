/* This is a program reading a file
 and convert it to a dictionary.
 You can search/insert/remove word from dictionary.
 I also use hash function to make the search faster
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define  WORD_LENGTH  50
#define  CAPACITY_INITIAL 50

int loadDictionary(char *inFileName, char ****array, int *count, int *capacity);
int saveDictionary(char* outFileName,char ****array, int *count, int *capacity);
int insertWord(char **dictionary, int *count, char word[]);
void printArray(char ***array, int *count);
void doubleCapacity(char*** array,int *count,int *capacity);
void menu(char* outFileName,char**** array,int *count,int *capacity);
void removeWord(char** array,int* count,char word[]);
int binarySearch(char** array,char* word,int iMin,int iMax);
int search(char** array,char* word,int count);
int hash_function(char* word);
char* savingComparison(int* track,char*** array,int* size,int* capacity);

/* main function read two file names as input file and output file*/
int main(int argc, char* argv[])
{
    char*** dictionary;  /* declare the dynamic array of strings */
                       /* the array is NOT allocated yet */
    int capacity[8];
    int i,j;
    for(i=0;i<8;i++){
        capacity[i]=CAPACITY_INITIAL;
    }
    int wordCount[8] = {};
    if (argc != 3)
    {
        fprintf(stderr,"Usage: %s inputfile outputfile\n", argv[0]);
        return 1;
    }

    if (loadDictionary(argv[1], &dictionary, wordCount, capacity) != 0)
    {
        fprintf(stderr,"    load dictionary failed! Program terminating...\n");
        return 1;  /* don't need to call exit - we're in main! */
    }

    printf("\n Finished loading %d words.\n", wordCount[0]+wordCount[1]+wordCount[2]+wordCount[3]+wordCount[4]+wordCount[5]+wordCount[6]+wordCount[7]);
    
    if((wordCount[0]+wordCount[1]+wordCount[2]+wordCount[3]+wordCount[4]+wordCount[5]+wordCount[6]+wordCount[7])<200){
        printArray(dictionary, wordCount);/* comment this out if using a big file!*/
    }
    
    menu(argv[2],&dictionary,wordCount,capacity);

    //free dictionary
    for(i=0;i<8;i++){
    	for(j=0;j<wordCount[i];j++){
        	free(dictionary[i][j]);
    	}
    	free(dictionary[i]);
    }
    free(dictionary);
    return 0;
}

/* load a Dictionary from file */
int loadDictionary(char *inFileName, char ****array, int *count, int *capacity)
{
    FILE *inFile;
    int i;
    char word[WORD_LENGTH];  /* this is the ONLY auto array we'll need */
    
    if ((inFile = fopen(inFileName, "r")) == NULL){
		fprintf(stderr,"Error opening input file, %s\n", inFileName);
		return -1;
    }
	
	  /* allocate the dictionary */
    *array = (char***)malloc(8*sizeof(char**));
    if (*array == NULL){
        fprintf(stderr, "Malloc of array in loadArray failed!\n");
        return -1;
    }
    
    /* allocate 8 dictionary space to store the whole word list */
    for(i=0;i<8;i++){
        (*array)[i] = (char**)malloc(capacity[i]*sizeof(char*));
        if ((*array)[i] == NULL){
            fprintf(stderr, "Malloc of array in loadArray failed!\n");
            return -1;
        }
    }

    printf("Reading file %s (each . is 5000 words read)\n", inFileName);
    for(i=0;i<8;i++){
       count[i] = 0;
    }
    while (fscanf(inFile, "%s", word) == 1)
    {
        int index = hash_function(word);
        if(index!=-1){
		if (count[index] >= capacity[index]){
			/* call a function that will double the size of the array and copy its contents */
				doubleCapacity(&(*array)[index],&count[index],&capacity[index]);
		}
		if(search((*array)[index],word,count[index])==0){
	        	    if (insertWord((*array)[index],&count[index], word) == 0){
	        	     	fprintf(stderr,"    Insert returned an error!\n");
	        	        fclose(inFile);
	        	        return 1;
	        	    }
	        }
	}
	if ((count[0]+count[1]+count[2]+count[3]+count[4]+count[5]+count[6]+count[7])% 5000 == 0){
	    printf(".");
	    fflush(stdout);  /* stdout is buffered, so have to force flush */
        }
    }
    fclose(inFile);
    return 0;
}

/* write all the words to an output file alphabetically*/
int saveDictionary(char* outFileName,char ****array, int *count, int *capacity){
    FILE *outFile;
    int i;
    char* current;
    outFile = fopen(outFileName, "w");
    if (outFile == NULL) {
        printf("I couldn't open results.dat for writing.\n");
        return -1;
    }
    
    int track[8]={0,0,0,0,0,0,0,0};
    
    int size = 0;
    for(i=0;i<8;i++){
        if(count[i]!=0){
            size++;
        }
    }
    while(size>0){
        current = savingComparison(track,*array,&size,count);
        fprintf(outFile, "%s\n",current);
    }
    fclose(outFile);
    return 0;
}

/*helper function for saving dictionary */
char* savingComparison(int* track,char*** array,int* size,int* count){
    int i,smallest;
    char* retVal = NULL;
    for(i=0;i<8;i++){
        if(track[i]!=-1&&count[i]>0){
            if(retVal==NULL){
                retVal = array[i][track[i]];
                smallest = i;
            }
            else{
                if(strcasecmp(array[i][track[i]],retVal)<0){
                    retVal = array[i][track[i]];
                    smallest = i;
                }
            }
        }
    }
    track[smallest]++;
    if(track[smallest]==(count[smallest])){
        track[smallest]=-1;
        *size=*size-1;
    }
    return retVal;
}

void printArray(char ***array, int *count)
{
   int i,j;
   printf("\n");
   for(j=0;j<8;j++){
       i=0;
       while (i<count[j]){
           printf("array[%d]: %s\n", j, array[j][i]);
           i++;
       }
   }
}


int insertWord(char **array, int *count, char word[])
{
    
    char *wordPtr;
    wordPtr = (char *)malloc((strlen(word)+1) * sizeof(char));
    if (wordPtr == NULL)
    {
        fprintf(stderr,"    Malloc of array[%d] failed!\n", *count);
        return 0;
    }
    /* Memory for this word has been allocated, so copy characters
    and insert into array */
    strcpy(wordPtr, word);
    int j,pos=0;
    while(pos<*count&&(strcasecmp(wordPtr,array[pos])>0)){
        pos++;
    }
    if(pos==*count){
        array[*count] = wordPtr;
    }
    else{
        for(j=*count; j>pos;j--){
            array[j]=array[j-1];
        }
        array[pos] = wordPtr;
    }
    (*count)++;
    return 1;
}


void doubleCapacity(char*** array,int *count,int *capacity){
    int i;
    char** arrayCopy = *array;
    *capacity = 2*(*capacity);
    *array = (char **)malloc(*capacity *sizeof(char*));
    for(i=0;i<*count;i++){
        (*array)[i] = arrayCopy[i];
    }
    free(arrayCopy);
}

/* menu funtion : 1.search 2.insert 3.remove 4.count */
void menu(char* outFileName,char**** array,int *count,int *capacity){
    char choice;
    char word[WORD_LENGTH];
    do{
        int ch;
        printf("Choose: 'S'earch, 'I'nsert, 'R'emove, 'C'ount, 'Q'uit :\n");
        scanf("%c", &choice);
        if(choice=='S'){
            printf("input the word you want to search:\n");
            scanf("%s",word);
            int index = hash_function(word);
            if(search((*array)[index],word,count[index])){
                printf("is found\n");
            }else{
                printf("is not found\n");
            }
        }
        else if(choice=='I'){
            printf("input a word to insert\n");
            scanf("%s",word);
	    int index = hash_function(word);
                if(search((*array)[index],word,count[index])){
                    printf("%s ignored (duplicate)\n",word);
                }
                else{
                    if(count[index]>=capacity[index]){
                        doubleCapacity(&((*array)[index]),&count[index],&capacity[index]);
                    }
                    insertWord((*array)[index],&count[index],word);
                    printf("Insert word %s successfully\n",word);
                }
        }
        else if(choice=='R'){
            printf("input a word to remove\n");
            scanf("%s",word);
	    int index = hash_function(word);
            removeWord((*array)[index],&count[index],word);
        }
            
        else if(choice=='C'){
            printf("The word number of the dictionary is %d\n",count[0]+count[1]+count[2]+count[3]+count[4]+count[5]+count[6]+count[7]);
        }
        else if(choice == 'Q'){
            printf("program is ending and dictionary will be sent to %s\n",outFileName);
            if(saveDictionary(outFileName, array, count, capacity)!=0){
                fprintf(stderr,"    save dictionary failed! Program terminating...\n");
            }
            break;
        }
        else if(choice == 'P'){
            printArray(*array,count);
        }
        else{
            printf("unrecognized order! please reinput!\n");
        }
        while ((ch = getchar()) != '\n' && ch != EOF);
    }while(1);
}

void removeWord(char** array,int* count,char word[]){
    int pos = search(array,word,*count);
    int i;
    if(pos==0){
        printf("%s ignored(not found)\n",word);
    }
    else{
        char* here = array[pos-1];
        for(i=pos-1;i<*count;i++){
            array[i]=array[i+1];
        }
        printf("%s removed\n",word);
        free(here);
        (*count)--;
    }
}

int search(char** array,char* word,int count){
	if(count==0){
		return 0;
	}
	else{	
		return binarySearch(array,word,0,count-1);
	}	
}

int binarySearch(char** array,char* word,int iMin,int iMax){
	if (iMax < iMin)
    		return 0;
  	else
   	 {
      		int iMid = (iMin+iMax)/2;
      		if(strcasecmp(word,array[iMid])<0)
            		return binarySearch(array, word, iMin, iMid-1);
      		else if (strcasecmp(word,array[iMid])>0)
      		      	return binarySearch(array, word, iMid+1, iMax);
      		else
        		return iMid+1;
    	}
}

int hash_function(char* word){
	int i=0;
    int hash_value = 0;
    while(word[i]!='\0'){
        hash_value+=(word[i]-'A');
        i++;
    }
    return hash_value%8;
}


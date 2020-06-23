#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>


#define ALPHABET_SIZE (224)
#define CHAR_TO_INDEX(c) ((int)c - (int)' ')
#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct TrieNode
{
    char letter;
    struct TrieNode *children[ALPHABET_SIZE];
    bool isEndOfWord;
    int f_count;

};
struct threadParameters{

    FILE *file;
    FILE *r_file;
    struct TrieNode *root;

};
struct TrieNode *getNode(void){
    struct TrieNode *pNode = NULL;

    pNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));

    if (pNode)
    {
        int i;

        pNode->isEndOfWord = false;

        for (i = 0; i < ALPHABET_SIZE; i++)
            pNode->children[i] = NULL;
    }

    return pNode;
}
void insert(struct TrieNode *root, const char *key){
    int level;
    int length = strlen(key);
    int index;

    struct TrieNode *pCrawl = root;

    for (level = 0; level < length; level++)
    {
        index = CHAR_TO_INDEX(key[level]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();

        pCrawl = pCrawl->children[index];
    }

    pCrawl->f_count = 1;
    pCrawl->isEndOfWord = true;
}
int search(struct TrieNode *root, const char *key){
    int level;
    int length = strlen(key);
    int index;
    struct TrieNode *pCrawl = root;

    for (level = 0; level < length; level++)
    {
        index = CHAR_TO_INDEX(key[level]);

        if (!pCrawl->children[index])
            return 0;

        pCrawl = pCrawl->children[index];
    }
    if(pCrawl !=NULL && pCrawl->isEndOfWord){

        pCrawl->f_count = pCrawl->f_count +1;
        return pCrawl->f_count;
    }
    else{
        return -1;
    }


}
void printtoFile(struct TrieNode *root,FILE *r_file,char *str, int level){


    if(root->isEndOfWord){
        str[level] = '\0';
        fprintf(r_file,"%s     %d\n",str,root->f_count);
    }
    int i;
    for (i = 0; i < ALPHABET_SIZE; i++)
    {
        if (root->children[i])
        {
            str[level] = i + ' ';
            printtoFile(root->children[i],r_file, str, level + 1);
            str[level] = '\0';
        }
    }

}

void doSequential(FILE *file,FILE *r_file, struct TrieNode *root){

    char *inputs = malloc(512*sizeof(char));

    while(!feof(file)){

        fgets(inputs,512,file);

        inputs [ strcspn(inputs, "\r\n") ] = '\0';

        int result_of_input = search(root,inputs);


        if(result_of_input == -1||result_of_input == 0){

            insert(root,inputs);

        }

    }


}
void doSequentialwithBuffer(FILE *file,FILE *r_file, struct TrieNode *root){

    char buffer1[512];
    char buffer2[512];

    fread(buffer1,sizeof(char),40,file);
    buffer1[40] = '\0';
    fgets(buffer2,512,file);
    strcat(buffer1,buffer2);

    while(!feof(file)){

        char *token = strtok(buffer1,"\n");

        while(token != NULL){

                int result_of_input = search(root,token);

                if(result_of_input == -1||result_of_input == 0){


                    insert(root,token);
                }

                token = strtok(NULL, "\n");

        }

        fread(buffer1,sizeof(char),40,file);
        buffer1[40] = '\0';
        fgets(buffer2,512,file);
        strcat(buffer1,buffer2);

    }

}

void *doSequentialwiththread(void *ptr){

    char *inputs = malloc(512*sizeof(char));
    struct threadParameters *parameters = (struct threadParameters *)ptr;

    while(!feof(parameters->file)){

        pthread_mutex_lock(&mutex);
        fgets(inputs,512,parameters->file);


        inputs [ strcspn(inputs, "\r\n") ] = '\0';

        int result_of_input = search(parameters->root,inputs);


        if(result_of_input == -1||result_of_input == 0){

            insert(parameters->root,inputs);

        }
        pthread_mutex_unlock(&mutex);


    }

    return NULL;

}
struct TrieNode *doSequentialincluderoot(FILE *file,FILE *r_file){

    char *inputs = malloc(512*sizeof(char));
    struct TrieNode *root = getNode();

    while(fgets(inputs,512,file)!=NULL){
        inputs [ strcspn(inputs, "\r\n") ] = '\0';

        int result_of_input = search(root,inputs);


        if(result_of_input == -1||result_of_input == 0){

            insert(root,inputs);

        }

    }

    return root;


}

void *doSequentialwiththreadincluderoot(void *ptr){

    char *inputs = malloc(512*sizeof(char));
    struct threadParameters *parameters = (struct threadParameters *)ptr;
    struct TrieNode *root = getNode();


    while(!feof(parameters->file)){

        //pthread_mutex_lock(&mutex);
        fgets(inputs,512,parameters->file);
        //pthread_mutex_unlock(&mutex);

        inputs [ strcspn(inputs, "\r\n") ] = '\0';

        int result_of_input = search(root,inputs);


        if(result_of_input == -1||result_of_input == 0){

            insert(root,inputs);

        }

    }
    return root;


}

int searchforCombine(struct TrieNode *root, const char *key,int i)
{
    int level;
    int length = strlen(key);
    int index;
    struct TrieNode *pCrawl = root;

    for (level = 0; level < length; level++)
    {
        index = CHAR_TO_INDEX(key[level]);

        if (!pCrawl->children[index])
            return 0;

        pCrawl = pCrawl->children[index];
    }
    if(pCrawl !=NULL && pCrawl->isEndOfWord){

        pCrawl->f_count = pCrawl->f_count + i;
        return pCrawl->f_count;
    }
    else{
        return -1;
    }


}
void insertforcombine(struct TrieNode *root, const char *key,int i){
    int level;
    int length = strlen(key);
    int index;

    struct TrieNode *pCrawl = root;

    for (level = 0; level < length; level++)
    {
        index = CHAR_TO_INDEX(key[level]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();

        pCrawl = pCrawl->children[index];
    }

    pCrawl->f_count = i;
    pCrawl->isEndOfWord = true;
}
void combineTries(struct TrieNode *root,struct TrieNode *generalroot,FILE *r_file,char *str, int level){


    if(root->isEndOfWord){

        str [ strcspn(str, "\r\n") ] = '\0';
        str[level] = '\0';
        int result_of_input = searchforCombine(generalroot,str,root->f_count);


        if(result_of_input == -1||result_of_input == 0){

            insertforcombine(generalroot,str,root->f_count);

        }
    }
    int i;
    for (i = 0; i < ALPHABET_SIZE; i++)
    {
        if (root->children[i])
        {
            str[level] = i + ' ';
            combineTries(root->children[i],generalroot,r_file, str, level + 1);
            str[level] = '\0';
        }
    }

}

void threadUseTask4(FILE *file, FILE *r_file, struct TrieNode *root){

    struct threadParameters parameters;
    parameters.file = file;
    parameters.r_file = r_file;
    parameters.root = root;

    pthread_t thread;
    pthread_create(&thread,NULL,doSequentialwiththread,&parameters);
    doSequential(file,r_file,root);
    pthread_join(thread,NULL);

    char *str = malloc(sizeof(char)*700);
    printtoFile(root,r_file,str,0);


}
void threadUseTask5(FILE *file, FILE *r_file){


    struct TrieNode *resultroot;
    struct TrieNode *resultroot2;
    struct threadParameters parameters;
    parameters.file = file;
    parameters.r_file = r_file;


    pthread_t thread;
    pthread_create(&thread,NULL,doSequentialwiththreadincluderoot,&parameters);
    resultroot2 = doSequentialincluderoot(file,r_file);
    pthread_join(thread,&resultroot);

    char *str = malloc(sizeof(char)*512);
    char *str2 = malloc(sizeof(char)*512);
    combineTries(resultroot,resultroot2,r_file,str,0);


    printtoFile(resultroot2,r_file,str2,0);

}
void doSequentialMemoryContent(char *token, struct TrieNode *root){

     while(token != NULL){
        token [ strcspn(token, "\r\n") ] = '\0';

        int result_of_input = search(root,token);


        if(result_of_input == -1||result_of_input == 0){

            insert(root,token);

        }
        token = strtok(NULL,"\n");

    }


}
void useTask6(FILE *file,FILE *r_file,struct TrieNode *root){

    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *allcontent = malloc(fsize + 1);
    fread(allcontent, 1, fsize, file);
    fclose(file);

    allcontent[fsize] = 0;

    char *token;
    token = strtok(allcontent,"\n");


    doSequentialMemoryContent(token,root);
    char *str = malloc(sizeof(char)*512);
    printtoFile(root,r_file,str,0);

}

int main()
{
    printf("Which data file to choose:\n");
    char filename[40];
    scanf("%s",filename);
    FILE *file = fopen(filename,"r");
    if(file == NULL){
        printf("Wrong input filename!!!\n");
        exit(0);
    }
    printf("Result file is result.txt\n");

    int check = 0;
    while(check != 7){


        printf("For Task 2, Enter the integer value (2) \n");
        printf("For Task 3, Enter the integer value (3) \n");
        printf("For Task 4, Enter the integer value (4) \n");
        printf("For Task 5, Enter the integer value (5) \n");
        printf("For Task 6, Enter the integer value (6) \n");
        printf("For Exit, Enter (7) \n");

        printf("Select: ");
        scanf("%d",&check);


        if(check == 2){
            time_t start, end;
            double cpu_time_used;
            start = time(NULL);
            FILE *r_file = fopen("result.txt","w");
            struct TrieNode *root = getNode();
            doSequential(file,r_file,root);

            char prefix[512];
            printtoFile(root,r_file,prefix,0);
            printf("Successfull entry = 2, Check result.txt\n");
            fclose(r_file);
            end = time(NULL);
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            printf("Elapsed time to do : %lf sec\n",cpu_time_used);
            printf("-----------------------------------------\n");
        }
        else if(check == 3){
            time_t start, end;
            double cpu_time_used;
            start = time(NULL);
            FILE *r_file = fopen("result.txt","w");
            struct TrieNode *root = getNode();
            doSequentialwithBuffer(file,r_file,root);

            char prefix[512];
            printtoFile(root,r_file,prefix,0);
            printf("Successfull entry = 3, Check result.txt\n");
            fclose(r_file);
            end = time(NULL);
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            printf("Elapsed time to do : %lf sec\n",cpu_time_used);
            printf("-----------------------------------------\n");

        }
        else if(check == 4){
            time_t start, end;
            double cpu_time_used;
            start = time(NULL);
            FILE *r_file = fopen("result.txt","w");
            struct TrieNode *root = getNode();

            threadUseTask4(file,r_file,root);

            printf("Successfull entry = 4, Check result.txt\n");
            fclose(r_file);
            end = time(NULL);
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            printf("Elapsed time to do : %lf sec\n",cpu_time_used);
            printf("-----------------------------------------\n");
        }
        else if(check == 5){
            time_t start, end;
            double cpu_time_used;
            start = time(NULL);
            FILE *r_file = fopen("result.txt","w");
            threadUseTask5(file,r_file);
            printf("Successfull entry = 5, Check result.txt\n");
            fclose(r_file);
            end = time(NULL);
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            printf("Elapsed time to do : %lf sec\n",cpu_time_used);
            printf("-----------------------------------------\n");

        }
        else if(check == 6){
            time_t start, end;
            double cpu_time_used;
            start = time(NULL);
            FILE *r_file = fopen("result.txt","w");
            struct TrieNode *root = getNode();

            useTask6(file,r_file,root);

            printf("Successfull entry = 6, Check result.txt\n");
            fclose(r_file);
            end = time(NULL);
            cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
            printf("Elapsed time to do : %lf sec\n",cpu_time_used);
            printf("-----------------------------------------\n");

        }
        else if(check == 7){

            printf("Exit!!! \n");

        }
        else{
            printf("Wrong number, Please try again");
            printf("-----------------------------------------\n");
        }
        rewind(file);



    }


    fclose(file);

    return 0;
}

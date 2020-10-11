/*Author: Sinaya Nudel 203191663*/
#include "assembler.h"

int main(int argc, char *argv[]){

	int status=0;

	if (argc < 2){
		fprintf(stderr,"Error. Could'nt find any file name.\n");
		return 1;
	}
	while(--argc){	
		status=open_file(argv[argc]);
		if(!status){ 
			fprintf(stderr,"Error. Couldn't open the file: \"%s%s\".\n", argv[argc],ENDING);
			return 2;
		}

	}

	return 0;
}

/*Gets the name of the file as parameter.
Opens the file and send the file to the relevent functions. 
Returns 1 if the file opened, 0 if couldn't open the file*/
int open_file(char *name){
	FILE *file;
	int status = 1; /*flag - 1 succesful file opening, 0 if errors occurred*/
	int IC; /*instuctions counter*/
	int DC; /*data counter*/
	int errors = 0; /*errors counter*/

	short data[MAX_WORDS]={0}; /*for data list*/
	short code[MAX_WORDS]={0}; /*for instuctions list*/
	
	lable_ptr symbols_head=NULL; /*symbols table head*/
	extern_ptr externs_head=NULL; /*externs list head*/

	char ending[] = ENDING;

        char *fullName = malloc(strlen(name) + strlen(ending) + 1); 
    	strcpy(fullName, name);
    	strcat(fullName, ending);

	file = fopen(fullName, "r");
	free(fullName);

	if (!file)
		status=0;
	else{
		fprintf(stdout,"\"%s%s\" was opened successfully.\n", name,ENDING);
		fprintf(stderr, "Errors for \"%s%s\":\n", name,ENDING);
		errors += file_reading(file, code, data, &symbols_head, &externs_head, &IC, &DC, FIRST_READING);
		if(errors==-1)
			fprintf(stderr,"notice: The file is empty.\n");
		else{
			if(errors==0) /*if after FIRST reading there is no errors - send the file to SECOND reading*/
				errors += file_reading(file, code, data, &symbols_head, &externs_head, &IC, &DC, SECOND_READING);
			fprintf(stderr,"Total number of errors in the file \"%s%s\": %d.\n", name ,ENDING,errors); 
			if(errors==0){ /*if after SECOND reading there is no errors- create output files*/
				create_obj(data, code, IC, DC, name);
				create_ext(&externs_head, name); 
				create_ent(&symbols_head, name);
				fprintf(stdout,"Successfully created output files for the file \"%s%s\".\n", name,ENDING);
			}

		}	reset(&symbols_head, &externs_head, data, code, IC ,DC); /*clean all the values from the lists*/

		fclose(file);
	}
	return status;
}

/*Gets the symbols table's head, externs head, data list, code(instuctions) list, data counter, instructions counter.
Resets and free all the lists values*/
void reset(lable_ptr* symbols_head, extern_ptr* externs_head, short* data, short* code, int ic, int dc){
	int i;

	for(i=0;i<ic;i++)
		code[i]=0;
	for(i=0;i<dc;i++)
		data[i]=0;
	free_lists(symbols_head, externs_head);
}

/*Gets data and code arrays as parametes with their counters and the name of the file.
Creates an object file*/
void create_obj(short* data, short* code, int ic, int dc, char* name){
	int i;
	FILE *file;
	char ending[] = ".ob";

        char *fullName = malloc(strlen(name) + strlen(ending) + 1); 
    	strcpy(fullName, name);
    	strcat(fullName, ending);

	file = fopen(fullName, "w");
	free(fullName);

	fprintf(file,"\t%d\t%d\n",ic,dc);
	for(i=0;i<ic;i++){
		fprintf(file, "%04d\t", (i+FIRST_ADDRESS));
		special_print(file, code[i]);
	}
	for(i=0;i<dc;i++){
		fprintf(file, "%04d\t", (ic+i+FIRST_ADDRESS));
		special_print(file, data[i]);
	}
	fclose(file);
}
/*Gets an externs list head and the name of the file.
Creates an extrerns file if the list isn't empty*/
void create_ext(extern_ptr* externs_head, char* name){
	if(!is_ext_empty(externs_head)){ /*checks if the list isn't empty*/ 
		FILE *file;
		char ending[] = ".ext";
		char *fullName = malloc(strlen(name) + strlen(ending) + 1); 
		strcpy(fullName, name);
		strcat(fullName, ending);
		file = fopen(fullName, "w");
		free(fullName);
		print_ext(file,externs_head); /*print all the externs in the list to the file*/
		fclose(file);
	}
}

/*Gets a symbols table head and the name of the file. 
If there is entries - creates an entries file*/
void create_ent(lable_ptr* symbols_head, char* name){ 
	if(!is_ent_empty(symbols_head)){/*checks if the list includes any entry.*/ 
		FILE *file;
		char ending[] = ".ent";
		char *fullName = malloc(strlen(name) + strlen(ending) + 1); 
		strcpy(fullName, name);
		strcat(fullName, ending);
		file = fopen(fullName, "w");
		free(fullName);
		print_ent(file,symbols_head); /*print all the entries in the list to the file*/
		fclose(file);
	}
}

/*Gets a file to write to, and a value (short type) as parametrs.
Prints the value to the file with "/" and "." instead of 1 and 0*/
void special_print(FILE* file, short word){
	short mask=1<<(WORD_BITS-1);

	while(mask){
		if(word&mask)
			fprintf(file,"/");
		else
			fprintf(file,".");
		mask>>=1;
	}
	fprintf(file,"\n");

}
			



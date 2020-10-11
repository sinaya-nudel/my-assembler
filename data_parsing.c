/*Author: Sinaya Nudel 203191663*/
#include "assembler.h"

/*this function send the elements to the speciefic parsing method, according to its data type.
returns number of errors*/
int parse_guidance(char *elements[MAX_ELEMENTS], Data_type data_type ,short* data, lable_ptr* symbols_head, char label[], Boolean isLabel, int line_num, int* DC, Reading reading){
	int errors=0;
	if(reading==FIRST_READING){ 
		switch(data_type) {
				case DATA :
					errors+=parse_data(elements,data, symbols_head,label, isLabel,line_num, DC);
					break; 
				case STRING :
					errors+=parse_string(elements,data, symbols_head,label, isLabel,line_num, DC);
					break;
				case EXTERN :
					errors+=parse_extern(elements,symbols_head,label, isLabel,line_num);
					break;
				case ENTRY : /*entries parsing is only at the second reading*/
				case NONE_DT :
					break;
				}
	}
	else if(reading==SECOND_READING && data_type==ENTRY)
		errors+=parse_entry(elements, symbols_head, label, isLabel,line_num); 

	return errors;
}

/*gets the line elements, the data array, the symbols head, the label name, boolean value if there is label, number of line, and DC counter.
parsing the data and returns number of errors*/
int parse_data(char *elements[MAX_ELEMENTS],short* data, lable_ptr* symbols_head, char label[], Boolean isLabel, int line_num, int* DC){
	int i=0;
	int j=0;
	int errors=0;
	Boolean comma_flag=TRUE;
	Boolean is_valid;  
	int nums[MAX_ELEMENTS]; /*int array, in order to store all the integers from the line*/

	if(elements[i]==NULL){
		fprintf(stderr,"Error in line: %d. There is no parameters for '.data' - must be at least one integer.\n",line_num);
		errors++;
		return errors;
	}
	for(i=0;elements[i]!=NULL&&i<=MAX_ELEMENTS;i++){
		is_valid=is_valid_num(elements[i], WORD_BITS);
		if(is_valid){
			if(comma_flag){	
				comma_flag=FALSE; 
				nums[j]=atoi(elements[i]);
				j++;
			}
			else{
				fprintf(stderr,"Error in line: %d. There is a missing comma.\n",line_num);
				errors++;
			}
				
		}
		else if(!strcmp(elements[i], ",")){
			if(!comma_flag){	
				comma_flag=TRUE;
			}
			else{
				fprintf(stderr,"Error in line: %d. There is an extra comma.\n",line_num);
				errors++;
			}
		}
		else{
			fprintf(stderr,"Error in line: %d. There is an invalid parameter for '.data' - must be integers only.\n",line_num);
			errors++;
		}
	}
	if(comma_flag){
		fprintf(stderr,"Error in line: %d. There is an extra comma at the end of the parameters.\n",line_num);
		errors++;
	}
	if(!errors)
		errors+=add_data(nums,j,data,symbols_head,label,isLabel,DC,line_num);
	return errors;
		
}

/*this function add the integers list to the data array. if there is label, adds the label to the symbols table with the according counter for the first number in the list.
returns number of errors*/
int add_data(int nums[], int len,short* data, lable_ptr* symbols_head,char label[], Boolean isLabel,int* DC,int line_num){
	int i;
	int errors=0;
	if(len>0&&isLabel)
		add_label(label, *DC, FALSE, FALSE, FALSE , symbols_head);
	for(i=0;i<len;i++){
		if(*DC>=MAX_WORDS){
			fprintf(stderr,"Error in line %d. The data counter exceeds the maximun number = %d.\n",line_num,MAX_WORDS);
			errors++;
			return errors;
		}
		data[*DC]=(short)nums[i];
		(*DC)++;
	}
	return errors;
}		

/*parsing the string data line. returns number of errros*/
int parse_string(char *elements[MAX_ELEMENTS],short* data, lable_ptr* symbols_head, char label[], Boolean isLabel, int line_num, int* DC){
	int errors=0;
	if(elements[0]==NULL){
		fprintf(stderr,"Error in line: %d. There is no parameter for '.string'.\n",line_num);
		errors++;
		return errors;
	}
	if(!is_valid_string(elements[0])){
		fprintf(stderr,"Error in line: %d. The parameter for '.string' must be a string .\n",line_num);
		errors++;
	}
	if(elements[1]!=NULL){
		fprintf(stderr,"Error in line: %d. There is an extra parameters for '.string' - must be only one parameter.\n",line_num);
		errors++;
	}
	if(!errors)
		errors+=add_string(elements[0],data,symbols_head,label,isLabel,DC,line_num);
	return errors;
}

/*gets a string and checks if this is valid string data type with " at the beggining and at the end.
returns true if valid, else returns false*/
Boolean is_valid_string(char* str){
	int i;
	if(str==NULL||str[0]!='"')
		return FALSE;
	for(i=1;str[i]!='"'&&str[i]!=0;i++)
		;
	if(str[i]==0)
		return FALSE;
	if(str[i]=='"'&&str[i+1]!=0)
		return FALSE;
	return TRUE;
}

/*this function adds the string to the data array. if there is label, adds the label to the symbols table with the according counter for the first char in the string.
returns number of errors*/		
int add_string(char* str, short* data, lable_ptr* symbols_head,char label[], Boolean isLabel,int* DC,int line_num){
	int i;
	int errors=0;
	if(isLabel)
		add_label(label, *DC, FALSE, FALSE ,FALSE, symbols_head);
	for(i=1;str[i]!='"';i++){
		if(*DC>=MAX_WORDS){
			fprintf(stderr,"Error in line %d. The data counter exceeds the maximun number = %d.\n",line_num,MAX_WORDS);
			errors++;
			return errors;
		}
		data[*DC]=(short)str[i];
		(*DC)++;
	}
	data[*DC]=0; /*adding zero to the data array for the end of the string*/
	(*DC)++;

	return errors;
}

/*this function parsing the entry line with its elements. returns number of errors*/
int parse_entry(char *elements[MAX_ELEMENTS], lable_ptr* symbols_head, char label[], Boolean isLabel, int line_num){
	int errors=0;
	if(isLabel)
		fprintf(stderr,"Notice: in line %d there is a label in a wrong location. The assembler will ignore that label.\n",line_num);

	if(elements[0]==NULL){
		fprintf(stderr,"Error in line: %d. There is no parameter for '.entry'.\n",line_num);
		errors++;
		return errors;
	}
	if(!is_valid_label(elements[0],line_num,symbols_head,TRUE,SECOND_READING)){
		errors++;
	}
	if(elements[1]!=NULL){
		fprintf(stderr,"Error in line: %d. There is an extra parameters for '.entry' - must includes only one parameter (label).\n",line_num);
		errors++;
	}
	if(!errors && !find_and_change_entry(elements[0], symbols_head)){ /*if the line parsing was valid but this label isn't exist in the table*/
		fprintf(stderr,"Error in line: %d. The label for '.entry' didn't found.\n",line_num);
		errors++;
	}
	return errors;
}

/*this function parsing the extern line with its elements. returns number of errors*/
int parse_extern(char *elements[MAX_ELEMENTS], lable_ptr* symbols_head, char label[], Boolean isLabel, int line_num){
	int errors=0;
	if(isLabel)
		fprintf(stderr,"Notice: in line %d there is a label in a wrong location. The assembler will ignore that label.\n",line_num);
	if(elements[0]==NULL){
		fprintf(stderr,"Error in line: %d. There is no parameter for '.extern'.\n",line_num);
		errors++;
		return errors;
	}
	if(!is_valid_label(elements[0],line_num,symbols_head,TRUE,FIRST_READING)){
		errors++;
	}
	if(elements[1]!=NULL){
		fprintf(stderr,"Error in line: %d. There is an extra parameters for '.extern' - must includes only one parameter (label).\n",line_num);
		errors++;
	}
	if(!errors)
		add_label(elements[0], 0, TRUE, FALSE, FALSE , symbols_head);
	return errors;
}

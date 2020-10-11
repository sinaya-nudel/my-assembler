/*Author: Sinaya Nudel 203191663*/
#include "assembler.h"

/*this function gets: the line tokens, the command type (Cmd), code array, symbols table head, externs head, label name as stirng, boolean value if there is label, line number, IC, and reading value- if second or first. 
Collecting the adressing methods from the elements, and checking if its valid for the relvent command type.
If first reading - adds label, calculates IC.
If second reading - coding the command to the code array.
returns number of errors*/
int parse_code(char *elements[MAX_ELEMENTS], Cmd cmd, short* code, lable_ptr* symbols_head, extern_ptr* externs_head, char label[], Boolean isLabel, int line_num, int* IC, Reading reading){
	int errors=0;
	Params this_params= {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS,{0},{0},{0},{0}};

	int params = get_params(cmd);
	if(!params && elements[0]!=NULL){
		fprintf(stderr,"Error in line: %d. This command should'nt include parametrs.\n",line_num);
		errors++;
	}
	if(params==1)
		errors+=get_dest_addressing(&this_params,elements,symbols_head,line_num, reading);
	if(params==2)
		errors+=get_source_and_dest_addressing(&this_params,elements,symbols_head,line_num,reading);
	if(!errors)
		errors+=is_valid_operands(cmd,this_params,line_num);
	if(!errors){
		if(reading==FIRST_READING){
			if(isLabel)
				add_label(label, *IC, FALSE, TRUE,FALSE , symbols_head);

			(*IC)+=calculate_words(this_params);
			if(*IC>=MAX_WORDS){
				fprintf(stderr,"Error in line %d. The instuctions counter exceeds the maximun number = %d.\n",line_num,MAX_WORDS);
				errors++;
			}
		}
		if(reading==SECOND_READING)
			errors+=add_code(IC, code, symbols_head, externs_head, cmd, this_params, line_num);
	}
	return errors;
}

/* Gets a Params struct (=the command's parameters) and calculating the number of words the command should take in the code array.
Returns number of errors*/
int calculate_words(Params this_params){
	int words=1;

	if(this_params.source!=NONE_ADRS)
		words++;	
	if(this_params.dest!=NONE_ADRS && this_params.dest!=REGISTER)
		words++;
	if(this_params.dest==REGISTER && this_params.source!=REGISTER)
		words++;
	if(this_params.dest==JUMP){
		if(this_params.first_jumping_param==REGISTER && this_params.second_jumping_param==REGISTER)
			words++;
		else
			words+=2;
	}

	return words;
}

/*Gets a Cmd (=command type) ,Params struct (= the command's parameters) and line number as int.
Checking if the command type supports the given adressing methods.
Returns number of errors*/
int is_valid_operands(Cmd cmd, Params this_params, int line_num){
	int errors=0;

	if(is_supported_addressing_method(cmd, this_params.source, this_params.dest)==FALSE){
		fprintf(stderr,"Error in line: %d. There is invalid addressing method for this command. %d %d \n",line_num, this_params.source,this_params.dest);
		errors++;
	}
	return errors;
}

/*Gets an Addressing_method and its parameter string, the line tokens, symbols table's head, line number as int, current token index as int, and Reading cycle (first or second).
Checking what is the addressing method for this single operand by the line tokens, and set the value and the string into the parameters. 
Returns number of errors*/
int get_operand(Addressing_method* operand, char* operand_string, char *elements[],lable_ptr* symbols_head,int line_num, int* i,Reading reading){

	int errors=0;

	if(elements[(*i)]==NULL){/*if empty token -> missing operand)*/
		errors++;
		fprintf(stderr,"Error in line: %d. Missing operands.\n",line_num);
	}
	else if(!strcmp(elements[(*i)], "#")){ /*if starts as immediate operand*/ 
		(*i)++;
		if(elements[(*i)]==NULL){
			errors++;
			fprintf(stderr,"Error in line: %d. Missing number after operand '#'.\n",line_num);
		}
		else{
			if(is_valid_num(elements[(*i)], IMMEDIATE_BITS)){ /*checking if the number of the immediate operand is valid*/
				strcpy(operand_string,elements[(*i)]); /*set the operand's string parameter*/
				(*operand)=IMMEDIATE;/*set the operand's type parameter*/
				(*i)++;
			}

			else{
				errors++;
				fprintf(stderr,"Error in line: %d. Invalid number - must be an integer after '#'.\n",line_num);
			}
		}
	}
	else if(get_register(elements[(*i)])!=NONE_RGSTR){ /*if register operand*/
		(*operand)=REGISTER;/*set the operand's type parameter*/
		strcpy(operand_string,elements[(*i)]);/*set the operand's string parameter*/
		(*i)++;
	}
	else if(is_valid_label(elements[(*i)],line_num,symbols_head,FALSE,reading)){ /*if starts as label -> jumping\direct operand*/
		(*i)++;
		if(elements[(*i)]!=NULL && !strcmp(elements[(*i)], "(")){ /*if continue as jumping operand*/
			strcpy(operand_string,elements[(*i)-1]); /*set the operand's string parameter - the label*/
			(*operand)=JUMP;/*set the operand's type parameter*/
			(*i)++;
		}
		else{/*if not jumping -> must be direct ,because of the label*/
			(*operand)=DIRECT; /* set the operand's type parameter*/
			strcpy(operand_string,elements[(*i)-1]);/*set the operand's string parameter - the lable*/
		}
	}

	return errors;
}
/*Gets a Param struct (for the command's parameters), the line tokens, symbols table's head, line number as int and Reading cycle (first or second).
Checking what is the addressing method for the dest and source operands by the line tokens, and set the values and the strings into the parameters. 
Returns number of errors*/
int get_source_and_dest_addressing(Params* this_params, char *elements[MAX_ELEMENTS],lable_ptr* symbols_head, int line_num,Reading reading){
	int errors=0;
	int i=0;

	errors+=get_operand(&this_params->source,this_params->source_string, elements,symbols_head,line_num, &i,reading);
	
	if(!errors){
		if((this_params->source)==NONE_ADRS||(this_params->source)==JUMP){/*checking if the source operand is valid*/		
			errors++;
			fprintf(stderr,"Error in line: %d.Invalid source operand.\n",line_num);
		}
		else if(elements[i]==NULL){
			errors++;
			fprintf(stderr,"Error in line: %d. Missing comma and operand.\n",line_num);
		}
		else if(!strcmp(elements[i], ",")){
			i++;
			errors+=get_operand(&this_params->dest,this_params->dest_string, elements,symbols_head, line_num, &i,reading);
			if(!errors){
				if((this_params->dest)==NONE_ADRS||(this_params->dest)==JUMP){	/*cheking if the dest operand is valid*/	
					errors++;
					fprintf(stderr,"Error in line: %d. Invalid destination operand.\n",line_num);
				}
				else if(elements[i]!=NULL){
					errors++;
					fprintf(stderr,"Error in line: %d. Extraneous text after end of command.\n",line_num);
				}
			}
		}
		else{
			errors++;
			fprintf(stderr,"Error in line: %d. Missing comma.\n",line_num);
		}
	
	}
	return errors;
}	

/*Gets a Param struct (for the command's parameters), the line tokens, symbols table's head, line number as int and Reading cycle (first or second).
Checking what is the addressing method for the dest operand by the line tokens, and set the value and the string into the parameters. 
Returns number of errors*/
int get_dest_addressing(Params* this_params, char *elements[MAX_ELEMENTS],lable_ptr* symbols_head ,int line_num,Reading reading){

	int errors=0;
	int i=0;

	errors+=get_operand(&this_params->dest, this_params->dest_string, elements,symbols_head, line_num, &i,reading);
	
	if(!errors){
		if((this_params->dest)==NONE_ADRS){		
			errors++;
			fprintf(stderr,"Error in line: %d. Invalid destination operand.\n",line_num);
		}
		else if((this_params->dest)==JUMP){
			errors+=get_operand(&this_params->first_jumping_param , this_params->first_jumping_param_string , elements,symbols_head,line_num, &i,reading);
			if(!errors){
				if((this_params->first_jumping_param)==JUMP||(this_params->first_jumping_param)==NONE_ADRS){
					errors++;
					fprintf(stderr,"Error in line: %d. Invalid parameter for 'jump' addressing method.\n",line_num);
				}
				else{
					if(elements[i]!=NULL && !strcmp(elements[i], ",")){
						i++;
						errors+=get_operand(&this_params->second_jumping_param , this_params->second_jumping_param_string, elements,symbols_head, line_num, &i,reading);
						if(!errors){
							if((this_params->second_jumping_param)==JUMP||(this_params->second_jumping_param)==NONE_ADRS){
								errors++;
								fprintf(stderr,"Error in line: %d. Invalid parameter for 'jump' addressing method.\n",line_num);
							}
							else{
								if(elements[i]!=NULL && !strcmp(elements[i], ")")){
									i++;
									if(elements[i]!=NULL){
										errors++;
										fprintf(stderr,"Error in line: %d. Extraneous text after end of command.\n",line_num);
									}
								}
								else{
									errors++;
									fprintf(stderr,"Error in line: %d. Missing ')' for 'jump' addressing method.\n",line_num);
								}
							}
									
						}/*end of if(!errors)*/
					}/*end of if(elements[i]!=NULL && !strcmp(elements[i], ",")*/

					else{
						errors++;
						fprintf(stderr,"Error in line: %d. Missing comma.\n",line_num);
					}
				}/*end of else*/

			}/*end of if(!errors)*/


		}/*end of else if((*dest)==JUMP)*/
		else if(elements[i]!=NULL){
			errors++;
			fprintf(stderr,"Error in line: %d. Extraneous text after end of command.\n",line_num);
		}
	
	}/*end of if(!errors)*/
	return errors;
}











/*Author: Sinaya Nudel 203191663*/
#include <ctype.h>
#include "assembler.h"

/*Each command has command name, opcode, number of parameters, supported addressing methods for source operand, and supported addressing methods for dest operand.
Notice that each empty cell of the addressing method's array must intializied to NONE_ADRS. Otherwise, it will be automatically intialized to 0 - and according to the addressing method enum type, 0 = IMMEDIATE. That will cause a logical error!!! */
static const command cmd_arr[] = {
	{"mov",0,2, {IMMEDIATE,DIRECT,REGISTER,NONE_ADRS} , {DIRECT,REGISTER,NONE_ADRS,NONE_ADRS}}, 
	{"cmp",1,2, {IMMEDIATE,DIRECT,REGISTER,NONE_ADRS} , {IMMEDIATE,DIRECT,REGISTER,NONE_ADRS}},
	{"add",2,2, {IMMEDIATE,DIRECT,REGISTER,NONE_ADRS} , {DIRECT,REGISTER,NONE_ADRS,NONE_ADRS}},
	{"sub",3,2, {IMMEDIATE,DIRECT,REGISTER,NONE_ADRS} , {DIRECT,REGISTER,NONE_ADRS,NONE_ADRS}},
	{"not",4,1, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {DIRECT,REGISTER,NONE_ADRS,NONE_ADRS}},
	{"clr",5,1, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {DIRECT,REGISTER,NONE_ADRS,NONE_ADRS}},
	{"lea",6,2, {DIRECT,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {DIRECT,REGISTER,NONE_ADRS,NONE_ADRS}},
	{"inc",7,1, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {DIRECT,REGISTER,NONE_ADRS,NONE_ADRS}},
	{"dec",8,1, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {DIRECT,REGISTER,NONE_ADRS,NONE_ADRS}},
	{"jmp",9,1, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {DIRECT,JUMP,REGISTER,NONE_ADRS}},
	{"bne",10,1, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {DIRECT,JUMP,REGISTER,NONE_ADRS}},
	{"red",11,1, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {DIRECT,REGISTER,NONE_ADRS,NONE_ADRS}},
	{"prn",12,1, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {IMMEDIATE,DIRECT,REGISTER,NONE_ADRS}},
	{"jsr",13,1, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {DIRECT,JUMP,REGISTER,NONE_ADRS}},
	{"rst",14,0, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS}},
	{"stop",15,0, {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS} , {NONE_ADRS,NONE_ADRS,NONE_ADRS,NONE_ADRS}}
}; 

static const char* registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

static const char* data_types[]={"data","string","entry","extern"};

/*gets the number as string, and number of bits- and check if this string is valid integer that can be express in this maximun number of bits.
returns true if valid, else retrurns false*/
Boolean is_valid_num(char x[], int bits){
	int i;
	float num;	
	int base=2;
	long max_num=1;
	long min_num=-1;

	if(x==NULL)
		return FALSE;
	for(i=0;x[i]!='\0';i++){
		if(x[i]=='-'||x[i]=='+'){
			if(i>0) /*if the minus or the plus is not in front of the number - this is not a signed integer. It's an invalid char*/
				return FALSE;
		}
		else if(!isdigit(x[i])) /*if there is invalid char (non integer char) which is not minus or plus sign*/
				return FALSE;
	}
	num=atoi(x);
	/*calculating the max number and the min number according to the number of bits*/
	for(i=0;i<bits;i++){
		max_num*=base;
		min_num*=base;
	}	
	min_num+=1;
	max_num-=1;	

	if(num<min_num||num>max_num)
		return FALSE;
	
	return TRUE;
}

/*gets a string that supposed to be a register name, and returns the register itself as the enum type. If the name isn't a register -returns NONE_RGSTR*/ 
Register get_register(char* element){
	int i;
	if(element!=NULL&&strlen(element)==2){
		for (i=0; i<REGISTERS_LEN; i++){
          		if (!strcmp(element, registers[i]))
				return i;
		}

	}
	return NONE_RGSTR;
}

/*gets a label name, line number, symbols table head, boolean value if new label, and Reading type - second or first.
checks if the label contains valid chars, if it's unique, and if not exceed the maximun label length.  
returns true if valid label and false if not. */
Boolean is_valid_label(char * label, int line_num, lable_ptr* symbols_head, Boolean isNew,Reading reading){
	int i;
	if (label!=NULL&&label[0]!=0&&isalpha(label[0])){
		for(i=1;i<strlen(label);i++){ 
			if (!isalpha(label[i])&&!isdigit(label[i])){
				if(reading==FIRST_READING)
					fprintf(stderr,"Error in line: %d. Label must contains letters and integers only.\n",line_num);
				return FALSE;
			}
		}
		if(is_label_exist(label,symbols_head,isNew)){
				if(reading==FIRST_READING){
					fprintf(stderr,"Error in line: %d. Label must be unique.\n",line_num);
					return FALSE;
				}
				else if(reading==SECOND_READING)
					return TRUE;
		}
		if(strlen(label)>MAX_LABLE_NAME){
			if(reading==FIRST_READING)
				fprintf(stderr,"Error in line: %d. Invalid label length. Maximun length is %d.\n",line_num,MAX_LABLE_NAME);
			return FALSE;
		}	
		return TRUE;
	}
	else{
		if(reading==FIRST_READING)
			fprintf(stderr,"Error in line: %d. Label must begin with a letter.\n",line_num);
		return FALSE;
	}

}

/*gets a command name as string.
returns the Cmd type.
If no command found- returns NONE_CMD*/
Cmd find_cmd(char* token){
	int i;
	Cmd cmd=NONE_CMD;
	for(i=0;i<CMD_LEN;i++){
		if(!strcmp(token,cmd_arr[i].name)){
			cmd=i;
			break;
		}
	}
	return cmd;
}

/*gets cmd type as parameter, and returns its opcode number. 
if no Cmd type was found - returns -1*/
int get_opcode(Cmd cmd){
	if(cmd>=0&&cmd<CMD_LEN)
		return cmd_arr[cmd].opcode;
	else
		return -1;
}
/*Gets command type, and addressing methods for source and dest operands as parameters. 
If one of the operands addressing methods doesn't supported by this command - returns false. Else, returns true.*/
Boolean is_supported_addressing_method(Cmd cmd, Addressing_method source, Addressing_method dest){
	int i;
	Boolean isValidDest=FALSE;
	Boolean isValidSource=FALSE;
	if(cmd>=0&&cmd<CMD_LEN){
		for(i=0; i<ADDRESSING_METHODS_TYPES;i++){
			if(cmd_arr[cmd].dest[i]==dest)
				isValidDest=TRUE;
		}
		for(i=0; i<ADDRESSING_METHODS_TYPES;i++){
			if(cmd_arr[cmd].source[i]==source)
				isValidSource=TRUE;
		}
	}
	if(!isValidDest)
		fprintf(stderr,"false dest %d %d .\n",cmd,dest);
	if(!isValidSource)
		fprintf(stderr,"false source .\n");
	return (isValidDest && isValidSource);
}

/*gets cmd type as parameter, and returns its numebr of parameters. 
if no Cmd type was found - returns -1*/
int get_params(Cmd cmd){
	if(cmd>=0&&cmd<CMD_LEN)
		return cmd_arr[cmd].params;
	else
		return -1;
}

/*gets a label name, symbols table head, and boolean value if this label supposed to be a new one.
checks if this label exist in the table (relevent only if this label supposed to be new) or in the other basic setting names such as data types, registers or commands.
returns true if exist, else returns false*/
Boolean is_label_exist(char* label,lable_ptr* symbols_head,Boolean isNew){
	int i;
	Cmd cmd;
	if(label!=NULL){
		if(isNew&&search_label(label,symbols_head)) /*if this label supposed to be new but the label already exist in the tabel*/
			return TRUE;

		for(i=0;i<DATA_TYPES_LEN;i++){
			if(!strcmp(label,data_types[i])){
				return TRUE;
			}
		}
		if(get_register(label)!=NONE_RGSTR)
			return TRUE;
		if((cmd=find_cmd(label))!=NONE_CMD)
			return TRUE;
	}
	return FALSE;

}

/*gets a data type name as string, and returns the data type itself as the enum type.
if no data type was found- return NONE_DT*/ 
Data_type find_Data_type(char * token){
	int i;
	Data_type data_type=NONE_DT;
	if(token[0]=='.'){ /*each data type must start with '.' */
		char * ptr= token;
		ptr++;
		for(i=0;i<DATA_TYPES_LEN;i++){
			if(!strcmp(ptr,data_types[i])){
				data_type=i;
				break;
			}
		}
	}
	return data_type;
}





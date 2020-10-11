/*Author: Sinaya Nudel 203191663*/
#include "assembler.h"
/*this function gets: the code array, IC counter, the number to insert to the code, starting and ending indexes for the bits range in the word. 
Inserting the number to the code array, in a position provided by the IC counter and the bits range indexes.*/
void set_code(short* code, int IC,int num, int start, int end){	
	int i;
	short isBit=1;
	short mask=1;
	mask<<=start;
	for(i=start;i<=end;i++){
		if(isBit&num)
			code[IC]|=mask;
		isBit<<=1;
		mask<<=1;
	}
}
/*this function gets: the code array, IC counter, source and destination registers as strings.
Finding each register value by its name, and insering each register to the code array in a position provided by the IC counter.
One word can contains both dest and source registers, just in different bits positions.*/
void set_registers(short* code,int IC, char* source, char* dest){
	Register source_register;
	Register dest_register;
	code[IC]=0;

	source_register = get_register(source);
	dest_register = get_register(dest);

	if(dest_register!=NONE_RGSTR)
		set_code(code,IC, dest_register, DEST_REGISTER_START, DEST_REGISTER_END);
	if(source_register!=NONE_RGSTR)
		set_code(code,IC, source_register, SOURCE_REGISTER_START, SOURCE_REGISTER_END);
}

/*this function gets: the code array, IC counter, label name as string, symbols table head, externs head, and line number as int.
Searching for the label in the symbols table and check its type. 
If no lable found in the table - returns error.
Else, inserting the label to the code array with its type and address.
If the label found as external data - also inserting the label to the externs list.
Returns number of errors (1 or 0)*/
int set_label(short* code, int IC, char* label, lable_ptr* symbols_head, extern_ptr* externs_head, int line_num){
	int errors=0;
	Coding_type type;
	int address;
	code[IC]=0;

	type=get_symbol_type(label, symbols_head);
	if(type==NONE_CT){
		errors++;
		fprintf(stderr,"Error in line %d. Unknown label.\n",line_num);
		return errors;
	}
	if(type==E)
		add_ext(label, IC+FIRST_ADDRESS, externs_head);

	set_code(code, IC,type, ERA_START, ERA_END);
	address=get_address(label, symbols_head);
	set_code(code,IC, address, ADDRESS_START, ADDRESS_END);

	return errors;
}
	

/*this function gets: the code array, IC counter, and number to insert as string
Inserting immediate operand to the code array, assumes the string is valid.*/
void set_immediate(short* code, int IC, char* num_string){
	int num;
	code[IC]=0;

	num= atoi(num_string);
	set_code(code,IC, num, IMMEDIATE_START, IMMEDIATE_END);
}

/*this function gets: the code array, IC counter, parameters and opcode.
Inserting the operation code to the array, with values provided by the parameters struct and the opcode.*/	
void set_operation(short* code,int IC,Params this_params, int opcode){
	code[IC]=0;

	if(this_params.dest!=NONE_ADRS)
		set_code(code,IC, this_params.dest, DEST_START, DEST_END);

	if(this_params.source!=NONE_ADRS)
		set_code(code,IC, this_params.source, SOURCE_START, SOURCE_END);

	set_code(code,IC, opcode, OPCODE_START, OPCODE_END);

	if(this_params.second_jumping_param!=NONE_ADRS)
		set_code(code,IC, this_params.second_jumping_param, SECOND_PARAM_START, SECOND_PARAM_END);

	if(this_params.first_jumping_param!=NONE_ADRS)
		set_code(code,IC, this_params.first_jumping_param, FIRST_PARAM_START, FIRST_PARAM_END);
}

/*this function gets: the code array, IC counter, symbols table head, externs list head, command type, parameters and line number.
Inserting all the operation's codes to the array and adding the external label to the externs list if necessary*/	
int add_code(int* IC, short* code, lable_ptr* symbols_head, extern_ptr* externs_head, Cmd cmd, Params this_params,int line_num){
	int errors=0;
	int opcode = get_opcode(cmd);

	set_operation(code,*IC,this_params,opcode); 
	(*IC)++;

	if(this_params.source==IMMEDIATE){
		set_immediate(code,*IC, this_params.source_string);
		(*IC)++;
	}
	else if(this_params.source==DIRECT){
		errors+=set_label(code, *IC, this_params.source_string, symbols_head,externs_head, line_num);
		(*IC)++;	
	}
	if(this_params.dest==REGISTER || this_params.source==REGISTER){
		set_registers(code,*IC, this_params.source_string, this_params.dest_string);
		(*IC)++;
	}
	if(this_params.dest==IMMEDIATE){
		set_immediate(code,*IC, this_params.dest_string);
		(*IC)++;
	}
	else if(this_params.dest==DIRECT||this_params.dest==JUMP){
		errors+=set_label(code, *IC, this_params.dest_string, symbols_head, externs_head, line_num);
		(*IC)++;	
	}
	if(this_params.dest==JUMP){
		if(this_params.first_jumping_param==IMMEDIATE){
			set_immediate(code,*IC, this_params.first_jumping_param_string);
			(*IC)++;
		}
		else if(this_params.first_jumping_param==DIRECT){
			errors+=set_label(code,*IC, this_params.first_jumping_param_string, symbols_head, externs_head, line_num);
			(*IC)++;	
		}
		if(this_params.second_jumping_param==REGISTER || this_params.first_jumping_param==REGISTER){
			set_registers(code,*IC, this_params.first_jumping_param_string, this_params.second_jumping_param_string);
			(*IC)++;
		}
		if(this_params.second_jumping_param==IMMEDIATE){
			set_immediate(code,*IC, this_params.second_jumping_param_string);
			(*IC)++;
		}
		else if(this_params.second_jumping_param==DIRECT){
			errors+=set_label(code, *IC, this_params.second_jumping_param_string, symbols_head, externs_head, line_num);
			(*IC)++;	
		}

	}
	
	return errors;
}










	

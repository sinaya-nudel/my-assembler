/*Author: Sinaya Nudel 203191663*/
#include "assembler.h"

/*Gets a file, code and data lists (as short array) and their counters, symbols table head and externs list head, and Reading flag- that can be FIRST or SECOND reading.
Returns number of errors or -1 if the file was empty.
The function reads each line from the file and send the elements to the pharse line function. If the file ends, checks the errors result and if there is no errors updates the values*/
int file_reading(FILE* file, short* code, short* data, lable_ptr* symbols_head, extern_ptr* externs_head, int *IC, int *DC, Reading reading){
	int lines=0; /*lines counter. empty lines (only white chars) also considered*/
	int errors=0;
	char line[LINE_LEN]; 
	if(reading==FIRST_READING){ /*if it's the first passing - intiallized data and code counters */ 
		(*IC)=0;
		(*DC)=0;
	}
	else if(reading==SECOND_READING){  /*if it's the second passing - intiallized code counters, and start from the beggining of the file again */ 
		(*IC)=0;
		rewind(file);
	}
	while(fgets(line, LINE_LEN, file)){
		lines++;
		if(line[0]!='\n')
			errors+=parse_line(lines,line,code, data ,symbols_head,externs_head,IC,DC, reading);
	}

	if(lines && !errors && reading==FIRST_READING){ /*if the function found lines and there was no errors- after the first reading- the address of each relevent symbol should updated with the ic and the first address values*/
		updated_data_val((*IC),symbols_head);
	}
	if(!lines)
		errors=-1;	
	
	return errors;	
}

/*Parsing each line, checks what kind of line it is - data/code/empty line/comment, and send the elements from the line to the relevent function. 
Returns the number of errors.*/
int parse_line(int line_num, char* line, short* code, short* data, lable_ptr* symbols_head, extern_ptr* externs_head, int *IC, int *DC, Reading reading){
	int i,j;
	int errors=0;
	char* token; /*pointer to an element of the line*/
	char label[MAX_LABLE_NAME]; /*the data/code label*/
	Data_type data_type; /*the data type*/
	Cmd cmd; /*the command type*/
	char temp[LINE_LEN*2]; 
	char *elements[MAX_ELEMENTS] = {0}; /*array of all the elements of the line that was seperated by white chars*/
	int token_len; /*the length of the token element*/
	Boolean isLabel=FALSE;/*label flag*/
	Boolean isOnlySpaces=TRUE; /*flag that notice if no-white char was found*/

	/*As the function uses white chars as delimeter- the code below copy the original line to new line with spaces in the relevent places so the specifiec elements will be seperated too. Also there is a check if the line include at list one no-white char*/
	for(i=0,j=0;line[i]!='\n';i++,j++){
		if(line[i]!=' '&&line[i]!='\t')
			isOnlySpaces=FALSE;
		if(line[i]==',' || line[i]=='(' || line[i]==')' || line[i]=='#'){
			temp[j]=' ';
			j++;
			temp[j]=line[i];
			j++;
			temp[j]=' ';												
		}
		else
			temp[j]=line[i];
	}
	temp[j]=0;

	if(isOnlySpaces==TRUE) /*if the line is empty*/
		return errors;

	token = strtok(temp, " \t");
	if (!token || token[0]==';') /*if the line is comment*/
		return errors;

	token_len=strlen(token);

	if (token[token_len-1]==':'){ /*if the element should be label*/
		for(i=0;i<token_len-1;i++)/*make a copy of the label without the ':' at the end*/
			label[i]=token[i];
		label[i]='\0';
		if(is_valid_label(label,line_num,symbols_head,TRUE,reading)) /*checking if it's a valid label*/
			isLabel=TRUE;
		else{
			errors++;
			return errors;
		}
	}

	if(isLabel){
		token = strtok(NULL, " \t");
		if(!token && reading==FIRST_READING){
			fprintf(stderr,"Notice: in line %d there is missing command\\data type after label. The assembler will ignore that label.\n",line_num);
			return errors;
		}
	}

	data_type=find_Data_type(token);

	if(data_type==NONE_DT){ /*if no data type was found - check command type*/
		cmd=find_cmd(token);
		if(cmd==NONE_CMD){ /*if no command type was found*/
			errors++;
			fprintf(stderr,"Error in line: %d. Unknown command\\data type.\n",line_num);
			return errors;
		}
	}
	token = strtok(NULL, " \t");
	for(i=0 ; token!=NULL&&i<=MAX_ELEMENTS ; i++) {/* walk through other tokens and save each token in the elements array */
		elements[i] = token;
		token = strtok(NULL, " \t");
	}

	if(data_type!=NONE_DT) /*if data type was found- send the elements to the speciefic method of data parsing*/
		errors+=parse_guidance(elements, data_type, data, symbols_head, label, isLabel, line_num, DC, reading);
	else /*else = if cmd type was found- send the elements to the speciefic method of code parsing*/
		errors+=parse_code(elements, cmd, code, symbols_head ,externs_head, label, isLabel, line_num, IC, reading);
	
	return errors;	
}



				











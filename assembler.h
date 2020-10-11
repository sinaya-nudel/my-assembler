/*Author: Sinaya Nudel 203191663*/

#ifndef ASSEMBLER_H
	#define ASSEMBLER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ENDING ".as"

#define MAX_LABLE_NAME 31 /*max label name length - 30 chars +1 for 0 at the end*/
#define MAX_WORDS 256 /*max words in the memmory array*/
#define LINE_LEN 82 /*max line length - 80 chars +1 for \n and +1 for 0 at the end*/
#define MAX_ELEMENTS 15 /*max elements (tokens seperated by white spaces) in one line*/

#define WORD_BITS 14 /*number of bits in one word*/
#define IMMEDIATE_BITS 12 /*number of bits for immediate operand value*/

#define ADDRESSING_METHODS_TYPES 4/*number of addressing methods types*/
#define DATA_TYPES_LEN 4/*number of data types*/
#define REGISTERS_LEN 8 /*number of registers types*/
#define CMD_LEN 16 /*number of commands types*/

#define FIRST_ADDRESS 100 /*index of the first address in the memmory*/


/*Bits range setting for each value in one word: */
#define ERA_START 0
#define ERA_END 1

#define DEST_START 2
#define DEST_END 3

#define SOURCE_START 4
#define SOURCE_END 5

#define OPCODE_START 6
#define OPCODE_END 9

#define SECOND_PARAM_START 10
#define SECOND_PARAM_END 11

#define FIRST_PARAM_START 12
#define FIRST_PARAM_END 13

#define IMMEDIATE_START 2
#define IMMEDIATE_END 13

#define ADDRESS_START 2
#define ADDRESS_END 13
	
#define DEST_REGISTER_START 2
#define DEST_REGISTER_END 7

#define SOURCE_REGISTER_START 8
#define SOURCE_REGISTER_END 13



typedef enum {FIRST_READING, SECOND_READING} Reading; /*reading cycle*/

typedef enum {FALSE,TRUE} Boolean;

typedef enum {MOV,CMP,ADD,SUB,NOT,CLR,LEA,INC,DEC,JMP,BNE,RED,PRN,JSR,RST,STOP,NONE_CMD} Cmd; /*commands types*/

typedef enum {DATA,STRING,ENTRY,EXTERN,NONE_DT} Data_type; /*data types*/

typedef enum {IMMEDIATE,DIRECT,JUMP,REGISTER,NONE_ADRS} Addressing_method; /*addressing methods types*/

typedef enum {A,E,R,NONE_CT} Coding_type; /*coding type - absolute/external/relocatable/none */

typedef enum {R0,R1,R2,R3,R4,R5,R6,R7,NONE_RGSTR} Register; /*registers types*/



/*struct for each command*/
typedef struct{
	char *name; /*the name of the command*/
	int opcode; /*the opcode of the command*/
	int params; /*numbers of parameters for the command*/
	Addressing_method source[ADDRESSING_METHODS_TYPES]; /*the addressing methods that this command supports for source operand*/
	Addressing_method dest[ADDRESSING_METHODS_TYPES]; /*the addressing methods that this command supports for dest operand*/
/*Notice that each empty cell of the addressing method's array must intiallizied to NONE_ADRS. Otherwise, it will be automatically intiallized to 0 - and according to the addressing method enum type, 0 = IMMEDIATE. That will cause a logical error. */
} command;

/*struct for externs - each extern include label string and address number(int)*/
typedef struct extern_struct* extern_ptr;

typedef struct extern_struct{
	char label[MAX_LABLE_NAME];
	int address;
	extern_ptr next;
}ext;

/*struct for symbols - each symbol include label string, address number(int),int value if extern, value if next to operation, and if entry*/

typedef struct table_struct* lable_ptr;

typedef struct table_struct{
	char label[MAX_LABLE_NAME];
	int address;
	Boolean isExternal;
	Boolean isOperation;
	Boolean isEntry;
	lable_ptr next;
}lable;

/*struct for each command's parameters*/
typedef struct params_struct{
	Addressing_method source;
	Addressing_method dest;
	Addressing_method first_jumping_param;
	Addressing_method second_jumping_param;
	char source_string[MAX_LABLE_NAME];
	char dest_string[MAX_LABLE_NAME];
	char first_jumping_param_string[MAX_LABLE_NAME];
	char second_jumping_param_string[MAX_LABLE_NAME];
}Params;


/*Methods declerations:

main.c*/
int open_file(char*);
void reset(lable_ptr*, extern_ptr*, short*, short*, int, int);
void create_obj(short*, short*, int, int,char*);
void create_ext(extern_ptr*, char*);
void create_ent(lable_ptr*, char*);
void special_print(FILE*, short);

/*data_parsing.c*/
int parse_guidance(char* [], Data_type ,short*, lable_ptr*, char[], Boolean, int, int*, Reading);
int parse_data(char* [],short*, lable_ptr*, char[], Boolean, int, int*);
int add_data(int[], int,short*, lable_ptr*,char[], Boolean,int*,int);
int parse_string(char* [],short*, lable_ptr*, char[], Boolean, int, int*);
Boolean is_valid_string(char*);
int add_string(char*, short*, lable_ptr*,char[], Boolean,int*,int);
int parse_entry(char* [], lable_ptr*, char[], Boolean, int);
int parse_extern(char* [], lable_ptr*, char[], Boolean, int);

/*code_parsing.c*/
int parse_code(char* [], Cmd, short*, lable_ptr* , extern_ptr*, char[], Boolean, int, int*,Reading);
int calculate_words(Params);
int is_valid_operands(Cmd cmd, Params, int line_num);
int get_operand(Addressing_method*, char*, char* [],lable_ptr*,int, int*,Reading);
int get_source_and_dest_addressing(Params*, char* [], lable_ptr*, int,Reading);
int get_dest_addressing(Params*, char* [], lable_ptr*,int,Reading);


/*reading.c*/
int file_reading(FILE*,short*,short*,lable_ptr*,extern_ptr*,int*,int*,Reading);
int parse_line(int,char*,short*,short*,lable_ptr*,extern_ptr*,int*,int*,Reading);

/*structs_methods.c*/
void add_ext(char*, int, extern_ptr*);
Boolean is_ext_empty(extern_ptr*);
void print_ext(FILE*,extern_ptr*);
void add_label(char*, int, Boolean, Boolean, Boolean, lable_ptr*);
void updated_data_val(int,lable_ptr*);
lable_ptr search_label(char*, lable_ptr*);
int get_address(char*, lable_ptr*);
Coding_type get_symbol_type(char*, lable_ptr*);
int find_and_change_entry(char*, lable_ptr*);
Boolean is_ent_empty(lable_ptr*);
void print_ent(FILE*,lable_ptr*);
void free_lists(lable_ptr*, extern_ptr*);

/*coding.c*/
void set_code(short*,int, int, int, int);
void set_registers(short*, int, char*, char*);
int set_label(short*, int, char*, lable_ptr*, extern_ptr*, int);
void set_immediate(short*,int, char*);
void set_operation(short*,int, Params, int);
int add_code(int*, short*, lable_ptr*, extern_ptr*, Cmd, Params,int);

/*other_parsing.c*/
Boolean is_valid_num(char [], int);
Register get_register(char*);
Boolean is_valid_label(char *, int, lable_ptr*, Boolean, Reading);
Cmd find_cmd(char*);
int get_opcode(Cmd);
int get_params(Cmd);
Boolean is_label_exist(char*,lable_ptr*,Boolean);
Data_type find_Data_type(char *);
Boolean is_supported_addressing_method(Cmd, Addressing_method, Addressing_method);

#endif


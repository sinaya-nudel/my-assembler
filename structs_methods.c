/*Author: Sinaya Nudel 203191663*/
#include "assembler.h"

/*adds new extern to the list. gets the label string, address number, and list head as parameters*/
void add_ext(char* lbl, int adrs, extern_ptr* head){
	extern_ptr temp = *head;
	extern_ptr new=(extern_ptr)malloc(sizeof(ext));
	if(new==NULL){
		fprintf(stderr,"MEMMORY ERROR. EXIT PROGRAM.\n");
		exit(1);
	}
	strcpy(new->label,lbl);
	new->address=adrs;
	new->next=NULL;

	if((*head)==NULL){
		(*head)=new;
		return;	
	}
	while((temp->next)!=NULL){
		temp=(temp->next);
	}
	temp->next=new;
}
/*gets the head of the externs list as parameter and returns true if the list empty, else returns false*/
Boolean is_ext_empty(extern_ptr* head){
	if((*head)==NULL)
		return TRUE;
	else
		return FALSE;
}
/*gets a file to write to, and externs list head as parameters. 
prints the externs to the file*/
void print_ext(FILE* file,extern_ptr* head){
	extern_ptr temp = *head;
	while(temp!=NULL){
		fprintf(file,"%s\t%d\n",temp->label,temp->address);
		temp=(temp->next);
	}
}

/*adds new symbol to the list. gets the label string, address number, boolean if is extern, boolean if is operation, boolean if is entry ,and list head as parameters*/
void add_label(char* lbl, int adrs, Boolean isEx, Boolean isOp, Boolean isEn , lable_ptr* head){
	lable_ptr temp = *head;
	lable_ptr new=(lable_ptr)malloc(sizeof(lable));
	if(new==NULL){
		fprintf(stderr,"MEMMORY ERROR. EXIT PROGRAM.\n");
		exit(1);
	}
	strcpy(new->label,lbl);
	new->address=adrs;
	new->isExternal=isEx;
	new->isOperation=isOp;
	new->isEntry=isEn;
	new->next=NULL;

	if((*head)==NULL){
		(*head)=new;
		return;	
	}
	while((temp->next)!=NULL){
		temp=(temp->next);
	}
	temp->next=new;	
}

		
/*gets ic counter, and symbols tabel head, and updates the symbols adresses value*/
void updated_data_val(int IC,lable_ptr* head){
	lable_ptr temp = *head;

	while(temp!=NULL){
		if((temp->isExternal)==FALSE){
			if((temp->isOperation)==FALSE)
				(temp->address) += IC + FIRST_ADDRESS;
			else
				(temp->address) += FIRST_ADDRESS;
		}
		temp=temp->next;
	}
}
/*gets a label name, and symbols table head, and check if the symbol exist in the tabel.
returns the symbol if found, else returns null*/
lable_ptr search_label(char* lbl, lable_ptr* head){
	lable_ptr temp = *head;
	while(temp!=NULL){
		if(!strcmp(lbl,temp->label))
			return temp;
		temp=temp->next;
	}
	return NULL;
}
/*gets a label name, and symbols table head. 
searchs for the symbol, and if found returns its address value. else, returns -1.*/
int get_address(char* lbl, lable_ptr* head){
	lable_ptr temp= search_label(lbl,head);
	if(temp!=NULL)
		return temp->address;
	else	
		return -1;
}


/*gets label string and head pointer to table as parameters.
returns the coding type E(==1) if found and extern, R(==2) if found and not extern, and NONE_CT if didn't found.*/
Coding_type get_symbol_type(char* lbl, lable_ptr* head){
	lable_ptr temp= search_label(lbl,head);
	if(temp!=NULL){
		if(temp->isExternal==TRUE)
			return E;
		else
			return R;
	}
	else	
		return NONE_CT;
}

/*gets a label name, and symbols table head. 
searchs for this symbol, and if found -change its isEntry boolean value to true, and returns 1. else, returns 0.*/	
int find_and_change_entry(char* lbl, lable_ptr* head){
	lable_ptr temp= search_label(lbl,head);
	if(temp!=NULL){
		temp->isEntry=TRUE;
		return 1;
	}
	else	
		return 0;
}
/*gets the head of the symbols table as parameter and returns true if no entries was found, else returns false*/
Boolean is_ent_empty(lable_ptr* head){
	Boolean isEmpty=TRUE;
	lable_ptr temp=*head;

	while(temp!=NULL){
		if((temp->isEntry)==TRUE){
			isEmpty=FALSE;
			return isEmpty;
		}
		temp=temp->next;
	}
	return isEmpty;
}
/*gets a file to write to, and symbols table head as parameters. 
search for entries in the table, and prints them to the file*/
void print_ent(FILE* file,lable_ptr* head){
	lable_ptr temp = *head;
	while(temp!=NULL){
		if((temp->isEntry)==TRUE)
			fprintf(file,"%s\t%d\n",temp->label,temp->address);	
		temp=(temp->next);
	}
}
/*gets symbols table head and enterns list head, and free all the nodes*/
void free_lists(lable_ptr* symbols_head, extern_ptr* externs_head){
	lable_ptr temp_symbol;
	extern_ptr temp_extern;

	while((*symbols_head)!=NULL){
		temp_symbol = *symbols_head;
		(*symbols_head)=(*symbols_head)->next;
		free(temp_symbol);
	}

	while((*externs_head)!=NULL){
		temp_extern = *externs_head;
		(*externs_head)=(*externs_head)->next;
		free(temp_extern);
	}
}
















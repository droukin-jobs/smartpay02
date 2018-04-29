#include <stdio.h>
#include <stdlib.h>
#include "terminal.h"
#include "json.h"
#include "packer.h"

//terminal array
#define MAX_DATA 4096
static terminal_type terminals[MAX_TERMINALS];
static int last_terminal = -1;

// adds terminal, if over limit returns -1
int add_terminal(){
	if((MAX_TERMINALS - 1) <= last_terminal) return -1;
	last_terminal += 1;
	int i = last_terminal;
	terminals[i].id = i;
	terminals[i].last_transaction = -1;
	return i;
}

// lists terminals as json objects
// in the data variable
// aborts (gracefully) execution if reached limit
int list_terminals(char *data, const int max_data){
	int i;
	char tmp[24]; //placeholder for terminal id
	if(last_terminal == -1){
		return last_terminal;
	}
	int data_len = sprintf(data,"\"terminals\":[\n");
	for(i=0;i<=last_terminal;i++){
		int len = sprintf(tmp,"{\"TerminalID\":\"%04d\"},\n",terminals[i].id);
		memcpy(data + data_len , tmp, strlen(tmp));
		data_len += len; //update data pointer
		if(data_len > max_data - len -2 ) break;
	}
	data[data_len - 2] = ' '; //remove last comma
	data[data_len] = ']';
	data[data_len+1] = '\0';
	return last_terminal;
}
// adds transaction to a terminal
// return last transaction if successful, -1 if not
int add_transaction(int id, int card, int acct){
	if(id > last_terminal) return -1;
	if((MAX_TRANSACTIONS - 1 ) <= terminals[id].last_transaction) return -1;
	if(card > 3 || acct > 3 || card < 0 || acct < 0) return -1;
	terminals[id].last_transaction += 1;
	int j = terminals[id].last_transaction;	
	terminals[id].transactions[j >> 1] |= PACK_CARD(j,card);
	terminals[id].transactions[j >> 1] |= PACK_ACCT(j,acct);
	return j;
}

// copy transaction data in JSON format to data param

int list_transactions(int id, char *data, const int max_data){
	char tmp[100]; //temporary var to store transaction data
	int lt = terminals[id].last_transaction;
	if(lt == -1){
		return lt;
	}
	int data_len = sprintf(data,"[\n");
	int i;
	for(i=0;i<=lt;i++){
		//extract transaction data 
		int c_id = UNPACK_CARD(i,terminals[id].transactions[i>>1]);
		int a_id = UNPACK_ACCT(i,terminals[id].transactions[i>>1]);
		int len = sprintf(tmp,"{\"CardType\":\"%s\",\"TransactionType\":\"%s\"},\n",cards[c_id],accts[a_id]);
		memcpy(data + data_len , tmp, len);
		data_len += len;//update data pointer
		//check if possible to add more transactions to data param
		if(data_len > max_data - len -2 ) break;
	}
	data[data_len - 2] = ' '; //last transaction does not need comma
	data[data_len] = ']';
	data[data_len+1] = '\0';
	return lt;
}


// shows terminal json object with transactions
void show_terminal_info(char* tmp, int id){
	
	if(id > last_terminal){
		json_error(tmp,"Invalid terminal");
		return;
	}
	char *data = (char*)malloc(MAX_DATA);
	if(list_transactions(id,data,MAX_DATA) == -1){
		sprintf(data,"[]");	
	}
	sprintf(tmp,"\"TerminalID\":\"%d\",\"Transactions\":%s",id, data);
}


// init cards and accts arrays
void init(void){
	sprintf(cards[0],"MasterCard");
	sprintf(cards[1],"Visa");
	sprintf(cards[2],"EFTPOS");
	sprintf(accts[0],"Credit");
	sprintf(accts[1],"Savings");
	sprintf(accts[2],"Cheque");
}

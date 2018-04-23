#include <stdio.h>
#include <stdlib.h>
#include "terminal.h"
#include "json.h"

//terminal array
#define MAX_DATA 4096
static terminal_type terminals[MAX_TERMINALS];
static int last_terminal = -1;

// adds terminal, if over limit returns -1
int add_terminal(){
	if((MAX_TERMINALS - 1) <= last_terminal) return -1;
	last_terminal += 1;
	int i = last_terminal;
	printf("creating terminal index %d",i);
	terminals[i].id = i;
	terminals[i].last_transaction = -1;
	printf(" done\n");
	return i;
}

// adds transaction to a terminal
// not fully implemented 
int add_transaction(int terminal, int card, int acct){
	int id = terminal;
	int i = id;
	printf("add transaction (%d,%d) for term %d of %d\n",card,acct,i,last_terminal);	
	if(terminal > last_terminal) return -1;
	printf("MAX = %d, transaction = %d\n", MAX_TRANSACTIONS, terminals[i].last_transaction);	
	if((MAX_TRANSACTIONS - 1 ) <= terminals[i].last_transaction) return -1;
	if(card > 3 || acct > 3 || card < 0 || acct < 0) return -1;
	terminals[i].last_transaction += 1;
	int j = terminals[i].last_transaction;	
	terminals[i].transactions[j].id = j;
	terminals[i].transactions[j].card = card;
	terminals[i].transactions[j].acct = acct;
	printf("added transaction %d\n",j);
	return j;
}

// lists terminals as json objects
// in the data variable
// aborts (gracefully) execution if reached limit
int list_terminals(char *data, const int max_data){
	int i;
	char tmp[24];
	if(last_terminal == -1){
		return last_terminal;
	}
	int data_len = sprintf(data,"\"terminals\":[\n");
	for(i=0;i<=last_terminal;i++){
		int len = sprintf(tmp,"{\"TerminalID\":\"%04d\"},\n",terminals[i].id);
		if(i == last_terminal) tmp[len - 2] = ' ';
		memcpy(data + data_len , tmp, strlen(tmp));
		data_len += len;
		if(data_len > max_data - len -2 ) break;
	}
	data[data_len] = ']';
	data[data_len+1] = '\0';
	return last_terminal;
}

int list_transactions(int id, char *data, const int max_data){
	int i;
	char tmp[24];
	int lt = terminals[i].last_transaction;
	if(lt == -1){
		return lt;
	}
	printf("list transaction\n");
	int data_len = sprintf(data,"[\n");
	for(i=0;i<=lt;i++){
		printf("listing transaction %d\n",i);
		int c_id = terminals[id].transactions[i].card;
		int a_id = terminals[id].transactions[i].acct;
		int len = sprintf(tmp,"{\"CardType\":\"%s\"},{\"TransactionType\":\"%s\"},\n",cards[c_id],accts[a_id]);
		printf("about to update data\n");
		if(i == lt) tmp[len - 2] = ' ';
		memcpy(data + data_len , tmp, strlen(tmp));
		printf("updated data\n");
		data_len += len;
		if(data_len > max_data - len -2 ) break;
	}
	data[data_len] = ']';
	data[data_len+1] = '\0';
	return lt;
}
// shows terminal json object with dummy (at the moment) transactions
void show_terminal_info(char* tmp, int id){
	if(id > last_terminal){
		json_error(tmp,"Invalid terminal");
		return;
	}
	char data[MAX_DATA];

	list_transactions(id,data,MAX_DATA);
	printf("end list transactions\n");
	sprintf(tmp,"{\"TerminalID\":\"%d\",%s",data);
}

void init(void){
	sprintf(cards[0],"MasterCard");
	sprintf(cards[1],"Visa");
	sprintf(cards[2],"EFTPOS");
	sprintf(accts[0],"Credit");
	sprintf(accts[1],"Savings");
	sprintf(accts[2],"Cheque");
}

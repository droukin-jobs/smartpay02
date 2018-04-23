#ifndef _terminal_h
#define _terminal_h

#define MAX_TRANSACTIONS 100
#define MAX_TERMINALS 100
//strlen of json {\n\"terminalID\":\"0000\"\n} 
#define MAX_TERMINAL_SIZE 25

static const char *terminal_create = "{\"cardType\":[\"Visa\",\"MasterCard\",\"EFTPOS\"],\"TransactionType\":[\"Cheque\",\"Savings\",\"Credit\"]}";   
enum en_cardsy$ {Visa,MasterCard,EFTPOS};
enum en_accts {Credit,Cheque,Savings};

char cards[3][11];
char accts[3][11];


typedef struct {
int id;
int card;
int acct;
} transaction_type;

typedef struct {
int id;
transaction_type transactions[MAX_TRANSACTIONS];
int last_transaction;
} terminal_type;


int add_terminal();
int add_transaction(int terminal, int card, int acct);
int list_terminals(char *data, const int max_data);
int list_transactions(int id, char *data, const int max_data);
void show_terminal_info(char* tmp, int id);
#endif

#ifndef LS_H_INCLUDED
#define LS_H_INCLUDED

/* Defines */



/* Variables */



/* Functions */

void ls_list(struct data_node *);
int ls_parse_dir(char *, int, char *);
void ls_print_entries(struct data_node *, int);
void ls_recursive_list(struct data_node *, char *, char *);
void ls_free_entries(void);



#endif

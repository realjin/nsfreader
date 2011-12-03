#include <alisttpl.h>

#include <index.h>


typedef struct _idmap_entry	{
	uint id;
	char docid[20];
}id_map_entry;
alisttpl_struct(id_map_entry);
typedef id_map_entry_alist id_map;

typedef struct _doc_terms	{
	uint docid;
	uint termid;
	uint tf;
}doc_terms;
alisttpl_struct(doc_terms);

id_map* nsf_create_id_map(char* fns[], int n);
void nsf_load_bow_idnsfid(id_map* m, char* fn);

void nsf_load_doclist(char* fn);

di* nsf_create_di(char* fns[], int n);
void nsf_load_doc_terms(doc_terms_alist* dlist, char* fn);
doc_terms_alist* nsf_create_doc_terms_list(char* fns[], int n);

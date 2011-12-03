#include "reader_nsf.h"

#include <alisttpl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


alisttpl_struct_impl(id_map_entry);
alisttpl_struct_impl(doc_terms);

id_map* nsf_create_id_map(char* fns[], int n)
{
	id_map* m = create_id_map_entry_alist();
	int i;
	
	for(i=0;i<n;i++)	{
		nsf_load_bow_idnsfid(m, fns[i]);
	}
	return m;
}


void nsf_load_bow_idnsfid(id_map* m, char* fn)
{
	id_map_entry* e;
	char line[100];
	FILE* f;

	f = fopen(fn, "r");

	while((fgets(line, sizeof(line), f))!=0)	{
		e = (id_map_entry*)malloc(sizeof(id_map_entry));
		sscanf(line, "%d %s", &e->id, &e->docid);

		add_id_map_entry(m, e);
	}

	fclose(f);

	//printf("map size now=%d\n", m->size);
}
//------------------------------------------doc terms
di* nsf_create_di(char* fns[], int n)
{
	di* ind;
	
	int i;
	FILE* f;
	__u32 did;
	char line[100];
	doc_terms* dtpair;
	di_doc* d;
	di_dterm* dt;

	ind = create_di();

	for(i=0;i<n;i++)	{
		f = fopen(fns[i], "r");
		/*
		if(fgets(line, sizeof(line), f)==0)	{
			printf("nsf_create_di: first line of file %s reading error!\n", fns[i]);
			return -1;
		}
		*/

		/*
		d = (doc_terms*)malloc(sizeof(doc_terms));
		sscanf(line, "%d %d %d", &d->docid, &d->termid, &d->tf);
		did = d->docid;
		*/

		did = -1;
		d = 0;
			
		while( (fgets(line,sizeof(line), f)) !=0 ){
			dtpair = (doc_terms*)malloc(sizeof(doc_terms));
			sscanf(line, "%d %d %d", &dtpair->docid, &dtpair->termid, &dtpair->tf);
#if 1
			if(dtpair->docid%1000 < 5)	{
				printf("[progress] did=%d\n", dtpair->docid);
			}
#endif
			if(dtpair->docid!=did)	{	//if term in same doc
				if(d)	{
					di_add_doc(ind, d);
					//printf("ind size=%d\n", ind->size);
				}
				//d = (di_doc*)malloc(sizeof(di_doc));
				d = di_create_doc();
				d->did = dtpair->docid;
				did = dtpair->docid;
			}

			dt = (di_dterm*)malloc(sizeof(di_dterm));
			dt->tid = dtpair->termid;
			dt->tf = dtpair->tf;

			di_add_dterm(d, dt);

			free(dtpair);
		}
		if(d)	{
			di_add_doc(ind, d);
			//printf("ind size=%d\n", ind->size);
		}

		fclose(f);
	}

	return ind;
}

void nsf_load_doc_terms(doc_terms_alist* dlist, char* fn)
{
	doc_terms* d;
	char line[100];
	FILE *f;
	f = fopen(fn, "r");

	while( (fgets(line,sizeof(line), f)) !=0 ){
		d = (doc_terms*)malloc(sizeof(doc_terms));
		//	printf("line=%s\n", line);
		sscanf(line, "%d %d %d", &d->docid, &d->termid, &d->tf);
		add_doc_terms(dlist, d);
	}

	fclose(f);


}

doc_terms_alist* nsf_create_doc_terms_list(char* fns[], int n)
{
	doc_terms_alist* dlist = create_doc_terms_alist();
	int i;

	for(i=0;i<n;i++)	{
		nsf_load_doc_terms(dlist, fns[i]);
	}

	return dlist;

}

#include "store.h"

//srlz: serialize

static iis_term_header* srlz_iiterm(ii_term* t)
{
	iis_term_header* h = (iis_term_header*)malloc(sizeof(iis_term_header));
	h->flag = _INDEXINGBASICS_STORE_FLAG_II_THEADER; 
	h->tid[0] = (t->tid >> 16) & 0xff;
	h->tid[1] = (t->tid >> 8) & 0xff;
	h->tid[2] = (t->tid) & 0xff;
	h->df = t->postings->size;
	return h;
}

static ii_term* desrlz_iisterm_header(iis_term_header* h)
{
	if(h->flag != _INDEXINGBASICS_STORE_FLAG_II_THEADER)	{
		printf("desrlz_iiterm_header error! (flag wrong)\n");
		return 0;
	}
	
	ii_term* t = (ii_term*)malloc(sizeof(ii_term));
	t->tid = (h->tid[0]<<16) + (h->tid[1]<<8) + (h->tid[2]);
	t->postings = create_posting_alist();
	return t;
}

static iis_term_body_entry* srlz_iiterm_posting(posting* pst)
{
	iis_term_body_entry* be = (iis_term_body_entry*)malloc(sizeof(iis_term_body_entry));
	be->flag = _INDEXINGBASICS_STORE_FLAG_II_TBODYENTRY; 
	be->did[0] = (pst->did >> 16) & 0xff;
	be->did[1] = (pst->did >> 8) & 0xff;
	be->did[2] = (pst->did ) & 0xff;
	be->tf = pst->tf;
	return be;
}

static posting* desrlz_iisterm_body_entry(iis_term_body_entry* be)
{
	if(be->flag != _INDEXINGBASICS_STORE_FLAG_II_TBODYENTRY)	{
		printf("desrlz_iiterm_body_entry error! (flag wrong)\n");
		return 0;
	}
	posting* p = (posting*)malloc(sizeof(posting));
	p->did = (((__u32)be->did[0])<<16) +(((__u32)be->did[1])<<8) + ((__u32)be->did[2]);
	//printf("did=%d, be->did=%d:%d:%d\n", p->did,be->did[0],be->did[1],be->did[2]);
	p->tf = be->tf;
	return p;
		
}

int save_ii(ii* ind, char* fn)
{
	FILE* f;
	__u8 end = 0xff;

	int i,j;
	ii_term* t;
	iis_term_header* h;
	iis_term_body_entry* be;
	
	//open------------
	f = fopen(fn, "wb");

	for(i=0;i<ind->size;i++)	{
		t = ind->list[i];
		h = srlz_iiterm(t);
		//write;
		fwrite((void*)(&h->flag), sizeof(__u8), 1, f);
		fwrite((void*)h->tid, sizeof(__u8), 3, f);	//mmm:removed temporarily
		fwrite((void*)(&h->df), sizeof(__u32), 1, f);
		free(h);
		for(j=0;j<t->postings->size;j++)	{
			be = srlz_iiterm_posting(t->postings->list[j]);
			//write;
			fwrite((void*)(&be->flag), sizeof(__u8), 1, f);
			fwrite((void*)be->did, sizeof(__u8), 3, f);
			fwrite((void*)(&be->tf), sizeof(__u32), 1, f);
			free(be);
		}
	}
	fwrite((void*)&end, sizeof(__u8), 1, f);

	fclose(f);

	//close
}

ii* load_ii(char* fn)
{
	FILE* f;

	//open---
	f = fopen(fn, "rb");

	iis_term_header* h;
	iis_term_body_entry* be;
	ii* ind;
	ii_term* t;
	posting* pst;
	int i;

	ind = create_ii_term_alist();

	while(1)	{
		//h = load(8byte);
		h = (iis_term_header*)malloc(sizeof(iis_term_header));
		fread((void*)(&h->flag), sizeof(__u8), 1, f);
		if(h->flag == 0xff)	{	//end of file
			break;
		}
		fread((void*)h->tid, sizeof(__u8), 3, f);
		fread((void*)(&h->df), sizeof(__u32), 1, f);
		//printf("header tid=%d:%d:%d, df=%d\n", h->tid[0],h->tid[1],h->tid[2], h->df);
		t = desrlz_iisterm_header(h);
		for(i=0;i<h->df;i++)	{
			//be = load(8byte);
			be = (iis_term_body_entry*)malloc(sizeof(iis_term_body_entry));
			fread((void*)(&be->flag), sizeof(__u8), 1, f);
			fread((void*)be->did, sizeof(__u8), 3, f);
			fread((void*)(&be->tf), sizeof(__u32), 1, f);
		//printf("body entry did=%d:%d:%d, tf=%d\n", be->did[0],be->did[1],be->did[2], be->tf);
			pst = desrlz_iisterm_body_entry(be);
			add_posting(t->postings, pst);
		}
		add_ii_term(ind, t);
	}

	fclose(f);

	return ind;
}


//------------------------------------------------di------------

static dis_doc_header* srlz_didoc(di_doc* d)
{
	dis_doc_header* h = (dis_doc_header*)malloc(sizeof(dis_doc_header));
	h->flag = _INDEXINGBASICS_STORE_FLAG_DI_DHEADER; 
	h->did[0] = (d->did >> 16) & 0xff;
	h->did[1] = (d->did >> 8) & 0xff;
	h->did[2] = (d->did) & 0xff;
	h->nt = d->terms->size;
	return h;
}

static di_doc* desrlz_disdoc_header(dis_doc_header* h)
{
	if(h->flag != _INDEXINGBASICS_STORE_FLAG_DI_DHEADER)	{
		printf("desrlz_disdoc_header error! (flag wrong)\n");
		return 0;
	}
	
	di_doc* d = (di_doc*)malloc(sizeof(di_doc));
	d->did = (h->did[0]<<16) + (h->did[1]<<8) + (h->did[2]);
//	printf("d->did=%d\n", d->did);
	d->terms = create_di_dterm_alist();
	return d;
}

static dis_doc_body_entry* srlz_didoc_dterm(di_dterm* dt)
{
	dis_doc_body_entry* be = (dis_doc_body_entry*)malloc(sizeof(dis_doc_body_entry));
	be->flag = _INDEXINGBASICS_STORE_FLAG_DI_DBODYENTRY; 
	be->tid[0] = (dt->tid >> 16) & 0xff;
	be->tid[1] = (dt->tid >> 8) & 0xff;
	be->tid[2] = (dt->tid ) & 0xff;
	be->tf = dt->tf;
	return be;
}

static di_dterm* desrlz_disdoc_body_entry(dis_doc_body_entry* be)
{
	if(be->flag != _INDEXINGBASICS_STORE_FLAG_DI_DBODYENTRY)	{
		printf("desrlz_didoc_body_entry error! (flag wrong)\n");
		return 0;
	}
	di_dterm* t = (di_dterm*)malloc(sizeof(di_dterm));
	t->tid = (((__u32)be->tid[0])<<16) +(((__u32)be->tid[1])<<8) + ((__u32)be->tid[2]);
	//printf("did=%d, be->did=%d:%d:%d\n", p->did,be->did[0],be->did[1],be->did[2]);
	t->tf = be->tf;
	return t;
		
}

int save_di(di* ind, char* fn)
{
	FILE* f;
	__u8 end = 0xff;

	int i,j;
	di_doc* d;
	dis_doc_header* h;
	dis_doc_body_entry* be;
	
	//open------------
	f = fopen(fn, "wb");

	for(i=0;i<ind->size;i++)	{
		d = ind->list[i];
		h = srlz_didoc(d);
		printf("in write\n");
		//write;
		fwrite((void*)(&h->flag), sizeof(__u8), 1, f);
		fwrite((void*)h->did, sizeof(__u8), 3, f);	//mmm:removed temporarily
		fwrite((void*)(&h->nt), sizeof(__u32), 1, f);
		free(h);
		for(j=0;j<d->terms->size;j++)	{
			be = srlz_didoc_dterm(d->terms->list[j]);
			//write;
			fwrite((void*)(&be->flag), sizeof(__u8), 1, f);
			fwrite((void*)be->tid, sizeof(__u8), 3, f);
			fwrite((void*)(&be->tf), sizeof(__u32), 1, f);
			free(be);
		}
	}
	fwrite((void*)&end, sizeof(__u8), 1, f);

	fclose(f);

	//close
}

di* load_di(char* fn)
{
	FILE* f;

	//open---
	f = fopen(fn, "rb");

	dis_doc_header* h;
	dis_doc_body_entry* be;
	di* ind;
	di_doc* d;
	di_dterm* dt;
	int i;

	ind = create_di_doc_alist();

	while(1)	{
		//h = load(8byte);
		h = (dis_doc_header*)malloc(sizeof(dis_doc_header));
		fread((void*)(&h->flag), sizeof(__u8), 1, f);
		if(h->flag == 0xff)	{	//end of file
			break;
		}
		fread((void*)h->did, sizeof(__u8), 3, f);
		fread((void*)(&h->nt), sizeof(__u32), 1, f);

#if 1
		if(h->did[0]%1000==1 && h->did[1]<3 && h->did[2]<3)	{
			printf("header did=%d:%d:%d, nt=%d\n", h->did[0],h->did[1],h->did[2], h->nt);
		}
#endif
		d = desrlz_disdoc_header(h);
		for(i=0;i<h->nt;i++)	{
			//be = load(8byte);
			be = (dis_doc_body_entry*)malloc(sizeof(dis_doc_body_entry));
			fread((void*)(&be->flag), sizeof(__u8), 1, f);
			fread((void*)be->tid, sizeof(__u8), 3, f);
			fread((void*)(&be->tf), sizeof(__u32), 1, f);
		//printf("body entry did=%d:%d:%d, tf=%d\n", be->did[0],be->did[1],be->did[2], be->tf);
			dt = desrlz_disdoc_body_entry(be);
			add_posting(d->terms, dt);
		}
		add_di_doc(ind, d);
	}

	fclose(f);

	return ind;
}

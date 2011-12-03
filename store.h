#ifndef _INDEXINGBASICS_STORE_
#define _INDEXINGBASICS_STORE_

#include "common.h"

#include <index.h>

#define _INDEXINGBASICS_STORE_FLAG_II_THEADER 0x11
#define _INDEXINGBASICS_STORE_FLAG_II_TBODYENTRY 0x12

#define _INDEXINGBASICS_STORE_FLAG_DI_DHEADER 0x21
#define _INDEXINGBASICS_STORE_FLAG_DI_DBODYENTRY 0x22

//iis: inverted index storage
typedef struct _iis_term_header	{
	__u8 flag;	//for robustness(others are ok even if some 8byte block broken)
	__u8 tid[3];
	__u32 df;
} iis_term_header;

//i.e. posting
typedef struct _iis_term_body_entry	{
	__u8 flag;
	__u8 did[3];
	__u32 tf;
} iis_term_body_entry;

#if 0
typedef struct _iis_term	{
	iis_term_header header;
	iis_term_body_entry* body;	//i.e. posting list
} iis_term;

typedef struct _iis {
	int size;
	iis_term* terms;
} iis;
#endif

//dis: document index storage
typedef struct _dis_doc_header	{
	__u8 flag;	//for robustness(others are ok even if some 8byte block broken)
	__u8 did[3];
	__u32 nt;	//total term kinds 
} dis_doc_header;

//i.e. posting
typedef struct _dis_doc_body_entry	{
	__u8 flag;
	__u8 tid[3];
	__u32 tf;
} dis_doc_body_entry;


/*--------------------------*
 *  Inverted index storage  *
 *--------------------------*/
int save_ii(ii* ind, char* fn);
ii* load_ii(char* fn);

/*--------------------------*
 *  Document index storage  *
 *--------------------------*/
int save_di(di* ind, char* fn);
di* load_di(char* fn);
#endif

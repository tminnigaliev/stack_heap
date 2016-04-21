#ifndef STH_TYPES_H_
#define STH_TYPES_H_

/*
 * type's name derived from STack Heap HEADER Type
 */
typedef enum
{
	STHH_UNUSED  = 0x10000000,
    STHH_CTX     = 0x10101010,  //STack Heap Header ConTeXt
    STHH_REC     = 0x10101011,  //STack Heap Header RECord
    STHH_DBG_CTX = 0x10101012,  //STack Heap Header DeBuG ConTeXt
    STHH_DBG_REC = 0x10101013   //STack Heap Header DeBuG RECord
}sth_header_t;

#endif



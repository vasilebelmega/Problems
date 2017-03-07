#pragma once
/*-- tlg.h --- LISTA SIMPLU INLANTUITA GENERICA ---*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#ifndef _LISTA_GENERICA_
#define _LISTA_GENERICA_

typedef struct celulag
{
	size_t d;
	void* info;           /* adresa informatie */
	struct celulag *urm;   /* adresa urmatoarei celule */
} TCelulaG, *TLG, **ALG; /* tipurile Celula, Lista si Adresa_Lista */

typedef int(*TFElem)(void*);     /* functie prelucrare element */
typedef int(*TFCmp)(void*, void*); /* functie de comparare doua elemente */

typedef void(*TFAfi)(void*);     /* functie afisare un element */

								 /*- inserare la inceput reusita sau nu (1/0) -*/
int InsLG(ALG, void*, size_t);

/*- inserare la sfarsit reusita sau nu (1/0) -*/
int InsLGLast(ALG, void*, size_t);

void DistrugeLG(ALG); /* eliminare celula si element */

void Distruge(ALG aL); /* distruge lista */

size_t LungimeLG(ALG);   /* numarul de elemente din lista */

						 /* afiseaza elementele din lista, folosind o functie de tip TFAfi */
void Afisare(ALG, TFAfi);

void* copie(void* el);
#endif

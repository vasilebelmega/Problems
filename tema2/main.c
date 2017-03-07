#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "tlg.h"

#define MAX_BUFFER 255
#define MAX_INT 4294967295

typedef struct
{
	int id;
	char *nume, *mod, *ip;
} TSwitch;

//function to convert an ip in number
unsigned int ipConvert(char* a)
{
		unsigned int c1, c2, c3, c4;
		unsigned int ip;
		char* temp;
		temp = strdup(a);
		c1=atoi(strtok(temp,"."));
		c2=atoi(strtok(NULL,"."));
		c3=atoi(strtok(NULL,"."));
		c4=atoi(strtok(NULL,"\0"));
		ip = (unsigned int)c4+c3*256+c2*256*256+c1*256*256*256;
		
		free(temp);
		return ip;
}

//function for do a copy for an element 
void* copie(void* el)
{
	TSwitch* p = (TSwitch*)el, *rez;
	rez = (TSwitch*)malloc(sizeof(TSwitch));
	if (!rez) return NULL;
	rez->nume = strdup(p->nume);
	rez->mod = strdup(p->mod);
	rez->ip = strdup(p->ip);
	rez->id = p->id;
	
	return rez;
}

void addInStiva(ALG AStiva, int id, char* nume, char* ip, char* mod, char* functionare)
{
	TLG StivaTemp;
	TSwitch* entry, *entryTemp, *entryLast = NULL;	

	entry = (TSwitch*)malloc(sizeof(TSwitch));
	if (!entry)	return;
	entry->id = id;
	entry->nume = strdup(nume);
	entry->ip = strdup(ip);
	entry->mod = strdup(mod);

	StivaTemp = (TLG)malloc(sizeof(TCelulaG));	
	StivaTemp = NULL;	
	
	// extract and save from our stack until the place of insertion
	while (*AStiva != NULL)
	{
		entryTemp = (TSwitch*)((*AStiva)->info);
		if (!strcmp(functionare, "NON_BASE") && (entry->id < entryTemp->id || (*AStiva)->urm == NULL)) 
			break;
		else
		{	
			if (!strcmp(functionare, "BASE") && (*AStiva)->urm == NULL) // save the last principal switch
				entryLast = (TSwitch*)copie(entryTemp);	
			else
				InsLG(&StivaTemp, entryTemp, sizeof(TSwitch));

			DistrugeLG(AStiva);
		}
	}

	// insert our new entry here
	InsLG(AStiva, (TSwitch*)entry, sizeof(TSwitch));

	// read the removed entries
	while (StivaTemp != NULL)
	{
		entryTemp = (TSwitch*)(StivaTemp->info);
		if (entryLast && entryLast->id > entryTemp->id) // found the place to insert the last principal switch
		{
			InsLG(AStiva, entryLast, sizeof(TSwitch));
			free(entryLast);
			entryLast = NULL;
		}

		InsLG(AStiva, entryTemp, sizeof(TSwitch));
		DistrugeLG(&StivaTemp);
	}
	if (entryLast)
	{
		InsLG(AStiva, entryLast, sizeof(TSwitch));
		free(entryLast);
	}
}

void addInCoada(ALG ACoada, ALG ACoadaLast, int id, char *nume, char *ip, char *mod)
{
	TLG CoadaTemp, CoadaTempLast; // save into another queue
	TSwitch* entry, *entryTemp;

	entry = (TSwitch*)malloc(sizeof(TSwitch));
	if (!entry)	return;
	entry->id = id;
	entry->nume = strdup(nume);
	entry->ip = strdup(ip);
	entry->mod = strdup(mod);

	CoadaTempLast = (TLG)malloc(sizeof(TCelulaG));	
	CoadaTemp = (TLG)malloc(sizeof(TCelulaG));
	CoadaTemp = NULL;

	while (*ACoada != NULL)
	{
		entryTemp = (TSwitch*)((*ACoada)->info);
		if (entry && entryTemp && ipConvert(entry->ip) < ipConvert(entryTemp->ip)) 
		{
			InsLGLast(&CoadaTempLast, (TSwitch*)entry, sizeof(TSwitch));	
			if (CoadaTemp == NULL) // first entry in our queue 
				CoadaTemp = CoadaTempLast; 
			free(entry);
			entry = NULL;
		}
		
		InsLGLast(&CoadaTempLast, (TSwitch*)entryTemp, sizeof(TSwitch));
		if (CoadaTemp == NULL) // first entry in our queue 
			CoadaTemp = CoadaTempLast; 
		DistrugeLG(ACoada);
	}

	if (entry) // not yet inserted
	{
		InsLGLast(&CoadaTempLast, (TSwitch*)entry, sizeof(TSwitch));
	}
	
	*ACoada = CoadaTemp;
	*ACoadaLast = CoadaTempLast;
	if (*ACoada == NULL) // first entry in our queue 
		*ACoada = *ACoadaLast; 
}

//delete from stack and return elementsdeleted
TSwitch* stergeDinStiva(ALG AStiva, int id)
{
	TLG StivaTemp;
	TSwitch *entryTemp, *entryDeleted;
	StivaTemp = (TLG)malloc(sizeof(TCelulaG));	
	StivaTemp = NULL;

	while (*AStiva != NULL)
	{
		entryTemp = (TSwitch*)((*AStiva)->info);
		if (entryTemp->id != id) // do not insert this switch into the save stack
		{
			InsLG(&StivaTemp, (TSwitch*)entryTemp, sizeof(TSwitch));
		}		
		else
		{
			entryDeleted = (TSwitch*)copie(entryTemp);
		}
		DistrugeLG(AStiva);
	}

	// rebuild the new stack without the switch with id "id"
	while (StivaTemp != NULL)
	{
		entryTemp = (TSwitch*)(StivaTemp->info);
		InsLG(AStiva, (TSwitch*)entryTemp, sizeof(TSwitch));
		DistrugeLG(&StivaTemp);
	}
	
	return entryDeleted;
}

//delete from queue and return elementsdeleted
TSwitch* stergeDinCoada(ALG ACoada, ALG ACoadaLast, int id)
{
	TLG CoadaTemp, CoadaTempLast; // save into another queue
	TSwitch *entryTemp, *entryDeleted;

	CoadaTempLast = (TLG)malloc(sizeof(TCelulaG));	
	CoadaTemp = (TLG)malloc(sizeof(TCelulaG));
	CoadaTemp = NULL;

	while (*ACoada != NULL)
	{
		entryTemp = (TSwitch*)((*ACoada)->info);
		if (entryTemp->id != id) // do not insert this switch into the new queue
		{
			InsLGLast(&CoadaTempLast, (TSwitch*)entryTemp, sizeof(TSwitch));
			if (CoadaTemp == NULL) // first entry in our queue 
				CoadaTemp = CoadaTempLast; 
		}
		else
		{
			entryDeleted = (TSwitch*)copie(entryTemp);
		}
		DistrugeLG(ACoada);
	}
	
	*ACoada = CoadaTemp;
	*ACoadaLast = CoadaTempLast;
	
	return entryDeleted;
}

//this function return -1 if id is in queue and i(id_stiva) if is in stack
int indiceStiva(int id, ALG Stive, TLG Coada, int nrStive)
{
	int i; 
	TLG pStiva;
	TSwitch* entry;
	
	while (Coada != NULL)
	{
		entry = (TSwitch*)(Coada->info);
		if (entry && entry->id == id)
			return -1; // found it in Coada!
		Coada = Coada->urm;
	}

	// if not found in Coada, look for it in all the stacks
	for (i = 0; i < nrStive; i++)
	{
		pStiva = Stive[i];
		while (pStiva != NULL)
		{
			entry = (TSwitch*)(pStiva->info);
			if (entry && entry->id == id)
				return i; // found it in stack "i"
			pStiva = pStiva->urm;
		}
	}

	return -2; // switch with id "id" does not exist;
}


void show(FILE* output, ALG Stive, TLG Coada, int nrStive)
{
	TSwitch* entry;
	TLG pStiva;
	int index = 0;

	// print the queue id's
	fprintf(output, "{");
	while (Coada != NULL)
	{
		entry = (TSwitch*)(Coada->info);
		if (entry)
			fprintf(output, "%d ", entry->id);
		Coada = Coada->urm;
	}
	fprintf(output, "}\n");

	// print the stacks
	for (index = 0; index < nrStive; index++)
	{
		fprintf(output, "%d:\n", index);
		pStiva = Stive[index];
		while (pStiva != NULL)
		{
			entry = (TSwitch*)(pStiva->info);
			if (entry)
				fprintf(output, "%d %s %s\n", entry->id, entry->ip, entry->nume);
			pStiva = pStiva->urm;
		}
	}
	fprintf(output, "\n");
}
//function ipmin find min from queue and print in file 
void ipmin(FILE *output, TLG Coada)
{	
	TSwitch *entry;
	unsigned int ipInt, minIpInt = MAX_INT;
	int doarStive = 0;
	if(Coada == NULL)
	{
		fprintf(output,"ipmin=%d\n", doarStive);
		return;
	}
	
	while (Coada != NULL)
	{
		entry = (TSwitch*)(Coada->info);
		if (entry)
		{	
			// transformi entry->ip in unsigned int si calculezi minimul din coada
			ipInt=ipConvert(entry->ip);
			if (ipInt < minIpInt) {
				minIpInt = ipInt;
			}
		}
			Coada = Coada->urm;
	}
	fprintf(output,"ipmin=%u\n", minIpInt);
}

int main(int argc, char** argv)
{
	char *buffer, *nume, *mod, *ip, *functionare;
	int id = 0, index = 0, nrStive = 0, id_stiva;
	FILE *input, *output;
	ALG Stive;
	TLG Coada, CoadaLast;
	TSwitch* entry;
	
	Coada = (TLG)malloc(sizeof(TCelulaG));
	CoadaLast = (TLG)malloc(sizeof(TCelulaG));
	Coada = NULL;

	input = fopen(argv[1], "r");
	if (input == NULL)
		return 0;
		
	output = fopen(argv[2], "w");
	if (output == NULL)
		return 0;
	//alocare spatiu pentru elementele necesare la citire
	buffer = (char*)malloc(MAX_BUFFER * sizeof(char)); if (buffer == NULL) return 0;
	nume = (char*)malloc(MAX_BUFFER * sizeof(char));   if (nume == NULL) return 0;
	ip = (char*)malloc(MAX_BUFFER * sizeof(char));     if (ip == NULL) return 0;
	mod = (char*)malloc(MAX_BUFFER * sizeof(char));	   if (mod == NULL) return 0;
	functionare = (char*)malloc(MAX_BUFFER * sizeof(char)); if (functionare == NULL) return 0;

	fscanf(input, "%d\n", &nrStive);
	Stive = (ALG)malloc(nrStive * sizeof(TLG));
	for (index = 0; index < nrStive; index++)
	{
		Stive[index] = (TLG)malloc(sizeof(TCelulaG));
		Stive[index] = NULL;
	}

	while (fscanf(input, "%s", buffer) != EOF)
	{
		if (!strcmp(buffer, "set"))
		{
			fscanf(input, "%d %s", &id, mod);
			if (!strcmp(mod, "STACK"))
			{
				fscanf(input, "%d" "%s", &id_stiva, functionare);
				index = indiceStiva(id, Stive, Coada, nrStive);
				if (index == -1)
				{
					entry = stergeDinCoada(&Coada, &CoadaLast, id);
				}
				else if (index >= 0)
				{
					entry = stergeDinStiva(&Stive[index], id);
				}
				addInStiva(&Stive[id_stiva], entry->id, entry->nume, entry->ip, entry->mod, functionare);
			}
			else if (!strcmp(mod, "SINGLE"))
			{
				index = indiceStiva(id, Stive, Coada, nrStive);
				if (index >= 0)
				{
					entry = stergeDinStiva(&Stive[index], id);
					addInCoada(&Coada, &CoadaLast, entry->id, entry->nume, entry->ip, entry->mod);			
				}
			}
		}
		else if (!strcmp(buffer, "add"))
		{		
			fscanf(input, "%d %s %s %s",&id, nume, ip, mod);
			
			if (!strcmp(mod, "STACK"))
			{
				fscanf(input, "%d %s", &id_stiva, functionare);
				addInStiva(&Stive[id_stiva], id, nume, ip, mod, functionare);
			}
			else if (!strcmp(mod, "SINGLE"))
			{
				addInCoada(&Coada, &CoadaLast, id, nume, ip, mod);			
			}
		}
		else if (!strcmp(buffer, "del"))
		{
			fscanf(input, "%d", &id);
			
			index = indiceStiva(id, Stive, Coada, nrStive);
			if (index == -1) // id-ul este in Coada
				stergeDinCoada(&Coada, &CoadaLast, id);
			else if (index >= 0) // id-ul este in Stive
				stergeDinStiva(&Stive[index], id);
		}
		else if (!strcmp(buffer, "show"))
		{
			show(output, Stive, Coada, nrStive);
		}
		else if (!strcmp(buffer, "ipmin"))
		{
			ipmin(output, Coada);
		}
	}

	if (Coada != CoadaLast)
		free(Coada);
	free(CoadaLast);

	for (index = 0; index < nrStive; index++)
		free(Stive[index]);
	free(Stive);
	free(buffer);
	free(mod);
	free(nume);
	free(functionare);
	free(ip);

	fclose(input);
	fclose(output);

	return 0;
}

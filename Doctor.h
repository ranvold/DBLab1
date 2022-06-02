#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Checker.h"
#include "Patient.h"

#define DOCTOR_IND "doctor.ind"
#define DOCTOR_DATA "doctor.fl"
#define DOCTOR_GARBAGE "doctor_garbage.txt"
#define INDEXER_SIZE sizeof(struct Indexer)
#define DOCTOR_SIZE sizeof(struct Doctor)


void noteDeletedDoctor(int id)
{
	FILE* garbageZone = fopen(DOCTOR_GARBAGE, "rb");		

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	int* delIds = malloc(garbageCount * sizeof(int));		

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);				
	}

	fclose(garbageZone);									
	garbageZone = fopen(DOCTOR_GARBAGE, "wb");				
	fprintf(garbageZone, "%d", garbageCount + 1);			

	for (int i = 0; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);				
	}

	fprintf(garbageZone, " %d", id);						
	free(delIds);											
	fclose(garbageZone);									
}

void overwriteGarbageId(int garbageCount, FILE* garbageZone, struct Doctor* record)
{
	int* delIds = malloc(garbageCount * sizeof(int));		

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);				
	}

	record->id = delIds[0];									

	fclose(garbageZone);									
	fopen(DOCTOR_GARBAGE, "wb");							
	fprintf(garbageZone, "%d", garbageCount - 1);			

	for (int i = 1; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);				
	}

	free(delIds);											
	fclose(garbageZone);									
}

int insertDoctor(struct Doctor record)
{
	FILE* indexTable = fopen(DOCTOR_IND, "a+b");			
	FILE* database = fopen(DOCTOR_DATA, "a+b");				
	FILE* garbageZone = fopen(DOCTOR_GARBAGE, "rb");		
	struct Indexer indexer;
	int garbageCount;

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount)										
	{
		overwriteGarbageId(garbageCount, garbageZone, &record);

		fclose(indexTable);									
		fclose(database);									

		indexTable = fopen(DOCTOR_IND, "r+b");				
		database = fopen(DOCTOR_DATA, "r+b");				

		fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
		fread(&indexer, INDEXER_SIZE, 1, indexTable);
		fseek(database, indexer.address, SEEK_SET);			
	}
	else                                                   
	{
		long indexerSize = INDEXER_SIZE;

		fseek(indexTable, 0, SEEK_END);						

		if (ftell(indexTable))								
		{
			fseek(indexTable, -indexerSize, SEEK_END);		
			fread(&indexer, INDEXER_SIZE, 1, indexTable);	

			record.id = indexer.id + 1;						
		}
		else                                                
		{
			record.id = 1;									
		}
	}

	record.firstPatientAddress = -1;
	record.patientsCount = 0;

	fwrite(&record, DOCTOR_SIZE, 1, database);				

	indexer.id = record.id;									
	indexer.address = (record.id - 1) * DOCTOR_SIZE;		
	indexer.exists = 1;										

	printf("Your doctor\'s id is %d\n", record.id);

	fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);			
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int getDoctor(struct Doctor* doctor, int id, char* error)
{
	FILE* indexTable = fopen(DOCTOR_IND, "rb");				
	FILE* database = fopen(DOCTOR_DATA, "rb");				

	if (!checkFileExistence(indexTable, database, error))
	{
		return 0;
	}

	struct Indexer indexer;

	if (!checkIndexExistence(indexTable, error, id))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);	
	fread(&indexer, INDEXER_SIZE, 1, indexTable);			

	if (!checkRecordExistence(indexer, error))
	{
		return 0;
	}

	fseek(database, indexer.address, SEEK_SET);				
	fread(doctor, sizeof(struct Doctor), 1, database);		
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int updateDoctor(struct Doctor doctor, char* error)
{
	FILE* indexTable = fopen(DOCTOR_IND, "r+b");			
	FILE* database = fopen(DOCTOR_DATA, "r+b");				

	if (!checkFileExistence(indexTable, database, error))
	{
		return 0;
	}

	struct Indexer indexer;
	int id = doctor.id;

	if (!checkIndexExistence(indexTable, error, id))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);	
	fread(&indexer, INDEXER_SIZE, 1, indexTable);			

	if (!checkRecordExistence(indexer, error))
	{
		return 0;
	}

	fseek(database, indexer.address, SEEK_SET);				
	fwrite(&doctor, DOCTOR_SIZE, 1, database);				
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int deleteDoctor(int id, char* error)
{
	FILE* indexTable = fopen(DOCTOR_IND, "r+b");			
															
	if (indexTable == NULL)
	{
		strcpy(error, "database files are not created yet");
		return 0;
	}

	if (!checkIndexExistence(indexTable, error, id))
	{
		return 0;
	}

	struct Doctor doctor;
	getDoctor(&doctor, id, error);

	struct Indexer indexer;

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);	
	fread(&indexer, INDEXER_SIZE, 1, indexTable);			

	indexer.exists = 0;										

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);

	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);			
	fclose(indexTable);										

	noteDeletedDoctor(id);									


	if (doctor.patientsCount)								
	{
		FILE* patientDb = fopen(PATIENT_DATA, "r+b");
		struct Patient patient;

		fseek(patientDb, doctor.firstPatientAddress, SEEK_SET);

		for (int i = 0; i < doctor.patientsCount; i++)
		{
			fread(&patient, PATIENT_SIZE, 1, patientDb);
			fclose(patientDb);
			deletePatient(doctor, patient, patient.patientId, error);

			patientDb = fopen(PATIENT_DATA, "r+b");
			fseek(patientDb, patient.nextAddress, SEEK_SET);
		}

		fclose(patientDb);
	}
	return 1;
}
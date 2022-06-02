#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Patient.h"

int checkFileExistence(FILE* indexTable, FILE* database, char* error)
{
	
	if (indexTable == NULL || database == NULL)
	{
		strcpy(error, "database files are not created yet");
		return 0;
	}

	return 1;
}

int checkIndexExistence(FILE* indexTable, char* error, int id)
{
	fseek(indexTable, 0, SEEK_END);

	long indexTableSize = ftell(indexTable);

	if (indexTableSize == 0 || id * sizeof(struct Indexer) > indexTableSize)
	{
		strcpy(error, "no such ID in table");
		return 0;
	}

	return 1;
}

int checkRecordExistence(struct Indexer indexer, char* error)
{
	
	if (!indexer.exists)
	{
		strcpy(error, "The record you\'re looking for has been removed");
		return 0;
	}

	return 1;
}


void info()
{
	FILE* indexTable = fopen("doctor.ind", "rb");

	if (indexTable == NULL)
	{
		printf("Error: database files are not created yet\n");
		return;
	}

	int doctorCount = 0;
	int patientCount = 0;

	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct Indexer);

	struct Doctor doctor;

	char dummy[51];

	for (int i = 1; i <= indAmount; i++)
	{
		if (getDoctor(&doctor, i, dummy))
		{
			doctorCount++;
			patientCount += doctor.patientsCount;

			printf("Doctor #%d has %d patient(s)\n", i, doctor.patientsCount);
		}
	}

	fclose(indexTable);

	printf("Total doctors: %d\n", doctorCount);
	printf("Total patients: %d\n", patientCount);
}
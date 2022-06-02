#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "Structures.h"
#include "Checker.h"
#include "Doctor.h"

#define PATIENT_DATA "patient.fl"
#define PATIENT_GARBAGE "patient_garbage.txt"
#define PATIENT_SIZE sizeof(struct Patient)


void reopenDatabase(FILE* database)
{
	fclose(database);
	database = fopen(PATIENT_DATA, "r+b");
}

void linkAddresses(FILE* database, struct Doctor doctor, struct Patient patient)
{
	reopenDatabase(database);								

	struct Patient previous;

	fseek(database, doctor.firstPatientAddress, SEEK_SET);

	for (int i = 0; i < doctor.patientsCount; i++)		    
	{
		fread(&previous, PATIENT_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	}

	previous.nextAddress = patient.selfAddress;				
	fwrite(&previous, PATIENT_SIZE, 1, database);				
}

void relinkAddresses(FILE* database, struct Patient previous, struct Patient patient, struct Doctor* doctor)
{
	if (patient.selfAddress == doctor->firstPatientAddress)		
	{
		if (patient.selfAddress == patient.nextAddress)			
		{
			doctor->firstPatientAddress = -1;
		}
		else                                              
		{
			doctor->firstPatientAddress = patient.nextAddress;  
		}
	}
	else                                                    
	{
		if (patient.selfAddress == patient.nextAddress)			
		{
			previous.nextAddress = previous.selfAddress;    
		}
		else                                                
		{
			previous.nextAddress = patient.nextAddress;		
		}

		fseek(database, previous.selfAddress, SEEK_SET);	
		fwrite(&previous, PATIENT_SIZE, 1, database);			
	}
}

void noteDeletedPatient(long address)
{
	FILE* garbageZone = fopen(PATIENT_GARBAGE, "rb");			

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	long* delAddresses = malloc(garbageCount * sizeof(long));

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%ld", delAddresses + i);		
	}

	fclose(garbageZone);									
	garbageZone = fopen(PATIENT_GARBAGE, "wb");				
	fprintf(garbageZone, "%ld", garbageCount + 1);			

	for (int i = 0; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %ld", delAddresses[i]);		
	}

	fprintf(garbageZone, " %d", address);					
	free(delAddresses);										
	fclose(garbageZone);									
}

void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Patient* record)
{
	long* delIds = malloc(garbageCount * sizeof(long));		

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);				
	}

	record->selfAddress = delIds[0];						
	record->nextAddress = delIds[0];

	fclose(garbageZone);									
	fopen(PATIENT_GARBAGE, "wb");							   
	fprintf(garbageZone, "%d", garbageCount - 1);			

	for (int i = 1; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);				
	}

	free(delIds);											
	fclose(garbageZone);									
}

int insertPatient(struct Doctor doctor, struct Patient patient, char* error)
{
	patient.exists = 1;

	FILE* database = fopen(PATIENT_DATA, "a+b");
	FILE* garbageZone = fopen(PATIENT_GARBAGE, "rb");

	int garbageCount;

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount)											
	{
		overwriteGarbageAddress(garbageCount, garbageZone, &patient);
		reopenDatabase(database);								
		fseek(database, patient.selfAddress, SEEK_SET);			
	}
	else                                                        
	{
		fseek(database, 0, SEEK_END);

		int dbSize = ftell(database);

		patient.selfAddress = dbSize;
		patient.nextAddress = dbSize;
	}

	fwrite(&patient, PATIENT_SIZE, 1, database);					

	if (!doctor.patientsCount)								    
	{
		doctor.firstPatientAddress = patient.selfAddress;
	}
	else                                                        
	{
		linkAddresses(database, doctor, patient);
	}

	fclose(database);											

	doctor.patientsCount++;										
	updateDoctor(doctor, error);								

	return 1;
}

int getPatient(struct Doctor doctor, struct Patient* patient, int patientId, char* error)
{
	if (!doctor.patientsCount)									
	{
		strcpy(error, "This doctor has no patients yet");
		return 0;
	}

	FILE* database = fopen(PATIENT_DATA, "rb");


	fseek(database, doctor.firstPatientAddress, SEEK_SET);		
	fread(patient, PATIENT_SIZE, 1, database);

	for (int i = 0; i < doctor.patientsCount; i++)				
	{
		if (patient->patientId == patientId)
		{
			fclose(database);
			return 1;
		}

		fseek(database, patient->nextAddress, SEEK_SET);
		fread(patient, PATIENT_SIZE, 1, database);
	}

	strcpy(error, "No such patient in database");					
	fclose(database);
	return 0;
}


int updatePatient(struct Patient patient, int patientId)
{
	FILE* database = fopen(PATIENT_DATA, "r+b");

	fseek(database, patient.selfAddress, SEEK_SET);
	fwrite(&patient, PATIENT_SIZE, 1, database);
	fclose(database);

	return 1;
}

int deletePatient(struct Doctor doctor, struct Patient patient, int patientId, char* error)
{
	FILE* database = fopen(PATIENT_DATA, "r+b");
	struct Patient previous;

	fseek(database, doctor.firstPatientAddress, SEEK_SET);

	do		                                                    
	{															
		fread(&previous, PATIENT_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	} while (previous.nextAddress != patient.selfAddress && patient.selfAddress != doctor.firstPatientAddress);

	relinkAddresses(database, previous, patient, &doctor);
	noteDeletedPatient(patient.selfAddress);						

	patient.exists = 0;											

	fseek(database, patient.selfAddress, SEEK_SET);				
	fwrite(&patient, PATIENT_SIZE, 1, database);					
	fclose(database);

	doctor.patientsCount--;										
	updateDoctor(doctor, error);

}
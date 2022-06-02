#pragma once
#include <stdio.h>
#include <string.h>
#include "Structures.h"
#include "Doctor.h"

void readDoctor(struct Doctor* doctor)
{
	char name[16];

	name[0] = '\0';

	printf("Enter doctor\'s name: ");
	scanf("%s", name);

	strcpy(doctor->name, name);

}

void readPatient(struct Patient* patient)
{
	int x;

	printf("Enter price: ");
	scanf("%d", &x);

	patient->price = x;

}
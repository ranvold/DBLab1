#pragma once

#include <stdio.h>
#include "Doctor.h"
#include "Structures.h"

void printDoctor(struct Doctor doctor)
{
	printf("Doctor\'s name: %s\n", doctor.name);
}

void printPatient(struct Patient patient, struct Doctor doctor)
{
	printf("Doctor: %s\n", doctor.name);
	printf("Price: %d\n", patient.price);
}
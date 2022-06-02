#include <stdio.h>
#include "Structures.h"
#include "Doctor.h"
#include "Patient.h"
#include "Read.h"
#include "Print.h"


int main()
{
	struct Doctor doctor;
	struct Patient patient;

	while (1)
	{
		int choice;
		int id;
		char error[51];

		printf("Choose option:\n0 - Quit\n1 - Insert Doctor\n2 - Get Doctor\n3 - Update Doctor\n4 - Delete Doctor\n5 - Insert Patient\n6 - Get Patient\n7 - Update Patient\n8 - Delete Patient\n9 - Info\n");
		scanf("%d", &choice);

		switch (choice)
		{
		case 0:
			return 0;

		case 1:
			readDoctor(&doctor);
			insertDoctor(doctor);
			break;

		case 2:
			printf("Enter ID: ");
			scanf("%d", &id);
			getDoctor(&doctor, id, error) ? printDoctor(doctor) : printf("Error: %s\n", error);
			break;

		case 3:
			printf("Enter ID: ");
			scanf("%d", &id);

			doctor.id = id;

			readDoctor(&doctor);
			updateDoctor(doctor, error) ? printf("Updated successfully\n") : printf("Error: %s\n", error);
			break;

		case 4:
			printf("Enter ID: ");
			scanf("%d", &id);
			deleteDoctor(id, error) ? printf("Deleted successfully\n") : printf("Error: %s\n", error);
			break;

		case 5:
			printf("Enter doctor\'s ID: ");
			scanf("%d", &id);

			if (getDoctor(&doctor, id, error))
			{
				patient.doctorId = id;
				printf("Enter patient ID: ");
				scanf("%d", &id);

				patient.patientId = id;
				readPatient(&patient);
				insertPatient(doctor, patient, error);
				printf("Inserted successfully. To access, use doctor\'s and patient\'s IDs\n");
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 6:
			printf("Enter doctor\'s ID: ");
			scanf("%d", &id);

			if (getDoctor(&doctor, id, error))
			{
				printf("Enter patient ID: ");
				scanf("%d", &id);
				getPatient(doctor, &patient, id, error) ? printPatient(patient, doctor) : printf("Error: %s\n", error);
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 7:
			printf("Enter doctor\'s ID: ");
			scanf("%d", &id);

			if (getDoctor(&doctor, id, error))
			{
				printf("Enter patient ID: ");
				scanf("%d", &id);

				if (getPatient(doctor, &patient, id, error))
				{
					readPatient(&patient);
					updatePatient(patient, id, error);
					printf("Updated successfully\n");
				}
				else
				{
					printf("Error: %s\n", error);
				}
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 8:
			printf("Enter doctor\'s ID: ");
			scanf("%d", &id);

			if (getDoctor(&doctor, id, error))
			{
				printf("Enter patient ID: ");
				scanf("%d", &id);

				if (getPatient(doctor, &patient, id, error))
				{
					deletePatient(doctor, patient, id, error);
					printf("Deleted successfully\n");
				}
				else
				{
					printf("Error: %s\n", error);
				}
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 9:
			info();
			break;

		default:
			printf("Invalid input, please try again\n");
		}

		printf("---------\n");
	}

	return 0;
}
#pragma once


struct Doctor
{
	int id;
	char name[16];
	long firstPatientAddress;
	int patientsCount;
};

struct Patient
{
	int doctorId;
	int patientId;
	int price;
	int exists;
	long selfAddress;
	long nextAddress;
};

struct Indexer
{
	int id;
	int address;
	int exists;
};
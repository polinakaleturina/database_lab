#pragma once
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <vector>

struct Record {
	int id;
	char name[20];
	char sname[40];
	int age;
	double salary;
	bool is_male;

	void print(std:: ostream& out, char sep = '\t');
};


class DB
{
public:
	DB();
	~DB() {
		fclose(_file);
	}
	
	bool open(const std::string& file_name);
	bool is_opened();

	int get_record_count();
	Record get_record(int index);
	bool append(const std::string& name, const std::string& sname, int age, double salary, bool is_male);
	bool edit(int index, const std::string& name, const std::string& sname, int age, double salary, bool is_male);
	bool delete_record(int index);

	bool export_to_file(const std::string& file);

	std::vector<int> find_by_sname(const std::string& sname);
	int find_by_id(int id);

	bool backup(const std::string& file);
	bool restore(const std::string& file);

private:
	std::string _file_name;
	FILE* _file;
	int _id;
};



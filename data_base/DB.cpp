#define _CRT_SECURE_NO_WARNINGS
#include "DB.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <filesystem>
#include <io.h>

void Record::print(std::ostream& out, char sep) {
	out << id << sep 
		<< name << sep 
		<< sname << sep
		<< age << sep 
		<< salary << sep 
		<< (is_male ? "male" : "female") << std::endl;
}

DB::DB() {
	_file = nullptr;
	_id = -1;
}

bool DB::open(const std::string& file_name) {
	_file_name = file_name;
	_file = fopen(file_name.c_str(), "ab+");
	fclose(_file);
	_file = fopen(file_name.c_str(), "rb+");
	int count = get_record_count();
	if (count == 0) {
		_id = 1;
	}
	else {
		Record rec = get_record(count - 1);
		_id = rec.id + 1;
	}
	return true;
}

bool DB::is_opened() {
	return _file != nullptr;
}

void copy_str(char* dest, const std::string& src, int size) {
	if (src.size() > size) {
		strncpy(dest, src.c_str(), size);
		dest[size] = '\0';

	}
	else {
		strcpy(dest, src.c_str());
	}
}

int DB::get_record_count() {
	struct stat buf;
	fstat(_fileno(_file), &buf);
	return buf.st_size / sizeof(Record);
}

Record DB::get_record(int index) {
	fseek(_file, index*sizeof(Record), SEEK_SET);
	Record res;
	int count = fread(&res, 1, sizeof(res), _file);
	return res;
}

bool DB::append(const std::string& name, const std::string& sname, int age, double salary, bool is_male) {
	Record rec;
	rec.id = _id++;
	copy_str(rec.name, name, 19);
	copy_str(rec.sname, sname, 39);
	rec.age = age;
	rec.salary = salary;
	rec.is_male = is_male;

	fwrite(&rec, sizeof(rec), 1, _file);
	fflush(_file);

	return true;

}

bool DB::edit(int index, const std::string& name, const std::string& sname, int age, double salary, bool is_male) {
	int count = get_record_count();
	if (index < 0 || index >= count) {
		return false;
	}

	int rec_size = sizeof(Record);
	Record rec = get_record(index);
	copy_str(rec.name, name, 19);
	copy_str(rec.sname, sname, 39);
	rec.age = age;
	rec.salary = salary;
	rec.is_male = is_male;

	fseek(_file, index * rec_size, SEEK_SET);
	fwrite(&rec, rec_size, 1, _file);
	fflush(_file);
	return true;
}

bool DB::delete_record(int index) {
	int count = get_record_count();
	if (index < 0 || index >= count) {
		return false;
	}

	int rec_size = sizeof(Record);
	for (int i = index + 1; i < count; i++) {
		Record rec = get_record(i);
		fseek(_file, (i-1)*rec_size, SEEK_SET);
		fwrite(&rec, rec_size, 1, _file);	
	}
	_chsize(_fileno(_file), (count - 1) * rec_size);
	fflush(_file);
	return true;
}

bool DB::export_to_file(const std::string& file_name) {
	std::ofstream file(file_name);
	int count = get_record_count();
	for (int i = 0; i < count; i++) {
		Record rec = get_record(i);
		rec.print(file, ',');
	}
	file.close();
	return true;
}

std::vector<int> DB::find_by_sname(const std::string& sname) {
	std::vector<int> res;

	int count = get_record_count();
	for (int i = 0; i < count; i++) {
		Record rec = get_record(i);
		if (sname == rec.sname) {
			res.push_back(i);
		}
	}	
	return res;
}

int DB::find_by_id(int id) {
	Record res;
	int index = id - 1;
	int index_max = get_record_count();
	if (index_max == 0 || index < 0 || index >= index_max) {
		return -1;
	}
	index_max = index;
	int index_min = -1;
	int iter_count = 0;
	while (true) {
		res = get_record(index);
		if (res.id == id) {
			break;
		}
		
		if (res.id > id) {
			index -= res.id - id;
			if (index <= index_min) {
				index = index_min + 1;
			}

			index_min = index;
		}
		else {
			index += id - res.id;
			if (index >= index_max) {
				index = index_max - 1;
			}

			index_max = index;
		}

		if (index_min >= index_max) {
			res.id = -1;
			break;
		}

		iter_count++;
		if (iter_count > 10) {
			res.id = -1;
			break;
		}
	}
	return res.id>0 ? index : -1;
}

bool DB::backup(const std::string& file) {
	if (!is_opened()) {
		return false;
	}

	fclose(_file);
	bool res = std::filesystem::copy_file(_file_name, file,
		std::filesystem::copy_options::overwrite_existing);
	_file = fopen(_file_name.c_str(), "rb+");
	return res;
}

bool DB::restore(const std::string& file) {
	if (!is_opened()) {
		return false;
	}

	fclose(_file);
	bool res = std::filesystem::copy_file(file, _file_name, 
		std::filesystem::copy_options::overwrite_existing);
	open(_file_name);
	return res;
}







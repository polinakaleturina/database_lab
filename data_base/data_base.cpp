#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include "DB.h"

void test() {
    DB db;
    db.open("d:/test/db/test.db");
    db.append("john", "black", 37, 45.5, true);
    db.append("mary", "white", 30, 50, false);

    //db.edit(4, "ivan", "cool", 50, 30, true);

    //db.delete_record(1);

    int count = db.get_record_count();
    for (int i = 0; i < count; i++) {
        Record rec = db.get_record(i);
        rec.print(std::cout);
    }

    //db.export_to_file("d:/test/db/test.csv");

    /*std::cout << "search by sname:\n";
    auto recs = db.find_by_sname("white");
    for (auto rec : recs) {
        rec.print(std::cout);
    }
    std::cout << "search by id:\n";
    auto rec = db.find_by_id(5);
    rec.print(std::cout);
    rec = db.find_by_id(2);
    rec.print(std::cout);*/
}

std::vector<std::string> pars_line(const std::string& line) {
    std::vector<std::string> res;
    std::stringstream ss(line);
    for (std::string item; std::getline(ss, item, ' ');) {
        res.push_back(item);
    }
    return res;
}   

int main()
{
    DB db;
    for (std::string line; std::getline(std::cin, line);) {
        auto items = pars_line(line);
        if (items.empty()) {
            return 0;
        }
        auto cmd = items[0];
        if (cmd == "exit") {
            return 0;
        }
        else if (cmd == "open") {
            if (items.size() < 2) {
                std::cout << "invalid\n";
            }
            else {
                db.open(items[1]);
            }
        }
        else {
            try {
                if (!db.is_opened()) {
                    std::cout << "data base is not opened\n";
                    continue;
                }

                if (cmd == "append") {
                    if (items.size() < 6) {
                        std::cout << "invalid\n";
                    }
                    else {
                        db.append(items[1], items[2], std::stoi(items[3]), std::stod(items[4]), items[5] == "true");
                    }
                }
                else if (cmd == "list") {
                    int count = db.get_record_count();
                    for (int i = 0; i < count; i++) {
                        Record rec = db.get_record(i);
                        rec.print(std::cout);
                    }
                }
                else if (cmd == "edit") {
                    if (items.size() < 7) {
                        std::cout << "invalid\n";
                    }
                    else {
                        db.edit(std::stoi(items[1]), items[2], items[3], std::stoi(items[4]), std::stod(items[5]), items[6] == "true");
                    }
                }
                else if (cmd == "delete") {
                    if (items.size() < 2) {
                        std::cout << "invalid\n";
                    }
                    else {
                        db.delete_record(std::stoi(items[1]));
                    }
                }
                else if (cmd == "export") {
                    if (items.size() < 2) {
                        std::cout << "invalid\n";
                    }
                    else {
                        db.export_to_file(items[1]);
                    }
                }
                else if (cmd == "backup") {
                    if (items.size() < 2) {
                        std::cout << "invalid\n";
                    }
                    else {
                        db.backup(items[1]);
                    }
                }
                else if (cmd == "restore") {
                    if (items.size() < 2) {
                        std::cout << "invalid\n";
                    }
                    else {
                        db.restore(items[1]);
                    }
                }
                else if (cmd == "find") {
                    if (items.size() < 2) {
                        std::cout << "invalid\n";
                    }
                    else {

                        try {
                            auto index = db.find_by_id(std::stoi(items[1]));
                            if (index >= 0) {
                                std::cout << index << '\t';
                                db.get_record(index).print(std::cout);
                            }
                            else {
                                std::cout << "not found\n";
                            }
                        }
                        catch (std::invalid_argument const& ex) {
                            auto indexs = db.find_by_sname(items[1]);
                            for (auto index : indexs) {
                                std::cout << index << '\t';
                                db.get_record(index).print(std::cout);
                            }
                            if (indexs.empty()) {
                                std::cout << "not found\n";
                            }
                        }
                    }
                }
            }
            catch (const std::exception& ex) {
                std::cout << ex.what() << '\n';
            }
        }
    }
}



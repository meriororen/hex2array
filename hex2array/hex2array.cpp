// hex2array.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdint>
#include "Line.h"
using namespace std;

uint8_t crc = 0;
long datacount = 0;


void print_data(Line *line, ofstream *of, int size) {
	char buff[35];
	int len;
	if (line == nullptr) len = size;
	else {
		len = line->bytecount;
		sprintf_s(buff, 30, "/* 0x%04X */\n\t\t\t", line->address);
		*of << (string)buff;
	}

	for (int i = 0; i < len; i++) {
		uint8_t data = line == nullptr ? 0xFF : (uint8_t)line->data[i];
		crc ^= data;
		sprintf_s(buff, 7, "0x%02X, ", data);
		*of << (string)buff;
		datacount++;
		//printf("\'%02X\', ", (uint8_t)lines->data[i]);
		if (!(datacount % 4)) {
			//printf("\n");
			*of << "\n\t\t\t";
		}
	}
}



int main(int argc, char ** argv) 
{
	if (argc < 3)
	{
		cout << "usage: " << argv[0] << " <hexfile.hex> <output.c>";
		return 1;
	}
	
	ifstream ifile;
	ofstream ofile;
	string l;
	int last_address = 0;
	int last_type = 0xFF;
	int last_size = 0;

	string ipath = argv[1];
	string opath = argv[2];

	cout << "opening " << ipath << "...\n"; 
	ifile.open(ipath, ios::binary | ios::in);
	cout << "opening " << opath << "...\n";
	ofile.open(opath, ios::out | ios::trunc);

	if (ifile.is_open() && ofile.is_open()) 
	{
		ofile << "#include <stdint.h>\n";
		ofile << "#include \"board.h\"\n\n";
		ofile << "const uint8_t hexdata[] = { \n\t\t\t";
		while (getline(ifile, l)) {
			//cout << stoi(line.substr(1, 2), nullptr, 16) << "\n";
			int bytecount = stoi(l.substr(1, 2), nullptr, 16);
			Line *line = new Line(bytecount);
			if (line != nullptr) {
				// get address 
				line->address = stoi(l.substr(3, 4), nullptr, 16);
				// get type 
				line->type = stoi(l.substr(7, 2), nullptr, 16);
				// get data
				for (int i = 0; i < bytecount; i++) {
					line->data[i] = (char)stoi(l.substr(9 + (i * 2), 2), nullptr, 16);
				}
				// get checksum
				line->checksum = stoi(l.substr(9 + (bytecount * 2), 2), nullptr, 16);

#if 0
				cout << "bytecount: " << setfill('0') << setw(2) << std::hex << lines->bytecount << "\n";
				cout << "address: " << setfill('0') << setw(2) << std::hex << lines->address << "\n";
				cout << "type: " << setfill('0') << setw(2) << std::hex << lines->type << "\n";
				cout << "data: [";
				for (int i = 0; i < lines->bytecount; i++) {
					cout << setfill('0') << setw(2) << std::hex << (int)lines->data[i];
					if (i != bytecount-1) cout << "|";
				}
				cout << "]\n";
				cout << setfill('0') << setw(2) << "checksum: " << std::hex << lines->checksum << "\n";
#endif
				char buff[100];
				switch (line->type) {
				case 0x00: // data type
					if (last_type == 0x04 || last_type == 0x00) {
						int blank = line->address - (last_address + last_size);
						if (blank > 0) {
							print_data(nullptr, &ofile, blank);
						}
						print_data(line, &ofile, 0);
					}
					else 
					{
						sprintf_s(buff, 35, "/* address: 0x0000XXXX */\n\t\t\t");
						ofile << (string)buff;
						print_data(line, &ofile, 0);
					}
					break;
				case 0x01: // end of file
					sprintf_s(buff, 25, "/* END OF DATA */\n\t\t\t");
					ofile << (string)buff;
					break;
				case 0x02:
					// TODO: for now do nothing
					break;
				case 0x03:
					// TODO: for now do nothing
					break;
				case 0x04:
					sprintf_s(buff, 35, "/* address: 0x%02X%02XXXXX */\n\t\t\t",
						(uint8_t)line->data[0], (uint8_t)line->data[1]);
					ofile << (string)buff;
					break;
				case 0x05:
					// TODO: for now do nothing
					break;
				default:
					break;
				}
				last_address = line->type == 0x00 ? line->address : last_address;
				last_type = line->type;
				last_size = line->type == 0x00 ? line->bytecount : last_size;
			}
			else 
			{
				cout << "Unable to allocate line memory!";
				return -1;
			}
		}
		ofile << "};\n";
		ofile << "int hexdata_size = " << datacount << ";\n";
		ofile << "uint8_t hexdata_crc = 0x" << hex << (int)crc << ";\n";
		ifile.close();
		ofile.close();
	}
	else 
	{
		cout << "Cannot open file!";
		return 1;
	}
	cout << "\nFinished. Total : " << datacount << " bytes, crc = " << hex << (int)crc;
	//while (1);
	return 0;
}


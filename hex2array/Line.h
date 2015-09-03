#pragma once
class Line
{
public:
	int bytecount;
	int address;
	int type;
	char *data;
	int checksum;
	Line(int bytecount);
	Line(const Line &l);
	~Line();

};


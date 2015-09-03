#include "stdafx.h"
#include "Line.h"

Line::Line(int bc)
{
	bytecount = bc;
	data = new char[bc];
}

Line::Line(const Line &l)
{
	data = new char[l.bytecount];
	memcpy(data, l.data, sizeof(l.data));
}

Line::~Line()
{
	delete[] data;
}
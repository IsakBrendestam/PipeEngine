#pragma once

typedef unsigned int ID;

constexpr ID INVALID_ID = 0;

class UniqueID
{
public:
	static ID GenerateID();

private:
	static ID m_id;
};
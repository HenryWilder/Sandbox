#pragma once

struct MemoryNode
{
	MemoryNode* m_next[256]; // If nothing comes next, use the ResponseMemory
	const char* m_resp; // Remembered response
};

struct MemoryTreeController
{
	enum class State
	{

	} m_state;
};
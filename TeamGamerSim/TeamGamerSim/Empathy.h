#pragma once

class Emotion
{
	signed int value; // Positive, neutral, negative
	bool direction; // At me or other?

public:
	int GetValue() const; 

};

class Thought
{
	Thought(Emotion);
};

#include "InputProcessing.h"

// Input should be top-left corner of the number followed by the size
char ReadNumber(int x, int y, NumberScale size)
{
	enum Sample : unsigned short
	{
		Top = 0b000000001,
		Middle = 0b000000010,
		Left = 0b000000100,
		Right = 0b000001000,
		BottomL = 0b000010000,
		BottomC = 0b000100000,
		BottomR = 0b001000000,
		TopL = 0b010000000,
		TopR = 0b100000000,
	};
	Sample sampleIDs[] = { Top, Middle, Left, Right, BottomL, BottomC, BottomR, TopL, TopR };
	POINT sampleOffsets[] = { { 5,0 },{ 5,8 },{ 0,8 },{ 10,8 },{ 0,12 },{ 5,12 },{ 10,12 },{ 0,7 },{ 10,7 } };
	enum SamplesToNumbers : unsigned short
	{
		_n1 = 0b000100011,
		_n2 = 0b001110011,
		_n3 = 0b000100001,
		_n4 = 0b010001110,
		_n5 = 0b100101001,
		_n6 = 0b010101101,
		_n7 = 0b000000011,
		_n8 = 0b000101101,
		_n9 = 0b100100001,
		_n0 = 0b110101101,
	};
	SamplesToNumbers values[] = { _n0,_n1,_n2,_n3,_n4,_n5,_n6,_n7,_n8,_n9 };

	int guess = 0b000000000;
	unsigned char threshold = 100;

	for (int sample = 0; sample < 9; ++sample)
	{
		if (GetPixelColor(x + sampleOffsets[sample].x, y + sampleOffsets[sample].y).Gray() > threshold) guess |= sampleIDs[sample];
	}

	for (char guessValue = 0; guessValue < 10; ++guessValue)
	{
		if (guess == values[guessValue]) return guessValue;
	}
	return 0; // Error returns zero
}

// Run this about once every frame
void ReadGameClock()
{
	int time = (int)ReadNumber(pnt::clk_decisecX, pnt::clk.y, NumberScale::Small); // Deciseconds
	int seconds = (int)ReadNumber(pnt::clk_secX, pnt::clk.y, NumberScale::Small); // Seconds (ones)
	int tensOfSeconds = (int)ReadNumber(pnt::clk_10secX, pnt::clk.y, NumberScale::Small); // Seconds (tens)
	int minute = (int)ReadNumber(pnt::clk.x, pnt::clk.y, NumberScale::Small); // Minutes

	time = time + 10 * seconds + 100 * tensOfSeconds + 600 * minute;

	g_gameState.gameData.time.UpdateTime(time);
}

void CheckVentsReset()
{
	if (GetPixelColor(pnt::ofc::ventWarning).RedDev() > 35 ||
		GetPixelColor(pnt::cam::ventWarning).RedDev() > 35)
	{
		g_gameState.gameData.ventilationNeedsReset = true;
	}
}

void GenerateSamplePoints(POINT* arr, POINT start, long scale)
{
	arr[0] = start;
	arr[1] = { arr[0].x, arr[0].y + scale };
	arr[2] = { arr[0].x + scale, arr[0].y };
	arr[3] = { arr[0].x, arr[0].y - scale };
	arr[4] = { arr[0].x - scale, arr[0].y };
}

int TestSamples(Button button, CNorm compare, double threshold)
{
	POINT samplePoint[5];

	GenerateSamplePoints(samplePoint, GetButtonPos(button), 4);

	int matchCount = 0;

	for (int i = 0; i < 5; ++i)
	{
		CNorm sample = GetPixelColor(samplePoint[i]).Normal();

		if (CDot(sample, compare) > threshold) ++matchCount; // Increment matchCount when a match is found
	}

	return matchCount;
}

int MaxInArray(int* arr, size_t size) // Returns the index of the highest value
{
	int currentMax = 0;
	int indexOfMax = 0;
	for (int index = 0; index < size; ++index) {
		const int& value = arr[index];
		if (value > currentMax) {
			currentMax = value;
			indexOfMax = index;
		}
	}
	return indexOfMax;
}

void UpdateState()
{
	constexpr double threshold = .99;
	State state = State::Office;

	int statesToTest[3] = { 0,0,0 }; // List of how many samples returned as matches for each of the buttons being tested

	for (int sysBtn = 0; sysBtn < 3; ++sysBtn) {
		statesToTest[sysBtn] = TestSamples(Button(SystemButton(sysBtn)), clr::sysButtonNrm, threshold);
	}
	int indexOfHighestValue = MaxInArray(statesToTest, 3);
	if (statesToTest[indexOfHighestValue] == 5) // We must have over 50% of the samples returning as matches
		state = State(indexOfHighestValue);

	g_gameState.state = state; // Update the global state

	switch (state)
	{
	case State::Office:
		break;

	case State::Camera:
	{
		int camsToTest[8] = {};
		for (int camera = 0; camera < 8; ++camera) {
			camsToTest[camera] = TestSamples(CameraButton(camera), clr::camButtonNrm, threshold);
		}
		g_gameState.stateData.cd.camera = Camera(MaxInArray(camsToTest, 8)); // If we've confirmed the state then there's no doubt we can identify the camera
	}
		break;

	case State::Vent:
		break;

	case State::Duct:
		break;
	}
}

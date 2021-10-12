#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>
#include <vector>

constexpr size_t c_sampleRate = 41000;
constexpr size_t c_audioBufferSize = 4096;

#define sign(x) (((x) > (decltype(x))(0)) - ((x) < (decltype(x))(0)))

using Vector1 = float;

#pragma region Vector math

inline Vector2  operator- (Vector2  vec)          { return      Vector2Negate(vec);             }
inline Vector2  operator+ (Vector2  a, Vector2 b) { return      Vector2Add(a, b);               }
inline Vector2& operator+=(Vector2& a, Vector2 b) { return (a = Vector2Add(a, b));              }
inline Vector2  operator+ (Vector2  a, float val) { return      Vector2AddValue(a, val);        }
inline Vector2& operator+=(Vector2& a, float val) { return (a = Vector2AddValue(a, val));       }
inline Vector2  operator- (Vector2  a, Vector2 b) { return      Vector2Subtract(a, b);          }
inline Vector2& operator-=(Vector2& a, Vector2 b) { return (a = Vector2Subtract(a, b));         }
inline Vector2  operator- (Vector2  a, float val) { return      Vector2SubtractValue(a, val);   }
inline Vector2& operator-=(Vector2& a, float val) { return (a = Vector2SubtractValue(a, val));  }
inline Vector2  operator* (Vector2  a, Vector2 b) { return      Vector2Multiply(a, b);          }
inline Vector2& operator*=(Vector2& a, Vector2 b) { return (a = Vector2Multiply(a, b));         }
inline Vector2  operator* (Vector2  a, float scl) { return      Vector2Scale(a, scl);           }
inline Vector2& operator*=(Vector2& a, float scl) { return (a = Vector2Scale(a, scl));          }
inline Vector2  operator/ (Vector2  a, Vector2 b) { return      Vector2Divide(a, b);            }
inline Vector2& operator/=(Vector2& a, Vector2 b) { return (a = Vector2Divide(a, b));           }
inline Vector2  operator/ (Vector2  a, float div) { return      Vector2Scale(a, 1.0f / div);    }
inline Vector2& operator/=(Vector2& a, float div) { return (a = Vector2Scale(a, 1.0f / div));   }

inline Vector3  operator- (Vector3  vec)          { return      Vector3Negate(vec);             }
inline Vector3  operator+ (Vector3  a, Vector3 b) { return      Vector3Add(a, b);               }
inline Vector3& operator+=(Vector3& a, Vector3 b) { return (a = Vector3Add(a, b));              }
inline Vector3  operator+ (Vector3  a, float val) { return      Vector3AddValue(a, val);        }
inline Vector3& operator+=(Vector3& a, float val) { return (a = Vector3AddValue(a, val));       }
inline Vector3  operator- (Vector3  a, Vector3 b) { return      Vector3Subtract(a, b);          }
inline Vector3& operator-=(Vector3& a, Vector3 b) { return (a = Vector3Subtract(a, b));         }
inline Vector3  operator- (Vector3  a, float val) { return      Vector3SubtractValue(a, val);   }
inline Vector3& operator-=(Vector3& a, float val) { return (a = Vector3SubtractValue(a, val));  }
inline Vector3  operator* (Vector3  a, Vector3 b) { return      Vector3Multiply(a, b);          }
inline Vector3& operator*=(Vector3& a, Vector3 b) { return (a = Vector3Multiply(a, b));         }
inline Vector3  operator* (Vector3  a, float scl) { return      Vector3Scale(a, scl);           }
inline Vector3& operator*=(Vector3& a, float scl) { return (a = Vector3Scale(a, scl));          }
inline Vector3  operator/ (Vector3  a, Vector3 b) { return      Vector3Divide(a, b);            }
inline Vector3& operator/=(Vector3& a, Vector3 b) { return (a = Vector3Divide(a, b));           }
inline Vector3  operator/ (Vector3  a, float div) { return      Vector3Scale(a, 1.0f / div);    }
inline Vector3& operator/=(Vector3& a, float div) { return (a = Vector3Scale(a, 1.0f / div));   }

#pragma endregion

double IPart(double x)
{
	return (double)(long)x;
}
double FPart(double x)
{
	return fmod(x, 1.0);
}

double Sine(double x)
{
	return sin(2.0*PI*x);
}
double Sawtooth(double x)
{
	return 2.0 * FPart(x) - 1.0;
}
double InverseSawtooth(double x)
{
	return -Sawtooth(x);
}
double Square(double x)
{
	x = FPart(x);
	if (x == 0.0 || x == 0.5)
		return 0.0;
	else
		return (x < 0.5 ? 1.0 : -1.0);
}
double Triangle(double x)
{
	x = FPart(x);
	double y = ((x >= 0.0 && x < 0.25) || (x >= 0.5 && x < 0.75) ? 4.0 * fmod(x,0.25) : 1.0 - (4.0 * fmod(x,0.25)));
	return (x < 0.5 ? y : -y);
}

void UpdateBuffer()
{

}

int main() {

#pragma region Setup phase

	int windowWidth = 1280;
	int windowHeight = 720;

	Rectangle canvas; {
		canvas.x = 0;
		canvas.y = 0;
		canvas.width = windowWidth;
		canvas.height = windowHeight;
	}

	InitWindow(windowWidth, windowHeight, "A program I'm making in C++");
	constexpr int c_frameRate = 30;
	SetTargetFPS(c_frameRate);

	InitAudioDevice();
	SetAudioStreamBufferSizeDefault(4096);
	AudioStream stream = InitAudioStream(c_sampleRate, 8 * sizeof(short), 1);
	short* buff = new short[c_audioBufferSize];
	PlayAudioStream(stream);

	double y0 = (double)windowHeight * 0.5;
	size_t volume = 8000;
	size_t maxVolume = 32000;
	size_t dt = c_audioBufferSize / windowWidth;
	double dxDisp = 1.0 / (double)windowWidth;

	bool streamChanged = true;

	size_t note = 0;

	struct Note
	{
		double hz, time;
	};
	double bps = 108.0 / 60.0;
	std::vector<Note> sequence =
	{ // Sharps: C, D, F, G
		{ 000.00, 0.125 }, { 659.25, 0.125 }, { 622.25, 0.125 }, { 554.37, 0.125 }, { 493.88, 0.125 }, { 493.88, 0.125 }, { 659.25, 0.125 }, { 415.30, 0.125 }, { 493.88, 0.075 }, { 554.37, 0.075 },
		{ 493.88, 0.125 }, { 440.00, 0.125 }, { 415.30, 0.125 }, { 369.99, 0.125 }, { 415.30, 0.125 }, { 277.18, 0.125 }, { 329.63, 0.250 },
		{ 369.99, 0.125 }, { 440.00, 0.125 }, { 415.30, 0.125 }, { 369.99, 0.125 }, { 415.30, 0.125 }, { 277.18, 0.125 }, { 329.63, 0.125 }, { 329.63, 0.075 },
		{ 369.99, 0.125 }, { 440.00, 0.125 }, { 415.30, 0.125 }, { 369.99, 0.125 }, { 329.63, 0.125 }, { 311.13, 0.125 }, { 277.18, 0.125 }, { 311.13, 0.125 },
		{ 277.18, 0.125 }, { 659.25, 0.125 }, { 622.25, 0.125 }, { 554.37, 0.125 }, { 493.88, 0.125 }, { 493.88, 0.125 }, { 659.25, 0.125 }, { 415.30, 0.125 }, { 493.88, 0.075 }, { 554.37, 0.075 },
		{ 493.88, 0.125 }, { 440.00, 0.125 }, { 415.30, 0.125 }, { 369.99, 0.125 }, { 415.30, 0.125 }, { 277.18, 0.125 }, { 329.63, 0.250 },
		{ 369.99, 0.125 }, { 440.00, 0.125 }, { 415.30, 0.125 }, { 369.99, 0.125 }, { 415.30, 0.125 }, { 277.18, 0.125 }, { 329.63, 0.125 },
		{ 369.99, 0.125 }, { 440.00, 0.125 }, { 415.30, 0.125 }, { 369.99, 0.125 }, { 415.30, 0.125 }, { 493.88, 0.125 }, { 523.25, 0.250 },
	};
	double freq = sequence[0].hz;
	double lastNoteTime = 0.0;
	double nextNoteTime = sequence[0].time * bps;

	while (!WindowShouldClose())
	{
		if (GetTime() > nextNoteTime)
		{
			freq = sequence[note].hz;
			streamChanged = true;

			note++;
			note = note % sequence.size();

			lastNoteTime = nextNoteTime;
			nextNoteTime += sequence[note].time * bps;

			streamChanged = true;
		}

		size_t samples;
		{
			double period;

			if (freq == 0.0)
				period = c_sampleRate * sequence[note].time * bps;
			else
				period = ((double)c_sampleRate / freq);

			samples = ceil(period * floor((double)c_audioBufferSize / period));
		}

		if (streamChanged)
		{
			for (size_t i = 0; i < samples; ++i)
			{
				if (freq != 0.0)
				{
					double x = freq * (double)i / (double)c_sampleRate; // 0..1 second
					double y = 0.0;
					{

						y = Sine(x) + Triangle(2.0 * x);

					}
					buff[i] = (short)((y > 1.0 ? 1.0 : (y < -1.0 ? -1.0 : y)) * volume);
				}
				else
					buff[i] = 0;
			}
			streamChanged = false;
		}

		if (IsAudioStreamProcessed(stream))
		{
			UpdateAudioStream(stream, buff, samples);
		}

		BeginDrawing(); {

			ClearBackground(BLACK);

			for (size_t x = 1; x < samples; ++x)
			{
				double yP = (double)buff[(x - 1) % c_audioBufferSize] / (double)maxVolume;
				double y = (double)buff[x % c_audioBufferSize] / (double)maxVolume;
				DrawLine((x - 1) / 4, windowHeight / 2 + yP * windowHeight / 2, x / 4, windowHeight / 2 + y * windowHeight / 2, RAYWHITE);
			}

			DrawLine(samples / 4, 0, samples / 4, windowHeight, YELLOW);

			DrawText(TextFormat("%i / 4096 BYTES SAMPLED", samples),0,0,8,RED);

		} EndDrawing();
	}

	delete[] buff;
	
	CloseAudioStream(stream);
	CloseAudioDevice();

	CloseWindow();

	return 0;
}
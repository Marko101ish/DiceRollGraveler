#include <array>
#include <functional>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>

using namespace std::chrono_literals;

static constexpr int DICE_SIZE = 4;
static constexpr int NUM_OF_TURNS = 231;
static constexpr int NUM_OF_SESSIONS = 84'000'000;
static constexpr int NUM_OF_THREADS = 12;
static constexpr int TARGET_NUMBER = 177;

int g_MaxOnes; // all threads write their results here
std::mutex g_MaxOnesMutex; // ensures all threads can write exclusively

// Helper function for formatting large integers
static std::string ThousandSeparator(int number)
{
	std::string num = std::to_string(number);
	if (number < 1000)
	{
		return num;
	}

	int count = 0;
	std::string res;
	for (int i = num.size() - 1; i >= 0; --i)
	{
		count++;
		res.push_back(num[i]);
		if (count == 3 && i > 0)
		{
			res.push_back(',');
			count = 0;
		}
	}

	std::reverse(res.begin(), res.end());

	return res;
}

// Code that's executed on every thread
static void ThreadProc(int id)
{
	std::uniform_int_distribution<int> dice_distribution(1, 4);
	std::mt19937 random_number_engine; // pseudorandom number generator
	random_number_engine.seed(time(NULL) + id); // ensures all threads have a different seed
	auto dice_roller = std::bind(dice_distribution, random_number_engine);

	int myMaxNumOfOnes = 0; // max times we got a 1 out of all sessions

	const auto start = std::chrono::steady_clock::now(); // START TIMESTAMP

	for (int session = 0; session < NUM_OF_SESSIONS; ++session)
	{
		int numOfOnes = 0;
		for (int turn = 0; turn < NUM_OF_TURNS; ++turn)
		{
			if (dice_roller() == 1)
			{
				++numOfOnes;
			}
		}
		if (numOfOnes > myMaxNumOfOnes)
		{
			myMaxNumOfOnes = numOfOnes;
		}
	}
	const auto end = std::chrono::steady_clock::now(); // END TIMESTAMP
	const auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);

	std::stringstream msg;
	msg << "Thread "<< id <<" took:" << elapsed.count() << "s, max ones: " << myMaxNumOfOnes << std::endl;
	std::cout << msg.str();

	const std::lock_guard<std::mutex> lock(g_MaxOnesMutex);
	if (myMaxNumOfOnes > g_MaxOnes)
	{
		g_MaxOnes = myMaxNumOfOnes;
	}
}

void RunSimulation()
{
	int threadID = 0;
	std::array<std::thread, NUM_OF_THREADS> threads;

	std::cout << "\n===================   SIMULATION PARAMETERS   ===================" << "\n\n";

	std::cout << "Needed number of paralysis procs: " << TARGET_NUMBER << std::endl;

	std::cout << "Number of threads: " << NUM_OF_THREADS << std::endl;
	std::cout << "Number of sessions per thread: " << ThousandSeparator(NUM_OF_SESSIONS) << std::endl;
	std::cout << "Total number of sessions: " << ThousandSeparator(NUM_OF_THREADS * NUM_OF_SESSIONS) << std::endl;

	std::cout << "\nPress ENTER to start the simulation" << std::endl;

	std::cin.get();

	std::cout << "\n===================   THREAD RESULTS   ===================" << "\n\n";

	const auto start = std::chrono::steady_clock::now(); // START TIMESTAMP

	// Start all threads
	for (int threadID = 0; threadID < NUM_OF_THREADS; ++threadID)
	{
		threads[threadID] = std::thread([threadID] { ThreadProc(threadID); });
	}

	// Wait for all threads to finish
	for (int threadID = 0; threadID < NUM_OF_THREADS; ++threadID)
	{
		threads[threadID].join();
	}

	const auto end = std::chrono::steady_clock::now(); // END TIMESTAMP
	const auto elapsed = end - start;

	std::cout << "\n===================   SIMULATION RESULTS   ===================" << "\n\n";

	std::cout << "Simulation took: " << std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() << "s ";
	std::cout << " (" << std::chrono::duration_cast<std::chrono::minutes>(elapsed).count() << "min)" << std::endl;
	std::cout << "\nPARALYSIS PROCS: " << g_MaxOnes << " ----- ";

	if (g_MaxOnes >= TARGET_NUMBER)
	{
		std::cout << "AWESOME! GRAVELER SURVIVED! :D" << std::endl;
	}
	else
	{
		std::cout << "SADLY GRAVELER COULDN'T WHITSTAND THE ATTACKS! :(" << std::endl;
	}
}

void main()
{
	RunSimulation();
}
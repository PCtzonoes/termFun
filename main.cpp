#include <chrono>
#include <fstream>
#include <iostream>
#include <ncurses.h>
#include <nlohmann/json.hpp>
#include <thread>

constexpr int targetFPS = 1;
constexpr std::chrono::nanoseconds targetFrameTime(std::chrono::seconds(1) / targetFPS);


struct Frame {
	std::vector<std::string> lines;
	double speedMultiplier{};
};

std::vector<Frame> frames;

//load frames from json in art folder
//load frames from json in specified folder
void loadFrames(const std::string &directory) {

	for (const auto &file: std::filesystem::directory_iterator(directory)) {
		if (file.path().extension() != ".json")
			continue;

		std::ifstream framesFile(file.path());
		nlohmann::json basicJson;
		framesFile >> basicJson;
		for (auto &frame: basicJson) {
			Frame newFrame;
			newFrame.lines = frame["data"].get<std::vector<std::string>>();
			newFrame.speedMultiplier = frame["speed"];
			frames.push_back(newFrame);
		}
	}
}
size_t currentFrameIndex = 0;
std::chrono::steady_clock::time_point nextFrameChangeTime;

void gameLogic() {
	// Update frame index based on time
	auto now = std::chrono::steady_clock::now();
	if (now >= nextFrameChangeTime) {
		currentFrameIndex = (currentFrameIndex + 1) % frames.size();
		nextFrameChangeTime = now + std::chrono::duration_cast<std::chrono::nanoseconds>(
		                              targetFrameTime * frames[currentFrameIndex].speedMultiplier);
	}
}

void drawGame() {
	// Clear the screen before drawing
	clear();

	// Draw the current frame
	int y = 0;
	for (const auto &line: frames[currentFrameIndex].lines) {
		mvaddstr(y++, 0, line.c_str());
	}

	// Redraw screen
	refresh();

	// Sleep after refreshing the screen
	std::this_thread::sleep_for(targetFrameTime);
}

nlohmann::json get_test_json();

int main() {
	// Setup ncurses environment
	initscr();            // Initialize the window
	nodelay(stdscr, true);// Makes getch() non-blocking
	cbreak();             // Line buffering disabled so we can read from terminal on the fly
	noecho();             // Don't echo() while we do getch
	keypad(stdscr, true); //Capture special keystrokes like Backspace, Delete and the four arrow keys by getch()
	curs_set(0);          // Hide the cursor

	nlohmann::json basicJson = get_test_json();
	for (auto &frame: basicJson) {
		Frame newFrame;
		newFrame.lines = frame["data"].get<std::vector<std::string>>();
		newFrame.speedMultiplier = frame["speed"];
		frames.push_back(newFrame);
	}

	if (frames.empty()) {
		std::cout << "No valid art frames found.\n";
		return 1;
	}

	nextFrameChangeTime = std::chrono::steady_clock::now();

	while (true) {
		auto frameStart = std::chrono::steady_clock::now();

		// Process user input
		int ch = getch();
		if (ch != ERR && (ch == 'q' || ch == 27)) {// 'q' or Esc key to quit
			break;
		}

		// Game logic
		gameLogic();

		// Draw the frame
		drawGame();

		auto frameEnd = std::chrono::steady_clock::now();
		auto frameDuration = frameEnd - frameStart;

		if (frameDuration < targetFrameTime)
			std::this_thread::sleep_for(targetFrameTime - frameDuration);
	}

	curs_set(1);// Show the cursor
	endwin();   // Close ncurses environment
	return 0;
}
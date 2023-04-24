#include <SFML/Graphics.hpp>
#include <iostream>
#include <unordered_map>
#include <string>
#include <thread>
#include "board.h"
#include "util.h"
#include "test.h"

using namespace std;
using namespace sf;


int play() {
	//Testing code for profiling
	//Start Test
	//Board* b = new Board();
	//b = b->get_best(white).first;
	//cout << "Finished Test" << endl;
	//return 0;
	//End Test
	
	char c;
	cin >> c;
	color_t color = c == 'w'? white: black;
	bool can_move = color == white;
	thread* move_thread;

	int piece_size = 45;
	int board_size = piece_size * 8;
	RenderWindow window(VideoMode(board_size, board_size), "chess");
	Texture* textures[]{
		load_texture("wp.png"), load_texture("wb.png"), load_texture("wn.png"), load_texture("wr.png"), load_texture("wq.png"), load_texture("wk.png"),
		load_texture("bp.png"), load_texture("bb.png"), load_texture("bn.png"), load_texture("br.png"), load_texture("bq.png"), load_texture("bk.png")
	};
	Board* board = new Board();

	//bot goes first
	if (color == black) {
		move_thread = new thread(
			[&]() {
				board = board->get_best(color == white ? black : white, Keyboard::isKeyPressed(Keyboard::LShift)).first;
				can_move = true;
				printf("has castled: %d, King moved: %d, rr moved: %d, lr moved: %d\n", board->wcasle, board->wkmove, board->wrrmove, board->wlrmove);
				double w = board->evaluate(white, true);
				cout << "-------------------" << endl;
				double b = board->evaluate(black, true);
				printf("White value: %f, Black value: %f\n", w, b);
				if (board->checkmate(white)) {
					cout << "white checkmate" << endl;
				}
				if (board->checkmate(black)) {
					cout << "black checkmate" << endl;
				} if (board->stalemate()) {
					cout << "stalemate" << endl;
				}
				cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
			}
		);
		move_thread->join();
	}

	RectangleShape square(Vector2f(piece_size, piece_size));
	square.setFillColor(Color::Green);
	CircleShape selection_circle;
	selection_circle.setRadius(piece_size / 2);
	selection_circle.setFillColor(Color::Transparent);
	selection_circle.setOutlineColor(Color::Red);
	selection_circle.setOutlineThickness(2);

	bool has_selection = false;
	uint64_t selected = 0;


	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
			}

			if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::Z && board->previous->previous) {
					Board* prev = board->previous;
					Board* prevprev = prev->previous;
					delete board;
					delete prev;
					board = prevprev;
				}
				if (event.key.code == Keyboard::R) {
					delete board;
					board = new Board();
				}
			}

			if (event.type == Event::MouseButtonPressed) {
				Vector2i position = Mouse::getPosition(window);
				int x = (board_size - position.x) / piece_size;
				int y = (board_size - position.y) / piece_size;
				if (color == black) {
					y = 7 - y;
				}
				int pos = x + y * 8;
				uint64_t click_pos = 1ULL << pos;
				uint64_t selection = 1ULL << selected;
				uint64_t mask = 0ULL;
				color == white ? board->get_white(mask) : board->get_black(mask);

				if (has_selection && can_move) {
					uint64_t moves = 0;
					board->get_moves(selected, moves);
					if (moves & click_pos) {
						Board* next = new Board(board);
						next->move(selection, click_pos); //selection
						if (!next->check(color)) {
							board = next;
							can_move = false;
							selected = pos;
							has_selection = false;
							//color = color == white ? black : white;
							move_thread = new thread(
								[&]() {
									board = board->get_best(color == white ? black : white, Keyboard::isKeyPressed(Keyboard::LShift)).first;
									can_move = true;
									printf("has castled: %d, King moved: %d, rr moved: %d, lr moved: %d\n", board->wcasle, board->wkmove, board->wrrmove, board->wlrmove);
									double w = board->evaluate(white, true);
									cout << "-------------------" << endl;
									double b = board->evaluate(black, true);
									printf("White value: %f, Black value: %f\n", w, b);
									if (board->checkmate(white)) {
										cout << "white checkmate" << endl;
									}
									if (board->checkmate(black)) {
										cout << "black checkmate" << endl;
									} if (board->stalemate()) {
										cout << "stalemate" << endl;
									}
									cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
								}
							);
						} else {
							delete next;
						}
						break;
					}
				}
				
				if (mask & click_pos) {
					selected = pos;
					has_selection = mask & selection;
				}
			}
		}

		window.clear(Color::White);

		for (int i = 0; i < 64; i++) {
			int row = i / 8;
			int column = i % 8;
			if (row % 2 == column % 2) {
				square.setPosition(column * piece_size, color == black ? row * piece_size : board_size - (row + 1) * piece_size);
				window.draw(square);
			}
		}

		for (int i = 0; i < 12; i++) {
			Texture texture = *textures[i];
			uint64_t sub = board->board[i];
			Sprite sprite(texture);
			for (int j = 0; j < 64; j++) {
				int row = j / 8;
				int column = j % 8;
				if (sub & 1ULL << j) {
					sprite.setPosition((7-column) * piece_size, color == black? row * piece_size: board_size - (row+1) * piece_size);
					window.draw(sprite);
				}
			}
		}

		if (has_selection) {
			int row = selected / 8;
			int column = selected % 8;
			selection_circle.setPosition((7-column) * 45, color == black ? row * piece_size : board_size - (row + 1) * piece_size);
			window.draw(selection_circle);
		}

		window.display();
	}
	return 0;
}


int engine() {
	char color;
	cin >> color;
	color_t engine_color = color == 'w' ? white : black;

	Board* board = new Board();

	while (true) {
		//Makes the opponent's move
		board = new Board(board);
		for (int i = 0; i < 12; i++) {
			uint64_t value;
			cin >> value;
			board->board[i] = value;
		}


		//Moves and transmits back
		board = board->get_best(engine_color).first;
		uint64_t prev = 0;
		engine_color == white ? board->previous->get_white(prev): board->previous->get_black(prev);
		uint64_t curr = 0;
		engine_color == white ? board->get_white(curr) : board->get_black(curr);

		uint64_t diff = prev ^ curr;
		int start = countr_zero(prev & diff);
		int end = countr_zero(curr & diff);

		cout << to_string(start) << endl;
		cout << to_string(end) << endl;
	}
}


int main(int argc, char* args[]) {
	//return engine();
	return play();
}

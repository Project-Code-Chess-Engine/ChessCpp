#include "board.h"
#include <iostream>
#include "test.h"
#include <unordered_map>
#include <limits>
#include <bit>
#include <algorithm>
#include <cmath>
#include <execution>
#include <exception>

const int EVAL_DEPTH = 4;
const bool debug = true;

Board::Board() {
	uint64_t pawns = 0b11111111ULL << 8;

	uint64_t bishops = 0b00100100;
	uint64_t knights = 0b01000010;
	uint64_t rooks = 0b10000001;
	uint64_t queens = 0b00010000;
	uint64_t kings = 0b00001000;


	board = {
		pawns, bishops, knights, rooks, queens, kings,
		pawns << 40, bishops << 56, knights << 56, rooks << 56, queens << 56, kings << 56
	};

	bcasle = false;
	wcasle = false;

	bkmove = false;
	wkmove = false;

	brrmove = false;
	wrrmove = false;

	blrmove = false;
	wlrmove = false;
}

void Board::get_white(uint64_t& mask) {
	for (int i = 0; i < 6; i++) {
		mask |= board[i];
	}
}

void Board::get_black(uint64_t& mask) {
	for (int i = 6; i < 12; i++) {
		mask |= board[i];
	}
}

void Board::get_all(uint64_t& mask) {
	get_white(mask);
	get_black(mask);
}

void Board::knight_attack(const uint8_t& position, uint64_t& mask) {
	mask = 0;
	uint64_t attack = knight_table[position];
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(w);
	get_black(b);
	uint64_t c = 1ULL << position & w ? w : b;

	mask |= (attack & ~c);
}

void Board::king_attack(const uint8_t& position, uint64_t& mask) {
	//castling needs to be added
	mask = 0;
	uint64_t attack = king_table[position];
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(w);
	get_black(b);
	uint64_t c;
	uint64_t attacks = 0;
	uint64_t a = w | b;

	if (1ULL << position & w) {
		c = w;
		attacked_squares(black, attacks);
		if (!wkmove && !check(white)) {
			if (!wlrmove && !(0b01110000 & (a | attacks))) {
				mask |= 0b10000000;
			}
			if (!wrrmove && !(0b00000110 & (a | attacks))) {
				mask |= 0b00000001;
			}
		}
	}
	else {
		c = b;
		attacked_squares(white, attacks);
		if (!bkmove && !check(black)) {
			if (!blrmove && !(8070450532247928832 & (a | attacks))) {
				mask |= 9223372036854775808;
			}
			if (!brrmove && !(432345564227567616 & (a | attacks))) {
				mask |= 72057594037927936;
			}
		}
	}

	mask |= (attack & ~c);
}

void Board::pawn_attack(const uint8_t& position, uint64_t& mask) {

}

void Board::bishop_attack(const uint8_t& position, uint64_t& mask) {
	mask = 0;
	uint64_t _pos = 1ULL << position;
	uint64_t pos = _pos;
	bool hit = false;
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(w);
	get_black(b);
	uint64_t a = w | b;
	uint64_t o = pos & w ? b : w;

	uint8_t y = position >> 3;
	uint8_t x = position % 8;

	int y_ = y;
	int x_ = x;

	for (int i = 0; i < 7; i++) {
		y_++;
		x_--;
		if (y_ > 7 || x_ < 0) {
			break;
		}
		pos <<= 7;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}

	pos = _pos;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_++;
		x_++;
		if (y_ > 7 || x_ > 7) {
			break;
		}
		pos <<= 9;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}

	pos = _pos;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_--;
		x_--;
		if (y_ < 0 || x_ < 0) {
			break;
		}
		pos >>= 9;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}

	pos = _pos;
	hit = false;
	y_ = y;
	x_ = x;

	for (int i = 0; i < 7; i++) {
		y_--;
		x_++;
		if (y_ < 0 || x_ > 7) {
			break;
		}
		pos >>= 7;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}

	//There must be an easier way to do this
}

void Board::rank_attack(const uint8_t& position, uint64_t& mask) {
	uint8_t line = 0b11111111;
	uint64_t pos = 1ULL << position;

	uint64_t w = 0;
	uint64_t b = 0;

	uint8_t y = position >> 3 << 3;
	uint8_t x = position % 8;
	uint8_t pos_line = pos >> y;

	get_white(w);
	get_black(b);
	uint64_t a = w | b;
	uint8_t o = (pos & w ? b : w) >> y;

	uint8_t rank = line ^ a >> y;
	uint8_t right = rank & (uint8_t)(line >> 8 - x);
	uint8_t left = rank & (uint8_t)(line << x + 1);
	uint8_t rfirst = std::countl_one((uint8_t)(right << 8 - x));
	uint8_t lfirst = std::countr_one((uint8_t)(left >> x + 1));

	left &= line >> (7 - x - lfirst);
	left |= (pos_line << lfirst + 1) & o;
	right &= line << (x - rfirst);
	right |= (pos_line >> rfirst + 1) & o;
	mask = (left | right);
	mask <<= y;
}

void Board::file_attack(const uint8_t& position, uint64_t& mask) {
	mask = 0;
	uint64_t _pos = 1ULL << position;
	uint64_t pos = _pos;
	bool hit = false;
	uint64_t w = 0;
	uint64_t b = 0;

	get_white(w);
	get_black(b);
	uint64_t a = w | b;
	uint64_t o = pos & w ? b : w;

	uint8_t y = position >> 3;

	int y_ = y;

	//up
	for (int i = 0; i < 7; i++) {
		y_++;
		if (y_ > 7) {
			break;
		}
		pos <<= 8;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}

	pos = _pos;
	hit = false;
	y_ = y;

	//down
	for (int i = 0; i < 7; i++) {
		y_--;
		if (y_ < 0) {
			break;
		}
		pos >>= 8;
		if (o & pos) {
			hit = true;
		}
		else if (a & pos) {
			break;
		}
		mask |= pos;
		if (hit) {
			break;
		}
	}
}

void Board::rook_attack(const uint8_t& position, uint64_t& mask) {
	uint64_t r = 0;
	uint64_t f = 0;
	rank_attack(position, r);
	file_attack(position, f);
	mask = r | f;
}

void Board::get_moves(const uint8_t& position, uint64_t& mask) {
	uint64_t pos = 1ULL << position;
	for (int i = 0; i < 6; i++) {
		uint64_t wboard = board[i];
		uint64_t bboard = board[i + black];
		uint64_t r = 0;
		uint64_t b = 0;
		if (wboard & pos || bboard & pos) {
			switch (i) {
			case pawns:
				pawn_attack(position, mask);
				break;
			case bishops:
				bishop_attack(position, mask);
				return;
			case knights:
				knight_attack(position, mask);
				break;
			case rooks:
				rook_attack(position, mask);
				return;
			case queens:
				rook_attack(position, r);
				bishop_attack(position, b);
				mask = r | b;
				return;
			case kings:
				king_attack(position, mask);
				break;
			default:
				throw std::runtime_error("Invalid Piece");
				break;
			}
		}
	}
}

void Board::get_attackers(const uint8_t& position, uint64_t& mask) {
	mask = 0;
	uint64_t b = 0;
	uint64_t w = 0;
	get_black(b);
	get_white(w);
	uint64_t pos = 1ULL << position;
	uint64_t o = pos & w ? b: w;

	int leading;
	while (o) {
		leading = std::countr_zero(o);
		uint64_t moves = 0;
		get_attacks(leading, moves);
		if (moves & pos) {
			mask |= 1ULL << leading;
		}
		o -= 1ULL << leading;
	}
}

void Board::get_attacks(const uint8_t& position, uint64_t& mask) {
	uint64_t pos = 1ULL << position;
	if ((pos & board[kings]) || (pos & board[black + kings])) {
		mask |= king_table[position];
	}
	else if (pos & board[pawns]) {
		mask |= wpc_table[position];
	}
	else if (pos & board[black + pawns]) {
		mask |= bpc_table[position];
	}
	else {
		get_moves(position, mask);
	}
}

void Board::attacked_squares(const color_t& color, uint64_t& mask) {
	uint64_t a = 0;
	color == white ? get_white(a) : get_black(a);
	int leading;
	while (a) {
		leading = std::countr_zero(a);
		uint64_t moves = 0;
		get_attacks(leading, moves);
		mask |= moves;
		a -= 1ULL << leading;
	}
}

bool Board::check(const color_t& color) {
	uint64_t king_pos = color == white ? std::countr_zero(board[kings]) : std::countr_zero(board[black + kings]);
	uint64_t attackers = 0;
	get_attackers(king_pos, attackers);
	return attackers;
}

bool Board::checkmate(const color_t& color) {
	std::vector<Board*> moves;
	bool r = false;
	if (check(color)) {
		moves = get_moves(color);
		for (Board* move : moves) {
			bool check = move->check(color);
			if (!check) {
				r = false;
				goto e;
			}
		}
		r = true;
	}
e:;
	for (Board* move : moves) {
		delete move;
	}
	return r;
}

bool Board::stalemate(const color_t& color) {
	return false;
}

bool Board::stalemate() {
	return false;
}

void Board::undo(const Move& move) {
    color_t color = turn;


	if (move.captured_piece != none){
        // we need to reassign the move
        if (!move.enpassant){
            board[move.captured_piece] |= 1ULL << move.end_position;
        }
        else{
            if (color == white){ // white
                board[move.captured_piece] |= (1ULL << move.end_position) >> 8;
            }
            else{ // black
                board[move.captured_piece] |= (1ULL << move.end_position) << 8;
            }
        }
    }
    // castling
    if (move.castled != castled_t::none){
        if (move.castled == castled_t::left){
            if (turn == white){
                board[piece_t::rooks + turn] &= ~10ULL;
                board[piece_t::rooks + turn] |= 80ULL;
                board[piece_t::kings + turn] = 80ULL;
            }
            else{
                board[piece_t::rooks + turn] &= ~10ULL;
                board[piece_t::kings + turn] = 80ULL;
            }
        }
    }
    
    // move back the piece
    board[move.moved_piece] |= 1ULL << move.start_position;
    
    
    // Change the player's turn
    if (turn == white) {
		turn = black;
	} else {
		turn = white;
	}
}

void Board::move(const Move& move) {
	
	// Check if the player is castling
	if (move.castled == none) {
		// Player is not castling
		
		// Remove the moved peice from the board
		board[move.moved_piece] &= ~(1ULL << move.start_position);

		// Remove the captured piece
		board[move.captured_piece] &= ~(1ULL << move.end_position);

		// Check if a propotion occurs
		if (not move.promotion) {
			// No promotion
			// Add the moved piece back onto the board
			board[move.moved_piece] |= 1ULL << move.end_position;
		} else {
			// Promotion
			// Add a queen onto the board
			board[turn + queens] |= 1ULL << move.end_position; 
		}
	} else {
		// Player is castling

		// Check the color of the king
		if (move.moved_piece == kings) {
			// White king

			// Check castling direction
			if (move.castled == castled_t::left) {
				//Left castle

				// Reset Pieces
				board[kings] = 0b00001000;
				board[rooks] |= 0b10000000;
				board[rooks] &= ~0b00010000;
			}
			else {
				// Right castle

				// Reset the pieces
				board[kings] = 0b00001000ULL << 56;
				board[rooks] |= 0b10000000;
				board[rooks] &= ~0b00000100;
			}

			// No longer castled
			wcasle = false;
		}
		else {
			// Black king

			// Check castling direction
			if (move.castled == castled_t::left) {
				// Castling left

				// Reset the pieces
				board[black + kings] &= ~2305843009213693952;
				board[black + rooks] |= 9223372036854775808;
				board[black + rooks] &= ~1152921504606846976;
			}
			else {
				// Castling right

				// Reset the pieces
				board[black + kings] &= ~144115188075855872;
				board[black + rooks] |= 9223372036854775808;
				board[black + rooks] &= ~288230376151711744;
			}

			// No longer castled
			bcasle = false;
		}
	}
	
	if (turn == white) {
		turn = black;
	} else {
		turn = white;
	}
}

std::vector<Board*> Board::get_moves(const color_t& color) {
	return {};
}

double Board::evaluate(color_t color) {
	int min;
	int max;
	int value = 0;
	Board* d_board = new Board();
	uint64_t center_pawns = 103481868288;
	uint64_t vision = 0;
	bool castled;
	bool cc;
	int target;
	//double trade = 0; //add the calculation of trades
	attacked_squares(color, vision);

	uint64_t d_pieces = 0;
	uint64_t pieces = 0;
	uint64_t p;
	uint64_t op_atk = 0;
	uint64_t a = 0;
	get_all(a);

	if (color == white) {
		get_white(pieces);
		attacked_squares(black, op_atk);
		d_board->get_white(d_pieces);
		p = board[pawns];
		center_pawns &= p;
		min = 0;
		max = 6;
		castled = wcasle;
		cc = !wkmove && (!wrrmove || !wlrmove);
		target = 7;
	}
	else {
		get_black(pieces);
		attacked_squares(white, op_atk);
		d_board->get_black(d_pieces);
		p = board[black + pawns];
		center_pawns &= p;
		min = 6;
		max = 12;
		castled = bcasle;
		cc = !bkmove && (!brrmove || !blrmove);
		target = 0;
	}

	double development = 0;
	for (int i = min; i < max; i++) {
		uint64_t sub = board[i];
		uint8_t val = value_table[i];
		uint16_t d = 0;
		uint64_t developed = (d_board->board[i] ^ sub) & ~d_board->board[i];
		d += std::popcount(developed & 35604928818740736) * 0.1;
		d += std::popcount(developed & 66229406269440) * 0.75;
		development -= std::popcount(op_atk & sub) * std::pow(val, 0.25);
		development += d / abs(4 * (val - 3.5));
		value += val * std::popcount(sub);
	}
	delete d_board;

	int leading;
	while (p) {
		leading = std::countr_zero(p);
		//check for passed pawns and give points based on travel distance
		//use an array to hash the lanes
		int x = leading % 8;
		uint64_t column = 72340172838076673 << x;
		int distance = abs(target - (leading >> 3));

		if (!(column & (op_atk | (a & ~(1ULL << leading))))) {
			development += 8.0 / distance;
		}
		else {
			development += 3 / distance;
		}

		p -= 1ULL << leading;
	}

	return value + development + std::popcount(center_pawns) * 0.5 + std::popcount(vision) * 0.05 + (cc ? 0 : (castled ? 1 : -1));
}

std::pair<Board*, double> Board::get_best(const color_t& color, const bool& show) {
	double alpha = std::numeric_limits<double>::min();
	double beta = std::numeric_limits<double>::max();

	color_t ocolor = color == white ? black : white;
	std::pair<Board*, double> eval;
	eval.second = std::numeric_limits<double>::min();

	std::vector<Board*> moves = get_moves(color);
	std::unordered_map<Board*, double> results;

	std::for_each(std::execution::par, moves.begin(), moves.end(), [&](Board* move) {
		results[move] = reval(move, color, ocolor, 1, alpha, beta);
	});


	for (const std::pair<Board*, double> &item : results) {
		if (debug && show) {
			std::cout << "result: " << item.second;
			render_board(item.first, item.second);
		}
		if (item.second > eval.second) {
			eval.first = item.first;
			eval.second = item.second;
		}
	}
		
	for (Board* move : moves) {
		if (move != eval.first) {
			delete move;
		}
	}
	return eval;
}

double reval(Board* board, const color_t& og_color, const color_t& curr_color, const int& depth, double alpha, double beta) {
	color_t opog_color = og_color == white ? black : white;
	color_t op_color = curr_color == white ? black : white;
	bool is_color = og_color == curr_color;

	double eval = is_color ? std::numeric_limits<double>::min() : std::numeric_limits<double>::max();
	std::vector<Board*> moves = board->get_moves(curr_color);

	bool check = board->check(curr_color);

	if (board->stalemate() || (!check && !moves.size())) {
		return 1;
	}

	if (!is_color && !moves.size() && check) {
		return std::numeric_limits<double>::max();
	}
	if (is_color && !moves.size() && check) {
		return std::numeric_limits<double>::min();
	}

	if (depth >= EVAL_DEPTH) {
		eval = board->evaluate(og_color) / board->evaluate(opog_color);
		goto e;
	}

	for (Board* move : moves) {
		double result = reval(move, og_color, op_color, depth + 1, alpha, beta);

		if (is_color) {
			if (result > eval) {
				eval = result;
			}
			if (eval > beta) {
				goto e;
			}
			alpha = std::max(eval, alpha);
		}
		else {
			if (result < eval) {
				eval = result;
			}
			if (eval < alpha) {
				goto e;
			}
			beta = std::min(eval, beta);
		}
	}

e:;
	for (Board* move : moves) {
		delete move;
	}
	return eval;
}


// greatness starts //
// Board::makeMove()
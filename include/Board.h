#pragma once
#include <stdint.h>
#include <utility>
#include <vector>
#include <array>

enum piece_t { pawns, bishops, knights, rooks, queens, kings, enpassant=12, empty };
enum castled_t { none, left, right };

const uint8_t value_table[] = {
	1, 3, 3, 5, 9, 0,
	1, 3, 3, 5, 9, 0
};

enum color_t { white = 0, black = 6 };

const uint64_t knight_table[] = {
	132096,
	329728,
	659712,
	1319424,
	2638848,
	5277696,
	10489856,
	4202496,
	33816580,
	84410376,
	168886289,
	337772578,
	675545156,
	1351090312,
	2685403152,
	1075839008,
	8657044482,
	21609056261,
	43234889994,
	86469779988,
	172939559976,
	345879119952,
	687463207072,
	275414786112,
	2216203387392,
	5531918402816,
	11068131838464,
	22136263676928,
	44272527353856,
	88545054707712,
	175990581010432,
	70506185244672,
	567348067172352,
	1416171111120896,
	2833441750646784,
	5666883501293568,
	11333767002587136,
	22667534005174272,
	45053588738670592,
	18049583422636032,
	145241105196122112,
	362539804446949376,
	725361088165576704,
	1450722176331153408,
	2901444352662306816,
	5802888705324613632,
	11533718717099671552,
	4620693356194824192,
	288234782788157440,
	576469569871282176,
	1224997833292120064,
	2449995666584240128,
	4899991333168480256,
	9799982666336960512,
	1152939783987658752,
	2305878468463689728,
	1128098930098176,
	2257297371824128,
	4796069720358912,
	9592139440717824,
	19184278881435648,
	38368557762871296,
	4679521487814656,
	9077567998918656
};

const uint64_t king_table[] = {
	770,
	1797,
	3594,
	7188,
	14376,
	28752,
	57504,
	49216,
	197123,
	460039,
	920078,
	1840156,
	3680312,
	7360624,
	14721248,
	12599488,
	50463488,
	117769984,
	235539968,
	471079936,
	942159872,
	1884319744,
	3768639488,
	3225468928,
	12918652928,
	30149115904,
	60298231808,
	120596463616,
	241192927232,
	482385854464,
	964771708928,
	825720045568,
	3307175149568,
	7718173671424,
	15436347342848,
	30872694685696,
	61745389371392,
	123490778742784,
	246981557485568,
	211384331665408,
	846636838289408,
	1975852459884544,
	3951704919769088,
	7903409839538176,
	15806819679076352,
	31613639358152704,
	63227278716305408,
	54114388906344448,
	216739030602088448,
	505818229730443264,
	1011636459460886528,
	2023272918921773056,
	4046545837843546112,
	8093091675687092224,
	16186183351374184448,
	13853283560024178688,
	144959613005987840,
	362258295026614272,
	724516590053228544,
	1449033180106457088,
	2898066360212914176,
	5796132720425828352,
	11592265440851656704,
	4665729213955833856
};

//White pawn captures
const uint64_t wpc_table[] = {
	512,
	1280,
	2560,
	5120,
	10240,
	20480,
	40960,
	16384,
	131072,
	327680,
	655360,
	1310720,
	2621440,
	5242880,
	10485760,
	4194304,
	33554432,
	83886080,
	167772160,
	335544320,
	671088640,
	1342177280,
	2684354560,
	1073741824,
	8589934592,
	21474836480,
	42949672960,
	85899345920,
	171798691840,
	343597383680,
	687194767360,
	274877906944,
	2199023255552,
	5497558138880,
	10995116277760,
	21990232555520,
	43980465111040,
	87960930222080,
	175921860444160,
	70368744177664,
	562949953421312,
	1407374883553280,
	2814749767106560,
	5629499534213120,
	11258999068426240,
	22517998136852480,
	45035996273704960,
	18014398509481984,
	144115188075855872,
	360287970189639680,
	720575940379279360,
	1441151880758558720,
	2882303761517117440,
	5764607523034234880,
	11529215046068469760,
	4611686018427387904,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

//Black pawn captures
const uint64_t bpc_table[] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	2,
	5,
	10,
	20,
	40,
	80,
	160,
	64,
	512,
	1280,
	2560,
	5120,
	10240,
	20480,
	40960,
	16384,
	131072,
	327680,
	655360,
	1310720,
	2621440,
	5242880,
	10485760,
	4194304,
	33554432,
	83886080,
	167772160,
	335544320,
	671088640,
	1342177280,
	2684354560,
	1073741824,
	8589934592,
	21474836480,
	42949672960,
	85899345920,
	171798691840,
	343597383680,
	687194767360,
	274877906944,
	2199023255552,
	5497558138880,
	10995116277760,
	21990232555520,
	43980465111040,
	87960930222080,
	175921860444160,
	70368744177664,
	562949953421312,
	1407374883553280,
	2814749767106560,
	5629499534213120,
	11258999068426240,
	22517998136852480,
	45035996273704960,
	18014398509481984
};


struct Move{
    uint8_t start_position;
    uint8_t end_position;
    bool check;
    castled_t castled;
    bool promotion;
    bool enpassant;
    piece_t captured_piece;
    piece_t moved_piece;
};

class Board {
public:
	bool bcasle; // black castle
	bool wcasle; // white castle

	bool bkmove; // black king move
	bool wkmove; // white king move

	bool brrmove; // black right rook moved
	bool wrrmove; // white right rook moved

	bool blrmove; // black left rook moved
	bool wlrmove; // white left rook moved

	std::array<uint64_t, 13> board;
	
	color_t turn = color_t::white;

	Board();

	void get_white(uint64_t& mask);
	void get_black(uint64_t& mask);
	void get_all(uint64_t& mask);

	void knight_attack(const uint8_t& position, uint64_t& mask);
	void king_attack(const uint8_t& position, uint64_t& mask);
	void pawn_attack(const uint8_t& position, uint64_t& mask);
	void bishop_attack(const uint8_t& position, uint64_t& mask);
	void rank_attack(const uint8_t& position, uint64_t& mask);
	void file_attack(const uint8_t& position, uint64_t& mask);
	void rook_attack(const uint8_t& position, uint64_t& mask);

	void get_moves(const uint8_t& position, uint64_t& mask);
	void get_attacks(const uint8_t& position, uint64_t& mask);
	void get_attackers(const uint8_t& position, uint64_t& mask);
	void attacked_squares(const color_t& color, uint64_t& mask);
	bool check(const color_t& color);
	bool checkmate(const color_t& color);
	bool stalemate(const color_t& color);
	bool stalemate();

	std::vector<Board*> get_moves(const color_t& color);

	void move(const Move& move);
	void undo(const Move& move);

	double evaluate(color_t color);
	std::pair<Board*, double> get_best(const color_t& color, const bool& show=false);
};

double reval(Board* board, const color_t& og_color, const color_t& curr_color, const int& depth, double alpha, double beta);
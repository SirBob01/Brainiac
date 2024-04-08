#include "State.hpp"

namespace Brainiac {
    State::State() {}

    State::State(std::string fen, Hasher &hasher) {
        std::vector<std::string> fields = tokenize(fen, ' ');
        int row = 7;
        int col = 0;
        for (auto &c : fields[0]) {
            if (c == '/') {
                row--;
                col = 0;
            } else if (c <= '9' && c >= '0') {
                col += c - '0';
            } else {
                int char_idx = 0;
                while (PIECE_CHARS[char_idx] != c) {
                    char_idx++;
                }
                board.set(Square(row * 8 + col),
                          Piece((char_idx / 6) * 6 + (char_idx % 6)));
                col++;
            }
        }

        castling = 0;
        for (auto &c : fields[2]) {
            if (c == 'K') castling |= (1 << CastlingRight::WK);
            else if (c == 'Q') castling |= (1 << CastlingRight::WQ);
            else if (c == 'k') castling |= (1 << CastlingRight::BK);
            else if (c == 'q') castling |= (1 << CastlingRight::BQ);
        }

        ep_dst = fields[3].length() == 2 ? string_to_square(fields[3])
                                         : Square::Null;
        turn = (fields[1][0] == 'w') ? Color::White : Color::Black;
        halfmoves = stoi(fields[4]);
        fullmoves = stoi(fields[5]);

        hash = hasher(board, castling, turn, ep_dst);
        generate_moves();
    }

    std::string State::fen() const {
        std::string fen = "";
        for (int row = 7; row >= 0; row--) {
            int counter = 0;
            for (int col = 0; col < 8; col++) {
                Piece piece = board.get(Square(row * 8 + col));
                if (piece != Piece::Empty) {
                    if (counter) {
                        fen += counter + '0';
                        counter = 0;
                    }
                    fen += PIECE_CHARS[piece];
                } else {
                    counter += 1;
                }
            }
            if (counter) fen += counter + '0';
            if (row) fen += '/';
        }
        fen += " ";
        fen += turn == Color::White ? "w" : "b";

        std::string castling_rights = "";

        if (castling & (1 << CastlingRight::WK)) castling_rights += 'K';
        if (castling & (1 << CastlingRight::WQ)) castling_rights += 'Q';
        if (castling & (1 << CastlingRight::BK)) castling_rights += 'k';
        if (castling & (1 << CastlingRight::BQ)) castling_rights += 'q';
        if (castling_rights.length() == 0) castling_rights = "-";
        fen += " " + castling_rights;

        if (ep_dst == Square::Null) {
            fen += " -";
        } else {
            fen += " ";
            fen += square_to_string(ep_dst);
        }

        fen += " ";
        fen += std::to_string(halfmoves);
        fen += " ";
        fen += std::to_string(fullmoves);
        return fen;
    }

    void State::print() const {
        if (turn == Color::White) std::cout << "White's turn.\n";
        else std::cout << "Black's turn.\n";
        board.print();
    }

    void State::generate_moves() {
        // Color op = static_cast<Color>(!turn);

        // Piece f_king = create_piece(PieceType::King, turn);
        // Piece f_pawn = create_piece(PieceType::Pawn, turn);
        // Piece f_rook = create_piece(PieceType::Rook, turn);
        // Piece f_knight = create_piece(PieceType::Knight, turn);
        // Piece f_bishop = create_piece(PieceType::Bishop, turn);
        // Piece f_queen = create_piece(PieceType::Queen, turn);

        // Piece o_king = create_piece(PieceType::King, op);
        // Piece o_pawn = create_piece(PieceType::Pawn, op);
        // Piece o_rook = create_piece(PieceType::Rook, op);
        // Piece o_knight = create_piece(PieceType::Knight, op);
        // Piece o_bishop = create_piece(PieceType::Bishop, op);
        // Piece o_queen = create_piece(PieceType::Queen, op);

        // MoveGen generator;
        // generator.friends = board.bitboard(turn);
        // generator.enemies = board.bitboard(op);
        // generator.all = generator.friends | generator.enemies;

        // generator.f_king = board.bitboard(f_king);
        // generator.f_pawn = board.bitboard(f_pawn);
        // generator.f_rook = board.bitboard(f_rook);
        // generator.f_knight = board.bitboard(f_knight);
        // generator.f_bishop = board.bitboard(f_bishop);
        // generator.f_queen = board.bitboard(f_queen);

        // generator.o_king = board.bitboard(o_king);
        // generator.o_pawn = board.bitboard(o_pawn);
        // generator.o_rook = board.bitboard(o_rook);
        // generator.o_knight = board.bitboard(o_knight);
        // generator.o_bishop = board.bitboard(o_bishop);
        // generator.o_queen = board.bitboard(o_queen);

        // generator.ep = 1ULL << ep_dst;
        // generator.turn = turn;

        // moves.clear();
        // check = generator.generate(moves);
    }
} // namespace Brainiac
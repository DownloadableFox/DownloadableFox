#include <bits/stdc++.h>
#include <cstdlib>
using namespace std;

#include <iostream>

namespace ConsoleColors {
    enum Code {
        FG_YELLOW = 93,
        FG_WIERD_WHITE = 37,
        FG_BLACK    = 30,
        FG_WHITE    = 97,
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_DEFAULT  = 39,
        FG_GRAY     = 90,
        BG_BLACK    = 40,
        BG_WHITE    = 107,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49,
        BG_GRAY     = 100
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}


enum Color {
    White,
    Black,
    None
};

enum Type {
    Pawn,
    Queen,
    King,
    Knight,
    Rook,
    Bishop,
    Empty
};

enum State {
    AI_Wins,
    Human_Wins,
    Tie,
    Running
};

class Piece {
    public:
        Type PieceType;
        Color PieceColor;
        Piece() {
            PieceType = Type::Empty;
            PieceColor = Color::None;
        }
};

class PiecePos {
    public:
        Type PieceType;
        Color PieceColor;
        std::pair <int, int> Position;
};

class Game {
    public:
        // Properties
        vector < vector<Piece> > Board;
        Color PlayerTurn;
        State GameState;

        // Methods
        void init();
        void Move();
        void ResetBoard();
        void PrintBoard();
        void SetGameStatus();
        bool Check();
        bool CheckMate();
        bool ValidCordinate(pair <int, int> point);
        int ScoreBasedOnEatenPieces(bool maximizing);
        int PieceValue(Type Piece);
        int Minimax(int Depth, bool maximizing, int alpha = -INT_MAX, int beta = INT_MAX);
        string boardToFEN();
        char LetterForPrinting(Type PieceForPrinting);
        pair <int, int> GetKingPos(Color KingColor);
        vector < pair < int, int > > Move(pair<int, int> position);
        /* Constructor */
        Game(){
            PlayerTurn = Color::Black; /* Player is black, Bot is white */
            GameState = State::Running;
            init();
        }
};

string Game::boardToFEN(){
    string FEN_Notation = "";
    for(int y = 0; y < 8; y++){
        for(int x = 0; x < 8; x++){
            if(this->Board[y][x].PieceColor == Color::None){
                int cou = 0;
                for(int i = x; i < 8; i++){
                    if(this->Board[y][x].PieceColor == Color::None){
                        cou++;
                    } else {
                        if(cou == 1){FEN_Notation += '1';}
                        if(cou == 2){FEN_Notation += '2';}
                        if(cou == 3){FEN_Notation += '3';}
                        if(cou == 4){FEN_Notation += '4';}
                        if(cou == 5){FEN_Notation += '5';}
                        if(cou == 6){FEN_Notation += '6';}
                        if(cou == 7){FEN_Notation += '7';}
                        if(cou == 8){FEN_Notation += '8';}
                        x = i-1;
                        break;
                    }
                }
            } else if(this->Board[y][x].PieceColor == Color::White){
                if(this->Board[y][x].PieceType == Type::Pawn){
                    FEN_Notation += 'P';
                } else if(this->Board[y][x].PieceType == Type::Queen){
                    FEN_Notation += 'Q';
                } else if(this->Board[y][x].PieceType == Type::King){
                    FEN_Notation += 'K';
                } else if(this->Board[y][x].PieceType == Type::Knight){
                    FEN_Notation += 'N';
                } else if(this->Board[y][x].PieceType == Type::Rook){
                    FEN_Notation += 'R';
                } else if(this->Board[y][x].PieceType == Type::Bishop){
                    FEN_Notation += 'B';
                }
            } else if(this->Board[y][x].PieceColor == Color::Black){
                if(this->Board[y][x].PieceType == Type::Pawn){
                    FEN_Notation += 'p';
                } else if(this->Board[y][x].PieceType == Type::Queen){
                    FEN_Notation += 'q';
                } else if(this->Board[y][x].PieceType == Type::King){
                    FEN_Notation += 'k';
                } else if(this->Board[y][x].PieceType == Type::Knight){
                    FEN_Notation += 'n';
                } else if(this->Board[y][x].PieceType == Type::Rook){
                    FEN_Notation += 'r';
                } else if(this->Board[y][x].PieceType == Type::Bishop){
                    FEN_Notation += 'b';
                }
            }
        }
        if(y != 8-1){FEN_Notation += '/';}
    }

    return FEN_Notation;
}

pair <int, int> Game::GetKingPos(Color KingColor) {
    for(int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (this->Board[y][x].PieceType == King && this->Board[y][x].PieceColor == KingColor) {
                return *(new pair<int, int> (y, x));
            }
        }
    }
    return *(new pair<int, int> (-1, -1)); // KING DOESN'T EXIST | SHOULD NEVER GET TO HERE
}

bool Game::Check() {
   for(int y = 0; y < 8; y++){
        for (int x = 0; x < 8; x++){
            vector < pair <int, int> > PosibleMoves = Move(*(new pair<int, int> (y, x)));
            for (auto pos: PosibleMoves) {
                if (this->Board[pos.first][pos.second].PieceType == King && this->Board[pos.first][pos.second].PieceColor != this->PlayerTurn) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Game::CheckMate() {
    vector < pair < int, int > > possibleMoves;

    Color KingColor = (this->PlayerTurn == Color::White) ? Color::Black : Color::White;
    pair <int, int> KingPos = this->GetKingPos(KingColor);
    //possibleMoves = this->Move(KingPos);

    possibleMoves.push_back({ KingPos.first + 0, KingPos.second + 1 });
    possibleMoves.push_back({ KingPos.first + 0, KingPos.second - 1 });
    possibleMoves.push_back({ KingPos.first + 1, KingPos.second + 0 });
    possibleMoves.push_back({ KingPos.first + 1, KingPos.second + 1 });
    possibleMoves.push_back({ KingPos.first + 1, KingPos.second - 1 });
    possibleMoves.push_back({ KingPos.first - 1, KingPos.second + 0 });
    possibleMoves.push_back({ KingPos.first - 1, KingPos.second + 1 });
    possibleMoves.push_back({ KingPos.first - 1, KingPos.second - 1 });
    for(int i = 0; i < possibleMoves.size(); i++){ /*eliminate moves that are not possible*/
        if( ! ( this->ValidCordinate( { possibleMoves[i].first, possibleMoves[i].second } ) ) ){
            possibleMoves.erase( possibleMoves.begin() + i );
        }
    }
    set < pair <int, int> > Blocked;
    for(int y = 0; y < 8; y++){
            for (int x = 0; x < 8; x++){
                if (this->Board[y][x].PieceColor == KingColor && this->Board[y][x].PieceType != Type::Empty) continue;
                vector < pair <int, int> > rawMoves = Move(*(new pair<int, int> (y, x)));
                set < pair <int, int> > ToCheck; for (auto p: rawMoves) ToCheck.insert(p);
                for (auto pos: possibleMoves) {
                    if (ToCheck.count(pos)) {
                        Blocked.insert(pos);
                    }
                }
            }
        }
    if (Blocked.size() != possibleMoves.size()) return false;
    return true;
}

bool Game::ValidCordinate(pair <int, int> point){
    if(point.first >= 0 && point.first <= 7 && point.second >= 0 && point.second <= 7){
        return true;
    }
    return false;
}

vector < pair < int, int > > Game::Move(pair<int, int> position){
    if (!ValidCordinate({position.first, position.second})) return *(new vector< pair<int, int> > ());
    vector < pair < int, int > > possibleMoves;
    Type PieceType = this->Board[position.first][position.second].PieceType;
    Color PieceColor = this->Board[position.first][position.second].PieceColor;

    if(PieceType == Type::Pawn){
        if (ValidCordinate({position.first + 1 * ((PieceColor == Color::White) ? 1 : -1), position.second})) if(this->Board[position.first + 1 * ((PieceColor == Color::White) ? 1 : -1)][position.second].PieceType == Type::Empty) {
            possibleMoves.push_back( { position.first + 1 * ((PieceColor == Color::White) ? 1 : -1), position.second } );
        }
        if (ValidCordinate({position.first + 2 * ((PieceColor == Color::White) ? 1 : -1), position.second })) if(position.first == ((PieceColor == Color::White) ? 1 : 6)  && this->Board[position.first + 2 * ((PieceColor == Color::White) ? 1 : -1)][position.second].PieceType == Type::Empty){
                possibleMoves.push_back( { position.first + 2 * ((PieceColor == Color::White) ? 1 : -1), position.second } );
        }
        if (ValidCordinate({position.first + 1 * ((PieceColor == Color::White) ? 1 : -1), position.second + 1})) if(this->Board[position.first + 1 * ((PieceColor == Color::White) ? 1 : -1)][position.second + 1].PieceColor != PieceColor && this->Board[position.first + 1 * ((PieceColor == Color::White) ? 1 : -1)][position.second + 1].PieceType != Type::Empty){
            possibleMoves.push_back( { position.first + 1 * ((PieceColor == Color::White) ? 1 : -1), position.second + 1} );
        }

        if (ValidCordinate({position.first + 1 * ((PieceColor == Color::White) ? 1 : -1), position.second - 1})) if(this->Board[position.first + 1 * ((PieceColor == Color::White) ? 1 : -1)][position.second - 1].PieceColor != PieceColor && this->Board[position.first + 1 * ((PieceColor == Color::White) ? 1 : -1)][position.second - 1].PieceType != Empty){
            possibleMoves.push_back( { position.first + 1 * ((PieceColor == Color::White) ? 1 : -1), position.second - 1} );
        }
    } else if(PieceType == Type::Bishop) {
        for(int i = 1; i < 8; i++){
            if(ValidCordinate( { position.first + i, position.second + i } ) ){
                if(this->Board[position.first + i][position.second + i].PieceColor == PieceColor){
                    break;
                }
                possibleMoves.push_back({ position.first + i, position.second + i });
                if (this->Board[position.first + i][position.second + i].PieceType != Type::Empty) break;
            }
        }
        for(int i = 1; i < 8; i++){
            if(ValidCordinate( { position.first - i, position.second - i } ) ){
                if(this->Board[position.first - i][position.second - i].PieceColor == PieceColor){
                    break;
                }
                possibleMoves.push_back({ position.first - i, position.second - i });
                if (this->Board[position.first - i][position.second - i].PieceType != Type::Empty) break;
            }
        }
        for(int i = 1; i < 8; i++){
            if(ValidCordinate( { position.first + i, position.second - i } ) ){
                if(this->Board[position.first + i][position.second - i].PieceColor == PieceColor){
                    break;
                }
                possibleMoves.push_back({ position.first + i, position.second - i });
                if (this->Board[position.first + i][position.second - i].PieceType != Type::Empty) break;
            }
        }
        for(int i = 1; i < 8; i++){
            if(ValidCordinate( { position.first - i, position.second + i } ) ){
                if(this->Board[position.first - i][position.second + i].PieceColor == PieceColor){
                    break;
                }
                possibleMoves.push_back({ position.first - i, position.second + i });
                if (this->Board[position.first - i][position.second + i].PieceType != Type::Empty) break;
            }
        }
        for(int i = 0; i < possibleMoves.size(); i++){ /*eliminate moves that are not possible*/
            if( ! ( this->ValidCordinate( { possibleMoves[i].first, possibleMoves[i].second } ) ) ){
                possibleMoves.erase( possibleMoves.begin() + i );
            }
        }
    } else if(PieceType == Type::Knight){ // caballo
        possibleMoves.push_back({ position.first + 1, position.second + 2 });
        possibleMoves.push_back({ position.first - 1, position.second + 2 });
        possibleMoves.push_back({ position.first - 2, position.second + 1 });
        possibleMoves.push_back({ position.first + 2, position.second + 1 });
        possibleMoves.push_back({ position.first - 1, position.second - 2 });
        possibleMoves.push_back({ position.first + 1, position.second - 2 });
        possibleMoves.push_back({ position.first - 2, position.second - 1 });
        possibleMoves.push_back({ position.first + 2, position.second - 1 });
        for(int i = 0; i < possibleMoves.size(); i++){
            if( ! ( this->ValidCordinate( { possibleMoves[i].first, possibleMoves[i].second } ) ) ){
                possibleMoves.erase( possibleMoves.begin() + i );
            }
        }
    } else if(PieceType == Type::Rook){
        for(int i = position.first + 1; i < 8; i++){
            if(this->Board[i][position.second].PieceColor == PieceColor){
                break;
            }
            possibleMoves.push_back({ i, position.second });
            if (this->Board[i][position.second].PieceType != Type::Empty) break;

        }
        for(int i = position.second + 1; i < 8; i++){
            if(this->Board[position.first][i].PieceColor == PieceColor){
                break;
            }
            possibleMoves.push_back({ position.first, i });
            if (this->Board[position.first][i].PieceType != Type::Empty) break;
        }
        for(int i = position.first -1; i > -1; i--){
            if(this->Board[i][position.second].PieceColor == PieceColor){
                break;
            }
            possibleMoves.push_back({ i, position.second });
            if (this->Board[i][position.second].PieceType != Type::Empty) break;
        }
        for(int i = position.second -1; i > -1; i--){
            if(this->Board[position.first][i].PieceColor == PieceColor){
                break;
            }
            possibleMoves.push_back({ position.first, i });
            if (this->Board[position.first][i].PieceType != Type::Empty) break;
        }
    } else if(PieceType == Type::Queen){

        // ROOK

                for(int i = position.first + 1; i < 8; i++){
            if(this->Board[i][position.second].PieceColor == PieceColor){
                break;
            }
            possibleMoves.push_back({ i, position.second });
            if (this->Board[i][position.second].PieceType != Type::Empty) break;

        }
        for(int i = position.second + 1; i < 8; i++){
            if(this->Board[position.first][i].PieceColor == PieceColor){
                break;
            }
            possibleMoves.push_back({ position.first, i });
            if (this->Board[position.first][i].PieceType != Type::Empty) break;
        }
        for(int i = position.first -1; i > -1; i--){
            if(this->Board[i][position.second].PieceColor == PieceColor){
                break;
            }
            possibleMoves.push_back({ i, position.second });
            if (this->Board[i][position.second].PieceType != Type::Empty) break;
        }
        for(int i = position.second -1; i > -1; i-- ){
            if(this->Board[position.first][i].PieceColor == PieceColor){
                break;
            }
            possibleMoves.push_back({ position.first, i });
            if (this->Board[position.first][i].PieceType != Type::Empty) break;
        }

        // BISHOP
        for(int i = 1; i < 8; i++){
            if(ValidCordinate( { position.first + i, position.second + i } ) ){
                if(this->Board[position.first + i][position.second + i].PieceColor == PieceColor){
                    break;
                }
                possibleMoves.push_back({ position.first + i, position.second + i });
                if (this->Board[position.first + i][position.second + i].PieceType != Type::Empty) break;
            }
        }
        for(int i = 1; i < 8; i++){
            if(ValidCordinate( { position.first - i, position.second - i } ) ){
                if(this->Board[position.first - i][position.second - i].PieceColor == PieceColor){
                    break;
                }
                possibleMoves.push_back({ position.first - i, position.second - i });
                if (this->Board[position.first - i][position.second - i].PieceType != Type::Empty) break;
            }
        }
        for(int i = 1; i < 8; i++){
            if(ValidCordinate( { position.first + i, position.second - i } ) ){
                if(this->Board[position.first + i][position.second - i].PieceColor == PieceColor){
                    break;
                }
                possibleMoves.push_back({ position.first + i, position.second - i });
                if (this->Board[position.first + i][position.second - i].PieceType != Type::Empty) break;
            }
        }
        for(int i = 1; i < 8; i++){
            if(ValidCordinate( { position.first - i, position.second + i } ) ){
                if(this->Board[position.first - i][position.second + i].PieceColor == PieceColor){
                    break;
                }
                possibleMoves.push_back({ position.first - i, position.second + i });
                if (this->Board[position.first - i][position.second + i].PieceType != Type::Empty) break;
            }
        }
        for(int i = 0; i < possibleMoves.size(); i++){ /*eliminate moves that are not possible*/
            if( ! ( this->ValidCordinate( { possibleMoves[i].first, possibleMoves[i].second } ) ) ){
                possibleMoves.erase( possibleMoves.begin() + i );
            }
        }
    } else if(PieceType == Type::King){
        possibleMoves.push_back({ position.first + 0, position.second + 1 });
        possibleMoves.push_back({ position.first + 0, position.second - 1 });
        possibleMoves.push_back({ position.first + 1, position.second + 0 });
        possibleMoves.push_back({ position.first + 1, position.second + 1 });
        possibleMoves.push_back({ position.first + 1, position.second - 1 });
        possibleMoves.push_back({ position.first - 1, position.second + 0 });
        possibleMoves.push_back({ position.first - 1, position.second + 1 });
        possibleMoves.push_back({ position.first - 1, position.second - 1 });
        for(int i = 0; i < possibleMoves.size(); i++){ /*eliminate moves that are not possible*/
            if( ! ( this->ValidCordinate( { possibleMoves[i].first, possibleMoves[i].second } ) ) ){
                possibleMoves.erase( possibleMoves.begin() + i );
            } else {
                if (this->Board[possibleMoves[i].first][possibleMoves[i].second].PieceColor == PieceColor) {
                    possibleMoves.erase( possibleMoves.begin() + i );
                }
            }
        }
    }

    for(int i = 0; i < possibleMoves.size(); i++){
        if( ! ( this->ValidCordinate( { possibleMoves[i].first, possibleMoves[i].second } ) ) ){
            possibleMoves.erase( possibleMoves.begin() + i );
            //         Array.erase(remove_if(Array.begin(), Array.end(), [](int n) { return n > 9; }),Array.end());
        }
    }

    for(int i = 0; i < possibleMoves.size(); i++){ /*CHECK IF IM EATING MYSELF*/
        if (ValidCordinate({possibleMoves[i].first, possibleMoves[i].second})) {
            if(this->Board[possibleMoves[i].first][possibleMoves[i].second].PieceColor == PieceColor) possibleMoves.erase( possibleMoves.begin() + i );
        } else {
            possibleMoves.erase( possibleMoves.begin() + i );
        }
    }
    return possibleMoves;
}

int Game::PieceValue(Type Piece) {
    if(Piece == Type::Bishop){
        return 3;
    } else if(Piece == Type::King){
        return 0;
    } else if(Piece == Type::Knight){
        return 3;
    } else if(Piece == Type::Pawn){
        return 1;
    } else if(Piece == Type::Queen){
        return 9;
    } else if(Piece == Type::Rook){
        return 5;
    }
    return 0;
}

int Game::ScoreBasedOnEatenPieces(bool maximizing){
    int WhitePointsInGame = 0, BlackPointsInGame = 0;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(this->Board[i][j].PieceColor == Color::White){ /*white piece*/
                WhitePointsInGame += PieceValue( this->Board[i][j].PieceType );
            } else if(this->Board[i][j].PieceColor == Color::Black){ /*black piece*/
                BlackPointsInGame += PieceValue( this->Board[i][j].PieceType );
            }
        }
    }
    if(maximizing) return (39 - WhitePointsInGame) - (39 - BlackPointsInGame); /* return (eaten pieces - pieces that they've eaten me)*/

    return (39 - BlackPointsInGame) - (39 - WhitePointsInGame);
}

void Game::SetGameStatus() {
    if( CheckMate() ){
        if(this->PlayerTurn == Color::White){
            this->GameState = State::AI_Wins;
        } else {
            this->GameState = State::Human_Wins;
        }
    } else {
        this->GameState = State::Running;
    }
}

int Game::Minimax(int Depth, bool maximizing, int alpha, int beta) {
    SetGameStatus();
    if(this->GameState == State::AI_Wins) return INT_MAX;
    if(this->GameState == State::Human_Wins) return -INT_MAX;
    if(this->GameState == State::Tie) return 0;
    if(Depth == 2){
        return this->ScoreBasedOnEatenPieces(maximizing);
    }
    int BestScore = INT_MIN;
    if(maximizing){ // our turn
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                if(this->Board[i][j].PieceType != Type::Empty && this->Board[i][j].PieceColor == Color::White){
                    int score = INT_MIN;
                    vector< pair<int, int> > M_ForMoves;
                    M_ForMoves = this->Move( *(new pair <int, int> (i, j)) );
                    for(int k = 0; k < M_ForMoves.size(); k++){
                        Type PieceInPossibleMovePlace = this->Board[M_ForMoves[k].first][M_ForMoves[k].second].PieceType;
                        Color ColorInPossibleMovePlace = this->Board[M_ForMoves[k].first][M_ForMoves[k].second].PieceColor;
                        if (PieceInPossibleMovePlace == Type::Empty) {
                           //Move
                            std::swap(this->Board[M_ForMoves[k].first][M_ForMoves[k].second], this->Board[i][j]);
                        } else {
                           //Eat
                            this->Board[M_ForMoves[k].first][M_ForMoves[k].second] = this->Board[i][j];
                            this->Board[i][j].PieceType = Type::Empty;
                            this->Board[i][j].PieceColor = Color::None;
                        }
                        //cout << "MAKE MOVE [" << Depth << "]" << endl;
                        //PrintBoard();
                        score = Game::Minimax(Depth + 1, !maximizing, alpha, beta);

                        if (PieceInPossibleMovePlace == Type::Empty) {
                            //Move REVERT
                            std::swap(this->Board[M_ForMoves[k].first][M_ForMoves[k].second], this->Board[i][j]);
                       } else {
                           //Eat REVERT
                            this->Board[i][j] = this->Board[M_ForMoves[k].first][M_ForMoves[k].second];
                            this->Board[M_ForMoves[k].first][M_ForMoves[k].second].PieceType = PieceInPossibleMovePlace;
                            this->Board[M_ForMoves[k].first][M_ForMoves[k].second].PieceColor = ColorInPossibleMovePlace;
                        }
                        //cout << "Reverted Move ["<< Depth <<"]" << endl;
                        //PrintBoard();
                        BestScore = max(BestScore, score);
                        alpha = max(alpha, score);
                        if (beta <= alpha) break;
                    }
                }
            }
        }
    } else { // their turn
        BestScore = INT_MAX;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                if(this->Board[i][j].PieceType != Type::Empty && this->Board[i][j].PieceColor == Color::Black){
                    int score = INT_MIN;
                    vector< pair<int, int> > M_ForMoves;
                    M_ForMoves = this->Move(*(new pair <int, int> (i, j)));
                    for(int k = 0; k < M_ForMoves.size(); k++){
                        Type PieceInPossibleMovePlace = this->Board[M_ForMoves[k].first][M_ForMoves[k].second].PieceType;
                        Color ColorInPossibleMovePlace = this->Board[M_ForMoves[k].first][M_ForMoves[k].second].PieceColor;
                        if (PieceInPossibleMovePlace == Type::Empty) {
                           //Move
                            std::swap(this->Board[M_ForMoves[k].first][M_ForMoves[k].second], this->Board[i][j]);
                        } else {
                           //Eat
                            this->Board[M_ForMoves[k].first][M_ForMoves[k].second] = this->Board[i][j];
                            this->Board[i][j].PieceType = Type::Empty;
                            this->Board[i][j].PieceColor = Color::None;
                        }
                        //cout << "MAKE MOVE [" << Depth << "]" << endl;
                        //PrintBoard();
                        score = Game::Minimax(Depth + 1, !maximizing, alpha, beta);
                        if (PieceInPossibleMovePlace == Type::Empty) {
                            //Move REVERT
                            std::swap(this->Board[M_ForMoves[k].first][M_ForMoves[k].second], this->Board[i][j]);
                        } else {
                           //Eat REVERT
                            this->Board[i][j] = this->Board[M_ForMoves[k].first][M_ForMoves[k].second];
                            this->Board[M_ForMoves[k].first][M_ForMoves[k].second].PieceType = PieceInPossibleMovePlace;
                            this->Board[M_ForMoves[k].first][M_ForMoves[k].second].PieceColor = ColorInPossibleMovePlace;
                        }
                        //cout << "Reverted Move ["<< Depth <<"]" << endl;
                        //PrintBoard();
                        BestScore = min(BestScore, score);
                        beta = min(beta, score);
                        if (beta <= alpha) break;
                    }
                }
            }
        }
    }
    return BestScore;
    cout << BestScore << endl;
}

void Game::ResetBoard() {
    vector < PiecePos > BoardPos = {
        {Type::Rook, Color::White, {0, 0}},  {Type::Knight, Color::White, {0, 1}}, {Type::Bishop, Color::White, {0, 2}}, {Type::King, Color::White, {0, 3}}, {Type::Queen, Color::White, {0, 4}}, {Type::Bishop, Color::White, {0, 5}}, {Type::Knight, Color::White, {0, 6}}, {Type::Rook, Color::White, {0, 7}}, // First Row (1A-1H)
        {Type::Pawn, Color::White, {1, 0}}, {Type::Pawn, Color::White, {1, 1}}, {Type::Pawn, Color::White, {1, 2}}, {Type::Pawn, Color::White, {1, 3}}, {Type::Pawn, Color::White, {1, 4}}, {Type::Pawn, Color::White, {1, 5}}, {Type::Pawn, Color::White, {1, 6}}, {Type::Pawn, Color::White, {1, 7}}, // Second Row (2A-2H)
        {Type::Pawn, Color::Black, {6, 0}}, {Type::Pawn, Color::Black, {6, 1}}, {Type::Pawn, Color::Black, {6, 2}}, {Type::Pawn, Color::Black, {6, 3}}, {Type::Pawn, Color::Black, {6, 4}}, {Type::Pawn, Color::Black, {6, 5}}, {Type::Pawn, Color::Black, {6, 6}}, {Type::Pawn, Color::Black, {6, 7}}, // BefLast Row (7A-7H)
        {Type::Rook, Color::Black, {7, 0}},  {Type::Knight, Color::Black, {7, 1}}, {Type::Bishop, Color::Black, {7, 2}}, {Type::King, Color::Black, {7, 3}}, {Type::Queen, Color::Black, {7, 4}}, {Type::Bishop, Color::Black, {7, 5}}, {Type::Knight, Color::Black, {7, 6}}, {Type::Rook, Color::Black, {7, 7}} // Last Row (8A-8H)
    };

    this->Board = *(new vector < vector < Piece > >(8)); /* Resets the vector Board and gives it the static size of 8. */
    for (vector<Piece> &vec: this->Board) {
        vec.resize(8); /* Multidimensional inner vector resize to static 8. */
    }

    for (auto &Row: this->Board) { /*Empty board*/
        for (auto &CurrentPiece: Row) {
            CurrentPiece.PieceType = Type::Empty;
            CurrentPiece.PieceColor = Color::None;
        }
    }

    for (PiecePos Piece: BoardPos) { /*Board fill*/
        this->Board[Piece.Position.first][Piece.Position.second].PieceType = Piece.PieceType;
        this->Board[Piece.Position.first][Piece.Position.second].PieceColor = Piece.PieceColor;
    }
}

char Game::LetterForPrinting(Type PieceForPrinting){
    if(PieceForPrinting == Type::Bishop){
        return 'A';
    } else if(PieceForPrinting == Type::King){
        return 'K';
    } else if(PieceForPrinting == Type::Knight){
        return 'C';
    } else if(PieceForPrinting == Type::Pawn){
        return 'P';
    } else if(PieceForPrinting == Type::Queen){
        return 'Q';
    } else if(PieceForPrinting == Type::Rook){
        return 'T';
    } else if(PieceForPrinting == Type::Empty){
        return '-';
    }
    return '#';
}

void Game::PrintBoard(){
    system("CLS");
    ConsoleColors::Modifier _def(ConsoleColors::FG_DEFAULT); //define color DEFAULT Foreground
    ConsoleColors::Modifier _bg_def(ConsoleColors::BG_DEFAULT); //define color DEFAULT Background
    ConsoleColors::Modifier _white(ConsoleColors::FG_WHITE); //define color WHITE Foreground
    ConsoleColors::Modifier _black(ConsoleColors::FG_BLACK); //define color BLACK Foreground
    ConsoleColors::Modifier _wierd_white(ConsoleColors::FG_WIERD_WHITE); //define color WIERD_WHITE Foreground
    ConsoleColors::Modifier _bg_gray(ConsoleColors::BG_GRAY); //define color GRAY Background
    ConsoleColors::Modifier _yellow(ConsoleColors::FG_YELLOW); //empty spaces
    cout << _bg_gray << _black;
    cout << "    ";
    for(int i = 0; i < 8; i++){
        cout << " " << char('A' + i) << " ";
    }
    cout << endl;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(j == 0) cout << _black << i + 1 << " | ";

            if(this->Board[i][j].PieceColor == Color::White){
                cout << _white;
            } else if(this->Board[i][j].PieceColor == Color::Black){
                cout << _black;
            } else if(this->Board[i][j].PieceColor == Color::None){
                cout << _yellow;
            }
            cout << "[" << LetterForPrinting(this->Board[i][j].PieceType) << "]";
        }
        cout << endl;
    }
    cout << _def << _bg_def;
}

void Game::init() {
    ResetBoard();
    PrintBoard();
    SetGameStatus();
    while (GameState == State::Running) {
        this->PrintBoard();
        if (this->PlayerTurn == Color::White) { // Our turn
            int bestScore = -INT_MAX;
            pair<int, int> From; pair<int, int> To;
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    if(this->Board[i][j].PieceType != Type::Empty && this->Board[i][j].PieceColor == White) {
                        int score;
                        vector < pair <int, int> > AllPossibleMoves;
                        AllPossibleMoves = Move({ i, j });
                        for(int k = 0; k < AllPossibleMoves.size(); k++){
                            Type PieceInPossibleMovePlace = this->Board[AllPossibleMoves[k].first][AllPossibleMoves[k].second].PieceType;
                            Color ColorInPossibleMovePlace = this->Board[AllPossibleMoves[k].first][AllPossibleMoves[k].second].PieceColor;

                            if (PieceInPossibleMovePlace == Type::Empty) {
                                std::swap(this->Board[AllPossibleMoves[k].first][AllPossibleMoves[k].second], this->Board[i][j]);
                            } else {
                                this->Board[AllPossibleMoves[k].first][AllPossibleMoves[k].second] = this->Board[i][j];
                                this->Board[i][j].PieceType = Type::Empty;
                                this->Board[i][j].PieceColor = Color::None;
                            }

                            score = this->Minimax(0, false);
                            if (PieceInPossibleMovePlace == Type::Empty) {
                                std::swap(this->Board[AllPossibleMoves[k].first][AllPossibleMoves[k].second], this->Board[i][j]);
                            } else {
                                this->Board[i][j] = this->Board[AllPossibleMoves[k].first][AllPossibleMoves[k].second];
                                this->Board[AllPossibleMoves[k].first][AllPossibleMoves[k].second].PieceType = PieceInPossibleMovePlace;
                                this->Board[AllPossibleMoves[k].first][AllPossibleMoves[k].second].PieceColor = ColorInPossibleMovePlace;
                            }
                            if (score > bestScore) {
                                score = bestScore;
                                From.first = i; From.second = j;
                                To.first = AllPossibleMoves[k].first; To.second = AllPossibleMoves[k].second;
                            }
                        }
                    }
                }
            }
            this->Board[To.first][To.second] = this->Board[From.first][From.second];
            this->Board[From.first][From.second].PieceType = Type::Empty;
            this->Board[From.first][From.second].PieceColor = Color::None;
        } else {
            string in, out;
            cout << "Cordenadas de origen: ";
            cin >> in;
            cout << "Cordenadas de llegada: ";
            cin >> out;
            this->Board[(int)(out[1] - '0')-1][out[0]-'A'] = this->Board[(int)(in[1] - '0')-1][in[0]-'A'];
            this->Board[(int)(in[1] - '0')-1][in[0]-'A'].PieceType = Type::Empty;
            this->Board[(int)(in[1] - '0')-1][in[0]-'A'].PieceColor = Color::None;
        }
        PlayerTurn = (PlayerTurn == Color::White) ? Color::Black : Color::White;
        SetGameStatus();
    }
}

int main() {
//    std::cout.sync_with_stdio(false);
    Game game = *(new Game());
}

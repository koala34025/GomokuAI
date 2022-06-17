#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <list>
#include <map>

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

int player;
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;
int d8x[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
int d8y[] = { -1, -1, -1, 0, 0, 1, 1, 1 };

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};

bool is_spot_on_board(Point p) {
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
}

int get_disc(Point p) {
    return board[p.x][p.y];
}

void set_disc(Point p, int disc) {
    board[p.x][p.y] = disc;
}

bool is_disc_at(Point p, int disc) {
    if (!is_spot_on_board(p))
        return false;
    if (get_disc(p) != disc)
        return false;
    return true;
}

bool is_spot_valid(Point center) {
    if (get_disc(center) != EMPTY)
        return false;
    return true;
}

// Generate a placement if it is in star(dist of 4) of a dot
std::list<Point> GeneratePlaces() {
    std::list<Point> places;

    bool added[SIZE][SIZE] = { 0 };
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (is_spot_valid(Point(i, j)))
                continue;
            
            for (int dir = 0; dir < 8; dir++) {
                for (int mul = 1; mul <= 4; mul++) {
                    int newI = i + d8x[dir] * mul;
                    int newJ = j + d8y[dir] * mul;
                    Point newPoint(newI, newJ);
                    if(!is_spot_on_board(newPoint) || !is_spot_valid(newPoint) || added[newI][newJ])
                        continue;
                    
                    added[newI][newJ] = true;
                    places.push_back(newPoint);
                }
            }
        }
    }
    
    // If the list is empty, go (7, 7) 
    if (places.empty())
        places.push_back(Point(7, 7));

    return places;
}

//---------------------------------------------------------------------------------------------------------------//

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void write_valid_spot(std::ofstream& fout) {
    srand(time(NULL));
    // Keep updating the output until getting killed.
    Point PlacePoint = GeneratePlaces().front();
    fout << PlacePoint.x << " " << PlacePoint.y << std::endl;
    // Remember to flush the output to ensure the last action is written to file.
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
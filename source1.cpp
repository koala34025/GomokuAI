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

const int oneValue = 10;
const int twoValue = 100;
const int threeValue = 1000;
const int fourValue = 100000;
const int fiveValue = 10000000;
const int blockedOneValue = 1;
const int blockedTwoValue = 10;
const int blockedThreeValue = 100;
const int blockedFourValue = 10000;

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

// Generate a placement if it is in square(5x5) of a dot
std::list<Point> GeneratePlaces() {
    const int NeighborDist = 2;
    std::list<Point> places;
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (!is_spot_valid(Point(i, j)))
                continue;
            
            bool HasDot = false;
            for (int newI = i-NeighborDist; newI <= i+NeighborDist; newI++) {
                if (newI < 0 || newI >= SIZE)
                    continue;

                for (int newJ = j-NeighborDist; newJ <= j+NeighborDist; newJ++) {
                    if (newJ < 0 || newJ >= SIZE)
                        continue;

                    if (!is_spot_valid(Point(newI, newJ))) {
                        HasDot = true;
                        break;
                    }
                }
                if (HasDot)
                    break;
            }
            if (HasDot)
                places.push_back(Point(i, j));
        }
    }
    
    // If the list is empty, go (7, 7) 
    if (places.empty())
        places.push_back(Point(7, 7));

    return places;
}

//---------------------------------------------------------------------------------------------------------------//



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
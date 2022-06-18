#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <list>
#include <map>
#define DEBUG 0

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

const int oneValue = 2; // Consider a dot if it in the star of another one
const int twoValue = 100;
const int deathThreeValue = 100;
const int threeValue = 1000;
const int deathFourValue = 10000;
const int fourValue = 100000;
const int fiveValue = 10000000;

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

int get_next_player(int player) {
    return 3 - player;
}

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

bool HasNeighborInValidSpot(Point p, int neighborDist, int checkAlly) { 
    // checkAlly: 1 for true, 0 for false, -1 for checkEnemy
    for (int newI = p.x - neighborDist; newI <= p.x + neighborDist; newI++) {
        if (newI < 0 || newI >= SIZE)
            continue;

        for (int newJ = p.y - neighborDist; newJ <= p.y + neighborDist; newJ++) {
            if (newJ < 0 || newJ >= SIZE)
                continue;
            if (newI == p.x && newJ == p.y)
                continue;

            Point newP(newI, newJ);
            if (checkAlly == 0) {
                if (!is_spot_valid(newP)) {
                    return true;
                }
            }
            else if(checkAlly == 1){
                if (get_disc(newP) == get_disc(p))
                    return true;
            }
            else if (checkAlly == -1) {
                if (get_disc(newP) == (3 - get_disc(p)))
                    return true;
            }
        }
    }
    return false;
}

// Generate a placement if it is in square(5x5) of a dot
std::list<Point> GeneratePlaces() {
    std::list<Point> places;
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (!is_spot_valid(Point(i, j)))
                continue;
            
            if (HasNeighborInValidSpot(Point(i, j), 2, 0))
                places.push_back(Point(i, j));
        }
    }
    
    // If the list is empty, go (7, 7) 
    if (places.empty())
        places.push_back(Point(7, 7));

    return places;
}

//---------------------------------------------------------------------------------------------------------------//

int FiveCount(int who) {
    int goal[] = { who, who, who, who, who };
    int cnt = 0;

    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 5; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 5; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 4; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i - k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    return cnt;
}

int FourCount(int who) {
    int goal[] = { EMPTY, who, who, who, who, EMPTY };
    int cnt = 0;

    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 6; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i + k][j] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 6; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i + k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 5; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i - k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    return cnt;
}

int DeathFourCount(int who) {
    // Goal = 211110, 011112;
    int goal[] = { who, who, who, who };
    int opponent = get_next_player(who);
    int cnt = 0;

    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            bool ok = true;
            if ((board[i][j] == EMPTY && board[i][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i][j + 5] == EMPTY)) {
                for (int k = 0; k < 4; k++) {
                    if (board[i][j + 1 + k] != goal[k]) {
                        ok = false;
                        break;
                    }
                }
            }
            else {
                ok = false;
            }
            if (ok) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 6; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            if ((board[i + 5][j] == EMPTY && board[i][j] == opponent) ||
                (board[i + 5][j] == opponent && board[i][j] == EMPTY)) {
                for (int k = 0; k < 4; k++) {
                    if (board[i + 1 + k][j] != goal[k]) {
                        ok = false;
                        break;
                    }
                }
            }
            else {
                ok = false;
            }
            if (ok) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 6; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            bool ok = true;
            if ((board[i][j] == EMPTY && board[i + 5][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i + 5][j + 5] == EMPTY)) {
                for (int k = 0; k < 4; k++) {
                    if (board[i + 1 + k][j + 1 + k] != goal[k]) {
                        ok = false;
                        break;
                    }
                }
            }
            else {
                ok = false;
            }
            if (ok) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 5; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            bool ok = true;
            if ((board[i][j] == EMPTY && board[i - 5][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i - 5][j + 5] == EMPTY)) {
                for (int k = 0; k < 4; k++) {
                    if (board[i - 1 - k][j + 1 + k] != goal[k]) {
                        ok = false;
                        break;
                    }
                }
            }
            else {
                ok = false;
            }
            if (ok) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Goal = 0111010, 2111010, 0111012
    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i][j + 6] == EMPTY)) {
                for (int k = 0; k < 5; k++) {
                    if (board[i][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 4) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 7; i++) {
        for (int j = 0; j < SIZE; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i][j + 6] == EMPTY)) {
                for (int k = 0; k < 5; k++) {
                    if (board[i + 1 + k][j] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i + 1 + k][j] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 4) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 7; i++) {
        for (int j = 0; j < SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i][j + 6] == EMPTY)) {
                for (int k = 0; k < 5; k++) {
                    if (board[i + 1 + k][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i + 1 + k][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 4) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 6; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i - 6][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i - 6][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i - 6][j + 6] == EMPTY)) {
                for (int k = 0; k < 5; k++) {
                    if (board[i - 1 - k][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i - 1 - k][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 4) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    return cnt;
}

int ThreeCount(int who) {
    int goal[] = { EMPTY, who, who, who, EMPTY };
    int cnt = 0;

    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 5; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 5; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 4; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i - k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Goal = 011010
    int opponent = get_next_player(who);

    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            int myDotCnt = 0;
            if (board[i][j] == EMPTY && board[i][j + 5] == EMPTY) {
                for (int k = 0; k < 4; k++) {
                    if (board[i][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 6; i++) {
        for (int j = 0; j < SIZE; j++) {
            int myDotCnt = 0;
            if (board[i][j] == EMPTY && board[i + 5][j] == EMPTY) {
                for (int k = 0; k < 4; k++) {
                    if (board[i + 1 + k][j] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i + 1 + k][j] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 6; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            int myDotCnt = 0;
            if (board[i][j] == EMPTY && board[i + 5][j + 5] == EMPTY) {
                for (int k = 0; k < 4; k++) {
                    if (board[i + 1 + k][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i + 1 + k][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 5; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            int myDotCnt = 0;
            if (board[i][j] == EMPTY && board[i - 5][j + 5] == EMPTY) {
                for (int k = 0; k < 4; k++) {
                    if (board[i - 1 - k][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i - 1 - k][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //if (DEBUG)
                    //std::cout << '\n' << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    return cnt;
}

int DeathThreeCount(int who) {
    // Goal = 21110, 01112;
    int goal[] = { who, who, who };
    int opponent = get_next_player(who);
    int cnt = 0;

    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            if ((board[i][j] == EMPTY && board[i][j + 4] == opponent) ||
                (board[i][j] == opponent && board[i][j + 4] == EMPTY)) {
                for (int k = 0; k < 3; k++) {
                    if (board[i][j + 1 + k] != goal[k]) {
                        ok = false;
                        break;
                    }
                }
            }
            else {
                ok = false;
            }
            if (ok) {
                //std::cout << "l: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 5; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            if ((board[i + 4][j] == EMPTY && board[i][j] == opponent) ||
                (board[i + 4][j] == opponent && board[i][j] == EMPTY)) {
                for (int k = 0; k < 3; k++) {
                    if (board[i + 1 + k][j] != goal[k]) {
                        ok = false;
                        break;
                    }
                }
            }
            else {
                ok = false;
            }
            if (ok) {
                //std::cout << "k: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 5; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            if ((board[i][j] == EMPTY && board[i + 4][j + 4] == opponent) ||
                (board[i][j] == opponent && board[i + 4][j + 4] == EMPTY)) {
                for (int k = 0; k < 3; k++) {
                    if (board[i + 1 + k][j + 1 + k] != goal[k]) {
                        ok = false;
                        break;
                    }
                }
            }
            else {
                ok = false;
            }
            if (ok) {
                //std::cout << "j: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 4; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            if ((board[i][j] == EMPTY && board[i - 4][j + 4] == opponent) ||
                (board[i][j] == opponent && board[i - 4][j + 4] == EMPTY)) {
                for (int k = 0; k < 3; k++) {
                    if (board[i - 1 - k][j + 1 + k] != goal[k]) {
                        ok = false;
                        break;
                    }
                }
            }
            else {
                ok = false;
            }
            if (ok) {
                //std::cout << "i: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Goal = 011010, 211010, 011012
    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 5] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i][j + 5] == EMPTY)) {
                for (int k = 0; k < 4; k++) {
                    if (board[i][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //std::cout << "h: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 6; i++) {
        for (int j = 0; j < SIZE; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 5] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i][j + 5] == EMPTY)) {
                for (int k = 0; k < 4; k++) {
                    if (board[i + 1 + k][j] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i + 1 + k][j] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //std::cout << "g: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 6; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 5] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i][j + 5] == EMPTY)) {
                for (int k = 0; k < 4; k++) {
                    if (board[i + 1 + k][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i + 1 + k][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //std::cout << "f: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 5; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i - 5][j + 5] == EMPTY) ||
                (board[i][j] == EMPTY && board[i - 5][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i - 5][j + 5] == EMPTY)) {
                for (int k = 0; k < 4; k++) {
                    if (board[i - 1 - k][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i - 1 - k][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //std::cout << "e: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Goal = 0110010, 2110010, 0110012
    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i][j + 6] == EMPTY)) {
                for (int k = 0; k < 5; k++) {
                    if (board[i][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //std::cout << "d: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 7; i++) {
        for (int j = 0; j < SIZE; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i][j + 6] == EMPTY)) {
                for (int k = 0; k < 5; k++) {
                    if (board[i + 1 + k][j] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i + 1 + k][j] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //std::cout << "c: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 7; i++) {
        for (int j = 0; j < SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i][j + 6] == EMPTY)) {
                for (int k = 0; k < 5; k++) {
                    if (board[i + 1 + k][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i + 1 + k][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //std::cout << "b: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 6; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i - 6][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i - 6][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i - 6][j + 6] == EMPTY)) {
                for (int k = 0; k < 5; k++) {
                    if (board[i - 1 - k][j + 1 + k] == opponent) {
                        myDotCnt = 0;
                        break;
                    }
                    if (board[i - 1 - k][j + 1 + k] == who) {
                        myDotCnt++;
                    }
                }
            }
            if (myDotCnt == 3) {
                //std::cout << "a: " << i << ' ' << j << '\n';
                cnt++;
            }
        }
    }

    return cnt;
}

int TwoCount(int who) {
    // Goal = 0110, 01010, 010010
    int goal_1[] = { EMPTY, who, who, EMPTY };
    int goal_2[] = { EMPTY, who, EMPTY, who, EMPTY };
    int goal_3[] = { EMPTY, who, EMPTY, EMPTY, who, EMPTY };
    int cnt = 0;

    // Goal 1
    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 4; j++) {
            bool ok = true;
            for (int k = 0; k < 4; k++) {
                if (board[i][j + k] != goal_1[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 4; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            for (int k = 0; k < 4; k++) {
                if (board[i + k][j] != goal_1[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 4; i++) {
        for (int j = 0; j < SIZE - 4; j++) {
            bool ok = true;
            for (int k = 0; k < 4; k++) {
                if (board[i + k][j + k] != goal_1[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 3; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 4; j++) {
            bool ok = true;
            for (int k = 0; k < 4; k++) {
                if (board[i - k][j + k] != goal_1[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Goal 2
    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i][j + k] != goal_2[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 5; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j] != goal_2[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 5; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j + k] != goal_2[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 4; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i - k][j + k] != goal_2[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Goal 3
    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i][j + k] != goal_3[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i < SIZE - 6; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i + k][j] != goal_3[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i < SIZE - 6; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i + k][j + k] != goal_3[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 5; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i - k][j + k] != goal_3[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                cnt++;
            }
        }
    }

    return cnt;
}

int OneCount(int who) {
    int cnt = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Point p(i, j);
            if (get_disc(p) != who)
                continue;

            if (HasNeighborInValidSpot(p, 1, 1))
                cnt += 2;
            if (HasNeighborInValidSpot(p, 1, -1))
                cnt++;
        }
    }

    return cnt;
}

int OverallScore(int who) {
    int score = 0, tmpCnt;

    if (DEBUG)
        std::cout << "For " << ((who == 1) ? "O" : "X");

    tmpCnt = OneCount(who);
    score += tmpCnt * oneValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", OneCount: " << tmpCnt;

    tmpCnt = TwoCount(who);
    score += tmpCnt * twoValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", TwoCount: " << tmpCnt;

    tmpCnt = DeathThreeCount(who);
    score += tmpCnt * deathThreeValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", DeathThreeCount: " << tmpCnt;

    tmpCnt = ThreeCount(who);
    score += tmpCnt * threeValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", ThreeCount: " << tmpCnt;

    tmpCnt = DeathFourCount(who);
    score += tmpCnt * deathFourValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", DeathFourCount: " << tmpCnt;

    tmpCnt = FourCount(who);
    score += tmpCnt * fourValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", FourCount: " << tmpCnt;

    tmpCnt = FiveCount(who);
    score += tmpCnt * fiveValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", FiveCount: " << tmpCnt;

    if (DEBUG)
        std::cout << "\n";

    return score;
}

int Evaluate() {
    int myScore = OverallScore(player);
    int opponentScore = OverallScore(get_next_player(player));

    int evaluation = myScore - opponentScore;

    // Fix starting
    if (opponentScore > 0) {
        evaluation++;
    }

    return evaluation;
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
    Point PlacePoint;
    int maxEval = -1e9;
    for (Point p : GeneratePlaces()) {
        if (DEBUG)
            std::cout << "Trying " << ((player == 1) ? "O" : "X") << " on (" << p.x << " " << p.y << ")\n";

        set_disc(p, player);
        int eval = Evaluate();
        if (eval > maxEval) {
            maxEval = eval;
            PlacePoint = p;
        }
        set_disc(p, EMPTY);
    }
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
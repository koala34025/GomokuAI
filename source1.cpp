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

const int oneValue = 10; // Consider a dot if it has neighbor ally dots(square)
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

bool HasNeighborInValidSpot(Point p, int neighborDist, bool checkAlly) {
    for (int newI = p.x - neighborDist; newI <= p.x + neighborDist; newI++) {
        if (newI < 0 || newI >= SIZE)
            continue;

        for (int newJ = p.y - neighborDist; newJ <= p.y + neighborDist; newJ++) {
            if (newJ < 0 || newJ >= SIZE)
                continue;

            if (!checkAlly) {
                if (!is_spot_valid(Point(newI, newJ))) {
                    return true;
                }
            }
            else {
                if (is_disc_at(Point(newI, newJ), get_disc(p)))
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
            
            if (HasNeighborInValidSpot(Point(i, j), 2, false))
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
            if (ok) {
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
            if (ok) {
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
            if (ok) {
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
            if (ok) {
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
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 6; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i][j + 6] == EMPTY)) {
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
            if (ok) {
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
            if (ok) {
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
            if (ok) {
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
            if (ok) {
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
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 5; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 6; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 5] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i][j + 5] == EMPTY)) {
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
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 6; i < SIZE; i++) {
        for (int j = 0; j < SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i][j + 6] == EMPTY)) {
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

            if (HasNeighborInValidSpot(p, 2, true))
                cnt++;
        }
    }

    return cnt;
}

int OverallScore(int who) {
    int score = 0;
    score += OneCount(who) * oneValue;
    score += TwoCount(who) * twoValue;
    score += DeathThreeCount(who) * deathThreeValue;
    score += ThreeCount(who) * threeValue;
    score += DeathFourCount(who) * deathFourValue;
    score += FourCount(who) * fourValue;
    score += FiveCount(who) * fiveValue;
    return score;
}

int Evaluate() {
    int myScore = OverallScore(player);
    int opponentScore = OverallScore(get_next_player(player));

    int evaluation = myScore - opponentScore;

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
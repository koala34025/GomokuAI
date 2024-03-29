#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <list>
#include <map>
#include <utility>
#include <algorithm>

#define DEBUG 0
#define SAFEDEPTH 2
#define MXDEPTH 4
#define GENDIST 2

struct Point {
    int x, y;
    int h;
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

int NOWDEPTH;
Point PlacePoint;

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

const int oneValue = 1;
const int twoValue = 50;
const int deathThreeValue = 50;
const int oneSpotThreeValue = 1000;
const int threeValue = 5000;
const int breakDeathFourValue = 10000;
const int deathFourValue = 10000;
const int fourValue = 100000;
const int fiveValue = 10000000;

long long int tried = 0;
int player;
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;
int d8x[] = { -1, 0, 1, -1, 1, -1, 0, 1 };
int d8y[] = { -1, -1, -1, 0, 0, 1, 1, 1 };
const int INF = 1e9;

struct PointGreater {
    bool operator()(const Point& lhs, const Point& rhs) {
        return lhs.h > rhs.h;
    }
};

struct PointLesser {
    bool operator()(const Point& lhs, const Point& rhs) {
        return lhs.h < rhs.h;
    }
};

void OrderMoves(std::list<Point>&, int);

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
            else if (checkAlly == 1) {
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

std::list<Point> GeneratePlaces() {
    // Generate a placement if it is in square(5x5) of a dot
    std::list<Point> places;

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (!is_spot_valid(Point(i, j)))
                continue;

            if (HasNeighborInValidSpot(Point(i, j), GENDIST, 0))
                places.push_back(Point(i, j));
        }
    }

    return places;
}

//---------------------------------------------------------------------------------------------------------------//

int FiveCount(int who) {
    int goal[] = { who, who, who, who, who };
    int cnt = 0;

    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return 1;
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i <= SIZE - 5; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return 1;
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i <= SIZE - 5; i++) {
        for (int j = 0; j <= SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return 1;
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 4; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i - k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return 1;
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
        for (int j = 0; j <= SIZE - 6; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return 1;
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i <= SIZE - 6; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i + k][j] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return 1;
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i <= SIZE - 6; i++) {
        for (int j = 0; j <= SIZE - 6; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i + k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return 1;
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 5; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 6; j++) {
            bool ok = true;
            for (int k = 0; k < 6; k++) {
                if (board[i - k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                return 1;
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
        for (int j = 0; j <= SIZE - 6; j++) {
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
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i <= SIZE - 6; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            if ((board[i][j] == EMPTY && board[i + 5][j] == opponent) ||
                (board[i][j] == opponent && board[i + 5][j] == EMPTY)) {
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
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i <= SIZE - 6; i++) {
        for (int j = 0; j <= SIZE - 6; j++) {
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
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 5; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 6; j++) {
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
                cnt++;
            }
        }
    }

    return cnt;
}

int BreakDeathFourCount(int who) {
    int opponent = get_next_player(who);
    int cnt = 0;

    // Goal = 0111010, 2111010, 0111012 - 0111100 etc.
    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i][j + 6] == EMPTY)) {
                if ((board[i][j + 1] & board[i][j + 2] & board[i][j + 3] & board[i][j + 4]) ||
                    (board[i][j + 5] & board[i][j + 2] & board[i][j + 3] & board[i][j + 4])) {
                    continue;
                }
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
    for (int i = 0; i <= SIZE - 7; i++) {
        for (int j = 0; j < SIZE; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i + 6][j] == EMPTY) ||
                (board[i][j] == EMPTY && board[i + 6][j] == opponent) ||
                (board[i][j] == opponent && board[i + 6][j] == EMPTY)) {
                if ((board[i + 1][j] & board[i + 2][j] & board[i + 3][j] & board[i + 4][j]) ||
                    (board[i + 5][j] & board[i + 2][j] & board[i + 3][j] & board[i + 4][j])) {
                    continue;
                }
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
    for (int i = 0; i <= SIZE - 7; i++) {
        for (int j = 0; j <= SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i + 6][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i + 6][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i + 6][j + 6] == EMPTY)) {
                if ((board[i + 1][j + 1] & board[i + 2][j + 2] & board[i + 3][j + 3] & board[i + 4][j + 4]) ||
                    (board[i + 5][j + 5] & board[i + 2][j + 2] & board[i + 3][j + 3] & board[i + 4][j + 4])) {
                    continue;
                }
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
        for (int j = 0; j <= SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i - 6][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i - 6][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i - 6][j + 6] == EMPTY)) {
                if ((board[i - 1][j + 1] & board[i - 2][j + 2] & board[i - 3][j + 3] & board[i - 4][j + 4]) ||
                    (board[i - 5][j + 5] & board[i - 2][j + 2] & board[i - 3][j + 3] & board[i - 4][j + 4])) {
                    continue;
                }
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
        for (int j = 0; j <= SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                if (j >= 1 && board[i][j - 1] == who) {
                    continue;
                }
                if (j < SIZE - 5 && board[i][j + 5] == who) {
                    continue;
                }
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i <= SIZE - 5; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                if (i >= 1 && board[i - 1][j] == who) {
                    continue;
                }
                if (i < SIZE - 5 && board[i + 5][j] == who) {
                    continue;
                }
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i <= SIZE - 5; i++) {
        for (int j = 0; j <= SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i + k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                if (i >= 1 && j >= 1 && board[i - 1][j - 1] == who) {
                    continue;
                }
                if (i < SIZE - 5 && j < SIZE - 5 && board[i + 5][j + 5] == who) {
                    continue;
                }
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 4; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 5; j++) {
            bool ok = true;
            for (int k = 0; k < 5; k++) {
                if (board[i - k][j + k] != goal[k]) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                if (i < SIZE - 1 && j >= 1 && board[i + 1][j - 1] == who) {
                    continue;
                }
                if (i >= 5 && j < SIZE - 5 && board[i - 5][j + 5] == who) {
                    continue;
                }
                cnt++;
            }
        }
    }

    return cnt;
}

int OneSpotThreeCount(int who) {
    // Goal = 011010, since only mid spot is worth choosing, down scale its scoring
    int cnt = 0;
    int opponent = get_next_player(who);

    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 6; j++) {
            int myDotCnt = 0;
            if (board[i][j] == EMPTY && board[i][j + 5] == EMPTY) {
                if ((board[i][j + 1] & board[i][j + 2] & board[i][j + 3]) ||
                    (board[i][j + 4] & board[i][j + 2] & board[i][j + 3])) {
                    continue;
                }
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
    for (int i = 0; i <= SIZE - 6; i++) {
        for (int j = 0; j < SIZE; j++) {
            int myDotCnt = 0;
            if (board[i][j] == EMPTY && board[i + 5][j] == EMPTY) {
                if ((board[i + 1][j] & board[i + 2][j] & board[i + 3][j]) ||
                    (board[i + 4][j] & board[i + 2][j] & board[i + 3][j])) {
                    continue;
                }
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
    for (int i = 0; i <= SIZE - 6; i++) {
        for (int j = 0; j <= SIZE - 6; j++) {
            int myDotCnt = 0;
            if (board[i][j] == EMPTY && board[i + 5][j + 5] == EMPTY) {
                if ((board[i + 1][j + 1] & board[i + 2][j + 2] & board[i + 3][j + 3]) ||
                    (board[i + 4][j + 4] & board[i + 2][j + 2] & board[i + 3][j + 3])) {
                    continue;
                }
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
        for (int j = 0; j <= SIZE - 6; j++) {
            int myDotCnt = 0;
            if (board[i][j] == EMPTY && board[i - 5][j + 5] == EMPTY) {
                if ((board[i - 1][j + 1] & board[i - 2][j + 2] & board[i - 3][j + 3]) ||
                    (board[i - 4][j + 4] & board[i - 2][j + 2] & board[i - 3][j + 3])) {
                    continue;
                }
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
        for (int j = 0; j <= SIZE - 5; j++) {
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
                if (board[i][j] == EMPTY) {
                    if (j >= 1 && board[i][j - 1] == who) {
                        continue;
                    }
                }
                if (board[i][j + 4] == EMPTY) {
                    if (j < SIZE - 5 && board[i][j + 5] == who) {
                        continue;
                    }
                }
                cnt++;
            }
        }
    }

    // Check vertically
    for (int i = 0; i <= SIZE - 5; i++) {
        for (int j = 0; j < SIZE; j++) {
            bool ok = true;
            if ((board[i][j] == EMPTY && board[i + 4][j] == opponent) ||
                (board[i][j] == opponent && board[i + 4][j] == EMPTY)) {
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
                if (board[i][j] == EMPTY) {
                    if (i >= 1 && board[i - 1][j] == who) {
                        continue;
                    }
                }
                if (board[i + 4][j] == EMPTY) {
                    if (i < SIZE - 5 && board[i + 5][j] == who) {
                        continue;
                    }
                }
                cnt++;
            }
        }
    }

    // Check diagonally "\"
    for (int i = 0; i <= SIZE - 5; i++) {
        for (int j = 0; j <= SIZE - 5; j++) {
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
                if (board[i][j] == EMPTY) {
                    if (i >= 1 && j >= 1 && board[i - 1][j - 1] == who) {
                        continue;
                    }
                }
                if (board[i + 4][j + 4] == EMPTY) {
                    if (i < SIZE - 5 && j < SIZE - 5 && board[i + 5][j + 5] == who) {
                        continue;
                    }
                }
                cnt++;
            }
        }
    }

    // Check diagonally "/"
    for (int i = 4; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 5; j++) {
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
                if (board[i][j] == EMPTY) {
                    if (i < SIZE - 1 && j >= 1 && board[i + 1][j - 1] == who) {
                        continue;
                    }
                }
                if (board[i + 4][j + 4] == EMPTY) {
                    if (i > 4 && j < SIZE - 5 && board[i - 5][j + 5] == who) {
                        continue;
                    }
                }
                cnt++;
            }
        }
    }

    // Goal = 211010, 011012 - 011010
    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 6; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i][j + 5] == EMPTY)) {
                if ((board[i][j + 1] & board[i][j + 2] & board[i][j + 3]) ||
                    (board[i][j + 4] & board[i][j + 2] & board[i][j + 3])) {
                    continue;
                }
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
    for (int i = 0; i <= SIZE - 6; i++) {
        for (int j = 0; j < SIZE; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i + 5][j] == opponent) ||
                (board[i][j] == opponent && board[i + 5][j] == EMPTY)) {
                if ((board[i + 1][j] & board[i + 2][j] & board[i + 3][j]) ||
                    (board[i + 4][j] & board[i + 2][j] & board[i + 3][j])) {
                    continue;
                }
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
    for (int i = 0; i <= SIZE - 6; i++) {
        for (int j = 0; j <= SIZE - 6; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i + 5][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i + 5][j + 5] == EMPTY)) {
                if ((board[i + 1][j + 1] & board[i + 2][j + 2] & board[i + 3][j + 3]) ||
                    (board[i + 4][j + 4] & board[i + 2][j + 2] & board[i + 3][j + 3])) {
                    continue;
                }
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
        for (int j = 0; j <= SIZE - 6; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i - 5][j + 5] == opponent) ||
                (board[i][j] == opponent && board[i - 5][j + 5] == EMPTY)) {
                if ((board[i - 1][j + 1] & board[i - 2][j + 2] & board[i - 3][j + 3]) ||
                    (board[i - 4][j + 4] & board[i - 2][j + 2] & board[i - 3][j + 3])) {
                    continue;
                }
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
        for (int j = 0; j <= SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i][j + 6] == EMPTY)) {
                if (board[i][j + 1] != who || board[i][j + 5] != who)
                    continue;
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
    for (int i = 0; i <= SIZE - 7; i++) {
        for (int j = 0; j < SIZE; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i + 6][j] == EMPTY) ||
                (board[i][j] == EMPTY && board[i + 6][j] == opponent) ||
                (board[i][j] == opponent && board[i + 6][j] == EMPTY)) {
                if (board[i + 1][j] != who || board[i + 5][j] != who)
                    continue;
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
    for (int i = 0; i <= SIZE - 7; i++) {
        for (int j = 0; j <= SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i + 6][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i + 6][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i + 6][j + 6] == EMPTY)) {
                if (board[i + 1][j + 1] != who || board[i + 5][j + 5] != who)
                    continue;
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
        for (int j = 0; j <= SIZE - 7; j++) {
            int myDotCnt = 0;
            if ((board[i][j] == EMPTY && board[i - 6][j + 6] == EMPTY) ||
                (board[i][j] == EMPTY && board[i - 6][j + 6] == opponent) ||
                (board[i][j] == opponent && board[i - 6][j + 6] == EMPTY)) {
                if (board[i - 1][j + 1] != who || board[i - 5][j + 5] != who)
                    continue;
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

    // Prevent goal1 incoming double three
    std::list<Point> twoHeads;
    // Goal 1
    // Check horizontally
    // Check vertically
    // Check diagonally "\"
    // Check diagonally "/"
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (j <= SIZE - 4) {
                bool ok = true;
                for (int k = 0; k < 4; k++) {
                    if (board[i][j + k] != goal_1[k]) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    cnt++;
                    Point head1(i, j);
                    Point head2(i, j + 3);
                    if (find(twoHeads.begin(), twoHeads.end(), head1) != twoHeads.end() ||
                        find(twoHeads.begin(), twoHeads.end(), head2) != twoHeads.end()) {
                        cnt += 20;
                    }
                    twoHeads.push_back(head1);
                    twoHeads.push_back(head2);
                }
            }
            if (i <= SIZE - 4) {
                bool ok = true;
                for (int k = 0; k < 4; k++) {
                    if (board[i + k][j] != goal_1[k]) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    cnt++;
                    Point head1(i, j);
                    Point head2(i + 3, j);
                    if (find(twoHeads.begin(), twoHeads.end(), head1) != twoHeads.end() ||
                        find(twoHeads.begin(), twoHeads.end(), head2) != twoHeads.end()) {
                        cnt += 20;
                    }
                    twoHeads.push_back(head1);
                    twoHeads.push_back(head2);
                }
            }
            if (i <= SIZE - 4 && j <= SIZE - 4) {
                bool ok = true;
                for (int k = 0; k < 4; k++) {
                    if (board[i + k][j + k] != goal_1[k]) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    cnt++;
                    Point head1(i, j);
                    Point head2(i + 3, j + 3);
                    if (find(twoHeads.begin(), twoHeads.end(), head1) != twoHeads.end() ||
                        find(twoHeads.begin(), twoHeads.end(), head2) != twoHeads.end()) {
                        cnt += 20;
                    }
                    twoHeads.push_back(head1);
                    twoHeads.push_back(head2);
                }
            }
            if (i >= 3 && j <= SIZE - 4) {
                bool ok = true;
                for (int k = 0; k < 4; k++) {
                    if (board[i - k][j + k] != goal_1[k]) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    cnt++;
                    Point head1(i, j);
                    Point head2(i - 3, j + 3);
                    if (find(twoHeads.begin(), twoHeads.end(), head1) != twoHeads.end() ||
                        find(twoHeads.begin(), twoHeads.end(), head2) != twoHeads.end()) {
                        cnt += 20;
                    }
                    twoHeads.push_back(head1);
                    twoHeads.push_back(head2);
                }
            }
            
        }
    }

    // Goal 2
    // Check horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - 5; j++) {
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
    for (int i = 0; i <= SIZE - 5; i++) {
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
    for (int i = 0; i <= SIZE - 5; i++) {
        for (int j = 0; j <= SIZE - 5; j++) {
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
        for (int j = 0; j <= SIZE - 5; j++) {
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
        for (int j = 0; j <= SIZE - 6; j++) {
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
    for (int i = 0; i <= SIZE - 6; i++) {
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
    for (int i = 0; i <= SIZE - 6; i++) {
        for (int j = 0; j <= SIZE - 6; j++) {
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
        for (int j = 0; j <= SIZE - 6; j++) {
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

    // Process during MiniMax();
    /*
    tmpCnt = FiveCount(who);
    score += tmpCnt * fiveValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", FiveCount: " << tmpCnt;

    if (score >= fiveValue)
        return score;
    */

    tmpCnt = FourCount(who);
    score += tmpCnt * fourValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", FourCount: " << tmpCnt;

    if (score >= fourValue)
        return score;

    tmpCnt = DeathFourCount(who);
    score += tmpCnt * deathFourValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", DeathFourCount: " << tmpCnt;

    tmpCnt = BreakDeathFourCount(who);
    score += tmpCnt * breakDeathFourValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", BreakDeathFourCount: " << tmpCnt;

    tmpCnt = ThreeCount(who);
    score += tmpCnt * threeValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", ThreeCount: " << tmpCnt;

    tmpCnt = OneSpotThreeCount(who);
    score += tmpCnt * oneSpotThreeValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", OneSpotThreeCount: " << tmpCnt;

    tmpCnt = DeathThreeCount(who);
    score += tmpCnt * deathThreeValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", DeathThreeCount: " << tmpCnt;

    tmpCnt = TwoCount(who);
    score += tmpCnt * twoValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", TwoCount: " << tmpCnt;

    tmpCnt = OneCount(who);
    score += tmpCnt * oneValue;
    if (DEBUG && tmpCnt > 0)
        std::cout << ", OneCount: " << tmpCnt;

    if (DEBUG)
        std::cout << "\n";

    return score;
}

int Evaluate(int who, bool maximizingPlayer) {
    int myScore = OverallScore(who);
    int opponentScore = OverallScore(get_next_player(who));

    int evaluation = myScore - opponentScore;

    // Fix starting
    if (opponentScore > 0) {
        evaluation++;
    }

    if (!maximizingPlayer) {
        // No need to check zero step win
        // Check one step win
        if (opponentScore >= fourValue) {
            evaluation -= fiveValue;
        }
        else if (opponentScore >= deathFourValue) {
            evaluation -= fiveValue;
        }
        // Check more steps win
        // idk
        else if (opponentScore >= threeValue) {
            if (myScore <= deathFourValue) {
                evaluation -= fourValue;
            }
        }
    }
    else {
        // No need to check zero step win
        // Check one step win
        if (myScore >= fourValue) {
            evaluation += fiveValue;
        }
        else if (myScore >= deathFourValue) {
            evaluation += fiveValue;
        }
        // Check more steps win
        // idk
        else if (myScore >= threeValue) {
            if (opponentScore <= deathFourValue) {
                evaluation += fourValue;
            }
        }
    }

    return evaluation;
}

//---------------------------------------------------------------------------------------------------------------//

std::pair<int, Point> AlphaBeta(int depth, int alpha, int beta, int who) {
    tried++;

    if (depth == 0) {
        return { 0, Point(-1, -1) };
    }

    std::list<Point> places = GeneratePlaces();
    OrderMoves(places, who);

    if (places.empty()) {
        return { 0, Point(7, 7) };
    }

    Point PlacePoint;
    for (Point p : places) {
        if (DEBUG)
            std::cout << "Trying " << (who == 1 ? "O" : "X") << " on (" << p.x << ' ' << p.y << ")\n";
        set_disc(p, who);
        int evaluation = -AlphaBeta(depth - 1, -beta, -alpha, get_next_player(who)).first;
        set_disc(p, EMPTY);
        if (evaluation >= beta) {
            return { beta, p };
        }
        if (evaluation > alpha) {
            alpha = evaluation;
            PlacePoint = p;
        }
    }

    return { alpha, PlacePoint };
}

//---------------------------------------------------------------------------------------------------------------//

void OrderMoves(std::list<Point>& places, int who) {
    int opponent = get_next_player(who);

    for (Point& p : places) {
        set_disc(p, who);
        int heuristic = 0;

        std::array<std::array<int, 9>, 4> star;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 9; j++) {
                star[i][j] = -1;
            }
        }
        for (int i = -4; i <= 4; i++) {
            if (is_spot_on_board(Point(p.x + i, p.y)))
                star[0][i + 4] = board[p.x + i][p.y];

            if (is_spot_on_board(Point(p.x + i, p.y + i)))
                star[1][i + 4] = board[p.x + i][p.y + i];

            if (is_spot_on_board(Point(p.x, p.y + i)))
                star[2][i + 4] = board[p.x][p.y + i];

            if (is_spot_on_board(Point(p.x - i, p.y + i)))
                star[3][i + 4] = board[p.x - i][p.y + i];
        }

        // 11111
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 5; j++) {
                if (star[i][j] == star[i][j + 1] &&
                    star[i][j + 1] == star[i][j + 2] &&
                    star[i][j + 2] == star[i][j + 3] &&
                    star[i][j + 3] == star[i][j + 4] &&
                    star[i][j + 4] == who)
                    heuristic += fiveValue;
            }
        }

        // 011110
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (star[i][j] == EMPTY && star[i][j + 5] == EMPTY)
                    if (star[i][j + 1] == star[i][j + 2] &&
                        star[i][j + 2] == star[i][j + 3] &&
                        star[i][j + 3] == star[i][j + 4] &&
                        star[i][j + 4] == who)
                        heuristic += fourValue;
            }
        }

        // 011112
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if ((star[i][j] == EMPTY && star[i][j + 5] == opponent) ||
                    (star[i][j] == opponent && star[i][j + 5] == EMPTY)) {
                    if (star[i][j + 1] == star[i][j + 2] &&
                        star[i][j + 2] == star[i][j + 3] &&
                        star[i][j + 3] == star[i][j + 4] &&
                        star[i][j + 4] == who)
                        heuristic += deathFourValue;
                }
            }
        }

        // Goal = 0111010, 2111010, 0111012 - 0111100 etc.
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 3; j++) {
                int myDotCnt = 0;
                if ((star[i][j] == EMPTY && star[i][j + 6] == EMPTY) ||
                    (star[i][j] == EMPTY && star[i][j + 6] == opponent) ||
                    (star[i][j] == opponent && star[i][j + 6] == EMPTY)) {
                    if ((star[i][j + 1] & star[i][j + 2] & star[i][j + 3] & star[i][j + 4]) ||
                        (star[i][j + 5] & star[i][j + 2] & star[i][j + 3] & star[i][j + 4])) {
                        continue;
                    }
                    for (int k = 0; k < 5; k++) {
                        if (star[i][j + 1 + k] == opponent) {
                            myDotCnt = 0;
                            break;
                        }
                        if (star[i][j + 1 + k] == who) {
                            myDotCnt++;
                        }
                    }
                }
                if (myDotCnt == 4) {
                    heuristic += deathFourValue;
                }
            }
        }

        // 01110
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 5; j++) {
                if (star[i][j] == EMPTY && star[i][j + 4] == EMPTY)
                    if (star[i][j + 1] == star[i][j + 2] &&
                        star[i][j + 2] == star[i][j + 3] &&
                        star[i][j + 3] == who)
                        heuristic += threeValue;
            }
        }

        // 011010
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int myDotCnt = 0;
                if (star[i][j] == EMPTY && star[i][j + 5] == EMPTY) {
                    if ((star[i][j + 1] & star[i][j + 2] & star[i][j + 3]) ||
                        (star[i][j + 4] & star[i][j + 2] & star[i][j + 3])) {
                        continue;
                    }
                    for (int k = 0; k < 4; k++) {
                        if (star[i][j + 1 + k] == opponent) {
                            myDotCnt = 0;
                            break;
                        }
                        if (star[i][j + 1 + k] == who) {
                            myDotCnt++;
                        }
                    }
                }
                if (myDotCnt == 3) {
                    heuristic += threeValue;
                }
            }
        }

        // 21110
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 5; j++) {
                if ((star[i][j] == EMPTY && star[i][j + 4] == opponent) ||
                    (star[i][j] == opponent && star[i][j + 4] == EMPTY)) {
                    if (star[i][j + 1] == star[i][j + 2] &&
                        star[i][j + 2] == star[i][j + 3] &&
                        star[i][j + 3] == who)
                        heuristic += deathThreeValue;
                }
            }
        }

        // 211010 - 011010
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                int myDotCnt = 0;
                if ((star[i][j] == EMPTY && star[i][j + 5] == opponent) ||
                    (star[i][j] == opponent && star[i][j + 5] == EMPTY)) {
                    if ((star[i][j + 1] & star[i][j + 2] & star[i][j + 3]) ||
                        (star[i][j + 4] & star[i][j + 2] & star[i][j + 3])) {
                        continue;
                    }
                    for (int k = 0; k < 4; k++) {
                        if (star[i][j + 1 + k] == opponent) {
                            myDotCnt = 0;
                            break;
                        }
                        if (star[i][j + 1 + k] == who) {
                            myDotCnt++;
                        }
                    }
                }
                if (myDotCnt == 3) {
                    heuristic += deathThreeValue;
                }
            }
        }

        // 0110010, 2110010, 0110012
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 3; j++) {
                int myDotCnt = 0;
                if ((star[i][j] == EMPTY && star[i][j + 6] == EMPTY) ||
                    (star[i][j] == EMPTY && star[i][j + 6] == opponent) ||
                    (star[i][j] == opponent && star[i][j + 6] == EMPTY)) {
                    if ((star[i][j + 1] & star[i][j + 2] & star[i][j + 3]) ||
                        (star[i][j + 4] & star[i][j + 2] & star[i][j + 3]) ||
                        (star[i][j + 4] & star[i][j + 5] & star[i][j + 3])) {
                        continue;
                    }
                    for (int k = 0; k < 5; k++) {
                        if (star[i][j + 1 + k] == opponent) {
                            myDotCnt = 0;
                            break;
                        }
                        if (star[i][j + 1 + k] == who) {
                            myDotCnt++;
                        }
                    }
                }
                if (myDotCnt == 3) {
                    heuristic += deathThreeValue;
                }
            }
        }

        // Goal = 0110, 01010, 010010
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 6; j++) {
                if (star[i][j] == EMPTY && star[i][j + 3] == EMPTY)
                    if (star[i][j + 1] == star[i][j + 2] &&
                        star[i][j + 2] == who)
                        heuristic += twoValue;
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 5; j++) {
                if (star[i][j] == EMPTY && star[i][j + 2] == EMPTY && star[i][j + 4] == EMPTY)
                    if (star[i][j + 1] == star[i][j + 3] &&
                        star[i][j + 3] == who)
                        heuristic += twoValue;
            }
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (star[i][j] == EMPTY && star[i][j + 2] == EMPTY && star[i][j + 3] == EMPTY && star[i][j + 5] == EMPTY)
                    if (star[i][j + 1] == star[i][j + 4] &&
                        star[i][j + 4] == who)
                        heuristic += twoValue;
            }
        }

        p.h = heuristic;
        set_disc(p, EMPTY);
    }
    places.sort(PointGreater());
}

//---------------------------------------------------------------------------------------------------------------//

int MiniMax(int depth, int alpha, int beta, bool maximizingPlayer) {
    tried++;

    int who = (maximizingPlayer == true ? player : get_next_player(player));

    if (depth == 0) {
        return Evaluate(player,  maximizingPlayer);
    }

    std::list<Point> places = GeneratePlaces();
    OrderMoves(places, who);

    if (places.empty()) {
        PlacePoint = Point(7, 7);
        return 0;
    }

    if (maximizingPlayer) {
        int maxEval = -INF;
        Point place(-1, -1);
        for (Point p : places) {
            set_disc(p, who);
            int evaluation;

            int myResult = FiveCount(player);
            if (myResult > 0){
                evaluation = fiveValue + depth;
//                set_disc(p, EMPTY);
//                if(depth == NOWDEPTH){
//                    PlacePoint = p;
//                }
//                return INF*2;
            }
            else{
                evaluation = MiniMax(depth - 1, alpha, beta, false);
            }

            set_disc(p, EMPTY);
            //maxEval = max(maxEval, evaluation);
            if (evaluation > maxEval) {
                maxEval = evaluation;
                place = p;
            }
            alpha = std::max(alpha, evaluation);
            if (beta <= alpha)
                break;
        }
        if (depth == NOWDEPTH) {
            PlacePoint = place;
        }
        return maxEval;
    }
    else {
        int minEval = INF;
        Point place(-1, -1);
        for (Point p : places) {
            set_disc(p, who);
            int evaluation;

            int myResult = FiveCount(get_next_player(player));
            if (myResult > 0){
                evaluation = -fiveValue - depth;
//                set_disc(p, EMPTY);
//                if(depth == NOWDEPTH){
//                    PlacePoint = p;
//                }
//                return -INF*2;
            }
            else{
                evaluation = MiniMax(depth - 1, alpha, beta, true);
            }

            set_disc(p, EMPTY);
            //minEval = min(minEval, evaluation);
            if (evaluation < minEval) {
                minEval = evaluation;
                place = p;
            }
            beta = std::min(beta, evaluation);
            if (beta <= alpha)
                break;
        }
        if (depth == NOWDEPTH) {
            PlacePoint = place;
        }
        return minEval;
    }
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

    for (NOWDEPTH = SAFEDEPTH;NOWDEPTH <= MXDEPTH; NOWDEPTH++) {
        int result = MiniMax(NOWDEPTH, -INF, INF, true);
        std::cout << "Result: " << result << ", " << "Tried: " << tried << "\n" ;
        
        if (PlacePoint.x == -1 && PlacePoint.y == -1) {
            std::cout << "No fout!\n";
        }
        else {
            fout << PlacePoint.x << " " << PlacePoint.y << std::endl;
            fout.flush();
        }

        std::cout << "Depth: " << NOWDEPTH << ", (" << PlacePoint.x << " " << PlacePoint.y << ")\n";
    }
    //std::cout << result.first << '\n';

    //if (DEBUG)
    //std::cout << "Tried: " << tried << '\n';
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

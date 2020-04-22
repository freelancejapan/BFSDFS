#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <random>
#include <algorithm>

#define RowCount 6
#define ColCount 6

//horizontal move
#define H 1

//vertical move
#define V 2

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine s(seed);

struct block {
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;
    int direction = V;
    bool isTarget = false;

    block(int px1, int py1, int px2, int py2, int pdirection, bool targetFlag) {
        x1 = px1;
        x2 = px2;
        y1 = py1;
        y2 = py2;
        direction = pdirection;
        isTarget = targetFlag;
    }
};

struct node {
    int CurrentView[RowCount][ColCount] = {};
    int level = 0;
    std::vector<block> vecs;
    node *parent = nullptr;

    std::vector<node *> children;
    // tree branch reach end
    bool isDeadNode = false;
};

//randomly get all next state
void iterateAllRandNext(node &n) {
    int vecsize = n.vecs.size();
    std::vector<int> idxArr;
    for (int i = 0; i < vecsize; i++) {
        idxArr.push_back(i);
    }
    shuffle(idxArr.begin(), idxArr.end(), s);
    for (int i = 0; i < vecsize; i++) {
        int idx = idxArr[i];
        block &target = n.vecs[idx];
        //vertical move
        if (target.direction == V) {
            std::vector<int> directions;
            //move top
            if (target.y1 > 0 and n.CurrentView[target.y1 - 1][target.x1] == 0) {
                directions.push_back(-1);
            }
            //move bottom
            if (target.y2 < RowCount - 1 and n.CurrentView[target.y2 + 1][target.x1] == 0) {
                directions.push_back(1);
            }
            //shuffle directions
            if (directions.size() == 2) {
                shuffle(directions.begin(), directions.end(), s);
            }
            //create child and append to childlist
            if (directions.size() > 0) {
                for (int d = 0; d < directions.size(); d++) {
                    node *tmp = new node;
                    tmp->parent = &n;
                    tmp->level = n.level + 1;
                    for (int tmpi = 0; tmpi < n.vecs.size(); tmpi++) {
                        tmp->vecs.push_back(n.vecs[tmpi]);
                        if (tmpi == idx) {
                            tmp->vecs[tmpi].y1 += d;
                            tmp->vecs[tmpi].y2 += d;
                        }
                    }
                    n.children.push_back(tmp);
                }
            }
        } else {
            //horizontal move
            std::vector<int> directions;
            //move left
            if (target.x1 > 0 and n.CurrentView[target.y1][target.x1 - 1] == 0) {
                directions.push_back(-1);
            }
            //move right
            if (target.x2 < ColCount - 1 and n.CurrentView[target.y1][target.x2 + 1] == 0) {
                directions.push_back(1);
            }
            //shuffle directions
            if (directions.size() == 2) {
                shuffle(directions.begin(), directions.end(), s);
            }
            //create child and append to childlist
            if (directions.size() > 0) {
                for (int d = 0; d < directions.size(); d++) {
                    node *tmp = new node;
                    tmp->parent = &n;
                    tmp->level = n.level + 1;
                    for (int tmpi = 0; tmpi < n.vecs.size(); tmpi++) {
                        tmp->vecs.push_back(n.vecs[tmpi]);
                        if (tmpi == idx) {
                            tmp->vecs[tmpi].x1 += d;
                            tmp->vecs[tmpi].x2 += d;
                        }
                    }
                    n.children.push_back(tmp);
                }
            }
        }
    }
}

//set view matrix
void setView(node &n) {
//    for (int row = 0; row < RowCount; row++) {
//        for (int col = 0; col < ColCount; col++) {
//            n.CurrentView[row][col] = 0;
//        }
//    }
    for (int i = 0; i < n.vecs.size(); i++) {
        if (n.vecs[i].direction == V) {
            if (n.vecs[i].isTarget) {
                for (int j = n.vecs[i].y1; j <= n.vecs[i].y2; j++) {
                    n.CurrentView[j][n.vecs[i].x1] += 3;
                }
            } else {
                for (int j = n.vecs[i].y1; j <= n.vecs[i].y2; j++) {
                    n.CurrentView[j][n.vecs[i].x1] += 2;
                }
            }

        } else {
            for (int j = n.vecs[i].x1; j <= n.vecs[i].x2; j++) {
                n.CurrentView[n.vecs[i].y1][j] += 1;
            }
        }
    }
}

void print(node &n) {
    std::cout << "Current Level : " << n.level << std::endl;
    std::cout << "Vector  List  : [ ";
    for (int i = 0; i < n.vecs.size(); i++) {
        if (n.vecs[i].direction == V) {
            std::cout << n.vecs[i].x1 << "|" << n.vecs[i].y1 << "-" << n.vecs[i].y2 << " ";
        } else {
            std::cout << n.vecs[i].x1 << "-" << n.vecs[i].x2 << "|" << n.vecs[i].y1 << " ";
        }
    }
    std::cout << "]" << std::endl;
    std::cout << "View  Matrix  : " << std::endl;
    for (int i = 0; i < RowCount; i++) {
        std::cout << "              ";
        for (int j = 0; j < ColCount; j++) {
            std::cout << n.CurrentView[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    node *test = new node;
    test->vecs.push_back(block(0, 0, 0, 1, V, false));
    test->vecs.push_back(block(3, 3, 5, 3, H, false));
    setView(*test);
    print(*test);
    return 0;
}

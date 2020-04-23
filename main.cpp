#include <iostream>
#include <vector>
#include <array>
#include <chrono>
#include <random>
#include <algorithm>

#include <opencv2/opencv.hpp>

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
                    }
                    tmp->vecs[idx].y1 += directions[d];
                    tmp->vecs[idx].y2 += directions[d];
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
                    }
                    tmp->vecs[idx].x1 += directions[d];
                    tmp->vecs[idx].x2 += directions[d];
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

bool isNodeEqual(node *n1, node *n2) {
    if (n1->vecs.size() != n2->vecs.size()) {
        return false;
    }
    for (int i = 0; i < n1->vecs.size(); i++) {
        if (n1->vecs[i].direction != n2->vecs[i].direction
            or n1->vecs[i].x1 != n2->vecs[i].x1
            or n1->vecs[i].x2 != n2->vecs[i].x2
            or n1->vecs[i].y1 != n2->vecs[i].y1
            or n1->vecs[i].y2 != n2->vecs[i].y2) {
            return false;
        }
    }

    return true;
}

void print(node &n) {
    if (n.parent == nullptr) {
        std::cout << "Parent : null " << std::endl;
    } else {
        std::cout << "Parent : " << n.parent << std::endl;
    }
    std::cout << "Children count : " << n.children.size() << std::endl;
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

bool isNodeExistInList(std::vector<node *> &duplist, node *n) {
    for (int i = 0; i < duplist.size(); i++) {
        if (isNodeEqual(duplist[i], n)) {
            return true;
        }
    }
    return false;
}

node *findFromTopToBottom2(std::vector<node *> &duplist, int level) {
    for (int i = 0; i < duplist.size(); i++) {
        if (duplist[i]->level == level and duplist[i]->isDeadNode == false
            and duplist[i]->children.size() == 0) {
            return duplist[i];
        }
    }
    return nullptr;
}

node *findFromToToBottom(node *rootNode, int level) {
//    if (rootNode->level == level) {
//        if (rootNode->isDeadNode != true
//            and rootNode->children.size() == 0) {
//            return rootNode;
//        }
//    }
//    //search level less than level
//    if (rootNode->level >= level) {
//        return nullptr;
//    }
    for (int i = 0; i < rootNode->children.size(); i++) {
        if (rootNode->children[i]->level < level) {
            if (rootNode->children[i]->isDeadNode != true
                and rootNode->children[i]->children.size() > 0) {
                node *res = findFromToToBottom(rootNode->children[i], level);
                if (res != nullptr) {
                    return res;
                }
            }
        } else {
            if (rootNode->children[i]->isDeadNode != true
                and rootNode->children[i]->children.size() == 0) {
                return rootNode->children[i];
            }
        }
    }
    return nullptr;
}

void DisplayNode(node *pnode) {
    cv::Mat img = cv::Mat::zeros(670, 670, CV_8UC3);
    //randu(img, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
    for (int i = 0; i < pnode->vecs.size(); i++) {
        if (pnode->vecs[i].direction == H) {
            //vertical
            cv::rectangle(img,
                          cv::Point(pnode->vecs[i].x1 * 110 + 10, pnode->vecs[i].y1 * 110 + 10),
                          cv::Point(pnode->vecs[i].x1 * 110 + 10 +
                                    (pnode->vecs[i].x2 - pnode->vecs[i].x1 + 1) * 100 +
                                    (pnode->vecs[i].x2 - pnode->vecs[i].x1) * 10,
                                    pnode->vecs[i].y1 * 110 + 10 + 100),
                          cv::Scalar(0, 255, 0),
                          -1);
        } else {
            cv::Scalar color(0, 255, 255);
            if (pnode->vecs[i].isTarget) {
                color = cv::Scalar(0, 0, 255);
            }
            cv::rectangle(img,
                          cv::Point(pnode->vecs[i].x1 * 110 + 10, pnode->vecs[i].y1 * 110 + 10),
                          cv::Point(pnode->vecs[i].x1 * 110 + 10 + 100,
                                    pnode->vecs[i].y1 * 110 + 10 +
                                    (pnode->vecs[i].y2 - pnode->vecs[i].y1 + 1) * 100 +
                                    (pnode->vecs[i].y2 - pnode->vecs[i].y1) * 10),
                          color,
                          -1);
        }
    }
    cv::imshow("CV Image", img);
    if (cv::waitKey(0) >= 0) return;
}

void DisplayNode(node *pnode, node *pnode2) {
    cv::Mat img = cv::Mat::zeros(670, 670, CV_8UC3);
    //randu(img, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
    for (int i = 0; i < pnode->vecs.size(); i++) {
        if (pnode->vecs[i].direction == H) {
            //vertical
            cv::rectangle(img,
                          cv::Point(pnode->vecs[i].x1 * 110 + 10, pnode->vecs[i].y1 * 110 + 10),
                          cv::Point(pnode->vecs[i].x1 * 110 + 10 +
                                    (pnode->vecs[i].x2 - pnode->vecs[i].x1 + 1) * 100 +
                                    (pnode->vecs[i].x2 - pnode->vecs[i].x1) * 10,
                                    pnode->vecs[i].y1 * 110 + 10 + 100),
                          cv::Scalar(0, 255, 0),
                          -1);
        } else {
            cv::Scalar color(0, 255, 255);
            if (pnode->vecs[i].isTarget) {
                color = cv::Scalar(0, 0, 255);
            }
            cv::rectangle(img,
                          cv::Point(pnode->vecs[i].x1 * 110 + 10, pnode->vecs[i].y1 * 110 + 10),
                          cv::Point(pnode->vecs[i].x1 * 110 + 10 + 100,
                                    pnode->vecs[i].y1 * 110 + 10 +
                                    (pnode->vecs[i].y2 - pnode->vecs[i].y1 + 1) * 100 +
                                    (pnode->vecs[i].y2 - pnode->vecs[i].y1) * 10),
                          color,
                          -1);
        }
    }

    cv::Mat img2 = cv::Mat::zeros(670, 670, CV_8UC3);
    randu(img2, cv::Scalar(0, 0, 0), cv::Scalar(255, 255, 255));
    for (int i = 0; i < pnode2->vecs.size(); i++) {
        if (pnode2->vecs[i].direction == H) {
            //vertical
            cv::rectangle(img2,
                          cv::Point(pnode2->vecs[i].x1 * 110 + 10, pnode2->vecs[i].y1 * 110 + 10),
                          cv::Point(pnode2->vecs[i].x1 * 110 + 10 +
                                    (pnode2->vecs[i].x2 - pnode2->vecs[i].x1 + 1) * 100 +
                                    (pnode2->vecs[i].x2 - pnode2->vecs[i].x1) * 10,
                                    pnode2->vecs[i].y1 * 110 + 10 + 100),
                          cv::Scalar(0, 255, 0),
                          -1);
        } else {
            cv::Scalar color(0, 255, 255);
            if (pnode2->vecs[i].isTarget) {
                color = cv::Scalar(0, 0, 255);
            }
            cv::rectangle(img2,
                          cv::Point(pnode2->vecs[i].x1 * 110 + 10, pnode2->vecs[i].y1 * 110 + 10),
                          cv::Point(pnode2->vecs[i].x1 * 110 + 10 + 100,
                                    pnode2->vecs[i].y1 * 110 + 10 +
                                    (pnode2->vecs[i].y2 - pnode2->vecs[i].y1 + 1) * 100 +
                                    (pnode2->vecs[i].y2 - pnode2->vecs[i].y1) * 10),
                          color,
                          -1);
        }
    }
    cv::hconcat(img, img2, img);
    cv::imshow("CV Image", img);
    if (cv::waitKey(0) >= 0) return;
}

void DisplayFullTree(node *pnode) {
    node *tmp = pnode;
    std::vector<node *> list;
    while (tmp->parent != nullptr) {
        list.push_back(tmp);
        tmp = tmp->parent;
    }
    list.push_back(tmp);
    for (int i = list.size() - 1; i >= 0; i--) {
        std::cout << "Image Level " << list[i]->level << std::endl;
        DisplayNode(list[i]);
    }
}

void bfsSearch() {
    node *root = new node;
    std::vector<node *> duplist;
    std::vector<node *> createdList;

    //add root to duplicate list
    duplist.push_back(root);
    createdList.push_back(root);

    root->vecs.push_back(block(0, 1, 0, 2, V, false));
    root->vecs.push_back(block(0, 4, 0, 5, V, false));
    root->vecs.push_back(block(1, 1, 1, 2, V, false));
    root->vecs.push_back(block(2, 1, 2, 3, V, false));
    root->vecs.push_back(block(3, 0, 3, 1, V, true));

    root->vecs.push_back(block(1, 0, 2, 0, H, false));
    root->vecs.push_back(block(3, 2, 5, 2, H, false));
    root->vecs.push_back(block(0, 3, 1, 3, H, false));
    root->vecs.push_back(block(3, 3, 4, 3, H, false));
    root->vecs.push_back(block(1, 5, 3, 5, H, false));

    //simplified edition
//    root->vecs.push_back(block(0, 0, 0, 1, V, false));
//    root->vecs.push_back(block(2, 2, 2, 3, V, false));
//    root->vecs.push_back(block(3, 1, 3, 2, V, true));
//    root->vecs.push_back(block(3, 3, 5, 3, H, false));
//    root->vecs.push_back(block(2, 4, 3, 4, H, false));
//    root->vecs.push_back(block(4, 4, 5, 4, H, false));
    root->level = 0;
    root->isDeadNode = false;
    setView(*root);
    iterateAllRandNext(*root);

    //DisplayNode(root);

    //print(*root);

    //add root's all child
    for (int i = 0; i < root->children.size(); i++) {
        duplist.push_back(root->children[i]);
        createdList.push_back(root->children[i]);
        //print(*root->children[i]);
        //DisplayNode(root->children[i]->parent, root->children[i]);
    }

    int itrIdx = 0;
    node *itrNode = root->children[itrIdx];
    while (true) {
        if (itrNode->isDeadNode) {
            //unexpected case
            std::cout << "Unexpected Dead Node , Node's Level Is " << itrNode->level << std::endl;
            break;
        } else if (itrNode->children.size() == 0) {
            //fill every children
            setView(*itrNode);
            //check node found
            bool isFoundT = false;
            bool isFoundF = false;
            for (int i = RowCount - 1; i >= 0; i--) {
                if (itrNode->CurrentView[i][3] == 1 ||
                    itrNode->CurrentView[i][3] == 2) {
                    isFoundF = true;
                }
                if (itrNode->CurrentView[i][3] == 3) {
                    isFoundT = true;
                    break;
                }
            }
            if (isFoundT == true and isFoundF == false) {
                std::cout << "Found " << itrNode->level << std::endl;
                std::cout << "duplicate list length " << duplist.size() << std::endl;
                DisplayFullTree(itrNode);
                return;
            }

            iterateAllRandNext(*itrNode);
            if (itrNode->children.size() <= 0) {
                std::cout << "Iteration Fail , Node's Level Is " << itrNode->level << std::endl;
                return;
            }
            std::vector<int> rmlist;
            for (int i = 0; i < itrNode->children.size(); i++) {
                //remove duplicated
                if (isNodeExistInList(duplist, itrNode->children[i])) {
                    rmlist.push_back(i);
                }
            }
            int tmpsize = rmlist.size();
            for (int j = 0; j < tmpsize; j++) {
                delete itrNode->children[rmlist[tmpsize - 1 - j]];
                itrNode->
                        children.erase(itrNode->children.begin() + rmlist[tmpsize - 1 - j]);
            }
            if (itrNode->children.size() <= 0) {
                //all child node has been iterated
                itrNode->isDeadNode = true;
            } else {
                //add left to duplcate
                for (int j = 0; j < itrNode->children.size(); j++) {
//                    //fill every children
//                    setView(*itrNode->children[j]);
//                    //check node found
//                    bool isFoundT = false;
//                    bool isFoundF = false;
//                    int tmpsum = 0;
//                    for (int i = RowCount - 1; i >= 0; i--) {
//                        if (itrNode->children[j]->CurrentView[i][3] == 1 ||
//                            itrNode->children[j]->CurrentView[i][3] == 2) {
//                            isFoundF = true;
//                        }
//                        if (itrNode->children[j]->CurrentView[i][3] == 3) {
//                            isFoundT = true;
//                            break;
//                        }
//                    }
//                    if (isFoundT == true and isFoundF == false) {
//                        std::cout << "Found " << itrNode->level << std::endl;
//                        std::cout << "duplicate list length " << duplist.size() << std::endl;
//
//                        DisplayFullTree(itrNode->children[j]);
//                        return;
//                    }
                    duplist.push_back(itrNode->children[j]);
                    //DisplayNode(itrNode->children[j]->parent, itrNode->children[j]);
                }
            }

//            if (itrNode->level > 4) {
//                return;
//            }

            //find sibling node with level equal to itrNode
            itrIdx++;
            if (itrIdx >= itrNode->parent->children.size()) {
                node *nextNode = findFromToToBottom(root, itrNode->level);
                //node *nextNode = findFromTopToBottom2(duplist, itrNode->level);
                if (nextNode == nullptr) {
                    std::cout << "Level From " << itrNode->level << " Goes To " << itrNode->level + 1 << std::endl;
                    if (itrNode->level + 1 >= 100) {
                        //DisplayFullTree(itrNode);
                        return;
                    }
                    nextNode = findFromToToBottom(root, itrNode->level + 1);
                    //nextNode = findFromTopToBottom2(duplist, itrNode->level + 1);
                    if (nextNode == nullptr) {
                        std::cout << "Error From Level " << itrNode->level << " To " << itrNode->level + 1 << std::endl;
                        std::cout << "Duplicate Count " << duplist.size() << std::endl;
                        print(*itrNode);
                        return;
                    } else {
                        itrIdx = 0;
                        itrNode = nextNode;
                    }
                } else {
                    itrIdx = 0;
                    itrNode = nextNode;
                }
            } else {
                itrNode = itrNode->parent->children[itrIdx];
            }
        } else {
            std::cout << "Unexpected Full Node , Node's Level Is " << itrNode->level << std::endl;
            break;
        }
    }
}

int main() {
    bfsSearch();
    return 0;
}

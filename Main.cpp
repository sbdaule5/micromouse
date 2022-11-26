#include <iostream>
#include <string>
#include <queue>
#include <utility>
#include <string>
#include <vector>
#include <algorithm>

#include "API.h"

using std::pair;
using std::queue;
using std::make_pair;
using std::vector;

enum Direction{
    EAST,
    NORTH,
    WEST,
    SOUTH,
    UNINITILIZED
};
char directions[] = {'e', 'n', 'w', 's', 'u'};
class Node{
public:
    bool walls[4] = {false, false, false, false};
    int value = 0;
    bool initilized = false;
    bool visited = false;
    int x, y;
    void init(Direction facing, bool frontWall, bool leftWall, bool rightWall)
    {
        walls[facing] = frontWall;
        walls[(facing+1)%4] = leftWall;
        walls[(facing+2)%4] = false;
        walls[(facing+3)%4] = rightWall;
    }
    Node(){
    }
};
void log(const std::string& text) {
    std::cerr << text << std::endl;
}

int main(int argc, char* argv[]) {
    log("Running...");
    int x = 0;
    int y = 0;
    Direction smallest;
    Node nodes[64][64];
    for(int i = 0; i < 256; i++){
        nodes[i/16][i%16].x = i/16;
        nodes[i/16][i%16].y = i%16;
        nodes[i/16][i%16].value = ((i/16 <= 7) ? (7 - i/16) : (i/16 - 8)) + ((i%16 <= 7) ? (7 - i%16) : (i%16 - 8));
        API::setText(i/16, i%16, std::to_string(nodes[i/16][i%16].value).c_str());
    }

    Direction facing = NORTH;

    API::setColor(0, 0, 'G');
    for(int i = 0; i < 16; i++){
        nodes[0][i].walls[WEST] = true;
        nodes[0][16 - i].walls[EAST] = true;
        nodes[i][0].walls[WEST] = true;
        nodes[16 - i][i].walls[WEST] = true;
    }

    while (true) {
        nodes[x][y].init(facing, API::wallFront(), API::wallLeft(), API::wallRight());
        nodes[x][y+1].walls[SOUTH] = nodes[x][y].walls[NORTH];
        nodes[x][y-1].walls[NORTH] = nodes[x][y].walls[SOUTH];
        nodes[x+1][y].walls[WEST] = nodes[x][y].walls[EAST];
        nodes[x-1][y].walls[EAST] = nodes[x][y].walls[WEST];
        for(int i = 0; i < 4; i++){
            if (nodes[x][y].walls[i]) API::setWall(x, y, directions[i]);
        }

        // calculating next cell and moving to it
        if(!(smallest != UNINITILIZED)){
            queue<pair<int, int>> q;
            q.push(make_pair(x, y));
            q.push(make_pair(x + 1, y));
            q.push(make_pair(x - 1, y));
            q.push(make_pair(x, y + 1));
            q.push(make_pair(x, y - 1));

            do{
                pair<int, int> crNodePos = q.front();
                q.pop();
                Node& cr = nodes[crNodePos.first][crNodePos.second];
                Node* neighbours[4];
                int neighboursFound = 0;
                int minFound = cr.value;

                if((crNodePos.first == 7 || crNodePos.first == 8) && (crNodePos.second == 7 || crNodePos.second == 8)){
                    continue;
                }
                if(crNodePos.first > 15 || crNodePos.first < 0 || crNodePos.second > 15 || crNodePos.first < 0){
                    continue;
                }

                if(cr.walls[NORTH] == false){
                    Node& next = nodes[crNodePos.first][crNodePos.second+1];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }
                if(cr.walls[SOUTH] == false){
                    Node& next = nodes[crNodePos.first][crNodePos.second-1];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }
                if(cr.walls[EAST] == false){
                    Node& next = nodes[crNodePos.first+1][crNodePos.second];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }
                if(cr.walls[WEST] == false){
                    Node& next = nodes[crNodePos.first-1][crNodePos.second];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }

                // compairing neighbours
                if(minFound >= cr.value){
                    cr.value = minFound + 1;
                    API::setText(cr.x, cr.y, std::to_string(cr.value).c_str());
                    for(int j = 0; j < neighboursFound; j++){
                        q.push(make_pair(neighbours[j]->x, neighbours[j]->y));    
                    }
                }
            }while(!q.empty());
        }
        
        // move to smallest neighbouring nodes
        smallest = UNINITILIZED;
        //if no north wall then check north
        if(nodes[x][y].walls[NORTH] == false){
            if(nodes[x][y].value > nodes[x][y+1].value){
                smallest = NORTH;
            }
        }
        if(nodes[x][y].walls[SOUTH] == false){
            if(nodes[x][y].value > nodes[x][y-1].value){
                smallest = SOUTH;
            }
        }
        if(nodes[x][y].walls[EAST] == false){
            if(nodes[x][y].value > nodes[x+1][y].value){
                smallest = EAST;
            }
        }
        if(nodes[x][y].walls[WEST] == false){
            if(nodes[x][y].value > nodes[x-1][y].value){
                smallest = WEST;
            }
        }
        if(smallest == UNINITILIZED) continue;
        while (facing != smallest) {
            API::turnRight();
            facing = (Direction)((facing+3)%4);
        }
        API::moveForward();

        // update value of current position
        switch(facing){
            case SOUTH:
                y--;
                break;
            case NORTH:
                y++;
                break;
            case EAST:
                x++;
                break;
            case WEST:
                x--;
                break;
        }
        API::setColor(x, y, 'G');
        nodes[x][y].visited = true;
    }
}

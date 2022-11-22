#include <iostream>
#include <string>
#include <queue>
#include <utility>
#include <string>

#include "API.h"

using std::pair;
using std::queue;
using std::make_pair;
enum Direction{
    EAST,
    NORTH,
    WEST,
    SOUTH
};
char directions[] = {'e', 'n', 'w', 's'};
class Node{
public:
    bool walls[4] = {false, false, false, false};
    int value = 1000;
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
    Node nodes[64][64];
    for(int i = 0; i < 256; i++){
        nodes[i/16][i%16].x = i/16;
        nodes[i/16][i%16].y = i%16;
    }

    Direction facing = NORTH;

    API::setColor(0, 0, 'G');
    API::setText(0, 0, "start");
    for(int i = 0; i < 16; i++){
        nodes[0][i].walls[WEST] = true;
        nodes[0][16 - i].walls[EAST] = true;
        nodes[i][0].walls[WEST] = true;
        nodes[16 - i][i].walls[WEST] = true;
    }

    while (true) {
        nodes[x][y].init(facing, API::wallFront(), API::wallLeft(), API::wallRight());
        for(int i = 0; i < 4; i++){
            if (nodes[x][y].walls[i]) API::setWall(x, y, directions[i]);
        }


        // calculating next cell and moving to it

        // clearing all nodes
        for(int i = 0; i < 256; i++){
            nodes[i/16][i%16].initilized = false;
            nodes[i/16][i%16].value = 1000;
        }
        queue<pair<int, int>> q;
        q.push(make_pair(8, 8));
        std::cerr << q.size() << std::endl;
        nodes[7][7].value = 0;
        nodes[7][8].value = 0;
        nodes[8][7].value = 0;
        nodes[8][8].value = 0;
        while(!q.empty()){
            // std::cerr << q.size() << std::endl;
            pair<int, int> pos = q.front();
            q.pop();
            Node& cr = nodes[pos.first][pos.second];
            cr.initilized = true;
            if(cr.walls[NORTH] == false){
                Node& next = nodes[cr.x][cr.y+1];
                if (next.initilized == false){
                    if(next.value > cr.value + 1) next.value = cr.value + 1;
                    API::setText(next.x, next.y, std::to_string(next.value).c_str());
                    // std::cerr << "Pushing " << next.x << ", " << next.y << "to queue it is initilized?" << next.initilized << std::endl;
                    q.push(make_pair(next.x, next.y));
                    next.initilized = true;
                }
            }
            if(cr.walls[SOUTH] == false){
                Node& next = nodes[cr.x][cr.y-1];
                if (next.initilized == false){
                    if(next.value > cr.value + 1) next.value = cr.value + 1;
                    API::setText(next.x, next.y, std::to_string(next.value).c_str());
                    // std::cerr << "Pushing " << next.x << ", " << next.y << "to queue it is initilized?" << next.initilized << std::endl;
                    q.push(make_pair(next.x, next.y));
                }
            }
            if(cr.walls[EAST] == false){
                Node& next = nodes[cr.x+1][cr.y];
                if (next.initilized == false){
                    if(next.value > cr.value + 1) next.value = cr.value + 1;
                    API::setText(next.x, next.y, std::to_string(next.value).c_str());
                    // std::cerr << "Pushing " << next.x << ", " << next.y << "to queue it is initilized?" << next.initilized << std::endl;
                    q.push(make_pair(next.x, next.y));
                }
            }
            if(cr.walls[WEST] == false){
                Node& next = nodes[cr.x-1][cr.y];
                if (next.initilized == false){
                    if(next.value > cr.value + 1) next.value = cr.value + 1;
                    API::setText(next.x, next.y, std::to_string(next.value).c_str());
                    // std::cerr << "Pushing " << next.x << ", " << next.y << "to queue it is initilized?" << next.initilized << std::endl;
                    q.push(make_pair(next.x, next.y));
                }
            }
        }
        if (!API::wallLeft()) {
            API::turnLeft();
            facing = (Direction)((facing+1)%4);
        }
        while (API::wallFront()) {
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

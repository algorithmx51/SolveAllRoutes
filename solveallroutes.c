#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"raylib.h"
#include"raymath.h"
#include<string.h>

#define NODES 100
#define GAP 30
#define SCALE 1.7

typedef struct Map Map;
typedef struct Node Node;
typedef struct Connection Connection;
typedef struct Solution Solution;
typedef struct Path Path;

Map *getMapFromUser(Vector2);
void printMapInfo(Map*);
void drawMap(Map*, Vector2, Color);
Solution *solveMap(Map*, Node*);
int validatePath(Map*, Path*);
void drawPath(Path *, Vector2);

struct Map {

    Node *Nodes;
    int numberOfNodes;

};

struct Node {

    Vector2 offset;

    int numberOfConnections;
    Connection *connectedNodes;

};

struct Connection {

    int hasItBeenCrossed;
    Node *N;

};

struct Solution {

    int howManyPaths;
    Path **paths;

};

struct Path {

    int length;
    Node **steps;

};

Map *getMapFromUser(Vector2 pos)
{

    // Draw a map of nodes
    Map *_map = (Map *) malloc(sizeof(Map));
    int usefulNodes = 0;

    _map->Nodes = (Node *) malloc(sizeof(Node) * NODES); // Create a 10x? grid of nodes
    _map->numberOfNodes = NODES;

    for(int i = 0; i < NODES; i++) {
        _map->Nodes[i].offset.x = (i % 10) * GAP * SCALE + ((float)GAP * SCALE / 2) * ((i % 20) / 10);
        _map->Nodes[i].offset.y =  (i / 10) * sqrt(pow((float)GAP * SCALE, 2) - pow( (float) GAP * SCALE / 2, 2) );
        _map->Nodes[i].numberOfConnections = 0;
    }

    Vector2 A = {-1,-1}, B = {-1, -1};
    int skipIteration = 0;

    float minDistance = 99999;
    Node *closestNodeA = NULL, *closestNodeB = NULL;

    while(!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("Connect the nodes using LEFT CLICK", pos.x, pos.y-50, 20, WHITE);

        for(int i = 0; i < NODES; i++) {
            DrawCircleLines(pos.x + _map->Nodes[i].offset.x, pos.y + _map->Nodes[i].offset.y, (float) 3 * SCALE + 5, WHITE);
            DrawCircleLines(pos.x + _map->Nodes[i].offset.x, pos.y + _map->Nodes[i].offset.y, (float) 3 * SCALE + 4, WHITE);

            for(int j = 0; j < _map->Nodes[i].numberOfConnections; j++) {
                DrawLineEx(Vector2Add(_map->Nodes[i].connectedNodes[j].N->offset, pos), Vector2Add(_map->Nodes[i].offset, pos), (float) 3 * (SCALE / 2 + 1), RED);
            }
        }

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && (A.x == -1 && A.y == -1)) {
            A = GetMousePosition();

            for(int i = 0 ; i < NODES; i++) {
                float delta = sqrt(pow(pos.x + _map->Nodes[i].offset.x - A.x, 2) + pow(pos.y + _map->Nodes[i].offset.y - A.y, 2));
                if(delta <= minDistance) {
                    minDistance = delta;
                    closestNodeA = &_map->Nodes[i];
                }
            }

            minDistance = 99999;

            WaitTime(0.1);
        }
        else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && (A.x != -1 && A.y != -1)) {
            B = GetMousePosition();

            for(int i = 0 ; i < NODES; i++) {
                float delta = sqrt(pow(pos.x + _map->Nodes[i].offset.x - B.x, 2) + pow(pos.y + _map->Nodes[i].offset.y - B.y, 2));
                if(delta <= minDistance) {
                    minDistance = delta;
                    closestNodeB = &_map->Nodes[i];
                }
            }

            minDistance = 99999;

            WaitTime(0.1);
        }

        if(closestNodeA)
            DrawCircleV(Vector2Add(closestNodeA->offset, pos), 3 * SCALE, PURPLE);
        if(closestNodeB)
            DrawCircleV(Vector2Add(closestNodeB->offset, pos), 3 * SCALE, PURPLE);

        if(A.x != -1 && A.y != -1 && B.x != -1 && B.y != -1) {

            if(closestNodeA == closestNodeB) {
                skipIteration = 1;
            }

            for(int i = 0; i < closestNodeA->numberOfConnections; i++)
                if(closestNodeA->connectedNodes[i].N == closestNodeB) {
                    skipIteration = 1;
                    break;
                }

            if(skipIteration) {
                A.x = -1;
                A.y = -1;
                B.x = -1;
                B.y = -1;
                closestNodeA = 0;
                closestNodeB = 0;
                EndDrawing();
                skipIteration = 0;
                continue;
            }

            if(closestNodeA->numberOfConnections) {
                closestNodeA->connectedNodes = (Connection *) realloc(closestNodeA->connectedNodes, sizeof(Connection) * (closestNodeA->numberOfConnections + 1) );
                closestNodeA->connectedNodes[closestNodeA->numberOfConnections].N = closestNodeB;
                closestNodeA->connectedNodes[closestNodeA->numberOfConnections].hasItBeenCrossed = 0;
            }
            else {
                closestNodeA->connectedNodes = (Connection *) malloc(sizeof(Connection));
                closestNodeA->connectedNodes[0].N = closestNodeB;
                closestNodeA->connectedNodes[0].hasItBeenCrossed = 0;
                usefulNodes++;
            }
            if(closestNodeB->numberOfConnections) {
                closestNodeB->connectedNodes = (Connection *) realloc(closestNodeB->connectedNodes, sizeof(Connection) * (closestNodeB->numberOfConnections + 1) );
                closestNodeB->connectedNodes[closestNodeB->numberOfConnections].N = closestNodeA;
                closestNodeB->connectedNodes[closestNodeB->numberOfConnections].hasItBeenCrossed = 0;
            }
            else {
                closestNodeB->connectedNodes = (Connection *) malloc(sizeof(Connection));
                closestNodeB->connectedNodes[0].N = closestNodeA;
                closestNodeB->connectedNodes[0].hasItBeenCrossed = 0;
                usefulNodes++;
            }

            closestNodeA->numberOfConnections++;
            closestNodeB->numberOfConnections++;


            A.x = -1;
            B.x = -1;
            A.y = -1;
            B.y = -1;
            closestNodeA = 0;
            closestNodeB = 0;

        }

        EndDrawing();

    }

    Map *map = (Map *) malloc(sizeof(Map));
    map->numberOfNodes = usefulNodes;
    map->Nodes = (Node *) malloc(sizeof(Node) * usefulNodes);

    for(int i = 0, j = 0; i < _map->numberOfNodes; i++) {
        if(_map->Nodes[i].numberOfConnections) {

            map->Nodes[ j ].connectedNodes = _map->Nodes[ i ].connectedNodes;
            map->Nodes[ j ].numberOfConnections = _map->Nodes[ i ].numberOfConnections;
            map->Nodes[ j ].offset = _map->Nodes[ i ].offset;

            j++;
        }

    }

    for(int i = 0; i < map->numberOfNodes; i++) {
        for(int j = 0; j < map->Nodes[ i ].numberOfConnections; j++) {
            for(int k = 0; k < map->numberOfNodes; k++) {
                if(map->Nodes[ k ].offset.x == map->Nodes[ i ].connectedNodes[ j ].N->offset.x && map->Nodes[ k ].offset.y == map->Nodes[ i ].connectedNodes[ j ].N->offset.y)
                    map->Nodes[ i ].connectedNodes[ j ].N = &map->Nodes[ k ];
            }
        }
    }

    free(_map);

    Vector2 minOffset = {9999, 9999};
    for(int i = 0; i < map->numberOfNodes; i++) {

        if(map->Nodes[i].offset.x < minOffset.x)
            minOffset.x = map->Nodes[i].offset.x;

        if(map->Nodes[i].offset.y < minOffset.y)
            minOffset.y = map->Nodes[i].offset.y;

    }

    for(int i = 0; i < map->numberOfNodes; i++) {
        map->Nodes[ i ].offset.x -= minOffset.x;
        map->Nodes[ i ].offset.y -= minOffset.y;
    }

    return map;

}

void printMapInfo(Map *map)
{
    printf("\nDisplaying information about Map %6p (%d Nodes)\n\n", (void *) map, map->numberOfNodes);
    for(int i = 0; i < map->numberOfNodes; i++) {
        printf("Node %6p : Offset (%.2f %.2f)\nNumber of connections : %d\n", (void *) &map->Nodes[i], map->Nodes[i].offset.x, map->Nodes[i].offset.y, map->Nodes[i].numberOfConnections);
        for(int j = 0; j < map->Nodes[i].numberOfConnections; j++) {
            printf("%6p (%.2f %.2f) ", (void *) map->Nodes[i].connectedNodes[j].N, map->Nodes[i].connectedNodes[j].N->offset.x, map->Nodes[i].connectedNodes[j].N->offset.y);
        }
        puts("\n\n");
    }
}

void drawMap(Map *map, Vector2 pos, Color c)
{

    for(int i = 0; i < map->numberOfNodes; i++) {
        DrawCircleLines(map->Nodes[ i ].offset.x + pos.x, map->Nodes[ i ].offset.y + pos.y, (float) 3 * SCALE + 5, c);
        DrawCircleLines(map->Nodes[ i ].offset.x + pos.x, map->Nodes[ i ].offset.y + pos.y, (float) 3 * SCALE + 4, c);
        DrawCircle(map->Nodes[ i ].offset.x + pos.x, map->Nodes[ i ].offset.y + pos.y, SCALE * 2, c);

        for(int j = 0; j  < map->Nodes[ i ].numberOfConnections; j++) {
            DrawLineEx( Vector2Add(map->Nodes[ i ].offset, pos), Vector2Add(map->Nodes[ i ].connectedNodes[ j ].N->offset, pos), (float) 3 * (SCALE / 2 + 1), c);
        }
    }

}

void step(Solution *S, Path *P, Node *n)  // P -> path up to this step
{
    for(int i = 0; i < n->numberOfConnections; i++) {

        printf("%p %d\n", n , P->length);
        if(!n->connectedNodes[i].hasItBeenCrossed) {
            n->connectedNodes[i].hasItBeenCrossed = 1;

            for(int j = 0; j < n->connectedNodes[i].N->numberOfConnections; j++)
                if(n->connectedNodes[i].N->connectedNodes[j].N == n) {
                    n->connectedNodes[i].N->connectedNodes[j].hasItBeenCrossed = 1;
                    break;
                }

            S->howManyPaths++;
            S->paths = (Path **) realloc(S->paths, sizeof(Path *) * S->howManyPaths);
            S->paths[S->howManyPaths-1] = (Path *) malloc(sizeof(Path));
            S->paths[S->howManyPaths-1]->length = P->length + 1;
            S->paths[S->howManyPaths-1]->steps = (Node **) malloc(sizeof(Node *) * S->paths[S->howManyPaths - 1]->length);

            for(int j = 0; j < P->length; j++)
                S->paths[S->howManyPaths-1]->steps[j] = P->steps[j];

            S->paths[S->howManyPaths-1]->steps[P->length] = n->connectedNodes[i].N;

            step(S, S->paths[S->howManyPaths-1], n->connectedNodes[i].N);

            n->connectedNodes[i].hasItBeenCrossed = 0;

            for(int j = 0; j < n->connectedNodes[i].N->numberOfConnections; j++)
                if(n->connectedNodes[i].N->connectedNodes[j].N == n) {
                    n->connectedNodes[i].N->connectedNodes[j].hasItBeenCrossed = 0;
                    break;
                }
        }

    }

}

Solution *solveMap(Map *map, Node *startingNode)
{

    Solution *_solution = (Solution *) malloc(sizeof(Solution));

    if(!startingNode || !map)
        return (Solution *)NULL;

    _solution->howManyPaths = 1;
    _solution->paths = (Path **) malloc(sizeof(Path *));
    _solution->paths[0] = (Path *) malloc(sizeof(Path));
    _solution->paths[0]->length = 1;
    _solution->paths[0]->steps = (Node **)malloc(sizeof(Node *));
    _solution->paths[0]->steps[0] = startingNode;

    step(_solution, _solution->paths[0], startingNode); // take a step into the deep dark forest of recursion

    printf("Found at most %d solutions\n", _solution->howManyPaths);

    int maximumSteps = 0;
    int example;
    for(int i = 0; i < _solution->howManyPaths; i++) {
        if(_solution->paths[i]->length > maximumSteps) {
            maximumSteps = _solution->paths[i]->length;
            example = i;
        }
    }

    if(!validatePath(map, _solution->paths[example])) {
        printf("None of them are valid :(\n");
        return (Solution *)NULL;
    }

    int correctSolutions = 0;
    for(int i = 0; i < _solution->howManyPaths; i++) {
        if(_solution->paths[i]->length == maximumSteps)
            correctSolutions++;
    }

    printf("%d of them are valid\n", correctSolutions);

    Solution *solution = (Solution *) malloc(sizeof(Solution));
    solution->howManyPaths = correctSolutions;

    solution->paths = (Path **) malloc(sizeof(Path *) * correctSolutions);

    for(int i = 0, j = 0; i < _solution->howManyPaths; i++)
        if(_solution->paths[i]->length == maximumSteps) {
            solution->paths[j] = _solution->paths[i];
            j++;
        }
        else {
            free(_solution->paths[i]->steps);
            free(_solution->paths[i]);
        }

    free(_solution->paths);
    free(_solution);

    return solution;
}

int validatePath(Map *map, Path *path)
{

    Node *N;
    int isPathValid = 1;

    for(int i = 0; i < path->length-1; i++) {
        N = path->steps[i];

        for(int j = 0; j < N->numberOfConnections; j++)
            if(N->connectedNodes[j].N == path->steps[i+1]) {
                N->connectedNodes[j].hasItBeenCrossed = 1;

                for(int k = 0; k < path->steps[i+1]->numberOfConnections; k++)
                    if(path->steps[i] == path->steps[i+1]->connectedNodes[k].N)
                        path->steps[i+1]->connectedNodes[k].hasItBeenCrossed = 1;
            }
    }

    for(int i = 0; i < map->numberOfNodes; i++)
        for(int j = 0; j < map->Nodes[i].numberOfConnections; j++)
            if(!map->Nodes[i].connectedNodes[j].hasItBeenCrossed)
                isPathValid = 0;
            else
                map->Nodes[i].connectedNodes[j].hasItBeenCrossed = 0;

    return isPathValid;
}


int main(void)
{

    InitWindow(800, 800, "");
    SetTargetFPS(60);

    Map *map = getMapFromUser((Vector2) {
        100, 100
    });

    BeginDrawing();
    DrawText("OK!", 600, 700, 70, RED);
    EndDrawing();
    WaitTime(0.7);

    printMapInfo(map);

    Node *closestNode = 0;
    Vector2 a = {-1, -1};

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawText("Which should the starting node be", 100, 700, 35, BLACK);

        drawMap(map, (Vector2) {
            100, 100
        }, BLACK);

        if(closestNode) {
            DrawCircle(closestNode->offset.x + 100, closestNode->offset.y + 100, 5, BLUE);
            EndDrawing();
            BeginDrawing();
            DrawCircle(closestNode->offset.x + 100, closestNode->offset.y + 100, 5, BLUE);
            EndDrawing();
            break;
        }
        else {

            if(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && a.x == -1 && a.y == -1) {
                a = GetMousePosition();
            }

            if(a.x != -1 && a.y != -1) {

                float minDistance = 9999;

                for(int i = 0; i < map->numberOfNodes; i++) {
                    float delta = sqrt(pow(100 + map->Nodes[i].offset.x - a.x, 2) + pow(100 + map->Nodes[i].offset.y - a.y, 2));
                    if(delta < minDistance) {
                        minDistance = delta;
                        closestNode = &map->Nodes[i];
                    }
                }
            }
        }

        EndDrawing();
    }

    WaitTime(0.3);

    BeginDrawing();
    DrawText("OK! Please wait", 100, 650, 35, RED);
    EndDrawing();

    Solution *solutions = solveMap(map, closestNode);

    /*   if(solutions)
           for(int i = 0; i < solutions->howManyPaths; i++) {
               for(int j = 0; j < solutions->paths[i]->length; j++) {
                   printf("%p ", solutions->paths[i]->steps[j]);
               }
               putc('\n', stdout);
           } */

    if(!solutions)
        return -1;

    int index = 0;
    int j = 0;
    char *buffer = malloc(sizeof(char) * 26);

    while(!WindowShouldClose()) {

        int isLeftArrowPressed = 0, isRightArrowPressed = 0;
        Vector2 Position = {100, 100};

        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
            do {

                BeginDrawing();
                ClearBackground(WHITE);
                drawMap(map, Position, BLACK);
                if(!solutions)
                    DrawText("This node has no valid solutions, pick another one", 10, 700, 20, RED);
                DrawCircleV(Vector2Add(closestNode->offset, Position), 5, BLUE);
                EndDrawing();

                if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

                    a = GetMousePosition();

                    float minDistance = 9999;

                    for(int i = 0; i < map->numberOfNodes; i++) {
                        float delta = sqrt(pow(Position.x + map->Nodes[i].offset.x - a.x, 2) + pow(Position.y + map->Nodes[i].offset.y - a.y, 2));
                        if(delta < minDistance) {
                            minDistance = delta;
                            closestNode = &map->Nodes[i];
                        }
                    }

                    if(solutions) {
                        for(int i = 0; i < solutions->howManyPaths; i++) {
                            for(int j = 0; j < solutions->paths[i]->length; j++)
                                free(solutions->paths[i]->steps);
                            free(solutions->paths[i]);
                        }
                        free(solutions->paths);
                        free(solutions);
                    }

                    solutions = solveMap(map, closestNode);
                    index = 0;
                    WaitTime(0.1);
                }
            }
            while(!solutions);

        WaitTime(0.12);

        double timeNow = GetTime(), elaspedTime;

        do {

            BeginDrawing();

            ClearBackground(WHITE);
            snprintf(buffer, 26, "%d / %d", index+1, solutions->howManyPaths);

            DrawText("Navigate through the solutions with the LEFT and RIGHT ARROW keys", 10, 780, 20, BLACK);
            DrawText(buffer, 700-strlen(buffer)*10, 10, 22, BLACK);

            drawMap(map, Position, BLACK);
            DrawCircleV(Vector2Add(closestNode->offset, Position), 4 * SCALE, GREEN);
            for(int k = 0; k <= j; k++)
                DrawLineEx(Vector2Add(solutions->paths[index]->steps[k]->offset, Position), Vector2Add(solutions->paths[index]->steps[k+1]->offset, Position), 2 * SCALE, RED);

            elaspedTime = GetTime() - timeNow;
            if(elaspedTime > 0.7){
                timeNow = GetTime();
                j++;
            }

            EndDrawing();

            if(j == solutions->paths[index]->length-1){
                WaitTime(2);
                timeNow = GetTime();
                j = 0;
            }

            if(WindowShouldClose())
                return 0;

        }
        while(!IsKeyPressed(KEY_LEFT) && !IsKeyPressed(KEY_RIGHT) && !IsMouseButtonDown(MOUSE_BUTTON_LEFT));

        isLeftArrowPressed = IsKeyPressed(KEY_LEFT);
        isRightArrowPressed = IsKeyPressed(KEY_RIGHT);

        index += -1 * isLeftArrowPressed +1 *isRightArrowPressed;
        j = 0;

        if(index < 0) index = 0;
        if(index > solutions->howManyPaths-1) index = solutions->howManyPaths-1;

        glfwPollEvents();
    }

    return 0;
}

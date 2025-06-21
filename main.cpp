#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

const int scr_w = 700;
const int scr_h = 900;

const int map_w = 20;
const int map_h = 20;

const int cell_size = 26;
const int grid_thickness = 1;

struct Node {
	bool is_obstacle = false;			// is the node is obstruction?
	bool is_visited = false;			// have we visited this node before?
	// float global_goal;				// total estimated cost from start -> this node -> end, neded for A*
	float local_goal = INFINITY;		// cost from the start to this node 
	Vector2 pos;						// nodes position in 2d space
	Node* parent = nullptr;
	vector<Node*> neighbors;
	Color color = DARKBLUE;
};

// 2D map of nodes
Node map[map_h][map_w];

Node* start_node;
Node* end_node;


void init_node_network() {
	for (int y = 0; y < map_h; y++) {
		for (int x = 0; x < map_w; x++) {
			map[y][x].pos = { float(x), float(y) };
			// set neighbors
			if (x > 0) {
				map[y][x].neighbors.push_back(&map[y][x - 1]);
			}
			if (x < map_w - 1) {
				map[y][x].neighbors.push_back(&map[y][x + 1]);
			}
			if (y > 0) {
				map[y][x].neighbors.push_back(&map[y - 1][x]);
			}
			if (y < map_h - 1) {
				map[y][x].neighbors.push_back(&map[y + 1][x]);
			}
		}
	}

	/*for (Node* nod : map[0][0].neighbors) {
		cout << "(" << nod->pos.x << ", " << nod->pos.y << ")" << endl;
	}*/

	int start_x = 1;
	int start_y = 1;
	start_node = &map[start_y][start_x];
	start_node->color = YELLOW;

	int end_x = 18;
	int end_y = 14;
	end_node = &map[end_y][end_x];
	end_node->color = RED;
}

Vector2 convert_to_px(Vector2 v) {
	Vector2 v2_to_px = { float(v.x) * cell_size, float(v.y) * cell_size };
	// centerize
	float w = scr_w/2 - map_w/2 * cell_size;
	float h = scr_h/2 - map_h/2 * cell_size;
	return { v2_to_px.x + w, v2_to_px.y + h };
}

void render_nodes() {
	Vector2 cell_sz = { cell_size - grid_thickness, cell_size - grid_thickness };
	for (int y = 0; y < map_h; y++) {
		for (int x = 0; x < map_w; x++) {
			Vector2 px_node_pos = convert_to_px(map[y][x].pos);
			DrawRectangleV(px_node_pos, cell_sz, map[y][x].color);
		}
	}
	
	// show neighbors of the start_node
	/*Vector2 c_node_pos = convert_to_px(start_node->pos);
	DrawRectangleV(c_node_pos, cell_sz, GREEN);
	for (Node* nod : start_node->neighbors) {
		Vector2 px_node_pos = convert_to_px(nod->pos);
		DrawRectangleV(px_node_pos, cell_sz, DARKGREEN);
	}*/
}

int main() {

	InitWindow(scr_w, scr_h, "A-Star");
	init_node_network();
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		render_nodes();
		EndDrawing();
	}
	return 0;
}

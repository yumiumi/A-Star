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
	int pos_x;							// nodes position in 2d space
	int pos_y;
	Node* parent;
	vector<Node*> neighbors;
	Color color = WHITE;
};

// 2D map of nodes
Node map[map_h][map_w];

Node* start_node;
Node* end_node;

Vector2 convert_to_px(Vector2 v) {
	Vector2 v2_to_px = { float(v.x) * cell_size, float(v.y) * cell_size };
	// centerize
	float w = scr_w/2 - map_w/2 * cell_size;
	float h = scr_h/2 - map_h/2 * cell_size;
	return { v2_to_px.x + w, v2_to_px.y + h };
}

void render_cells() {
	Vector2 cell_sz = { cell_size - grid_thickness, cell_size - grid_thickness };
	for (int y = 0; y < map_h; y++) {
		for (int x = 0; x < map_w; x++) {
			Vector2 v2_cell_pos = { float(x), float(y) };
			Vector2 px_cell_pos = convert_to_px(v2_cell_pos);
			DrawRectangleV(px_cell_pos, cell_sz, DARKBLUE);
		}
	}
}

int main() {

	InitWindow(scr_w, scr_h, "A-Star");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);
		render_cells();
		EndDrawing();
	}
	return 0;
}

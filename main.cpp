#include "raylib.h"
#include "raymath.h"
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

const int scr_w = 700;
const int scr_h = 900;

const int map_w = 30;
const int map_h = 30;

const int cell_size = 20;
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
Node* current_node;

bool is_solved = false;

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

			if (x > 0 && y > 0) {
				map[y][x].neighbors.push_back(&map[y - 1][x - 1]);
			}
			if (x < map_w - 1 && y > 0) {
				map[y][x].neighbors.push_back(&map[y - 1][x + 1]);
			}
			if (x < map_w && y < map_h) {
				map[y][x].neighbors.push_back(&map[y + 1][x + 1]);
			}
			if (x > 0 && y < map_h - 1) {
				map[y][x].neighbors.push_back(&map[y + 1][x - 1]);
			}
		}
	}

	/*for (Node* nod : map[0][0].neighbors) {
		cout << "(" << nod->pos.x << ", " << nod->pos.y << ")" << endl;
	}*/

	int start_x = 4;
	int start_y = 7;
	start_node = &map[start_y][start_x];
	start_node->local_goal = 0;
	start_node->color = YELLOW;

	int end_x = 10;
	int end_y = 19;
	end_node = &map[end_y][end_x];
	end_node->color = RED;
}

vector<Node*> node_queue; // priority queue for checking nodes

void check_node() {

	node_queue.clear();

	node_queue.push_back(start_node);

	while (!node_queue.empty()) {

		int id_smallest_lg = -1;
		float smallest_lg;
		for (int i = 0; i < node_queue.size(); i++){
			if (id_smallest_lg == -1 || node_queue[i]->local_goal < smallest_lg) {
				smallest_lg = node_queue[i]->local_goal;
				id_smallest_lg = i;
			}
		}
		assert(id_smallest_lg > -1);
		assert(id_smallest_lg < node_queue.size());
		current_node = node_queue[id_smallest_lg];
		current_node->is_visited = true;
		
		// check all neighbors of current node
		for (Node* n_node : current_node->neighbors) {
			if (n_node->is_visited || n_node->is_obstacle) {
				continue;
			}
			float distance = Vector2Distance(n_node->pos, current_node->pos);
			if (current_node->local_goal + distance < n_node->local_goal) {
				n_node->local_goal = current_node->local_goal + distance;
				n_node->parent = current_node;
			}

			auto it = find(node_queue.begin(), node_queue.end(), n_node);
			if (it != node_queue.end()) {
				continue;
			}
			else {
				node_queue.push_back(n_node);
			}
			
			// check if the n_node is end_node
			if (n_node == end_node && n_node->pos.x) {
				is_solved = true;
			}
		}
		node_queue.erase(node_queue.begin() + id_smallest_lg);
		if (is_solved) {
			is_solved = false;
			break;
		}
	}
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
			if (map[y][x].is_visited) {
				if (map[y][x].pos.x != start_node->pos.x || map[y][x].pos.y != start_node->pos.y) {
					DrawRectangleV(px_node_pos, cell_sz, DARKGREEN);
				}
			}
			if (map[y][x].is_obstacle) {
				DrawRectangleV(px_node_pos, cell_sz, GRAY);
			}
		}
	}
	if (end_node->parent != nullptr) {
		Node* p = end_node;
		//cout << "start node = " << start_node->pos.x << ", " << start_node->pos.y << endl;
		while (p->parent != nullptr) {
			if (p != end_node && p != start_node) {
				Vector2 px_p_pos = convert_to_px(p->pos);
				DrawRectangleV(px_p_pos, cell_sz, DARKPURPLE);
			}
			p = p->parent;
		}
	}


	 /*//show neighbors of the start_node
	Vector2 c_node_pos = convert_to_px(start_node->pos);
	DrawRectangleV(c_node_pos, cell_sz, GREEN);
	for (Node* nod : start_node->neighbors) {
		Vector2 px_node_pos = convert_to_px(nod->pos);
		DrawRectangleV(px_node_pos, cell_sz, DARKGREEN);
	}*/
}

bool is_start_active = false;
bool is_end_active = false;
void change_nodes_pos(){
	float w = scr_w/2 - map_w/2 * cell_size;
	float h = scr_h/2 - map_h/2 * cell_size;
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		
		Vector2 m_px = GetMousePosition();
		m_px = { m_px.x - w, m_px.y - h };
		// Convert mouse position from pixels to cells
		Vector2 m_cell = { floor(m_px.x / cell_size), floor(m_px.y / cell_size) };

		if (is_start_active == false) {
			if (m_cell.x == start_node->pos.x && m_cell.y == start_node->pos.y) {
				is_start_active = true;
				start_node->color = GREEN;
			}
		}
		else{
			if (m_cell.x == start_node->pos.x && m_cell.y == start_node->pos.y) {
				is_start_active = false;
				start_node->color = YELLOW;
			}
			else {
				map[int(start_node->pos.y)][int(start_node->pos.x)].color = DARKBLUE;
				if (!map[int(m_cell.y)][int(m_cell.x)].is_obstacle) {
					start_node = &map[int(m_cell.y)][int(m_cell.x)];
				}
				is_start_active = false;
				start_node->color = YELLOW;
			}
		}

		if (is_end_active == false) {
			if (m_cell.x == end_node->pos.x && m_cell.y == end_node->pos.y) {
				is_end_active = true;
				end_node->color = ORANGE;
			}
		}
		else{
			if (m_cell.x == end_node->pos.x && m_cell.y == end_node->pos.y) {
				is_end_active = false;
				end_node->color = RED;
			}
			else {
				map[int(end_node->pos.y)][int(end_node->pos.x)].color = DARKBLUE;
				if (!map[int(m_cell.y)][int(m_cell.x)].is_obstacle) {
					end_node = &map[int(m_cell.y)][int(m_cell.x)];
				}
				is_end_active = false;
				end_node->color = RED;
			}
		}
	}
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {

		Vector2 m_px = GetMousePosition();
		m_px = { m_px.x - w, m_px.y - h };
		// Convert mouse position from pixels to cells
		Vector2 m_cell = { floor(m_px.x / cell_size), floor(m_px.y / cell_size) };
		if (map[int(m_cell.y)][int(m_cell.x)].is_obstacle) {
			map[int(m_cell.y)][int(m_cell.x)].is_obstacle = false;
		}
	}
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

		Vector2 m_px = GetMousePosition();
		m_px = { m_px.x - w, m_px.y - h };
		// Convert mouse position from pixels to cells
		Vector2 m_cell = { floor(m_px.x / cell_size), floor(m_px.y / cell_size) };
		if (map[int(m_cell.y)][int(m_cell.x)].pos.x != start_node->pos.x || map[int(m_cell.y)][int(m_cell.x)].pos.y != start_node->pos.y) {
			if (map[int(m_cell.y)][int(m_cell.x)].pos.x != end_node->pos.x || map[int(m_cell.y)][int(m_cell.x)].pos.y != end_node->pos.y) {
				map[int(m_cell.y)][int(m_cell.x)].is_obstacle = true;
			}
		}
	}
}

void render() {
	BeginDrawing();
	ClearBackground(BLACK);
	render_nodes();
	EndDrawing();
}

int main() {

	InitWindow(scr_w, scr_h, "A-Star");
	init_node_network();
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		for (int y = 0; y < map_h; y++) {
			for (int x = 0; x < map_w; x++) {
				map[y][x].is_visited = false;
				map[y][x].local_goal = INFINITY;
				map[y][x].parent = nullptr;
			}
		}
		start_node->local_goal = 0.0f;
		check_node();
		change_nodes_pos();
		render();
	}
	return 0;
}

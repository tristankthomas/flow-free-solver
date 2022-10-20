#include "extensions.h"
#include "options.h"

//////////////////////////////////////////////////////////////////////
// For sorting colors

int color_features_compare(const void* vptr_a, const void* vptr_b) {

	const color_features_t* a = (const color_features_t*)vptr_a;
	const color_features_t* b = (const color_features_t*)vptr_b;

	int u = cmp(a->user_index, b->user_index);
	if (u) { return u; }

	int w = cmp(a->wall_dist[0], b->wall_dist[0]);
	if (w) { return w; }

	int g = -cmp(a->wall_dist[1], b->wall_dist[1]);
	if (g) { return g; }

	return -cmp(a->min_dist, b->min_dist);

}

//////////////////////////////////////////////////////////////////////
// Place the game colors into a set order

void game_order_colors(game_info_t* info,
                       game_state_t* state) {

	if (g_options.order_random) {
    
		srand(now() * 1e6);
    
		for (size_t i=info->num_colors-1; i>0; --i) {
			size_t j = rand() % (i+1);
			int tmp = info->color_order[i];
			info->color_order[i] = info->color_order[j];
			info->color_order[j] = tmp;
		}

	} else { // not random

		color_features_t cf[MAX_COLORS];
		memset(cf, 0, sizeof(cf));

		for (size_t color=0; color<info->num_colors; ++color) {
			cf[color].index = color;
			cf[color].user_index = MAX_COLORS;
		}
    

		for (size_t color=0; color<info->num_colors; ++color) {
			
			int x[2], y[2];
			
			for (int i=0; i<2; ++i) {
				pos_get_coords(state->pos[color], x+i, y+i);
				cf[color].wall_dist[i] = get_wall_dist(info, x[i], y[i]);
			}

			int dx = abs(x[1]-x[0]);
			int dy = abs(y[1]-y[0]);
			
			cf[color].min_dist = dx + dy;
			
		

		}


		qsort(cf, info->num_colors, sizeof(color_features_t),
		      color_features_compare);

		for (size_t i=0; i<info->num_colors; ++i) {
			info->color_order[i] = cf[i].index;
		}
    
	}

	if (!g_options.display_quiet) {

		printf("\n************************************************"
		       "\n*               Branching Order                *\n");
		if (g_options.order_most_constrained) {
			printf("* Will choose color by most constrained\n");
		} else {
			printf("* Will choose colors in order: ");
			for (size_t i=0; i<info->num_colors; ++i) {
				int color = info->color_order[i];
				printf("%s", color_name_str(info, color));
			}
			printf("\n");
		}
		printf ("*************************************************\n\n");

	}

}



//////////////////////////////////////////////////////////////////////
// Helper function for below
// This function was taken from https://github.com/mzucker/flow_solver

int game_is_deadend(const game_info_t* info,
                    const game_state_t* state,
                    pos_t pos) {

    assert(pos != INVALID_POS && !state->cells[pos]);

    int x, y;
    pos_get_coords(pos, &x, &y);
  
    int num_free = 0;

    // checks in all four directions
    for (int dir = DIR_LEFT; dir <= DIR_DOWN; ++dir) {
        pos_t neighbor_pos = offset_pos(info, x, y, dir);
        // checks if in bounds
        if (neighbor_pos != INVALID_POS) {
            // checks if a true free cell
            if (!state->cells[neighbor_pos]) {
                ++num_free;
            } else {
                // checks if its an uncompleted goal position
                for (size_t color=0; color<info->num_colors; ++color) {
                    // skips if completed
                    if (state->completed & (1 << color)) {
                        continue;
                    }

                    if (neighbor_pos == state->pos[color] ||
                            neighbor_pos == info->goal_pos[color]) {
                        ++num_free;
                    }
                }
                                                                    
            }
        }
    }
    // returns if less than 1 free cell
    return num_free <= 1;

}


//////////////////////////////////////////////////////////////////////
// Check for dead-end regions of freespace where there is no way to
// put an active path into and out of it. Any freespace node which
// has only one free neighbor represents such a dead end. For the
// purposes of this check, cur and goal positions count as "free".
// This function was taken from https://github.com/mzucker/flow_solver
// and updated for a 12 cell check instead of 4.

int game_check_deadends(const game_info_t* info,
                        const game_state_t* state) {

    size_t color = state->last_color;
    if (color >= info->num_colors) { return 0; }
  
    pos_t cur_pos = state->pos[color];
    int x, y;
    pos_get_coords(cur_pos, &x, &y);
    // iterates through neighboring cells
    for (int dir1 = DIR_LEFT; dir1 <= DIR_DOWN; ++dir1) {
        pos_t neighbor_pos_4 = offset_pos(info, x, y, dir1);

        // checks for dead end conditions
        if (neighbor_pos_4 != INVALID_POS && !state->cells[neighbor_pos_4] && game_is_deadend(info, state, neighbor_pos_4)) {

                return 1;

        }
        // iterates through neighboring cells of neighboring cells
	    for (int dir2 = DIR_LEFT; dir2 <= DIR_DOWN; ++dir2) {
            // skips if dir2 brings back to current postition
            if (dir1 == DIR_LEFT && dir2 == DIR_RIGHT) continue;
            else if (dir1 == DIR_RIGHT && dir2 == DIR_LEFT) continue;
            else if (dir1 == DIR_UP && dir2 == DIR_DOWN) continue;
            else if (dir1 == DIR_DOWN && dir2 == DIR_UP) continue;

            pos_t neighbor_pos_12 = pos_offset_pos(info, neighbor_pos_4, dir2);
            // checks for dead end conditions
            if (neighbor_pos_12 != INVALID_POS && !state->cells[neighbor_pos_12] && game_is_deadend(info, state, neighbor_pos_12)) {

                return 1;

            }


        }
    }

    return 0;

}

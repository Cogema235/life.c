#include "sim.c"

int main(int argc, char** argv)
{
    size_t x_render_grid_size = 41;
    size_t y_render_grid_size = 41;
    size_t max_universe_size = 100;
    char living_cell_maker = '#';
    char dead_cell_marker = ' ';

    GRID* scene = create_grid(x_render_grid_size, y_render_grid_size);
    GRID* universe = create_grid(41, 41);
    set_column(universe, 20, '#');

    init_grid(universe, "0");
    set_column(universe, 20, '1');
    for (size_t i = 10; i < 25; i++) 
    {
        universe->grid[i][20] = '1';
    }
    display_grid(encadre(replace(replace(phagocyte(scene, universe, universe->x_size / 2, universe->y_size / 2), '1', living_cell_maker), '0', dead_cell_marker), '+', '+'));

    system("sleep 1s");

    for (int i = 0; i < 1000; i++) 
    {
        system("clear");

        if (universe->x_size < max_universe_size) 
        {
            push_universe(universe);
        }

        display_grid(encadre(replace(replace(phagocyte(scene, evolve_universe(universe), universe->x_size / 2, universe->y_size / 2), '1', living_cell_maker), '0', dead_cell_marker), '+', '+'));

        system("sleep 0.02s");
    }
    free(universe->grid);
    free(scene->grid);
}
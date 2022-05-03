#include "grid.c"

char cell_next_state(size_t cell_x, size_t cell_y, GRID *grid)
{
    int living_neighbours_number = 0;

    for (int i = cell_x - 1; i <= cell_x + 1; i++)
    {
        for (int j = cell_y - 1; j <= cell_y + 1; j++)
        {
            if (i >= 0 && j >= 0 && i < grid->x_size && j < grid->y_size && (i != cell_x || j != cell_y) && grid->grid[i][j] == '1')
            {
                living_neighbours_number ++;
            }
        }
    }
    switch (living_neighbours_number)
    {
        case 3 :    return '1';
        case 2 :    return grid->grid[cell_x][cell_y];
        default :   return '0';
    }
}

int reached_borders(GRID *grid)
{
    for (int i = 0; i < grid->x_size; i += grid->x_size - 1)
    {
        for (int j = 0; j < grid->y_size; j++)
        {
            if (grid->grid[i][j] == '1')
            {
                return 1;
            }
        }
    }
    for (int j = 0; j < grid->y_size; j += grid->y_size - 1)
    {
        for (int i = 0; i < grid->x_size; i++)
        {
            if (grid->grid[i][j] == '1')
            {
                return 1;
            }
        }
    }
    return 0;
}

GRID* push_universe(GRID *grid)
{
    if (reached_borders(grid))
    {
        push_grid(grid);
        push_grid(grid);

        for (int i = grid->x_size - 2; i >= 0; i--)
        {
            for (int j = 0; j < grid->y_size; j++)
            {
                grid->grid[i + 1][j] = grid->grid[i][j];
                grid->grid[i][j] = '0';
            }
        }
        for (int j = grid->y_size - 2; j > 0; j--)
        {
            for (int i = 0; i < grid->x_size; i++)
            {
                grid->grid[i][j + 1] = grid->grid[i][j];
                grid->grid[i][j] = '0';
            }
        }
    }
    return grid;
}

GRID* evolve_universe(GRID *current_grid)
{
    static GRID *tmp_grid = NULL;

    if (!tmp_grid)
    {
        tmp_grid = create_grid(current_grid->x_size, current_grid->y_size);
    }
    else if (tmp_grid->x_size < current_grid->x_size || tmp_grid->y_size < current_grid->y_size)
    {
        push_grid(tmp_grid);
        push_grid(tmp_grid);
    }
    init_grid(tmp_grid, "0");

    for (int i = 0; i < tmp_grid->x_size; i++)
    {
        for (int j = 0; j < tmp_grid->y_size; j++)
        {
            tmp_grid->grid[i][j] = cell_next_state(i, j, current_grid);
        }
    }
    for (int i = 0; i < current_grid->x_size; i++)
    {
        for (int j = 0; j < current_grid->y_size; j++)
        {
            current_grid->grid[i][j] = tmp_grid->grid[i][j];
        }
    }
    return current_grid;
}
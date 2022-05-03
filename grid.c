#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "debug.c"

typedef struct
{
    size_t x_size;
    size_t y_size;
    char** grid;
}
GRID;

GRID* create_grid(size_t x_size, size_t y_size)
{
    char** block = malloc(sizeof(char*) *x_size);

    for (int i = 0; i < x_size; i++)
    {
        block[i] = malloc(sizeof(char) *y_size);
    }
    GRID *grid = malloc(sizeof(GRID));
    grid->grid = block;
    grid->x_size = x_size;
    grid->y_size = y_size;

    return grid;
}

GRID* resize_grid(GRID *grid, size_t x_new_size, size_t y_new_size)
{
    grid->grid = realloc(grid->grid, sizeof(char*) * x_new_size);

    for (int i = 0; i < y_new_size; i++)
    {
        grid->grid[i] = realloc(grid->grid[i], sizeof(char) * y_new_size);
    }
    grid->x_size = x_new_size;
    grid->y_size = y_new_size;

    return grid;
}

GRID* push_grid(GRID *grid)
{
    return resize_grid(grid, grid->x_size + 1, grid->y_size + 1);
}

GRID* push_back(GRID* grid)
{
    return resize_grid(grid, grid->x_size - 1, grid->y_size - 1);
}

GRID* init_grid(GRID *grid, char *value)
{
    for (int i = 0; i < grid->x_size; i++)
    {
        for (int j = 0; j < grid->y_size; j++)
        {
            grid->grid[i][j] = *value;
        }
    }
    return grid;
}

GRID* phagocyte(GRID* a, GRID* b, size_t centerx, size_t centery) /* Immerge b dans a, centre de b en -> centre de a. */
{
    size_t a_x_center = a->x_size / 2;
    size_t a_y_center = a->y_size / 2;
    size_t b_x_center = b->x_size / 2;
    size_t b_y_center = b->y_size / 2;
    size_t iatib;
    size_t jatjb;

    if (a->x_size >= b->x_size)
    {
        for (size_t i = 0; i < b->x_size; i++)
        {
            iatib = a_x_center + i - b_x_center;
            for(size_t j = 0; j < b->y_size; j++)
            {
                jatjb = a_y_center + j - b_y_center;
                if (iatib < a->x_size && iatib >= 0 && jatjb < a->y_size && jatjb >= 0)
                {
                    a->grid[iatib][jatjb] = b->grid[i][j];
                }
            }
        }
    }
    else
    {
        b_x_center = centerx;
        b_y_center = centery;

        for (size_t i = 0; i < a->x_size; i++)
        {
            for (size_t j = 0; j < a->y_size; j++)
            {
                a->grid[i][j] = b->grid[i + b_x_center - a_x_center][j + b_y_center - a_x_center];
            }
        }
    }
    return a;
}

GRID* replace(GRID* grid, char from_chr, char to_chr)
{
    for (size_t i = 0; i < grid->x_size; i++)
    {
        for(size_t j = 0; j < grid->y_size; j++)
        {
            if (grid->grid[i][j] == from_chr)
            {
                grid->grid[i][j] = to_chr;
            }
        }
    }
    return grid;
}

char put_char(char chr)
{
    write(STDOUT_FILENO, &chr, sizeof(char));
}

GRID* display_grid(GRID *grid)
{
    for (int j = 0; j < grid->y_size; j++)
    {
        for (int i = 0; i < grid->x_size; i++)
        {
            put_char(grid->grid[i][j]);
            put_char(' ');
        }
        put_char('\n');
    }
    return grid;
}

size_t max(size_t a, size_t b)
{
    return (a >= b) * a + (a < b) * b;
}

GRID* seed_to_grid(char* seed)
{
    long tmp_x = 0;
    long tmp_y = 0;
    size_t max_ = 1;
    size_t i = 0;
    int was_on_x = 0;

    GRID* grid = create_grid(max_, max_);

    while (seed[i])
    {
	if (!was_on_x)
	{
	    tmp_x = seed[i] - '0';
	    was_on_x++;
	}
	else
	{
	    tmp_y = seed[i] - '0';
	    was_on_x--;
	}
	if (!was_on_x)
	{
	    max_ = max(max(tmp_x, tmp_y), max_);
	    resize_grid(grid, max_ + 1, max_ + 1);
	    grid->grid[tmp_x][tmp_y] = '1';
	}	
        i++;
    }
    for (tmp_x; tmp_x >= 0; tmp_x--)
    {
        for (tmp_y; tmp_y >= 0; tmp_y--)
	{
	    if (!grid->grid[tmp_x][tmp_y])
	    {
                grid->grid[tmp_x][tmp_y] = '0';
	    }
	}
    }
    return grid;
}

GRID* set_column(GRID* grid, size_t x_, char marker)
{
    for (size_t j = 0; j < grid->y_size; j++)
    {
        grid->grid[x_][j] = marker;
    }
    return grid;
}

GRID* set_line(GRID* grid, size_t y_, char marker)
{
    for (size_t i = 0; i < grid->x_size; i++)
    {
        grid->grid[i][y_] = marker;
    }
    return grid;
}

GRID* encadre(GRID* grid, char h_marker, char v_marker)
{
    return set_line(set_line(set_column(set_column(grid, 0, v_marker), grid->x_size - 1, v_marker), 0, h_marker), grid->y_size - 1, h_marker);
}

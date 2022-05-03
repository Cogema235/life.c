#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define passed printf("%s\n", "reached");

typedef struct
{
	size_t	x_size;
	size_t 	y_size;
	char**	grid;
} 
GRID;

char* put_string(char *str)
{
    int i;
    i = 0;
    
    while (str[i])
    {
        write(STDOUT_FILENO, &str[i], sizeof(char));
    }
    return str;
}

char put_char(char chr)
{
    write(STDOUT_FILENO, &chr, sizeof(char));
}

GRID*	create_grid(size_t x_size, size_t y_size)
{
    	char** 	block = malloc(sizeof(char*) * x_size);

    	for (int i = 0; i < x_size; i++)
    	{
        	block[i] = malloc(sizeof(char) * y_size);
    	}
		GRID* grid = malloc(sizeof(GRID));
    	grid->grid = block;
		grid->x_size = x_size;
		grid->y_size = y_size;
	
	return grid;
}

GRID* 	resize_grid(GRID* grid, size_t x_new_size, size_t y_new_size)
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

void* 	write_row(char* line, size_t length)
{
    	for (int i = 0; i < length; i++)
		{
        		write(STDOUT_FILENO, &line[i], sizeof(char));
				write(STDOUT_FILENO, " ", sizeof(char));
    	}
    	return line;
}

GRID* 	display_grid(GRID* grid)
{
    	for (int i = 0; i < grid->y_size; i++)
    	{
        	write_row(grid->grid[i], grid->x_size);
			write(STDOUT_FILENO, "\n", sizeof(char));
    	}
    	return grid;
}

int 	cell_next_state(size_t cell_x, size_t cell_y, GRID* grid)
{
    	int	living_neighbours_number;

    	living_neighbours_number = 0;

    	for (int i = cell_x - 1; i <= cell_x + 1; i++)
    	{
        		for (int j = cell_y - 1; j <= cell_y + 1; j++)
        		{
            			if (i >= 0 && j >= 0 && i < grid->x_size && j < grid->y_size)
            			{
                			living_neighbours_number += grid->grid[i][j] - '0';
            			}
        		}
    	}
    	return (living_neighbours_number >= 2 && living_neighbours_number < 3) + '0';
}

GRID*	init_grid(GRID* grid, char* value)
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

GRID*	calc_render_grid(GRID* cell_grid, size_t render_x_size, size_t render_y_size, char living_marker, char dead_marker)
{
    	float	x_char_per_case;
    	float 	y_char_per_case;

	static GRID* render_grid = NULL;

    	x_char_per_case =  render_x_size / cell_grid->x_size;
    	y_char_per_case = render_y_size / cell_grid->y_size;
	
	if (!render_grid)
	{
		render_grid = create_grid(render_x_size, render_y_size);
	}
	else if (render_x_size != render_grid->x_size || render_y_size != render_grid->y_size)
	{
		resize_grid(render_grid, render_x_size, render_y_size);
	}
	init_grid(render_grid, &dead_marker);

    	for (int i = 0; i < cell_grid->x_size; i++)
    	{
        	for (int j = 0; j < cell_grid->y_size; j++)
       		{	
            		if (cell_grid->grid[i][j] == '1') 
            		{
                		render_grid->grid[(int)(i * x_char_per_case)][(int)(j * y_char_per_case)] = living_marker;
            		}
        	}
    	}
	return render_grid;
}

GRID*	load_grid_from_file(char* path)
{
		FILE*	source_grid;
		char	chr;
		int 	i;
		int 	j;

		i = 0;
		j = 0;
		GRID* cell_grid = create_grid(i + 1, j + 1);
		source_grid = fopen(path, "r");
		do
		{
			chr = fgetc(source_grid);
			put_char(chr);
			if (chr == '0' || chr  == '1')
			{
				cell_grid->grid[i][j] == chr;
				i++;
				resize_grid(cell_grid, cell_grid->x_size + 1, cell_grid->y_size);
			}
			else if (chr == '\n')
			{
				j++;
				resize_grid(cell_grid, cell_grid->x_size, cell_grid->y_size + 1);
			}
		}
		while (chr != EOF);

		fclose(source_grid);

		return cell_grid;
}

int* 	reached_borders(GRID* grid)
{
	static int touched_borders[2];

	touched_borders[0] = 0;
	touched_borders[1] = 0;

	for (int i = 0; i < grid->x_size && !touched_borders[0]; i += grid->x_size - 1)
	{
		for (int j = 0; j < grid->y_size && !touched_borders[0]; j++)
		{
			if (grid->grid[i][j] == '1')
			{
				touched_borders[0] = 1;
			}
		}
	}
	for (int j = 0; j < grid->y_size && !touched_borders[1]; j += grid->y_size - 1)
	{
		for (int i = 0; i < grid->x_size && !touched_borders[1]; i++)
		{
			if (grid->grid[i][j] == '1')
			{
				touched_borders[1] = 1;
			}
		}
	}
	return touched_borders;
}

GRID*	push_universe(GRID* grid)
{
	int* touched_borders = reached_borders(grid);

	if (touched_borders[0])
	{
		grid = resize_grid(grid, grid->x_size + 2, grid->y_size);
	}
	if (touched_borders[1])
	{
		grid = resize_grid(grid, grid->x_size, grid->y_size + 2);
	}
	for (int i = grid->x_size - 2; i >= 0 && touched_borders[0]; i--)
	{
		for (int j = 0; j < grid->y_size; j++)
		{
			grid->grid[i + 1][j] = grid->grid[i][j];
		}
	}
	for (int j = grid->y_size; j >= 0 && touched_borders[1]; j--)
	{
		for (int i = 0; i < grid->x_size; i++)
		{
			grid->grid[i][j + 1] = grid->grid[i][j];
		}
	}
	return grid;
}	

GRID* evolve_grid(GRID* current_grid)
{
	static GRID* tmp_grid = NULL;

	if (!tmp_grid)
	{
		tmp_grid = create_grid(current_grid->x_size, current_grid->y_size);
	}
	else if (tmp_grid->x_size != current_grid->x_size || tmp_grid->y_size != current_grid->y_size)
	{
		resize_grid(tmp_grid, current_grid->x_size, current_grid->y_size);
	}
	for (int i = 0; i < current_grid->x_size; i++)
	{
		for (int j = 0; j < current_grid->y_size; j++)
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

GRID* push_grid(GRID* grid, size_t x_size_increment, size_t y_size_increment)
{
	return resize_grid(grid, grid->x_size + x_size_increment, grid->y_size + y_size_increment);
}
/*
int     main(int argc, char** argv)
{
    	size_t  x_render_grid_size = 50;
    	size_t  y_render_grid_size = 50;
    	char    living_cell_maker = '#';
    	char    dead_cell_marker = '.';

		char* 	path = "/home/noa/Desktop/Life/universe.cell";
    
		GRID* universe_grid = load_grid_from_file(path);
	
		for (int i = 0; i < 10; i++)
		{
			system("clear");
			display_grid(calc_render_grid(evolve_grid(push_universe(universe_grid)), x_render_grid_size, y_render_grid_size, living_cell_maker, dead_cell_marker));
			system("sleep 0.5s");
		}
		
		free(path);
		free(universe_grid);
}	
*/
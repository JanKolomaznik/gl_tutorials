__kernel void game_of_life(__global uchar* current, __global uchar* next, int width, int height)
{
	int x = get_global_id(0);
	int y = get_global_id(1);

	int num_neighbors = 0;
	for (int dy = -1; dy <= 1; ++dy) {
		for (int dx = -1; dx <= 1; ++dx) {
			if (dx == 0 && dy == 0) {
				continue;
			}
			int nx = (x + dx + width) % width;
			int ny = (y + dy + height) % height;

			num_neighbors += current[ny * width + nx];
		}
	}

	int current_cell = current[y * width + x];
	next[y * width + x] = (current_cell && (num_neighbors == 2 || num_neighbors == 3)) || (!current_cell && num_neighbors == 3);
}

__kernel void scale_to_255(__global const uchar* input, __global uchar* output, int size) {
	int index = get_global_id(0);
	if (index < size) {
		output[index] = input[index] * 255;
	}
}

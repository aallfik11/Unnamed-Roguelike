#include <array>
#include <cmath>
#include <iostream>
#include <random>


const int map_width  = 225;
const int map_height = 50;

enum TileType
{
    Floor,
    Wall,
    Stairs,
    Player
};

struct Tile
{
    bool     flag = false;
    TileType type = Floor;
};

std::array<std::array<Tile, map_width>, map_height> map;
std::array<std::array<Tile, map_width>, map_height> map_cpy;

std::random_device                 rd;
std::mt19937                       mt(rd());
std::uniform_int_distribution<int> rando(0, 100);

int chance = 38;

void render_map()
{
    for (auto y : map)
    {
        for (auto x : y)
        {
            if (x.type == Wall)
                std::cout << "█";
            else if (x.type == Stairs)
                std::cout << 'E';
            else if (x.type == Player)
                std::cout << '@';
            else
                std::cout << '.';
        }
        std::cout << std::endl;
    }
}

int check_neighbours(int y, int x)
{
    int wall_counter = 0;
    for (int column = y - 1; column <= y + 1; column++)
    {
        for (int cell = x - 1; cell <= x + 1; cell++)
        {
            if (column != y || cell != x)
            {
                wall_counter += map[column][cell].type;
            }
        }
    }

    return wall_counter;
}
void four_five()
{
    int iterations = 0;
    for (iterations = 0; iterations < 15; iterations++)
    {
        for (int y = 1; y < map_height - 1; y++)
        {
            for (int x = 1; x < map_width - 1; x++)
            {
                int walls = check_neighbours(y, x);
                if (map.at(y).at(x).type == Wall)
                {
                    if (walls <= 2)
                        map_cpy.at(y).at(x).type = Floor;
                }
                else if (walls >= 5)
                    map_cpy.at(y).at(x).type = Wall;
            }
        }
        map = map_cpy;
    }
}

void flood_fill(int y, int x)
{
    if ((map[y][x].type == Floor) && (map[y][x].flag == false))
        map[y][x].flag = true;
    else
        return;
    flood_fill(y - 1, x);
    flood_fill(y + 1, x);
    flood_fill(y, x - 1);
    flood_fill(y, x + 1);
}

bool is_cave_interconnected()
{
    for (auto y : map)
        for (auto x : y)
            if (x.type == Floor && x.flag == false)
                return false;
    return true;
}

void generate_map()
{
    for (int y = 0; y < map_height; y++)
    {
        for (int x = 0; x < map_width; x++)
        {
            if (x == 0 || x == map_width - 1 || y == 0 || y == map_height - 1)
            {
                map.at(y).at(x).type = Wall;
                continue;
            }
            if (rando(rd) < chance)
            {
                map.at(y).at(x).type = Wall;
            }
        }
    }
    map_cpy = map;
}

void rectify_map()
{
    for (auto &y : map)
        for (auto &x : y)
        {
            if (x.type == Floor && x.flag == false)
            {
                x.type = Wall;
            }
        }
}
int count_walls()
{
    int walls = 0;
    for (auto &y : map)
        for (auto &x : y)
            if (x.type == Wall)
                walls++;
    return walls;
}

std::uniform_int_distribution<int> randox(1, map_width - 1);
std::uniform_int_distribution<int> randoy(1, map_height - 1);

int main()
{
    double max_wall_percentage = 0.45l;
    double wall_percentage     = 0.0l;
    double map_size            = map_height * map_width;
    while (true)
    {
        std::array<std::array<Tile, map_width>, map_height> temp;
        map = temp;
        generate_map();
        four_five();
        // render_map();
        int x = randox(rd);
        int y = randoy(rd);
        while (map.at(y).at(x).type == Wall)
        {
            x = randox(rd);
            y = randoy(rd);
        }
        flood_fill(y, x);
        rectify_map();
        double walls_d  = count_walls();
        wall_percentage = walls_d / map_size;
        if (wall_percentage > max_wall_percentage)
            continue;
        else
            break;
    }
    int stairs_x, stairs_y;
    while (true)
    {
        stairs_x = randox(rd);
        stairs_y = randoy(rd);
        if (map.at(stairs_y).at(stairs_x).type == Floor)
        {
            map.at(stairs_y).at(stairs_x).type = Stairs;
            break;
        }
    }
    while (true)
    {
        int x = randox(rd);
        int y = randoy(rd);
        if (map[y][x].type == Floor &&
            (abs(x - stairs_x) > 30 && abs(y - stairs_y) > 10))
        {
            map[y][x].type = Player;
            break;
        }
    }
    render_map();
    std::cout << std::endl
              << "Walls: " << count_walls() << std::endl
              << "Free Spaces: " << (map_width * map_height) - count_walls()
              << std::endl
              << "Wall Percentage: " << wall_percentage;
    // std::cout << std::endl
    //           << is_cave_interconnected();
}
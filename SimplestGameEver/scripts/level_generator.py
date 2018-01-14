import os
import random
import sys

from collections import defaultdict, namedtuple

HEIGHT = 800
WIDTH = 1200

Position = namedtuple('Position', ['x', 'y'])
Size = namedtuple('Size', ['x', 'y'])

def make_entity(pos_list, size_list):
    return {
        'pos': Position(*pos_list),
        'size': Size(*size_list)
    }


all_entities = []

def random_color():
    return [random.random() for _ in range(3)]

def generate_walls():
    data = {
        'pos': [],
        'size': [],
        'color': []
    }
    num_walls = random.randint(4, 10)
    vertical_wall_size = [random.randint(5, 50), random.randint(0, HEIGHT)]
    horizontal_wall_size = [random.randint(0, WIDTH), random.randint(5, 50)]
    for _ in range(num_walls):
        while True:
            pos = [random.randint(0, WIDTH), random.randint(0, HEIGHT)]
            size = vertical_wall_size if random.randint(0, 1) == 0 else horizontal_wall_size
            color = [1, 1, 1]
            
            entity = make_entity(pos, size)
            if not check_collisions(entity):
                data['pos'].append(map(str, pos))
                data['size'].append(map(str, size))
                data['color'].append(map(str, color))
                all_entities.append(entity)
                break
    
    return  data

def generate_door():
    pos = [50, 420]
    size = [random.randint(5, 50), random.randint(50, 100)]
    color = [0, 1, 0]
    all_entities.append(make_entity(pos, size))
    return {'pos': map(str, pos), 'size': map(str, size), 'color': map(str, color)}

def generate_exit():
    while True:
        pos = [random.randint(WIDTH / 2.0, WIDTH), random.randint(0, HEIGHT)]
        size = [random.randint(5, 50), random.randint(50, 100)]
        color = [1, 0, 0]

        entity = make_entity(pos, size)
        if not check_collisions(entity):
            all_entities.append(entity)
            break
    
    return {'pos': map(str, pos), 'size': map(str, size), 'color': map(str, color)}


def generate_enemies(level):
    data = {
        'pos': [],
        'size': [],
        'color': [],
        'direction': []
    }

    if level in range(4):
        num_enemies = random.randint(2, 5)
    elif level in range(4, 7):
        num_enemies = random.randint(5, 9)
    elif level in range(7, 10):
        num_enemies = random.randint(7, 12)
    else:
        num_enemies = random.randint(12, 16)

    for _ in range(num_enemies):
        while True:
            pos = [random.randint(100, WIDTH), random.randint(0, HEIGHT)]
            size = [random.randint(5, 100), random.randint(5, 100)]
            color = random_color()
            direction = random.randint(0,1)

            entity = make_entity(pos, size)
            if not check_collisions(entity):
                data['pos'].append(map(str, pos))
                data['size'].append(map(str, size))
                data['color'].append(map(str, color))
                data['direction'].append(list(str(direction)))
                all_entities.append(entity)
                break
   
    return data

def check_collisions(entity):
    for other in all_entities:
        if check_collision(entity, other):
            return True
    return False

def check_collision(first, second):
    collision_x = (first['pos'].x + first['size'].x > second['pos'].x and 
                   second['pos'].x + second['size'].x > first['pos'].x)

    collision_y = (first['pos'].y + first['size'].y > second['pos'].y and 
                   second['pos'].y + second['size'].y > first['pos'].y)

    return collision_x and collision_y


def join_data(data):
    if isinstance(data, tuple):
        return ','.join(sum((data), []))
    else:
        return ','.join(data['pos']  + data['size'] + data['color'])


# 20 X 30 Grid
# step = 40
# goal = (exit.x, exit.y)
# start = (player.x, player,y)
graph = [[str((i, j)) for i in range(20)] for j in range(30)]

def recursive_dfs(graph, start, path=[]):
  '''recursive depth first search from start'''
  path = path + [start]
  for node in graph[start]:
    if not node in path:
      path=recursive_dfs(graph, node, path)
  return path


def a_star_pathfinding(start, goal):
    # The set of nodes already evaluated.
    closedSet = {}
    # The set of currently discovered nodes still to be evaluated.
    # Initially, only the start node is known.
    openSet = {start}
    # For each node, which node it can most efficiently be reached from.
    # If a node can be reached from many nodes, cameFrom will eventually contain the
    # most efficient previous step.
    cameFrom = the empty map

    # For each node, the cost of getting from the start node to that node.
    gScore = {'default': 1000000000}  # map with default value of Infinity
    # The cost of going from start to start is zero.
    gScore[start] = 0 
    # For each node, the total cost of getting from the start node to the goal
    # by passing by that node. That value is partly known, partly heuristic.
    fScore = {'default': 100000000000} # map with default value of Infinity
    # For the first node, that value is completely heuristic.
    fScore[start] = heuristic_cost_estimate(start, goal)

    while openSet is not empty:
        current = min([fScore[x] for x in openSet]) # the node in openSet having the lowest fScore[] value
        if current = goal:
            return reconstruct_path(cameFrom, current)

        openSet.remove(current)
        closedSet.add(current)
        for get_neighbors(current):
            if neighbor in closedSet:
                continue        # Ignore the neighbor which is already evaluated.
            # The distance from start to a neighbor
            tentative_gScore = gScore[current] + dist_between(current, neighbor)
            if neighbor not in openSet:  # Discover a new node
                openSet.add(neighbor)
            else if tentative_gScore >= gScore[neighbor]
                continue        # This is not a better path.

            # This path is the best until now. Record it!
            cameFrom[neighbor] = current
            gScore[neighbor] = tentative_gScore
            fScore[neighbor] = gScore[neighbor] + heuristic_cost_estimate(neighbor, goal)

    return failure

def get_neighbors(current):
    pass

def dist_between(first, second):
    pass

def reconstruct_path(cameFrom, current):
    total_path = [current]
    while current in cameFrom.keys():
        current = cameFrom[current]
        total_path.append(current)
    return total_path

# We don't care about the shortest path, just that there exists a path
def heuristic_cost_estimater(start, goal):
    return 1


def print_level(wall_data, door_data, exit_data, enemy_data):
    print("walls")
    for wall in zip(wall_data['pos'], wall_data['size'], wall_data['color']):
        print(join_data(wall))
    print("door")
    print(join_data(door_data))
    print('exit')
    print(join_data(exit_data))
    print('enemies')
    for enemy in zip(enemy_data['pos'], enemy_data['size'], enemy_data['color'], enemy_data['direction']):
        print(join_data(enemy))


def main(level):
    data = {
        'wall_data': generate_walls(),
        'door_data': generate_door(),
        'exit_data': generate_exit(),
        'enemy_data': generate_enemies(level)
    }
    print_level(**data)

if __name__ == '__main__':
    if len(sys.argv) > 1:
        filename = os.path.join('levels', "level{}.txt".format(sys.argv[1]))
        with open(filename, 'w') as f:
                sys.stdout = f
                main()
    else:
        for i in range(2, 11):
            filename = os.path.join('levels', "level{}.txt".format(i))
            with open(filename, 'w') as f:
                sys.stdout = f
                main(i)

import os
import random
import sys

from collections import defaultdict

HEIGHT = 800
WIDTH = 1200

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
        pos = [random.randint(0, WIDTH), random.randint(0, HEIGHT)]
        size = vertical_wall_size if random.randint(0, 1) == 0 else horizontal_wall_size
        color = [1, 1, 1]
        data['pos'].append(map(str, pos))
        data['size'].append(map(str, size))
        data['color'].append(map(str, color))
    return  data

def generate_door():
    pos = [50, 420]
    size = [random.randint(5, 50), random.randint(50, 100)]
    color = [0, 1, 0]
    return {'pos': map(str, pos), 'size': map(str, size), 'color': map(str, color)}

def generate_exit():
    pos = [random.randint(WIDTH / 2.0, WIDTH), random.randint(0, HEIGHT)]
    size = [random.randint(5, 50), random.randint(50, 100)]
    color = [1, 0, 0]
    return {'pos': map(str, pos), 'size': map(str, size), 'color': map(str, color)}


def generate_enemies():
    data = {
        'pos': [],
        'size': [],
        'color': [],
        'direction': []
    }
    num_enemies = random.randint(2, 15)
    for _ in range(num_enemies):
        pos = [random.randint(100, WIDTH), random.randint(0, HEIGHT)]
        size = [random.randint(5, 100), random.randint(5, 100)]
        color = random_color()
        direction = random.randint(0,1)
        data['pos'].append(map(str, pos))
        data['size'].append(map(str, size))
        data['color'].append(map(str, color))
        data['direction'].append(list(str(direction)))
    return data

def join_data(data):
    if isinstance(data, tuple):
        return ','.join(sum((data), []))
    else:
        return ','.join(data['pos']  + data['size'] + data['color'])


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


def main():
    data = {
        'wall_data': generate_walls(),
        'door_data': generate_door(),
        'exit_data': generate_exit(),
        'enemy_data': generate_enemies()
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
                main()

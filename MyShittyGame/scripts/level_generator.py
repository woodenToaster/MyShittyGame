import os
import random
import sys

from collections import defaultdict

HEIGHT = 800
WIDTH = 1200

def generate_walls():
    data = {
        'pos': [],
        'size': [],
        'color': []
    }
    num_walls = random.randint(4, 10)
    for _ in range(num_walls):
        pos = [random.randint(0, WIDTH), random.randint(0, HEIGHT)]
        size = [random.randint(5, 50), random.randint(0, HEIGHT)]
        color = random.sample(range(0, 255), 3)
        data['pos'].append(map(str, pos))
        data['size'].append(map(str, size))
        data['color'].append(map(str, color))
    return  data

def generate_door():
    pos = [random.randint(0, WIDTH / 2.0), random.randint(0, HEIGHT)]
    size = [random.randint(5, 50), random.randint(50, 100)]
    color = random.sample(range(0, 255), 3)
    return {'pos': map(str, pos), 'size': map(str, size), 'color': map(str, color)}

def generate_exit():
    pos = [random.randint(WIDTH / 2.0, WIDTH), random.randint(0, HEIGHT)]
    size = [random.randint(5, 50), random.randint(50, 100)]
    color = random.sample(range(0, 255), 3)
    return {'pos': map(str, pos), 'size': map(str, size), 'color': map(str, color)}


def generate_enemies():
    data = {
        'pos': [],
        'size': [],
        'color': []
    }
    num_enemies = random.randint(2, 15)
    for _ in range(num_enemies):
        pos = [random.randint(100, WIDTH), random.randint(0, HEIGHT)]
        size = [random.randint(5, 100), random.randint(5, 100)]
        color = random.sample(range(0, 255), 3)
        data['pos'].append(map(str, pos))
        data['size'].append(map(str, size))
        data['color'].append(map(str, color))
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
    # from pdb import set_trace; set_trace()
    print(join_data(door_data))
    print('exit')
    print(join_data(exit_data))
    print('enemies')
    for enemy in zip(enemy_data['pos'], enemy_data['size'], enemy_data['color']):
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
    for i in range(2, 11):
        filename = os.path.join('levels', "level{}.txt".format(i))
        with open(filename, 'w') as f:
            sys.stdout = f
            main()


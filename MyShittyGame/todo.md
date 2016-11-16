# TODO

## Gameplay
- [X] Add sounds
- [ ] Add text (lives, level, time)
- [X] Show a visible exit
- [ ] Door should always be visible.  Prevents sudden death.

## Level Generation
- [ ] Make sure there is a path to the exit (A*)
- [X] Make sure no placed entities overlap
- [X] Fewer enemies at first, then gradually increase the number
- [ ] Generate levels at runtime instead of creating files.

## Rendering
- [ ] Use indexed buffer draws
- [ ] Add lighting and shadows
- [ ] Add textures to objects

## Refactoring
- [ ] Change all public members to private and add getters/setters
- [X] Remove duplicate code from Enemy::checkEnemyCollistion

## Testing
- [ ] Set up Travis CI for Linux
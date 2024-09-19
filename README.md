# Brainiac

Brainiac is a UCI compliant chess engine.

## Performance

TBD.

## Build

To build the engine executable, run

1. Go to the build folder, `cd build`
2. Run `cmake .. && make -j 3`

## TODO

### Performance Enhancements

- Aspiration window
- Add heuristics for "protected pieces" and "hanging pieces" (penalty)
- Optimize SEE algorithm
- Mate distance pruning
- King safety (evaluation)

## License

Code and documentation Copyright (c) 2021 Keith Leonardo

Code released under the [MIT License](https://choosealicense.com/licenses/mit/).

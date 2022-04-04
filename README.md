# Brainiac

Brainiac is a chess artificial intelligence engine.

## Performance

TBD.

## Build

To build the engine executable, run

1. Go to the build folder, `cd build`
2. Run `cmake .. && make -j 3`

## TODO

### Performance Enhancements

- Implement PV-search (principal variation)
- Implement killer-move heuristic for move ordering
- Implement iterative deepening

### Engine Interface

- Fix up main program command-line interface
- Integrate UCI protocol

## License

Code and documentation Copyright (c) 2021 Keith Leonardo

Code released under the [MIT License](https://choosealicense.com/licenses/mit/).

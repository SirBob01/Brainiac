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

- PV-search
- Null-move pruning
- Futility pruning
- Killer heuristic (move ordering)
- King safety (evaluation)
- Dynamic thinking time for live games

### Engine Interface

- Fix up main program command-line interface
- Integrate [UCI protocol](http://wbec-ridderkerk.nl/html/UCIProtocol.html)

## License

Code and documentation Copyright (c) 2021 Keith Leonardo

Code released under the [MIT License](https://choosealicense.com/licenses/mit/).

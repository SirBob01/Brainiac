# Brainiac

Brainiac is a chess artificial intelligence engine powered by the HyperNEAT algorithm.

## Performance

TBD.

## Training

The HyperNEAT algorithm uses evolution to train neural networks. Genome neural networks, known as Compositional Pattern-Producing Networks (CPPN), are randomly generated via mutations and crossover. This is then used to "paint" a pattern on a 4-dimensional hypercube, which represents the weights and biases of the resulting phenome neural network.

The phenome neural network acts as a state evaluation function, meaning it can measure how well a certain board position is for either white or black. Because chess is a perfect information game, Brainiac can use this function to select moves based on how well the resulting position is.

To train, an initial population of genomes is created and divided into species to preserve variation in the gene pool. Each genome competes with every other genome by playing both white and black, and their fitness functions are evaluated. After every set of simulations, the worst performing genomes are culled from the population, and the best performing members will be allowed to reproduce. These new genomes will be part of the next generation.

Over time, this should improve the overall gene pool, creating networks that can evaluate the board and predict whether a given move is most likely to win the game.

## License

Code and documentation Copyright (c) 2019-2020 Keith Leonardo

Code released under the [MIT License](https://choosealicense.com/licenses/mit/).
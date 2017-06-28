A program used to "count cards" in the game of BlackJack to inform the user of the current house edge.

## Manual

```b X``` --- ```b``` is the command for burning a card ```X```.
Example: ```b 10``` to burn a face card or ```b 11``` to burn a Ace

```s``` --- is the command to simulate NUMBER_OF_SIMULATIONS (default 10000000) playouts at the current state of the game and provides information
Example:
input :
```s```

output :
```EV       : -0.470%
Playouts : 10000000
left    : 416```

EV represents the Expected Value for you
Playouts represents the number of games simulated
left represents the number of cards left in the shoe


```r``` --- is the command to reset the game

## Compiling

To compile simply run ```make``` within the root directory. This should work on linux by default and work on windows if you have installed the necessary tools.

## Testing

To test simply run ```make check``` within the root directory.

## Cleaning

To clean after a make or make check simply run ```make clean``` within the root directory.
\ Test: Stack manipulation operations
\ Expected output: 42, 42, 10, 20, 5, 7, 3, 14

\ dup: duplicate top value
42 dup . .

\ swap: swap top two values (10 20 swap -> stack is [20,10], so . . prints 10, 20)
10 20 swap . .

\ drop: discard top value
5 99 drop .


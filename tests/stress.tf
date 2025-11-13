\ Test: Stress test with many operations
\ Expected output: 55, 3628800, 1, 1

\ Sum from 1 to 10: ((((((((1+2)+3)+4)+5)+6)+7)+8)+9)+10
1 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + .

\ Factorial 10: 1*2*3*4*5*6*7*8*9*10
1 2 * 3 * 4 * 5 * 6 * 7 * 8 * 9 * 10 * .

\ Deep stack operations (push 10, drop 9, leaves bottom element = 1)
1 2 3 4 5 6 7 8 9 10
drop drop drop drop drop drop drop drop drop .

\ Multiple dup operations (1 dup dup dup -> [1,1,1,1], drop 3 times -> [1])
1 dup dup dup drop drop drop .

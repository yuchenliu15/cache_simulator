# Cache simulator
A command line tool to simulate load, store, and modify data in a LRU cache.

## Usage
`./cache [-hv] -s <s> -E <E> -b <b> -t<tracefile>`
- -h: Optional help flag that prints usageinfo
- -v: Optional verbose flag that displays traceinfo
- -s: Number of set index bits (S = 2sis the number ofsets)
- -E: Associativity (number of lines perset)
- -b:Number of block bits (B = 2bis the blocksize)
- -t: tracefile e.g.
  `
  I 0400d7d4,8
    M 0421c7f0,4
    L 04f6b868,8
    S 7ff0005c8,8
  `

## Challenges
- Implement an efficient 2D array data structure with properties including LRU and easy to use APIs
- Parse and read command line arguments, flags and tracefiles's actions

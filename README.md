# meca-c

Using second-order cellular automata as a means to encrypt data.

Implemented using modern C features such as `_Generic` and has a single macro function to encrypt/decrypt.

## Options

```$ ./bin/meca -h
MECA Encryptor (c) John Holly 2023 - Encrypt files/data using second-order cellular automata
usage:
./bin/meca -i <input-file> -o <output-file> \
        [-c <chunk-size> | --chunk-size <chunk-size>] [-w <1|2|4|8> | --word-size <1|2|4|8>] \
        [--rule <hex|decimal> | -r <hex|decimal>] [--epochs <decimal|hex> | -e <decimal|hex>]
```

**NOTE:** all integers can be specified as hex on the command line

- **[i/input-file]:** specify an input file to encrypt/decrypt
- **[o/output-file]:** specify and output file for encrypted/decrypted data
- **[c/chunk-size]:** specify how large a chunk is (i.e.) how many specified words are read to be treated as t[0] and t[-1]
- **[w/word-size]:** specify the word size meaning how large each entity in the chunk buffer is (1, 2, 4, 8 bytes)
- **[r/rule]:** specify the rule to use during epochs of encryption
- **[e/epochs]:** specify the number of timesteps to evolve the cellular automata

## FIXME

- Cat'ing input into the program breaks the progress bar
- Tom-foolery using uint64_t words
- Parallelize
- Introduce key material

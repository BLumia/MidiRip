## MIDI Rip

### Info

This is just a simple c++ tiny little program that search Standard Midi File header data from binary data file and dump the `.mid` file out.

### Usage

You can use any c++ compiler to compile the source code. Assume you now got `midirip.exe`, then do this:

``` bash
./midirip.exe <your binary file>
```

Dumped midi files will be like `midirip_extract_0001.mid` .

This program works under windows and linux.

### License

WTFPL
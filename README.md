# What is this?
Made for my personal use. Fgraph simply reads someone elses C code and makes a graph of what functions depend on what using graphviz format.  

# Usage 
Minimalist commandline program. Very similar to how you would use or pipe stuff like `cat` or `grep.`  
```fgraph | dot -Tpng -o out.png```

# Output
[](https://github.com/Jankatay/fgraph/blob/master/out.png)

# Why?
I decided I will try and make a project with as least tooling like old programmers to grow some muscle memory. It also helps learning how to read documentation from manpages and header files.
Rules for this "game" were as follows.
- No internet besides pushing to github. This means no googling or stackoverflow etc.
- No IDE. I am only allowed vim without plugins. Vim's builtin syntax highlighting and autocomplete disabled as well.
- No desktop environment, the entire thing has to be done from tty mode.

## Result
At first I geniuenly did not have enough programming endurance to code for long but it got better as time went on.
I did let myself use gdb and valgrind too so it was not as bad, just different.
Now I do understand some naming conventions tho. It used to be necessary to make even smallest of projects greppable without LSPs I guess.

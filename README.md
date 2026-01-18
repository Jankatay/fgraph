# What is this?
Made for my personal use. Fgraph simply reads someone elses C code and makes a graph of what functions depend on what using graphviz format.  

# Usage 
Minimalist commandline program. Very similar to how you would use or pipe stuff like `cat` or `grep.`  
```fgraph | dot -Tpng -o out.png```

# Output
<img src="out.png" alt="">

# Why?
I decided I will try and make a project with very VERY little amount of tooling like old programmers to grow some muscle memory. It also helps learning how to read documentation from manpages and header files.
Rules for this "game" were as follows.
- No IDE. I am only allowed vim without plugins. Vim's builtin autocomplete (does exist, in fact) and syntax highlighting disabled as well.
- No graphical desktop, the entire thing has to be done from tty1 for code and tty2 for manpages.
- No internet besides pushing to github. This means no googling or stackoverflow etc.

### Result
At first I geniuenly did not have enough programming endurance to code for long but it got better as time went on.
I did let myself use gdb and valgrind too so it was not as bad, just different.
Now I do understand some naming conventions tho. It used to be necessary to make even smallest of projects greppable without LSPs I guess.

### Fun fact
I did not hear or notice my laptop fans spin once during this entire project.

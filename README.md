# cdtags 

Sort of like a replacement for "cd" but with configurable aliases.  Written in C++ & Bash.  

Often I find myself doing:
   
    cd /some/long/hard/to/remember/path/to/project/src

and this is typed a lot.  Wouldn't it be faster to do:

    cdt proj/src

## Alternative:

You can probably use $CDPATH to do most of this, or aliases. 

This was heavily inspired by [dtags |https://github.com/joowani/dtags].

## Why:

While dtags is fantastic, My biggest issue is that enabling it on my terminal can take a long time,
especially when my home directory and python install just happens to be on an NFS mount.  The time 
spent opening a terminal is sometimes 10+ seconds.   

The bash config for this is fairly straightforward, it doesn't call out to python or run anything
potentially expensive, but yet implements most of the same functionality as dtags. 

## Requirements:

- C++11 compatible compiler.
- boost

## Installation:

    mkdir build
    cd build 
    cmake -DCMAKE_BUILD_TYPE=Release .. 
    make 
    sudo make install

Now add the following to your ~/.bashrc: 
    source /usr/local/share/cdtags/cdtags-activate.sh

# TODO 
- Understand $CDPATH more and see if I can integrate it with the completion infrastructure.
- zsh completion


# Usage

## Adding a tag
    
    cdtags add <tag> <path>

## Remove a tag

    cdtags remove <tag>

## List current tags:
    
    cdtags list

## Changing directories:

    cdt path

Where "path" can be an absolute path, relative path, or tag-relative path. Command line completion
also works here.

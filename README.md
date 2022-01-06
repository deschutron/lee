# lee
_A_ _splendid_ _language_ _for_ _rapid_ _application_ _development_

Here is my latest draft of the code for the Lee programming language.

It's designed to be the language I would want to use to throw together some programs quickly. It's meant to bring the fun back into programming, like when you're first learning. To achieve this, I use the principles of Object Oriented Mathematics - my conception of combining mathematical algebra notation with object-oriented programming syntax. The language is also designed based on functional programming principles. In mathematics, you reason about what things are, which are defined in various ways, sometimes recursively, and it gives the mind (well, my mind) easy handles with which to manipulate the entities being reasoned about. So Lee is designed to work that way.

A lot of inspiration for how this is to be realised comes from Javascript, especially with ES5+ notation. Javascript already has some of the object-oriented functional programming ease-of-use and beauty that I want in Lee. Lee is what I feel myself wanting programming in next when I'm writing code in Javascript.

# Build instructions
<code>
  ./build.sh
</code>

# Execution instructions
<code>
  ./lee.elf
</code>

# Structure
<code>lee_parser.peg</code> contains the parser tree.
<code>packcc.elf</code> generates the parser in C code in files <code>lee_parser.h</code> and <code>lee_parser.c</code>.
<code>lee.c</code> contains the <code>main</code> function. That's the one entry point for the program at the moment.
It takes in a file of Lee code on stdin and then evaluates and executes it.
Node syntax and data are stored in a tree of <code>node</code> objects. As such <code>node.h</code> and <code>node.c</code> are central to the functionality of the program.

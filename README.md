# Meridian

An WIP compiler for a custom programming language i've named "Meridian".
The language is heavily inspired by Standard ML & Haskell

# Documentation

## Basics

### Examples

#### Lisp Pretty Printer Example
```
type Atom =
    | String String
    | Number Float
    | Boolean Bool
    | Identifier String
    | List [Atom]

let prettyPrinter atom = match atom
    String str -> "\"" ++ str "\""
    Number n -> to_string n
    Boolean n -> if n then "#t" else "#f"
    Identifier id -> id
    List lst -> (foldl (fn acc x -> acc ++ x ++ " ") "(") ++ ")"

let exampleCode = List [String "hello, world", Boolean false, Number 25.03, List []]

let main = println (prettyPrinter exampleCode)
```

# Building

## Dependencies

- GCC
- CMake
- llvm-18
- GPerf

The better option is to use the nix package manager and run `nix develop`

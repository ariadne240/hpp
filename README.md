# CLEE Document

## Motivation

### Theorem & Proof System
For an expressive mathematical theorem and proof system, we necessarily need a very complicated language with diverse features. But then it will be extremely difficult for a human to write and understand the code written in the language. Therefore, we need an appropriate text editor suitable to easy generation and modification of codes. The language and the text editor should bond tightly and work as a united system, so we call it a CLEE (complicated language & easy editor) system.

### Recording System
We normally use LaTeX system to write and store mathematical formulae. It is de facto standard in academics. However, it rarely covers the expression of the context or semantics. More than two distinct meanings are represented by a single symbol. It is almost impossible to keep LaTeX’s base system without further radical improvements and simultaneously make a development in semantics. New CLEE language has `.clee` as a common filename extension. It has brought most grammatical structures from Haskell and shares some features with C++.

## Grammatical Features

### Ownership
A variable can have its owner in CLEE. We can declare an owner by the keyword `owner`.
```haskell
owner owner0
owner owner1, owner2, owner3
```
A variable of an owner can have a reference of only owner-free and the owner's variables. An owner-free one can only mention owner-free variables. The owner of a variable is shown in its type declaration.
```haskell
owner<owner0> a :: Int
a = 2
b :: Int
b = a + 2 -- compile error
owner<owner0> c :: Int
c = a + 2 -- not error
```
A variable can have multiple owners. Then a variable can reference another if they share a common owner.
```haskell
owner<owner0, owner1> a :: Int
a = 2
owner<owner0, owner2> b :: Int
b = a + 2 -- not error
owner<owner2, owner3> c :: Int
c = a + 2 -- compile error
```

### Exclusive Ownership
A variable might have an exclusive owner. The keyword `exowner` does the job. An owner cannot be both a (normal) owner and an exclusive owner.
```haskell
exowner owner0
owner owner0 -- compile error
```
A variable cannot have more than one exclusive owner. It also cannot simultaneously have a normal owner and an exclusive owner.
```haskell
exowner<owner0> a :: Int
a = 2
exowner<owner0, owner1> b :: Int
b = 2 -- compile error
```
If a variable has an exclusive owner, it can be only referenced by another variable of same exclusive owner.
```haskell
exowner<owner0> a :: Int
a = 2
exowner<owner0> b :: Int
b = a + 2 -- not error
owner<owner1> c :: Int
c = a + 2 -- compile error
owner<owner0> d :: Int -- compile error
```

### Human-assisted Overloading
The current language is not suitable for supporting the proof-making procedure effectively. The essence of proof is that it cannot be produced in a mechanical way. It can be (easily) proven that it is not possible to build a machine which fetches mathematical formulae as input and generates the proof as output. A human mind should intervene and give some creativeness. For instance, we often need to identify two different objects which can be proven with inventiveness. Actually, identification takes critical roles in mathematics.
In computer languages such as C++ or Java, there is a feature called overloading, which allows different functions/methods with a same name when they have different input types. The functions of a same name with same input types are, however, not permitted, because of ambiguity. But if two functions are proven to have same results, we can choose an appropriate function to use.
For instance, it is quite often that an asymptotically fast algorithm shows lower performance at small size of input. Schönhage-Strassen algorithm is faster than Karatsuba algorithm for sufficiently large numbers, whereas slower for numbers below about $2^(2^15)$. Let us have such functions `ss` and `karatsuba` in Haskell. Both have the same type of `Integer -> Integer -> Integer`. Then, we would sometimes want to use `ss` and sometimes desires `karatsuba`. But it is counterintuitive to have different names for procedures with same results and intentions.
If we use *human-assisted overloading* (HAO), this problem can be solved. We can write codes like below:
```haskell
multiply :: Integer -> Integer -> Integer
multiply = ss
multiply :: Integer -> Integer -> Integer
multiply = karatsuba
```
It is not allowed in the original Haskell (e.g. Haskell2010) to have variables of a duplicate name. But in CLEE, this is not only possible but also recommended. It allows the compiler to optimize it at compile time. It is necessary for human-assisted overloaded variables to have explicit type declarations. The type declaration for each implementation must be exactly identical. Due to the restriction, we do not need multiple type declarations. It is valid to omit all but one declaration.
```haskell
multiply :: Integer -> Integer -> Integer
multiply = ss
multiply = karatsuba
```
Two implementations can have different owners.
```haskell
owner<owner0, owner1> multiply :: Integer -> Integer -> Integer
multiply = ss
owner<owner2> multiply :: Integer -> Integer -> Integer
multiply = karatsuba
```
For a variable `owner<owner1, owner2> a`, both variables are accessible. For a variable `owner<owner1> b` only the first implementation would be directly accessible. 

#### `default`
It is possible to set a default implementation to human-assisted overloaded variables. There cannot be more than one default implementation. If we add a `default` keyword to the code, it would be reflected while compiling.
```haskell
default multiply :: Integer -> Integer -> Integer
multiply = ss
multiply :: Integer -> Integer -> Integer
multiply = karatsuba
```

####`nooverload`
If we want a variable to be not overloaded, we can give a `nooverload` keyword in type declaration. There cannot be more than one nooverload implementation. Otherwise the compiler would give a compile error.
```haskell
nooverload multiply :: Integer -> Integer -> Integer
multiply = ss
multiply :: Integer -> Integer -> Integer
multiply = karatsuba -- compile error
```

### Blocks
A CLEE code could have a lot of blocks. A reference of the variables in the block from outside the block gives a compile error.
```haskell
{
 a :: Int
 a = 2
}
b :: Int
b = a + 2 -- compile error
```
A pure block cannot affect `main` since it is inaccessible from outside. There are some keywords which we can add to blocks. By giving keywords to blocks, we can get additional functions.

#### Namespace Blocks
`namespace` keyword can give names to blocks. Then we can access the variable in block by its name. The double colon `::` are used for this, as in C++. Unlike in type declaration, the double colon should stick to both parameters, such as `block::var`, not `block :: var`. Moreover, a variable logically in the block can be defined outside the block, also by mention of the name of the block.
```haskell
namespace<block0> {
 a :: Int
 a = 2
}
b :: Int
b = a + 2 -- compile error
c :: Int
c = block0::a + 2 -- not error
block0::d :: Int
d = block0::a + 2 -- not error
e :: Int
e = d + 2 -- compile error
```
Namespace blocks can be nested.
```haskell
namespace<block0> {
 namespace<block1> {
  a :: Int
  a = 2
 }
}
b :: Int
b = block0::block1::a + 2 -- not error
```

#### Keyword-Share Blocks
`owner`, `exowner`, `default`, `nooverload` can be applied to all variables in a specific block. It is clear that `owner` and `exowner` cannot apply to one block together.
```haskell
owner<owner0> nooverload namespace<block0> {
 a :: Int
 a = 2
 a :: Int
 a = 3 -- compile error (nooverload)
}
b :: Int
b = a + 2 -- compile error (owner)
```

#### Noblocks
`noblock` makes a block accessible freely from outside. In fact, a block with the keyword, called a noblock, does not localize the scope of its variables in it. A noblock can have name, but it should not have a `namespace` keyword.
```haskell
owner<owner0> noblock<block0>{
a :: Int
a = 2
}
b :: Int
b = a + 2 -- compile error (owner)
owner<owner0> c :: Int
c = a + 2 -- not error (noblock)
noblock<block0> namespace<block0>{
} -- compile error
```

```
It might be an axiom block, a proof block, or a reference block. A pure axiom block is inconvenient to use, but simple to comprehend theoretically. It has a collection of words (dictionary), axioms and axiom schemata.
```

### `Logic`
Haskell has a built-in type `Logic`. It is a new type for logical elements.
We can easily define a boolean type `Bool` as follows: `data Bool = False | True`. But it is not easy to define an integer type `Int`: `data Int = I# Int#`
A logic type is for a string with logical contexts. For example, let us have a simple `Logic` variable. We can think of a logical and function `land`. A basic and function
would have a type of `Bool -> Bool -> Bool` or `[Bool] -> Bool`. But to preserve the structure of a string, we need a different type.
```haskell
land :: [Logic] -> Logic
land (x:xs) = 
land2 :: Logic -> Logic -> Logic
land2 x y = 
```

### Sentences
A sentence is represented by a variable of typeclass `Senable`.
```haskell
class Senable a where
 senify :: a -> Senable
```
A CLEE code is a collection of sentences, which consists of words. Every word is split by one or more blank symbols. A word in a sentence could have different meanings in different scopes. We can set scopes at desired positions using blocks, opened and closed by a pair of curly braces. It is also possible to use nested blocks.

### Axioms
An axiom has an `axiom` keyword.

# CLEE To be Added
CLEE is a variant of Haskell. It is not a subset or a superset of Haskell, but little modification would be enough to transform a Haskell code into a CLEE code.

## Overloading
Overloading should be more interesting. We always have to declare the type of the variables *right* before its content, unlike Haskell.
```haskell
a = 2 -- without declaration, compile error
```
A variable can be overloaded with same type or different types.
```haskell
default overload<0> a :: Int
a = 2
overload<0> a :: Int
a = 3
overload<0> a :: String
a = "String"
```
A type of any occurrence of an overloaded variable is done by a compiler. A variable which satisfies the condition is used. If there are many which satisfies the type condiiton, then the compiler may use any of them. If there is no such variable, then it gives the compile error.
```haskell
default overload<0> a :: Int
a = 2
overload<1> a :: Double
a = 3.0
b :: Double
b = a -- b = 3.0
```

### `default`
Theere can be only one `default` keyword among the variables of same name. The default definition is always preferred by the compiler when it satisfies the type conditions. If it fails the condition, the compiler chooses the next candidate by its mean. It depends on the implementation of the compiler; it could be random. You can give more information by `.cset` file.
```haskell
default overload<0> a :: Int
a = 2
overload<0> a :: Int
a = 3
b :: Int
b = a + 1 -- b == 3
```

### `overload`
You can deprecate or forbid overloading variables. The default overload value is 0. `overload<n>` makes the variable to have an overload value of `n`. The `overload<0>` variable can be freely overloaded. The `overload<max>` variable is completely banned to be overloaded. The `overload<1>` variables cannot be overloaded unless it is ignored by `forceload<1>` keyword. `overload<2>` cannot be ignored by `forceload<1>`. However, `forceload<2>` or `forceload<3>` ignores it. More information could be given to the compiler by `.cset` file.
```haskell
overload<0> a :: Int
a = 2 -- compile error by below overload<1>
overload<1> a :: Int
a = 3
```
```haskell
overload<0> forceload<1> a :: Int
a = 2 -- not error
overload<1> a :: Int
a = 3
overload<0> forceload<1> a :: String
a = "String" -- not error
```
```haskell
overload<max> a :: Int
a = 2
overload<0> forceload<3> a :: Int
a = 3 -- compile error by above overload<max>
```

### Overloading name
Variables can have its overloading name. We can use `lname` keyword for the purpose.
```haskell
default overload<0> lname<n0> a :: Int
a = 2
overload<0> lname<n1> a :: Int
a = 3
b :: Int
b = a + 1 -- b == 3
c :: Int
c = a.n0 + 1 -- c == 3
d :: Int
d = a.n1 + 1 -- d == 4
```

### `.cset`
A CSET (Compile set) file is a structured collection of compilation options. It has `.cset` as a file extension.

# *More* Modification would ocurr.

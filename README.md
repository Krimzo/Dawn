# About
Dawn is currently under development and may contain bugs.

```dawn
func main(ref args) {
    print("Hello World!")
}
```

# Variables
```dawn
const a = 7 // constant
var b = 9   // variable
ref p = b   // reference
```

# Primitives
```dawn
var b = false  // bool (1 byte)
var i = -10    // integer (8 bytes)
var f = 6.0    // float (8 bytes)
var c = 'd'    // char (1 byte)
var s = "nice" // string (32 bytes)
```

# Strings
```dawn
const normal_string = "some string"
const complex_string = "some value = {2 + 3}"
```

# Arrays
```dawn
var a0 = int[10]            // array of 10 int elements
var a1 = [4, "cool", false] // array of 3 elements
a1[1] += " stuff"           // access of the second element
```

# Enums
```dawn
enum Animal {
    dog
    cat = -6 // enums can store values
    mouse    // but don't have to (value is set to nothing)
}
var animal = Animal{cat}
var value = animal->value
```

# Structs
```dawn
struct Person {
    not_important // fields don't need initializers (value is set to nothing and can't be changed)
    name = ""     // but should have them
    age = 0

    func who() {
        print("{self->name} is {self->age} years old")
    }

    oper +(ref age) {
        return Person{ name = self->name, age = self->age + age }
    }

    cast string {
        return "Person({self->name}, {self->age})"
    }
}
const person = Person{ name="Krim", age=24 }
person->who()
```

# Branching
```dawn
var value = 5

if value > 0 {
    print("it's positive")
}
elif value < 0 {
    print("it's negative")
}
else {
    print("it's zero")
}

switch value {
    case 1, 2, 3 {
        print("first case")
    }
    case 5 {
        print("second case")
    }
    default {
        print("default case")
    }
}
```

# Loops
```dawn
var i = 0
loop {
    if i >= 5 {
        break
    }
    print(i)
    i += 1
}

i = 0
while i < 5 {
    print(i)
    i += 1
}

/* for can loop:
    ranges ( start_incl >> end_excl )
    strings ( "some_string" )
    arrays ( [elem1, elem2] )
    */
for i : 0 >> 5 {
    print(i)
}
for c : "stuff" {
    print(c)
}
for val : [1, 4.2, ["other", " array"]] {
    print(val)
}
```

# Functions
```dawn
func some_fun(var arg0, ref arg2) {
}
some_fun(5, false)
```

# Lambdas
```dawn
var sub = `ref a, ref b`{
    return a - b
}
print(sub(5, 3))
``{ print("lambdas are cool") }()
```

# Errors
```dawn
var a = 5
try {
    throw a
}
catch err { // caught by reference
    err += 7
}
print(a)
```

# Misc
```dawn
import "path..." // importing modules (uses relative path from the source file)
var i = int(...) // casting basic types [bool, int, float, char, string] (use standard.cpp for more functions)
```

# C++
Syntax is stored inside syntax.h and can be changed.<br/>
Binding C++ functions is also possible. Look at standard.cpp.<br/>

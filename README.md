# About
Dawn is currently under development and may contain bugs.

```dawn
func main(ref args) {
    print("Hello World!")
}
```

# Primitives
```dawn
let b = false  // bool (1 byte)
let i = -10    // integer (8 bytes)
let f = 6.0    // float (8 bytes)
let c = 'd'    // char (1 byte)
let s = "nice" // string (32 bytes)
```

# Arrays
```dawn
let a0 = int[10]            // array of 10 int elements
var a1 = [4, "cool", false] // array of 3 elements
a1[1] += " stuff"           // access of the second element
```

# Enums
```dawn
enum Animal {
    dog
    cat = -6 // enums can store values
    mouse    // but don't have to
}
let animal = Animal{cat}
let value = animal::value
```

# Structs
```dawn
struct Person {
    name    // fields don't need initializers
    age = 0 // but can have them

    func who() {
        print(self::name, " is ", self::age, " years old")
    }

    oper +(ref age) {
        return Person{ name = self::name, age = self::age + age }
    }
}
let person = Person{ age=23, name="Krim" }
person::who()
```

# Variables
```dawn
let a = 17 // constant
var b = 9  // variable
ref p = b  // reference
```

# Functions
```dawn
func some_fun(var arg0, ref arg2) {
}
some_fun(5, false)
```

# Branching
```dawn
let value = 5

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

for ref i: 0 >> 5 {
    print(i)
}
```

# Throwing
```dawn
var a = 5
try {
    throw a
}
catch err {
    err += 7
}
print(a)
```

# Lambdas
```dawn
let sub = `ref a, ref b` {
    return a - b
}
print(sub(5, 3))
``{ print("lambdas are cool") }()
```

# Misc
```dawn
import "path..."    // importing modules
let i = to_int(...) // casting primitive types
```

# About
Dawn is currently under development and may contain bugs.

```dawn
func main(array& args) {
    print("Hello World!")
}
```

# Variables
```dawn
int a = 7  // constant
int? b = 9 // variable
int& p = b // reference
```

# Types
```dawn
nothing n = ()                    // nothing (0 bytes)
bool b = false                    // bool (1 byte)
int i = -10                       // integer (8 bytes)
float f = 6.0                     // float (8 bytes)
char c = 'd'                      // char (1 byte)
string s = "nice"                 // string
function fn = ``{}                // function
SomeEnum se = SomeEnum::something // enum
SomeStruct ss = SomeStruct{}      // struct
array a = []                      // array
range r = 0>>10                   // range (16 bytes)
```

# Strings
```dawn
string normal_string = "some string"
string complex_string = "some value = {2 + 3}"
```

# Arrays
```dawn
array a0 = int[10]             // array of 10 int elements
array? a1 = [4, "cool", false] // array of 3 elements
a1[1] += " stuff"              // access of the second element
```

# Enums
```dawn
enum Animal {
    dog
    cat = -6 // enums can store values
    mouse    // but don't have to (value is set to nothing)
}
Animal animal = Animal::cat
int value = animal.value
```

# Structs
```dawn
struct Person {
    not_important : nothing
    name : string
    age : int

    func who() {
        print("{self.name} is {self.age} years old")
    }

    oper +(int age) {
        return Person{ name = self.name, age = self.age + age }
    }

    cast string {
        return "Person({self.name}, {self.age})"
    }
}
Person person1 = Person{ name="Krim", age=24 }
Person? person2 = Person((), "Krim", 22)
person2.age += 2
person1.who()
person2.who()
```

# Branching
```dawn
int value = 5

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
int? i = 0
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
func some_fun(int arg0, bool& arg2) {
}
some_fun(5, false)
```

# Lambdas
```dawn
function sub = `int a, int b`{
    return a - b
}
print(sub(5, 3))
``{ print("lambdas are cool") }()
```

# Errors
```dawn
int a = 5
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
int i = int(...) // casting basic types [bool, int, float, char, string] (use standard.cpp for more functions)
```

# C++
Syntax is stored inside syntax.h and can be changed.<br/>
Binding C++ functions is also possible. Look at standard.cpp.<br/>

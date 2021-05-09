# Styling
micron's kernel requires a specific styling to keep the codebase clean and consistent.

## Names
All names must be in snake_case and must be descriptive.
```c++
// NO
void fooPtr(baz& Bar);

// STILL NO, BUT BETTER
void foo_pointer(baz& bar);
```

## Namespaces
All methods should be in namespaces. You may **not** create a new root namespace without approval.
```c++
// NO
void foo_pointer(baz& bar);

// NO
void fie::quux::foo_pointer(baz& bar);

// PERFECT
void sys::quux::foo_pointer(baz& bar);
```

## Formatting
Put brackets on the same line.
Do not use brackets on one-statement conditionals.
    Break lines and tab on one-statement conditionals.
Prefer to align names when defining structures.
Space between brackets and parenthesis.
```c++
// FUCK NO
void sys::quux::foo_pointer(baz& bar){
    if (bar.fie == 0) { return; }
    else if (bar.fie < 0)
    { bar.fie++; }
    else if (bar.fie > 0)
        { bar.fie++; }
    else{
        bar.quux = 0;
    }    
}

// PERFECTION
void sys::quux::foo_pointer(baz& bar) {

    if (bar.fie == 0) 
        return;
    else if (bar.fie < 0)
        bar.fie++;
    else if (bar.fie > 0)
        bar.fie++;
    else
        bar.quux = 0; 
    
}
```
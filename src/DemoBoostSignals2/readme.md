# boost::signals2

- Implementation of a managed signals and slots system
- **Signal:**
    - Also called publishers or events
- **Slots:**
    - Also called subscribers or event receivers (callbacks)

- Slots are called when signals are emitted

### Trivia - boost::bind
```cpp
int f(int a, int b)
{
    return a + b;
}

int g(int a, int b, int c)
{
    return a + b + c;
}

bind(f, _2, _1)(x, y);        // f(y, x)
bind(g, _1, 9, _1)(x);        // g(x, 9, x)
bind(g, _3, _3, _3)(x, y, z); // g(z, z, z)
bind(g, _1, _1, _1)(x, y, z); // g(x, x, x)
```
# Implementing interfaces - Rubber duck approach


## Context:


- I have a `Toaster` class which implements an active object pattern

- `Toaster` constructor requires (amongst other parameters) a `ITempSensor` object, which models an **Aggregation** relationship

- `ITempSensor` is an interface that models the basics of a temperature sensor

- The reason to have an interface instead of a direct implementation is that I want to be able to use polymorphism to construct `Toaster` objects in 3 basic different scenarios:

    - A demo scenario

        - In this case, the `Toaster` object would be constructed with a `TempSensorDemo` implementation of the `ITempSensor` interface

    - A unit test scenario

        - In this case, the `Toaster` object would be constructed with a `TempSensorMock` implementation of the `ITempSensor` interface

    - A real scenario

        - In this case, the `Toaster` object would be constructed with a `TempSensorI2C` implementation of the `ITempSensor` interface

## Problem:


- The problem right now is that `Toaster`'s constructor calls a method that is defined only in the implementation of `TempSensorDemo` class (not part of the common interface members). The method is the following:
```C++
template <typename F>
void register_callback(F &&handler)
{
    m_signal.connect(handler);
}
```

- This makes issues a compilation error and breaks the polymorphism strategy

## Possible solutions

### Idea n 1:

- Make the `register_calback` method as a virtual method of the interface

    - But because this method is a template, it is not possible to define it in the interface as a virtual method

    - I could transform it into a non-template method in the interface class but I'm not sure that this is in fact something that should be common to all implementations of `ITempSensor`

## Idea n 2:

- Define a common `initialize` method in the interface, this way all classes that implement `ITempSensor` could override it with it's own specific needs

    - In the case of `TempSensorDemo`, it would register the callback and the `Toaster` constructor would only call `m_temp_sensor->initialize()` and don't care about what is done internally in each different implementation...

    - The problem with this was that the `void register_callback(F &&handler)` method currently expects a parameter that is only known within the `Toaster` object context, so I'd need a virtual `initialize` method with the following signature: `void register_callback(F &&handler)` which goes back to the problem of templates and virtual methods don't liking each other

    - I thought about the possibility of somehow implementing a virtual `initialize` method which is variadic (with the `(...)` syntax) and the idea would be that the variadic implementation on the interface wold serve as a fallback option

        - Problem is that to override, I need the exat same signature, and if a implement `initialize (...)`, I can't access the parameter, which I need to
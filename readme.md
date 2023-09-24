# Active Object Pattern

- This repository implements (or at least tries to) the active object pattern in C++ as is described in the book "Practical UML Statecharts in C/C++: Event-Driven Programming for Embedded Systems - Miro Samek".

- The example implemented is the Toaster example, presented in Chapter 2
<img src="img/toaster-statechart.png" width="400"/>

- In this pattern, Active Objects (Actors) are event-driven, strictly encapsulated software objects running in their own threads of control that communicate with one another asynchronously by exchanging events.


## How to operate the repository
- To format the code base with clang-format:
```bash
./bbuild.sh -f
```

- To build:
```bash
./bbuild.sh -b <target>
```
- Here, `<target>` should be substituted by one of the `Demo<...>` sub-folders inside the `src` folder or the `test` folder
  - Example: `./bbuild.sh -b DemoThreadSafeQueue`
  - Example: `./bbuild.sh -b test`

- To rebuild:
```bash
./bbuild.sh -r <target>
```

- To execute the built binary:
```bash
./bbuild.sh -e <target>
```

- To format, build and execute:
```bash
./bbuild.sh -f -b -e <target>
```
  - Example: `./bbuild.sh -f -b -e DemoToasterActiveObject`
  - Example: `./bbuild.sh -f -b -e test`

- To check all options available::
```bash
./bbuild.sh --help
```
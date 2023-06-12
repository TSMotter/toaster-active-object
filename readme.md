# Active Object Pattern

## References
- [Key Concept: Active Object (Actor) - 2005-2023 Quantum Leaps](https://www.state-machine.com/active-object)

- In this pattern, Active Objects (Actors) are event-driven, strictly encapsulated software objects running in their own threads of control that communicate with one another asynchronously by exchanging events.
## How to operate the repository
- To format the code base with clang-format:
```bash
./do.sh -f
```

- To build:
```bash
./do.sh -b <target>
```
- Here, `<target>` should be substituted by one of the `Demo<...>` sub-folders inside the `src` folder or the `test` folder
  - Example: `./do.sh -b DemoThreadSafeQueue`
  - Example: `./do.sh -b test`

- To rebuild:
```bash
./do.sh -r <target>
```

- To execute the built binary:
```bash
./do.sh -e <target>
```

- To format, build and execute:
```bash
./do.sh -f -b -e <target>
```
  - Example: `./do.sh -f -b -e DemoToasterActiveObject`
  - Example: `./do.sh -f -b -e test`

- To check all options available::
```bash
./do.sh --help
```
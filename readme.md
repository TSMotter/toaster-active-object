# CPP study repository - Threaded Avtive Object

- To format the code base with clang-formatter:
```bash
./format.sh
```

- To build some of the demos or tests:
```bash
./bbuild.sh <target>
```
- Here, `<target>` should be substituted by one of the `Demo<...>` or the `test` folders
  - Example: `./bbuild.sh DemoActiveObject`
  - Example: `./bbuild.sh test`

- To run:
```bash
./run.sh <target>
```

- To format, build and run:
```bash
./bbuild.sh <target> run
```
  - Example: `./bbuild.sh DemoToasterActiveObject run`
  - Example: `./bbuild.sh test run`
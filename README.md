# Computer-Architecture
ECGR 4181 Event-Driven Cpu Simulator

## Extensibility Summary

### Pipeline

A common `Pipeline` interface is provided that allows a `Cpu` to create a linked list of `Pipeline` instances that are each responsible for their own logic and responsibilities. The `Cpu` simply provides access to the registers/memory and any flags/control they might need.

The various test methods in the `main.cpp` file take full advantage of this setup to test each fo the `Fetch`, `Decode`, `Execute` and `Store` units in isolation. Each sets up a `Cpu` with only the one unit under test and a `TestPipeline` that echoes whatever instructions it receives when appropriate. Since the `Cpu` handles all `Pipelines` in a generic way, and sets no expectations on their behavior or number, it can support any number of serial pipeline stages. 

`Pipeline` setups that form trees or graphs are not supported at this time, but could be by turning the `Pipeline::next` attribute into a container rather than a single reference.

### Event

A common `Event` interface is provided that can be extended to allow messages that contain any kind of data to be scheduled at specific times by the simulation. The `Pipeline` interface uses this to create a `PipelineInsertEvent` that is capable of passing along an `Instruction` to any other `Pipeline`.

### Instruction
The `Instruction` interface provides a very generic structure for specifying an `operation` and set of `arguments`. Instances of `Instruction` are then decoded into more specific sub classes of `Instruction` that contain all of the logic/data they need.

Rather than have the `Cpu` contain the code for how to process every type of `Instruction` a single interface is provided as an entry point that the `Cpu` can invoke. Adding new `Instructions` to the simulation only requires a simple subclass and a new line in the `Decode` switch to detect/allow it. Other than the `Decode` stage which must deal with specifics, all other `Pipeline` stages handle `Instructions` at the most general level possible.


# Cpu Design

The cpu is a multi stage pipeline cpu capable of executing one instruction at a time. It is not a superscalar design.

Currently, all memories are modeled as components of the cpu itself. When memory simulation needs become more complex, this will need to change.

The cpu structure does not constrain the sizes of any of its memories or number of stages in its pipeline.

Pipeline stages can be easily added to the cpu using the `addPipeline` method. Each new pipeline will be linked to the previously added one to form a linked list.

The cpu itself does not constrain the type or behavior of the pipelines. As long as they implement the base `Pipeline` interface they will be accepted. Each pipeline is responsible for defining the types of events it will accept and generate, as well as what data is will track and process.

## Pipelines
Currently the simulation configures a `Cpu` with 4 `Pipeline` stages: `Fetch`, `Decode`, `Execute`, and `Store`. Pipelines pass instructions to each other via scheduling a `PipelineInsertEvent`.

### Fetch

The `Fetch` unit is the only pipeline to not accept the generic `PipelineInsertEvent` that comes loaded with an `Instruction`. Instead it receives `FetchEvent` instances that merely act as a signal to load the next instruction.

When a `Fetch` unit receives a `FetchEvent` it uses the `Cpu` `programCounter` to load the next instruction from the `Cpu` `Program`. The `Fetch` unit also contains logic for incrementing the `Cpu` `programCounter` and predicting branches. It coordinates branch predictions with the `Execute` stage by setting flags and storing the value of the pc when the prediction was made.

### Decode

The `Decode` unit is responsible for translating generic `Instruction` instances into more specific `DecodedInstruction` instances that have an `execute` method that will be triggered by the `Execute` unit.

Adding a new instruction to the simulation can be done by adding a reference to the `operation` in the `Decode` unit and extending the `DecodedInstruction` structure with an overloaded `execute` method. The `Execute` unit handles almost all `Instruction` instances in exactly the same manner because all `DecodedInstructions` share the same `execute` interface.

### Execute

The `Execute` unit is responsible for actually executing the logic stored in the `Instruction`. Rather than put all of the code for every instruction, and a complex switch to detect which path to take, each `Instruction` is responsible for containing the logic needed and exposing that through the `execute` method. 

The only instructions that the `Execute` unit explicitly looks for are `Store` instructions and the `halt` command, which it simply passes along to the `Store` unit unmodified.

### Store

The `Store` unit is responsible for operations that require writing data into memory. Additionally, as the last stage in the pipeline, it also handles the `halt` command by marking the `Cpu` `Program` as complete.

All events also share a common base `Event` struct that can be extended to accommodate passing data to other devices in the simulation at specific times.

## Events

Events are designed primarily for allowing simulation devices to communicate with each other with respect to specific times. The primary use case in the simulation currently is for each `Pipeline` unit to send the `Instruction` it just partially processed on to the next unit in the line. Since the next `Pipeline's` memory will already have an `Instruction` in it since they get `ticked` front to back, the `PipelineInsertEvent` that sends on the `Instruction` is scheduled for the next clock cycle.

The base `Event` structure only requires a `time` and a target `Device` to be sent to. Extensions to `Event` are free to require any additional information, like an `Instruction`, that they require as part of their message. The `Device` receiving the `Event` is responsible for casting the instance and processing any data that it might contain.

## Instructions

The base `Instruction` structure provides a simple interface for representing `Instruction` data. It had an `operation` that is the `string` that represents its assembly `op code`. It also has a `vector` if `int` values that represent its arguments, which are primarily register addresses or integer immediate values.

All `Pipeline` units have a register capable of storing a single `Instruction` that they will process the next time they are `ticked`.

### Decoded Instructions
The `DecodedInstruction` interface is an extension of the `Instruction` structure that adds an `execute` method that can be invoked by the `Execute` unit. Each specific `Instruction` is responsible for defining what `executing` it means. This divides the logic and makes each `Instruction` independent from the others. Since they are divided, adding new `Instruction` types is a straightforward and simple process.

The `DecodedInstruction` structure is also responsible for detecting if an instruction is a `floating-point` or `integer` instruction type. It sets a flag that specific `DecodedInstruction` extensions can use to determine which registers/memories to target.

There are currently three types of `DecodedInstructions` that represent different classes of `Instruction` that the `Cpu` is capable of executing. These are: `MemoryInstruction`, `ControlInstruction`, and `ArithmeticInstruction`

### MemoryInstruction

The `MemoryInstruction` structure provides a standard interface for processing memory related instructions like `Load` and `Store`.

### ControlInstruction

The `ControlInstruction` structure provides a standard interface for processing execution control instructions. It does this by exposing a `take` method and `destination` attribute. The `take` method can be overloaded by extensions to determine the behavior of how the decision to branch or not is made. The core `ControlInstruction::execute` method should be sufficient for handling most branching logic given a properly overridden `take` method.

### ArithmeticInstruction

The `ArithmeticInstruction` structure provides a standard interface for `Instructions` that perform calculations. It also has support for `immediate` variants of these instructions as long as the `immediate value` is an integer.

Extensions to `ArithmeticInstruction` can override the `execute` method to determine how the calculation is made. Using the `isFp` flag set by the `DecodedInstruction` structure, a single extension should be able to support both `integer` and `floating-point` variants of the instruction easily.

### Future considerations
When we get to parsing the actual binary file, the `Instruction` structure may be split into multiple structures to represent each `type` of instruction specified by the `RISC-V ISA`. In this case the `Decode` unit will perform the conversion to the `DecodedInstruction` structures that the `Execute` unit is expecting by using the binary values rather than the ones from the current `Instruction` structure. An alternative would be to parse the file and construct `Program` structure by mapping the binary values into the existing `Instruction` format. In this case the current `Cpu` model would not require any modifications to support the binary input file.


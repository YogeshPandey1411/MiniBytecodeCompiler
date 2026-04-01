# Mini Bytecode Compiler and Virtual Machine

## 📌 Project Overview

This project implements a **Mini Bytecode Compiler and Virtual Machine** based on core principles of compiler design. The system translates a simple custom programming language into **bytecode**, which is then intended to be executed using a **stack-based virtual machine**.

The project demonstrates the complete compilation pipeline from **source code → intermediate representation → execution model**, similar to real-world systems like JVM and Python VM.

---

## 🎯 Objectives

* To understand and implement the **compiler pipeline**
* To design a **custom intermediate representation (bytecode)**
* To build a **stack-based virtual machine**
* To demonstrate how high-level code is executed internally

---

## ⚙️ Features (Phase 2 - 50–60% Completed)

* ✅ Lexical Analysis (Tokenizer)
* ✅ Syntax Analysis (Parser)
* ✅ Abstract Syntax Tree (AST) Generation
* ✅ Bytecode Generation (Partial)
* ⏳ Virtual Machine Execution (To be implemented in Phase 3)

---

## 🏗️ Project Structure

```
MiniBytecodeCompiler/
│
├── src/
│   ├── main.c
│   ├── lexer.c / lexer.h
│   ├── parser.c / parser.h
│   ├── ast.c / ast.h
│   ├── codegen.c / codegen.h
│   ├── vm.c / vm.h
│
├── examples/
│   └── test1.txt
│
├── Makefile
└── README.md
```

---

## 🔄 Compilation Pipeline

```
Source Code
    ↓
Lexical Analyzer
    ↓
Parser + AST
    ↓
Bytecode Generator
    ↓
Virtual Machine (Phase 3)
    ↓
Execution Output
```

---

## 🧠 Technologies Used

* Language: **C**
* Concepts: **Compiler Design, AST, Bytecode, Virtual Machine**
* Tools: **GCC, Git, GitHub**

---

## ▶️ How to Run

### Step 1: Compile the project

```
make
```

### Step 2: Run the compiler

```
./compiler
```

---

## 📌 Example

### Input

```
print x + 5
```

### Output (Phase 2)

```
LOAD x
PUSH 5
ADD
PRINT
```

---

## 👥 Team Members

* **Yogesh Pandey** (Team Lead)
* Avdhesh Sayana
* Rishav Kukreti
* Animesh Tripathi

---

## 📅 Future Work (Phase 3)

* Implement full **Virtual Machine execution**
* Support multiple statements
* Add error handling
* Extend language features (loops, conditions)

---

## 📖 References

* Compilers: Principles, Techniques, and Tools (Dragon Book)
* Java Virtual Machine Documentation
* Python Virtual Machine Concepts

---

## ⭐ Conclusion

This project provides a hands-on implementation of compiler design concepts and demonstrates how a programming language is translated and executed internally using a virtual machine.

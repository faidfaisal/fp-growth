# FP-Growth Algorithm

An efficient implementation of the FP-Growth (Frequent Pattern Growth) algorithm for frequent itemset mining and association rule learning.

## Overview

This project implements the FP-Growth algorithm, a sophisticated method for discovering frequent patterns in transactional databases. Unlike traditional Apriori-based approaches, FP-Growth leverages an efficient tree structure (FP-tree) to minimize database scans, enabling rapid identification of frequent itemsets and generation of strong association rules.

## Features

- **Efficient FP-tree Construction**: Builds a compact tree structure to represent transaction data
- **Minimal Database Scans**: Requires only two passes through the dataset
- **Frequent Itemset Mining**: Discovers all frequent patterns meeting minimum support threshold
- **Association Rule Generation**: Derives strong association rules from frequent itemsets
- **Scalable Performance**: Handles large datasets efficiently with reduced memory overhead

## Algorithm Overview

The FP-Growth algorithm operates in two main phases:

1. **FP-tree Construction**: Scans the database to build a compressed representation of frequent items
2. **Pattern Mining**: Recursively mines the FP-tree to extract frequent patterns without candidate generation

This approach significantly outperforms traditional methods, especially for large datasets with long frequent patterns.

## Prerequisites

- C++ compiler with C++11 support or later (e.g., g++, clang++)
- Standard Template Library (STL)

## Installation

Clone the repository:
```bash
git clone https://github.com/yourusername/fp-growth.git
cd fp-growth
```

Compile the program:
```bash
g++ -std=c++11 -o fpgrowth main.cpp -O2
```

## Usage

Run the FP-Growth algorithm on your dataset:
```bash
./fpgrowth   [output_file]
```

### Parameters

- `input_file`: Path to the transaction database file
- `min_support`: Minimum support threshold (decimal or percentage)
- `output_file`: (Optional) File to write the results

### Input Format

The input file should contain transactions in the following format:
```
item1 item2 item3
item2 item4
item1 item3 item5
...
```

Each line represents a transaction with space-separated items.

### Example
```bash
./fpgrowth transactions.txt 0.3 results.txt
```

This command mines frequent itemsets from `transactions.txt` with a minimum support of 30% and writes the results to `results.txt`.

## Output

The program generates:

1. **Frequent Itemsets**: All itemsets meeting the minimum support threshold
2. **Association Rules**: Rules with their support and confidence metrics

Sample output:
```
Frequent Itemsets:
{item1, item2}: support = 0.45
{item1, item3}: support = 0.38
...

Association Rules:
{item1} => {item2}: support = 0.45, confidence = 0.75
...
```

## Project Structure
```
fp-growth/
├── fptree.cpp        # FP-Growth algorithm implementation
├── README.md         # This file
└── examples/         # Sample datasets and outputs
```

## Applications

FP-Growth is widely used in:

- Market basket analysis
- Recommendation systems
- Web usage mining
- Bioinformatics
- Text mining

## Performance

FP-Growth demonstrates superior performance compared to Apriori-based algorithms:

- Faster execution on dense datasets
- Lower memory consumption
- Better scalability for long patterns

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Course Information

**ESE 327 Project 1**  
Developed as part of the ESE 327 coursework.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## References

- Han, J., Pei, J., & Yin, Y. (2000). Mining frequent patterns without candidate generation. *ACM SIGMOD Record*, 29(2), 1-12.

## Author

[Faid Faisal]  
[faidfaisal1@gmail.com]  

## Acknowledgments

- Course instructor and TAs for guidance
- Original FP-Growth paper authors

---

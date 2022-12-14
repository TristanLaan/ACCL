# ACCL: Alveo Collective Communication Library

[![Documentation Status](https://readthedocs.org/projects/accl/badge/?version=latest)](https://accl.readthedocs.io/en/latest/?badge=latest)

ACCL is a Vitis kernel and associated XRT drivers which together provide MPI-like collectives for Xilinx FPGAs. ACCL is designed to enable compute kernels resident in FPGA fabric to communicate directly under host supervision but without requiring data movement between the FPGA and host. Instead, ACCL uses Vitis-compatible TCP and UDP stacks to connect FPGAs directly over Ethernet at up to 100 Gbps on Alveo cards.

ACCL currently supports Send/Recv and the following collectives:
* Broadcast
* Scatter
* Gather
* All-gather
* Reduce
* All-reduce
* Reduce-Scatter

## Installation
See [INSTALL.md](INSTALL.md) to learn how to build ACCL-enabled designs and interact with them from C++.
To use ACCL from Python, refer to [PyACCL](https://github.com/Xilinx/pyaccl).

## Citation
If you use our work or would like to cite it in your own, please use the following citation:

```
@INPROCEEDINGS{9651265,
  author={He, Zhenhao and Parravicini, Daniele and Petrica, Lucian and O’Brien, Kenneth and Alonso, Gustavo and Blott, Michaela},
  booktitle={2021 IEEE/ACM International Workshop on Heterogeneous High-performance Reconfigurable Computing (H2RC)},
  title={ACCL: FPGA-Accelerated Collectives over 100 Gbps TCP-IP},
  year={2021},
  pages={33-43},
  doi={10.1109/H2RC54759.2021.00009}}
```
